// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_GC_INFO_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_GC_INFO_H_

#include <atomic>
#include "base/gtest_prod_util.h"
#include "third_party/blink/renderer/platform/heap/finalizer_traits.h"
#include "third_party/blink/renderer/platform/heap/name_traits.h"
#include "third_party/blink/renderer/platform/heap/visitor.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/assertions.h"
#include "third_party/blink/renderer/platform/wtf/deque.h"
#include "third_party/blink/renderer/platform/wtf/hash_counted_set.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"
#include "third_party/blink/renderer/platform/wtf/hash_set.h"
#include "third_party/blink/renderer/platform/wtf/hash_table.h"
#include "third_party/blink/renderer/platform/wtf/linked_hash_set.h"
#include "third_party/blink/renderer/platform/wtf/list_hash_set.h"
#include "third_party/blink/renderer/platform/wtf/type_traits.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

// GCInfo contains meta-data associated with object classes allocated in the
// Blink heap. This meta-data consists of a function pointer used to trace the
// pointers in the class instance during garbage collection, an indication of
// whether or not the instance needs a finalization callback, and a function
// pointer used to finalize the instance when the garbage collector determines
// that the instance is no longer reachable. There is a GCInfo struct for each
// class that directly inherits from GarbageCollected or
// GarbageCollectedFinalized.
struct GCInfo {
  bool HasFinalizer() const { return non_trivial_finalizer_; }
  bool HasVTable() const { return has_v_table_; }

  TraceCallback trace_;
  FinalizationCallback finalize_;
  NameCallback name_;
  bool non_trivial_finalizer_;
  bool has_v_table_;
};

#if DCHECK_IS_ON()
PLATFORM_EXPORT void AssertObjectHasGCInfo(const void*, uint32_t gc_info_index);
#endif

class PLATFORM_EXPORT GCInfoTable {
 public:
  // At maximum |kMaxIndex - 1| indices are supported.
  //
  // We assume that 14 bits is enough to represent all possible types: during
  // telemetry runs, we see about 1,000 different types; looking at the output
  // of the Oilpan GC Clang plugin, there appear to be at most about 6,000
  // types. Thus 14 bits should be more than twice as many bits as we will ever
  // need.
  static constexpr uint32_t kMaxIndex = 1 << 14;

  // Sets up a singleton table that can be acquired using Get().
  static void CreateGlobalTable();

  static GCInfoTable& Get() { return *global_table_; }

  inline const GCInfo* GCInfoFromIndex(uint32_t index) {
    DCHECK_GE(index, 1u);
    DCHECK(index < kMaxIndex);
    DCHECK(table_);
    const GCInfo* info = table_[index];
    DCHECK(info);
    return info;
  }

  uint32_t EnsureGCInfoIndex(const GCInfo*, std::atomic_uint32_t*);

  uint32_t GcInfoIndex() { return current_index_; }

 private:
  FRIEND_TEST_ALL_PREFIXES(GCInfoTest, InitialEmpty);
  FRIEND_TEST_ALL_PREFIXES(GCInfoTest, ResizeToMaxIndex);

  // Use GCInfoTable::Get() for retrieving the global table outside of testing
  // code.
  GCInfoTable();

  void Resize();

  // Singleton for each process. Retrieved through Get().
  static GCInfoTable* global_table_;

  // Holds the per-class GCInfo descriptors; each HeapObjectHeader keeps an
  // index into this table.
  const GCInfo** table_ = nullptr;

  // GCInfo indices start from 1 for heap objects, with 0 being treated
  // specially as the index for freelist entries and large heap objects.
  uint32_t current_index_ = 0;

  // The limit (exclusive) of the currently allocated table.
  uint32_t limit_ = 0;

  Mutex table_mutex_;
};

