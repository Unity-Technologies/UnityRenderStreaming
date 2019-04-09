/*
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Graham Dennis (graham.dennis@gmail.com)
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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_TRANSFORMS_TRANSFORM_OPERATIONS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_TRANSFORMS_TRANSFORM_OPERATIONS_H_

#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/platform/geometry/layout_size.h"
#include "third_party/blink/renderer/platform/transforms/transform_operation.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {
class FloatBox;

class PLATFORM_EXPORT EmptyTransformOperations final {
  DISALLOW_NEW();
};

class PLATFORM_EXPORT TransformOperations {
  DISALLOW_NEW();

 public:
  explicit TransformOperations(bool make_identity = false);
  TransformOperations(const EmptyTransformOperations&) {}

  bool operator==(const TransformOperations& o) const;
  bool operator!=(const TransformOperations& o) const { return !(*this == o); }

  void Apply(const FloatSize& sz, TransformationMatrix& t) const {
    for (auto& operation : operations_)
      operation->Apply(t, sz);
  }

  // Return true if any of the operation types are 3D operation types (even if
  // the values describe affine transforms)
  bool Has3DOperation() const {
    for (auto& operation : operations_)
      if (operation->Is3DOperation())
        return true;
    return false;
  }

  // Returns true if any operation has a non-trivial component in the Z
  // axis.
  bool HasNonTrivial3DComponent() const {
    for (auto& operation : operations_) {
      if (operation->HasNonTrivial3DComponent())
        return true;
    }
    return false;
  }

  bool DependsOnBoxSize() const {
    for (auto& operation : operations_) {
      if (operation->DependsOnBoxSize())
        return true;
    }
    return false;
  }

  bool OperationsMatch(const TransformOperations&) const;

  void clear() { operations_.clear(); }

  Vector<scoped_refptr<TransformOperation>>& Operations() {
    return operations_;
  }
  const Vector<scoped_refptr<TransformOperation>>& Operations() const {
    return operations_;
  }

  wtf_size_t size() const { return operations_.size(); }
  const TransformOperation* at(wtf_size_t index) const {
    return index < operations_.size() ? operations_.at(index).get() : nullptr;
  }

  bool BlendedBoundsForBox(const FloatBox&,
                           const TransformOperations& from,
                           const double& min_progress,
                           const double& max_progress,
                           FloatBox* bounds) const;
  TransformOperations BlendByMatchingOperations(const TransformOperations& from,
                                                const double& progress) const;
  scoped_refptr<TransformOperation> BlendByUsingMatrixInterpolation(
      const TransformOperations& from,
      double progress) const;
  TransformOperations Blend(const TransformOperations& from,
                            double progress) const;
  TransformOperations Add(const TransformOperations& addend) const;
  TransformOperations Zoom(double factor) const;

 private:
  Vector<scoped_refptr<TransformOperation>> operations_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_TRANSFORMS_TRANSFORM_OPERATIONS_H_
