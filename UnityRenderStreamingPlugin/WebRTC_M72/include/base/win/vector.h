// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_WIN_VECTOR_H_
#define BASE_WIN_VECTOR_H_

#include <ivectorchangedeventargs.h>
#include <windows.foundation.collections.h>
#include <wrl/implements.h>

#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

#include "base/base_export.h"
#include "base/containers/flat_map.h"
#include "base/logging.h"

namespace base {
namespace win {

template <typename T>
class Vector;

namespace internal {

template <typename T>
using Complex =
    typename ABI::Windows::Foundation::Collections::IVector<T>::T_complex;

template <typename T>
using Logical = typename ABI::Windows::Foundation::Internal::GetLogicalType<
    Complex<T>>::type;

template <typename T>
using Abi =
    typename ABI::Windows::Foundation::Internal::GetAbiType<Complex<T>>::type;

class BASE_EXPORT VectorChangedEventArgs
    : public Microsoft::WRL::RuntimeClass<
          Microsoft::WRL::RuntimeClassFlags<
              Microsoft::WRL::WinRtClassicComMix |
              Microsoft::WRL::InhibitRoOriginateError>,
          ABI::Windows::Foundation::Collections::IVectorChangedEventArgs> {
 public:
  VectorChangedEventArgs(
      ABI::Windows::Foundation::Collections::CollectionChange change,
      unsigned int index)
      : change_(change), index_(index) {}

  ~VectorChangedEventArgs() override = default;

  // ABI::Windows::Foundation::Collections::IVectorChangedEventArgs:
  IFACEMETHODIMP get_CollectionChange(
      ABI::Windows::Foundation::Collections::CollectionChange* value) override;
  IFACEMETHODIMP get_Index(unsigned int* value) override;

 private:
  const ABI::Windows::Foundation::Collections::CollectionChange change_;
  const unsigned int index_;
};

template <typename T>
class VectorView
    : public Microsoft::WRL::RuntimeClass<
          Microsoft::WRL::RuntimeClassFlags<
              Microsoft::WRL::WinRtClassicComMix |
              Microsoft::WRL::InhibitRoOriginateError>,
          ABI::Windows::Foundation::Collections::IVectorView<Logical<T>>,
          ABI::Windows::Foundation::Collections::VectorChangedEventHandler<
              Logical<T>>> {
 public:
  using LogicalT = Logical<T>;
  using AbiT = Abi<T>;

  explicit VectorView(Microsoft::WRL::ComPtr<Vector<LogicalT>> vector)
      : vector_(std::move(vector)) {
    vector_->add_VectorChanged(this, &vector_changed_token_);
  }

  ~VectorView() {
    if (vector_)
      vector_->remove_VectorChanged(vector_changed_token_);
  }

  // ABI::Windows::Foundation::Collections::IVectorView:
  IFACEMETHODIMP GetAt(unsigned index, AbiT* item) override {
    return vector_ ? vector_->GetAt(index, item) : E_CHANGED_STATE;
  }

  IFACEMETHODIMP get_Size(unsigned* size) override {
    return vector_ ? vector_->get_Size(size) : E_CHANGED_STATE;
  }

  IFACEMETHODIMP IndexOf(AbiT value, unsigned* index, boolean* found) override {
    return vector_ ? vector_->IndexOf(std::move(value), index, found)
                   : E_CHANGED_STATE;
  }

  IFACEMETHODIMP GetMany(unsigned start_index,
                         unsigned capacity,
                         AbiT* value,
                         unsigned* actual) override {
    return vector_ ? vector_->GetMany(start_index, capacity, value, actual)
                   : E_CHANGED_STATE;
  }

  // ABI::Windows::Foundation::Collections::VectorChangedEventHandler:
  IFACEMETHODIMP Invoke(
      ABI::Windows::Foundation::Collections::IObservableVector<LogicalT>*
          sender,
      ABI::Windows::Foundation::Collections::IVectorChangedEventArgs* e)
      override {
    DCHECK_EQ(vector_.Get(), sender);
    vector_.Reset();
    sender->remove_VectorChanged(vector_changed_token_);
    return S_OK;
  }

 private:
  Microsoft::WRL::ComPtr<Vector<LogicalT>> vector_;
  EventRegistrationToken vector_changed_token_;
};

template <typename T>
HRESULT CopyTo(const T& value, T* ptr) {
  *ptr = value;
  return S_OK;
}

template <typename T>
HRESULT CopyTo(const Microsoft::WRL::ComPtr<T>& com_ptr, T** ptr) {
  return com_ptr.CopyTo(ptr);
}

template <typename T>
HRESULT CopyN(typename std::vector<T>::const_iterator first,
              unsigned count,
              T* result) {
  std::copy_n(first, count, result);
  return S_OK;
}

template <typename T>
HRESULT CopyN(
    typename std::vector<Microsoft::WRL::ComPtr<T>>::const_iterator first,
    unsigned count,
    T** result) {
  for (unsigned i = 0; i < count; ++i)
    CopyTo(*first++, result++);
  return S_OK;
}

template <typename T>
bool IsEqual(const T& lhs, const T& rhs) {
  return lhs == rhs;
}

template <typename T>
bool IsEqual(const Microsoft::WRL::ComPtr<T>& com_ptr, const T* ptr) {
  return com_ptr.Get() == ptr;
}

}  // namespace internal

// This file provides an implementation of Windows::Foundation::IVector. It
// functions as a thin wrapper around an std::vector, and dispatches method
// calls to either the corresponding std::vector API or appropriate
// std::algorithms. Furthermore, it notifies its observers whenever its
// observable state changes. A base::win::Vector can be constructed for any type
// T, and is implicitly constructible from a std::vector. In the case where T is
// a pointer derived from IUnknown, the std::vector needs to be of type
// Microsoft::WRL::ComPtr<T>. This enforces proper reference counting and
// improves safety.
template <typename T>
class Vector
    : public Microsoft::WRL::RuntimeClass<
          Microsoft::WRL::RuntimeClassFlags<
              Microsoft::WRL::WinRt | Microsoft::WRL::InhibitRoOriginateError>,
          ABI::Windows::Foundation::Collections::IVector<internal::Logical<T>>,
          ABI::Windows::Foundation::Collections::IObservableVector<
              internal::Logical<T>>> {
 public:
  // windows.foundation.collections.h defines the following template and
  // semantics in Windows::Foundation::Internal:
  //
  // template <class LogicalType, class AbiType>
  // struct AggregateType;
  //
  //   LogicalType - the Windows Runtime type (eg, runtime class, interface
  //                 group, etc) being provided as an argument to an _impl
  //                 template, when that type cannot be represented at the ABI.
  //   AbiType     - the type used for marshalling, ie "at the ABI", for the
  //                 logical type.
  using LogicalT = internal::Logical<T>;
  using AbiT = internal::Abi<T>;

  using StorageT =
      std::conditional_t<std::is_convertible<AbiT, IUnknown*>::value,
                         Microsoft::WRL::ComPtr<std::remove_pointer_t<AbiT>>,
                         AbiT>;

  Vector() = default;
  explicit Vector(const std::vector<StorageT>& vector) : vector_(vector) {}
  explicit Vector(std::vector<StorageT>&& vector)
      : vector_(std::move(vector)) {}

  // ABI::Windows::Foundation::Collections::IVector:
  IFACEMETHODIMP GetAt(unsigned index, AbiT* item) override {
    if (index >= vector_.size())
      return E_BOUNDS;
    return internal::CopyTo(vector_[index], item);
  }

  IFACEMETHODIMP get_Size(unsigned* size) override {
    *size = vector_.size();
    return S_OK;
  }

  IFACEMETHODIMP GetView(
      ABI::Windows::Foundation::Collections::IVectorView<LogicalT>** view)
      override {
    return Microsoft::WRL::Make<internal::VectorView<LogicalT>>(this).CopyTo(
        view);
  }

  IFACEMETHODIMP IndexOf(AbiT value, unsigned* index, boolean* found) override {
    auto iter = std::find_if(vector_.begin(), vector_.end(),
                             [&value](const StorageT& elem) {
                               return internal::IsEqual(elem, value);
                             });
    *index = iter != vector_.end() ? std::distance(vector_.begin(), iter) : 0;
    *found = iter != vector_.end();
    return S_OK;
  }

  IFACEMETHODIMP SetAt(unsigned index, AbiT item) override {
    if (index >= vector_.size())
      return E_BOUNDS;

    vector_[index] = std::move(item);
    NotifyVectorChanged(
        ABI::Windows::Foundation::Collections::CollectionChange_ItemChanged,
        index);
    return S_OK;
  }

  IFACEMETHODIMP InsertAt(unsigned index, AbiT item) override {
    if (index > vector_.size())
      return E_BOUNDS;

    vector_.insert(std::next(vector_.begin(), index), std::move(item));
    NotifyVectorChanged(
        ABI::Windows::Foundation::Collections::CollectionChange_ItemInserted,
        index);
    return S_OK;
  }

  IFACEMETHODIMP RemoveAt(unsigned index) override {
    if (index >= vector_.size())
      return E_BOUNDS;

    vector_.erase(std::next(vector_.begin(), index));
    NotifyVectorChanged(
        ABI::Windows::Foundation::Collections::CollectionChange_ItemRemoved,
        index);
    return S_OK;
  }

  IFACEMETHODIMP Append(AbiT item) override {
    vector_.push_back(std::move(item));
    NotifyVectorChanged(
        ABI::Windows::Foundation::Collections::CollectionChange_ItemInserted,
        vector_.size() - 1);
    return S_OK;
  }

  IFACEMETHODIMP RemoveAtEnd() override {
    if (vector_.empty())
      return E_BOUNDS;

    vector_.pop_back();
    NotifyVectorChanged(
        ABI::Windows::Foundation::Collections::CollectionChange_ItemRemoved,
        vector_.size());
    return S_OK;
  }

  IFACEMETHODIMP Clear() override {
    vector_.clear();
    NotifyVectorChanged(
        ABI::Windows::Foundation::Collections::CollectionChange_Reset, 0);
    return S_OK;
  }

  IFACEMETHODIMP GetMany(unsigned start_index,
                         unsigned capacity,
                         AbiT* value,
                         unsigned* actual) override {
    if (start_index > vector_.size())
      return E_BOUNDS;

    *actual = std::min<unsigned>(vector_.size() - start_index, capacity);
    return internal::CopyN(std::next(vector_.begin(), start_index), *actual,
                           value);
  }

  IFACEMETHODIMP ReplaceAll(unsigned count, AbiT* value) override {
    vector_.assign(value, std::next(value, count));
    NotifyVectorChanged(
        ABI::Windows::Foundation::Collections::CollectionChange_Reset, 0);
    return S_OK;
  }

  // ABI::Windows::Foundation::Collections::IObservableVector:
  IFACEMETHODIMP add_VectorChanged(
      ABI::Windows::Foundation::Collections::VectorChangedEventHandler<
          LogicalT>* handler,
      EventRegistrationToken* token) override {
    token->value = handler_id_++;
    handlers_.emplace_hint(handlers_.end(), token->value, handler);
    return S_OK;
  }

  IFACEMETHODIMP remove_VectorChanged(EventRegistrationToken token) override {
    handlers_.erase(token.value);
    return S_OK;
  }

  void NotifyVectorChanged(
      ABI::Windows::Foundation::Collections::CollectionChange change,
      unsigned int index) {
    auto args =
        Microsoft::WRL::Make<internal::VectorChangedEventArgs>(change, index);

    // Invoking the handlers could result in mutations to the map, thus we make
    // a copy beforehand.
    auto handlers = handlers_;
    for (auto& handler : handlers)
      handler.second->Invoke(this, args.Get());
  }

  const std::vector<AbiT>& vector_for_testing() { return vector_; }

 private:
  ~Vector() override {
    // Handlers should not outlive the Vector. Furthermore, they must ensure
    // they are unregistered before the the handler is destroyed. This implies
    // there should be no handlers left when the Vector is destructed.
    DCHECK(handlers_.empty());
  }

  std::vector<StorageT> vector_;
  base::flat_map<int64_t,
                 ABI::Windows::Foundation::Collections::
                     VectorChangedEventHandler<LogicalT>*>
      handlers_;
  int64_t handler_id_ = 0;
};

}  // namespace win
}  // namespace base

#endif  // BASE_WIN_VECTOR_H_
