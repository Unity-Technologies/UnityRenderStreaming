// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_THREADING_SEQUENCE_BOUND_H_
#define BASE_THREADING_SEQUENCE_BOUND_H_

#include <new>
#include <type_traits>

#include "base/bind.h"
#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/location.h"
#include "base/memory/aligned_memory.h"
#include "base/memory/ptr_util.h"
#include "base/sequenced_task_runner.h"

namespace base {

// SequenceBound facilitates owning objects that live on a specified sequence,
// which is potentially different than the owner's sequence.  It encapsulates
// the work of posting tasks to the specified sequence to construct T, call
// methods on T, and destroy T.
//
// It does not provide explicit access to the underlying object directly, to
// prevent accidentally using it from the wrong sequence.
//
// Like std::unique_ptr<T>, a SequenceBound<T> may be moved between owners,
// and posted across threads.  It may also be up-casted (only), to permit
// SequenceBound to be used with interfaces.
//
// Basic usage looks like this:
//
//   // Some class that lives on |main_task_runner|.
//   class MyClass {
//    public:
//     explicit MyClass(const char* widget_title) {}
//     virtual ~MyClass() { ... }
//     virtual void DoSomething(int arg) { ... }
//   };
//
//   // On any thread...
//   scoped_refptr<SequencedTaskRunner> main_task_runner = ...;
//   auto widget = SequenceBound<MyClass>(main_task_runner, "My Title");
//   widget.Post(&MyObject::DoSomething, 1234);
//
// Note that |widget| is constructed asynchronously on |main_task_runner|,
// but calling Post() immediately is safe, since the actual call is posted
// to |main_task_runner| as well.
//
// |widget| will be deleted on |main_task_runner| asynchronously when it goes
// out of scope, or when Reset() is called.
//
// Here is a more complicated example that shows injection and upcasting:
//
//   // Some unrelated class that uses a |MyClass| to do something.
//   class SomeConsumer {
//    public:
//    // Note that ownership of |widget| is given to us!
//    explicit SomeConsumer(SequenceBound<MyClass> widget)
//        : widget_(std::move(widget)) { ... }
//
//    ~SomeConsumer() {
//      // |widget_| will be destroyed on the associated task runner.
//    }
//
//     SequenceBound<MyClass> widget_;
//   };
//
//   // Implementation of MyClass.
//   class MyDerivedClass : public MyClass { ... };
//
//   auto widget =
//     SequenceBound<MyDerivedClass>(main_task_runner, ctor args);
//   auto c = new SomeConsumer(std::move(widget));  // upcasts to MyClass

namespace internal {

// If we can't cast |Base*| into |Derived*|, then it's a virtual base if and
// only if |Base| is actually a base class of |Derived|.  Otherwise (including
// unrelated types), it isn't.  We default to Derived* so that the
// specialization below will apply when the cast to |Derived*| is valid.
template <typename Base, typename Derived, typename = Derived*>
struct is_virtual_base_of : public std::is_base_of<Base, Derived> {};

// If we can cast |Base*| into |Derived*|, then it's definitely not a virtual
// base.  When this happens, we'll match the default third template argument.
template <typename Base, typename Derived>
struct is_virtual_base_of<Base,
                          Derived,
                          decltype(static_cast<Derived*>(
                              static_cast<Base*>(nullptr)))> : std::false_type {
};

};  // namespace internal

template <typename T>
class SequenceBound {
 public:
  // Allow explicit null.
  SequenceBound() = default;

