/*
 * Copyright (C) 2005, 2006, 2007, 2008, 2011 Apple Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_WTF_HASH_SET_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_WTF_HASH_SET_H_

#include <initializer_list>
#include "third_party/blink/renderer/platform/wtf/allocator/partition_allocator.h"
#include "third_party/blink/renderer/platform/wtf/hash_table.h"
#include "third_party/blink/renderer/platform/wtf/wtf_size_t.h"

namespace WTF {

struct IdentityExtractor;

// Note: empty or deleted values are not allowed, using them may lead to
// undefined behavior. For pointer keys this means that null pointers are not
// allowed; for integer keys 0 or -1 can't be used as a key. This restriction
// can be lifted if you supply custom key traits.
template <typename ValueArg,
          typename HashArg = typename DefaultHash<ValueArg>::Hash,
          typename TraitsArg = HashTraits<ValueArg>,
          typename Allocator = PartitionAllocator>
class HashSet {
  USE_ALLOCATOR(HashSet, Allocator);

 private:
  typedef HashArg HashFunctions;
  typedef TraitsArg ValueTraits;
  typedef typename ValueTraits::PeekInType ValuePeekInType;

 public:
  typedef typename ValueTraits::TraitType ValueType;
  using value_type = ValueType;

 private:
  typedef HashTable<ValueType,
                    ValueType,
                    IdentityExtractor,
                    HashFunctions,
                    ValueTraits,
                    ValueTraits,
                    Allocator>
      HashTableType;

 public:
  typedef HashTableConstIteratorAdapter<HashTableType, ValueTraits> iterator;
  typedef HashTableConstIteratorAdapter<HashTableType, ValueTraits>
      const_iterator;
  typedef typename HashTableType::AddResult AddResult;

  HashSet() {
    static_assert(Allocator::kIsGarbageCollected ||
                      !IsPointerToGarbageCollectedType<ValueArg>::value,
                  "Cannot put raw pointers to garbage-collected classes into "
                  "an off-heap HashSet. Use HeapHashSet<Member<T>> instead.");
  }
  HashSet(const HashSet&) = default;
  HashSet& operator=(const HashSet&) = default;
  HashSet(HashSet&&) = default;
  HashSet& operator=(HashSet&&) = default;

  HashSet(std::initializer_list<ValueType> elements);
  HashSet& operator=(std::initializer_list<ValueType> elements);

  void swap(HashSet& ref) { impl_.swap(ref.impl_); }

  unsigned size() const;
  unsigned Capacity() const;
  bool IsEmpty() const;

  void ReserveCapacityForSize(unsigned size) {
    impl_.ReserveCapacityForSize(size);
  }

  iterator begin() const;
  iterator end() const;

  iterator find(ValuePeekInType) const;
  bool Contains(ValuePeekInType) const;

  // An alternate version of find() that finds the object by hashing and
  // comparing with some other type, to avoid the cost of type
  // conversion. HashTranslator must have the following function members:
  //   static unsigned hash(const T&);
  //   static bool equal(const ValueType&, const T&);
  template <typename HashTranslator, typename T>
  iterator Find(const T&) const;
  template <typename HashTranslator, typename T>
  bool Contains(const T&) const;

  // The return value is a pair of an iterator to the new value's location,
  // and a bool that is true if an new entry was added.
  template <typename IncomingValueType>
  AddResult insert(IncomingValueType&&);

  // An alternate version of add() that finds the object by hashing and
  // comparing with some other type, to avoid the cost of type conversion if
  // the object is already in the table. HashTranslator must have the
  // following function members:
  //   static unsigned hash(const T&);
  //   static bool equal(const ValueType&, const T&);
  //   static translate(ValueType&, T&&, unsigned hashCode);
  template <typename HashTranslator, typename T>
  AddResult AddWithTranslator(T&&);

  void erase(ValuePeekInType);
  void erase(iterator);
  void clear();
  template <typename Collection>
  void RemoveAll(const Collection& to_be_removed) {
    WTF::RemoveAll(*this, to_be_removed);
  }

  ValueType Take(iterator);
  ValueType Take(ValuePeekInType);
  ValueType TakeAny();

  template <typename VisitorDispatcher, typename A = Allocator>
  std::enable_if_t<A::kIsGarbageCollected> Trace(VisitorDispatcher visitor) {
    impl_.Trace(visitor);
  }

 protected:
  ValueType** GetBufferSlot() { return impl_.GetBufferSlot(); }

 private:
  HashTableType impl_;
};

struct IdentityExtractor {
  STATIC_ONLY(IdentityExtractor);
  template <typename T>
  static const T& Extract(const T& t) {
    return t;
  }
};

template <typename Translator>
struct HashSetTranslatorAdapter {
  STATIC_ONLY(HashSetTranslatorAdapter);
  template <typename T>
  static unsigned GetHash(const T& key) {
    return Translator::GetHash(key);
  }
  template <typename T, typename U>
  static bool Equal(const T& a, const U& b) {
    return Translator::Equal(a, b);
  }
  template <typename T, typename U, typename V>
  static void Translate(T& location, U&& key, const V&, unsigned hash_code) {
    Translator::Translate(location, std::forward<U>(key), hash_code);
  }
};

template <typename Value,
          typename HashFunctions,
          typename Traits,
          typename Allocator>
HashSet<Value, HashFunctions, Traits, Allocator>::HashSet(
    std::initializer_list<ValueType> elements) {
  if (elements.size())
    impl_.ReserveCapacityForSize(SafeCast<wtf_size_t>(elements.size()));
  for (const ValueType& element : elements)
    insert(element);
}

template <typename Value,
          typename HashFunctions,
          typename Traits,
          typename Allocator>
auto HashSet<Value, HashFunctions, Traits, Allocator>::operator=(
    std::initializer_list<ValueType> elements) -> HashSet& {
  *this = HashSet(std::move(elements));
  return *this;
}

template <typename T, typename U, typename V, typename W>
inline unsigned HashSet<T, U, V, W>::size() const {
  return impl_.size();
}

template <typename T, typename U, typename V, typename W>
inline unsigned HashSet<T, U, V, W>::Capacity() const {
  return impl_.Capacity();
}

template <typename T, typename U, typename V, typename W>
inline bool HashSet<T, U, V, W>::IsEmpty() const {
  return impl_.IsEmpty();
}

template <typename T, typename U, typename V, typename W>
inline typename HashSet<T, U, V, W>::iterator HashSet<T, U, V, W>::begin()
    const {
  return impl_.begin();
}

template <typename T, typename U, typename V, typename W>
inline typename HashSet<T, U, V, W>::iterator HashSet<T, U, V, W>::end() const {
  return impl_.end();
}

template <typename T, typename U, typename V, typename W>
inline typename HashSet<T, U, V, W>::iterator HashSet<T, U, V, W>::find(
    ValuePeekInType value) const {
  return impl_.find(value);
}

template <typename Value,
          typename HashFunctions,
          typename Traits,
          typename Allocator>
inline bool HashSet<Value, HashFunctions, Traits, Allocator>::Contains(
    ValuePeekInType value) const {
  return impl_.Contains(value);
}

template <typename Value,
          typename HashFunctions,
          typename Traits,
          typename Allocator>
template <typename HashTranslator, typename T>
typename HashSet<Value, HashFunctions, Traits, Allocator>::
    iterator inline HashSet<Value, HashFunctions, Traits, Allocator>::Find(
        const T& value) const {
  return impl_.template Find<HashSetTranslatorAdapter<HashTranslator>>(value);
}

template <typename Value,
          typename HashFunctions,
          typename Traits,
          typename Allocator>
template <typename HashTranslator, typename T>
inline bool HashSet<Value, HashFunctions, Traits, Allocator>::Contains(
    const T& value) const {
  return impl_.template Contains<HashSetTranslatorAdapter<HashTranslator>>(
      value);
}

template <typename T, typename U, typename V, typename W>
template <typename IncomingValueType>
inline typename HashSet<T, U, V, W>::AddResult HashSet<T, U, V, W>::insert(
    IncomingValueType&& value) {
  return impl_.insert(std::forward<IncomingValueType>(value));
}

template <typename Value,
          typename HashFunctions,
          typename Traits,
          typename Allocator>
template <typename HashTranslator, typename T>
inline typename HashSet<Value, HashFunctions, Traits, Allocator>::AddResult
HashSet<Value, HashFunctions, Traits, Allocator>::AddWithTranslator(T&& value) {
  // Forward only the first argument, because the second argument isn't actually
  // used in HashSetTranslatorAdapter.
  return impl_
      .template InsertPassingHashCode<HashSetTranslatorAdapter<HashTranslator>>(
          std::forward<T>(value), value);
}

template <typename T, typename U, typename V, typename W>
inline void HashSet<T, U, V, W>::erase(iterator it) {
  impl_.erase(it.impl_);
}

template <typename T, typename U, typename V, typename W>
inline void HashSet<T, U, V, W>::erase(ValuePeekInType value) {
  erase(find(value));
}

template <typename T, typename U, typename V, typename W>
inline void HashSet<T, U, V, W>::clear() {
  impl_.clear();
}

template <typename T, typename U, typename V, typename W>
inline auto HashSet<T, U, V, W>::Take(iterator it) -> ValueType {
  if (it == end())
    return ValueTraits::EmptyValue();

  ValueType result = std::move(const_cast<ValueType&>(*it));
  erase(it);

  return result;
}

template <typename T, typename U, typename V, typename W>
inline auto HashSet<T, U, V, W>::Take(ValuePeekInType value) -> ValueType {
  return Take(find(value));
}

template <typename T, typename U, typename V, typename W>
inline auto HashSet<T, U, V, W>::TakeAny() -> ValueType {
  return Take(begin());
}

template <typename C, typename W>
inline void CopyToVector(const C& collection, W& vector) {
  typedef typename C::const_iterator iterator;

  {
    // Disallow GC across resize allocation, see crbug.com/568173
    typename W::GCForbiddenScope scope;
    vector.resize(collection.size());
  }

  iterator it = collection.begin();
  iterator end = collection.end();
  for (unsigned i = 0; it != end; ++it, ++i)
    vector[i] = *it;
}

}  // namespace WTF

using WTF::HashSet;

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_WTF_HASH_SET_H_