// GCInfoAtBaseType should be used when returning a unique 14 bit integer
// for a given gcInfo.
template <typename T>
struct GCInfoAtBaseType {
  STATIC_ONLY(GCInfoAtBaseType);
  static uint32_t Index() {
    static_assert(sizeof(T), "T must be fully defined");
    static const GCInfo kGcInfo = {
        TraceTrait<T>::Trace,          FinalizerTrait<T>::Finalize,
        NameTrait<T>::GetName,         FinalizerTrait<T>::kNonTrivialFinalizer,
        std::is_polymorphic<T>::value,
    };
    // This is more complicated than using threadsafe initialization, but this
    // is instantiated many times (once for every GC type).
    static std::atomic_uint32_t gc_info_index{0};
    uint32_t index = gc_info_index.load(std::memory_order_acquire);
    if (!index)
      index = GCInfoTable::Get().EnsureGCInfoIndex(&kGcInfo, &gc_info_index);
    DCHECK_GE(index, 1u);
    DCHECK_LT(index, GCInfoTable::kMaxIndex);
    return index;
  }
};

template <typename T,
          bool = WTF::IsSubclassOfTemplate<typename std::remove_const<T>::type,
                                           GarbageCollected>::value>
struct GetGarbageCollectedType;

template <typename T>
struct GetGarbageCollectedType<T, true> {
  STATIC_ONLY(GetGarbageCollectedType);
  using type = typename T::GarbageCollectedType;
};

template <typename T>
struct GetGarbageCollectedType<T, false> {
  STATIC_ONLY(GetGarbageCollectedType);
  using type = T;
};

template <typename T>
struct GCInfoTrait {
  STATIC_ONLY(GCInfoTrait);
  static uint32_t Index() {
    return GCInfoAtBaseType<typename GetGarbageCollectedType<T>::type>::Index();
  }
};

template <typename U>
class GCInfoTrait<const U> : public GCInfoTrait<U> {};

template <typename T, typename U, typename V, typename W, typename X>
class HeapHashMap;
template <typename T, typename U, typename V>
class HeapHashSet;
template <typename T, typename U, typename V>
class HeapLinkedHashSet;
template <typename T, wtf_size_t inlineCapacity, typename U>
class HeapListHashSet;
template <typename ValueArg, wtf_size_t inlineCapacity>
class HeapListHashSetAllocator;
template <typename T, wtf_size_t inlineCapacity>
class HeapVector;
template <typename T, wtf_size_t inlineCapacity>
class HeapDeque;
template <typename T, typename U, typename V>
class HeapHashCountedSet;

template <typename T, typename U, typename V, typename W, typename X>
struct GCInfoTrait<HeapHashMap<T, U, V, W, X>>
    : public GCInfoTrait<HashMap<T, U, V, W, X, HeapAllocator>> {};
template <typename T, typename U, typename V>
struct GCInfoTrait<HeapHashSet<T, U, V>>
    : public GCInfoTrait<HashSet<T, U, V, HeapAllocator>> {};
template <typename T, typename U, typename V>
struct GCInfoTrait<HeapLinkedHashSet<T, U, V>>
    : public GCInfoTrait<LinkedHashSet<T, U, V, HeapAllocator>> {};
template <typename T, wtf_size_t inlineCapacity, typename U>
struct GCInfoTrait<HeapListHashSet<T, inlineCapacity, U>>
    : public GCInfoTrait<
          ListHashSet<T,
                      inlineCapacity,
                      U,
                      HeapListHashSetAllocator<T, inlineCapacity>>> {};
template <typename T, wtf_size_t inlineCapacity>
struct GCInfoTrait<HeapVector<T, inlineCapacity>>
    : public GCInfoTrait<Vector<T, inlineCapacity, HeapAllocator>> {};
template <typename T, wtf_size_t inlineCapacity>
struct GCInfoTrait<HeapDeque<T, inlineCapacity>>
    : public GCInfoTrait<Deque<T, inlineCapacity, HeapAllocator>> {};
template <typename T, typename U, typename V>
struct GCInfoTrait<HeapHashCountedSet<T, U, V>>
    : public GCInfoTrait<HashCountedSet<T, U, V, HeapAllocator>> {};

}  // namespace blink

#endif