  // Construct a new instance of |T| that will be accessed only on
  // |task_runner|.  One may post calls to it immediately upon return.
  // This is marked as NO_SANITIZE because cfi doesn't like that we're casting
  // uninitialized memory to a |T*|.  However, it's safe since (a) the cast is
  // defined (see http://eel.is/c++draft/basic.life#6 for details), and (b) we
  // don't use the resulting pointer in any way that requries it to be
  // constructed, except by posting such a access to |impl_task_runner_| after
  // posting construction there as well.
  template <typename... Args>
  SequenceBound(scoped_refptr<base::SequencedTaskRunner> task_runner,
                Args&&... args) NO_SANITIZE("cfi-unrelated-cast")
      : impl_task_runner_(std::move(task_runner)) {
    // Allocate space for but do not construct an instance of |T|.
    storage_ = AlignedAlloc(sizeof(T), alignof(T));
    t_ = reinterpret_cast<T*>(storage_);

    // Post construction to the impl thread.
    impl_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&ConstructOwnerRecord<Args...>, base::Unretained(t_),
                       std::forward<Args>(args)...));
  }

  ~SequenceBound() { Reset(); }

  // Move construction from the same type can just take the pointer without
  // adjusting anything.  This is required in addition to the move conversion
  // constructor below.
  SequenceBound(SequenceBound&& other) { MoveRecordFrom(other); }

  // Move construction is supported from any type that's compatible with |T|.
  // This case handles |From| != |T|, so we must adjust the pointer offset.
  template <typename From>
  SequenceBound(SequenceBound<From>&& other) {
    MoveRecordFrom(other);
  }

  SequenceBound& operator=(SequenceBound&& other) {
    // Clean up any object we currently own.
    Reset();
    MoveRecordFrom(other);
    return *this;
  }

  template <typename From>
  SequenceBound<T>& operator=(SequenceBound<From>&& other) {
    // Clean up any object that we currently own.
    Reset();
    MoveRecordFrom(other);
    return *this;
  }

  // Move everything from |other|, doing pointer adjustment as needed.
  // This method is marked as NO_SANITIZE since (a) it might run before the
  // posted ctor runs on |impl_task_runner_|, and (b) implicit conversions to
  // non-virtual base classes are allowed before construction by the standard.
  // See http://eel.is/c++draft/basic.life#6 for more information.
  template <typename From>
  void MoveRecordFrom(From&& other) NO_SANITIZE("cfi-unrelated-cast") {
    // |other| might be is_null(), but that's okay.
    impl_task_runner_ = std::move(other.impl_task_runner_);

    // Note that static_cast<> isn't, in general, safe, since |other| might not
    // be constructed yet.  Implicit conversion is supported, as long as it
    // doesn't convert to a virtual base.  Of course, it allows only upcasts.
    t_ = other.t_;

    // The original storage is kept unmodified, so we can free it later.
    storage_ = other.storage_;

    other.storage_ = nullptr;
    other.t_ = nullptr;
  }

  // Post a call to |method| to |impl_task_runner_|.
  template <typename... Args>
  void Post(const base::Location& from_here,
            void (T::*method)(Args...),
            Args&&... args) const {
    impl_task_runner_->PostTask(
        from_here, base::BindOnce(
                       [](void (T::*method)(Args...), T* t, Args... args) {
                         (t->*method)(std::forward<Args>(args)...);
                       },
                       std::move(method), base::Unretained(t_),
                       std::forward<Args>(args)...));
  }

  // TODO(liberato): Add PostOrCall(), to support cases where synchronous calls
  // are okay if it's the same task runner.

  // TODO(liberato): Add PostAndReply()

  // TODO(liberato): Allow creation of callbacks that bind to a weak pointer,
  // and thread-hop to |impl_task_runner_| if needed.

  // Post destruction of any object we own, and return to the null state.
  void Reset() {
    if (is_null())
      return;

    // Destruct the object on the impl thread.
    impl_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&DeleteOwnerRecord, base::Unretained(t_),
                                  base::Unretained(storage_)));

    impl_task_runner_ = nullptr;
    t_ = nullptr;
    storage_ = nullptr;
  }

  // Return whether we own anything.  Note that this does not guarantee that any
  // previously owned object has been destroyed.  In particular, it will return
  // true immediately after a call to Reset(), though the underlying object
  // might still be pending destruction on the impl thread.
  bool is_null() const { return !t_; }

 private:
  // Pointer to the object,  Pointer may be modified on the owning thread.
  T* t_ = nullptr;

  // Original allocated storage for the object.
  void* storage_ = nullptr;

  // The task runner on which all access to |t_| should happen.
  scoped_refptr<base::SequencedTaskRunner> impl_task_runner_;

  // For move conversion.
  template <typename U>
  friend class SequenceBound;

  // Run on impl thread to construct |t|'s storage.
  template <typename... Args>
  static void ConstructOwnerRecord(T* t, Args&&... args) {
    new (t) T(std::forward<Args>(args)...);
  }

  // Destruct the object associated with |t|, and delete |storage|.
  static void DeleteOwnerRecord(T* t, void* storage) {
    t->~T();
    AlignedFree(storage);
  }

  // To preserve ownership semantics, we disallow copy construction / copy
  // assignment.  Move construction / assignment is fine.
  DISALLOW_COPY_AND_ASSIGN(SequenceBound);
};

}  // namespace base

#endif  // BASE_THREADING_SEQUENCE_BOUND_H_
