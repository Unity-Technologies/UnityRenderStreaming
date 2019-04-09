/*
 * Copyright (C) 2005, 2006, 2008 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_WTF_HASH_COUNTED_SET_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_WTF_HASH_COUNTED_SET_H_

#include "base/macros.h"
#include "third_party/blink/renderer/platform/wtf/allocator/partition_allocator.h"
#include "third_party/blink/renderer/platform/wtf/assertions.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace WTF {

// An unordered hash set that keeps track of how many times you added an item to
// the set. The iterators have fields ->key and ->value that return the set
// members and their counts, respectively.
template <typename Value,
          typename HashFunctions = typename DefaultHash<Value>::Hash,
          typename Traits = HashTraits<Value>,
          typename Allocator = PartitionAllocator>
class HashCountedSet {
  USE_ALLOCATOR(HashCountedSet, Allocator);

 private:
  typedef HashMap<Value,
                  unsigned,
                  HashFunctions,
                  Traits,
                  HashTraits<unsigned>,
                  Allocator>
      ImplType;

 public:
  typedef Value ValueType;
  using value_type = ValueType;
  typedef typename ImplType::iterator iterator;
  typedef typename ImplType::const_iterator const_iterator;
  typedef typename ImplType::AddResult AddResult;

  HashCountedSet() {
    static_assert(Allocator::kIsGarbageCollected ||
                      !IsPointerToGarbageCollectedType<Value>::value,
                  "Cannot put raw pointers to garbage-collected classes into "
                  "an off-heap HashCountedSet. Use "
                  "HeapHashCountedSet<Member<T>> instead.");
  }

  void swap(HashCountedSet& other) { impl_.swap(other.impl_); }

  unsigned size() const { return impl_.size(); }
  unsigned Capacity() const { return impl_.capacity(); }
  bool IsEmpty() const { return impl_.IsEmpty(); }

  // Iterators iterate over pairs of values (called key) and counts (called
  // value).
  iterator begin() { return impl_.begin(); }
  iterator end() { return impl_.end(); }
  const_iterator begin() const { return impl_.begin(); }
  const_iterator end() const { return impl_.end(); }

  iterator find(const ValueType& value) { return impl_.find(value); }
  const_iterator find(const ValueType& value) const {
    return impl_.find(value);
  }
  bool Contains(const ValueType& value) const { return impl_.Contains(value); }
  unsigned count(const ValueType& value) const { return impl_.at(value); }

  // Increases the count if an equal value is already present the return value
  // is a pair of an iterator to the new value's location, and a bool that is
  // true if an new entry was added.
  AddResult insert(const ValueType&);

  // Generalized add(), adding the value N times.
  AddResult insert(const ValueType&, unsigned);

  // Reduces the count of the value, and removes it if count goes down to
  // zero, returns true if the value is removed.
  bool erase(const ValueType& value) { return erase(find(value)); }
  bool erase(iterator);

  // Removes the value, regardless of its count.
  void RemoveAll(const ValueType& value) { RemoveAll(find(value)); }
  void RemoveAll(iterator);

  // Clears the whole set.
  void clear() { impl_.clear(); }

  Vector<Value> AsVector() const;

  template <typename VisitorDispatcher, typename A = Allocator>
  std::enable_if_t<A::kIsGarbageCollected> Trace(VisitorDispatcher visitor) {
    impl_.Trace(visitor);
  }

 protected:
  typename ImplType::ValueType** GetBufferSlot() {
    return impl_.GetBufferSlot();
  }

 private:
  ImplType impl_;

  DISALLOW_COPY_AND_ASSIGN(HashCountedSet);
};

template <typename T, typename U, typename V, typename W>
inline typename HashCountedSet<T, U, V, W>::AddResult
HashCountedSet<T, U, V, W>::insert(const ValueType& value, unsigned count) {
  DCHECK_GT(count, 0u);
  AddResult result = impl_.insert(value, 0);
  result.stored_value->value += count;
  return result;
}

template <typename T, typename U, typename V, typename W>
inline typename HashCountedSet<T, U, V, W>::AddResult
HashCountedSet<T, U, V, W>::insert(const ValueType& value) {
  return insert(value, 1u);
}

template <typename T, typename U, typename V, typename W>
inline bool HashCountedSet<T, U, V, W>::erase(iterator it) {
  if (it == end())
    return false;

  unsigned old_val = it->value;
  DCHECK(old_val);
  unsigned new_val = old_val - 1;
  if (new_val) {
    it->value = new_val;
    return false;
  }

  impl_.erase(it);
  return true;
}

template <typename T, typename U, typename V, typename W>
inline void HashCountedSet<T, U, V, W>::RemoveAll(iterator it) {
  if (it == end())
    return;

  impl_.erase(it);
}

template <typename Value,
          typename HashFunctions,
          typename Traits,
          typename Allocator,
          typename VectorType>
inline void CopyToVector(
    const HashCountedSet<Value, HashFunctions, Traits, Allocator>& collection,
    VectorType& vector) {
  {
    // Disallow GC across resize allocation, see crbug.com/568173
    typename VectorType::GCForbiddenScope scope;
    vector.resize(collection.size());
  }

  auto it = collection.begin();
  auto end = collection.end();
  for (unsigned i = 0; it != end; ++it, ++i)
    vector[i] = (*it).key;
}

template <typename T, typename U, typename V, typename W>
inline Vector<T> HashCountedSet<T, U, V, W>::AsVector() const {
  Vector<T> vector;
  CopyToVector(*this, vector);
  return vector;
}

}  // namespace WTF

using WTF::HashCountedSet;

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_WTF_HASH_COUNTED_SET_H_
