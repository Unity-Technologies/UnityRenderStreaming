/*
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_ACCESSIBILITY_AX_PROGRESS_INDICATOR_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_ACCESSIBILITY_AX_PROGRESS_INDICATOR_H_

#include "base/macros.h"
#include "third_party/blink/renderer/modules/accessibility/ax_layout_object.h"

namespace blink {

class AXObjectCacheImpl;
class HTMLProgressElement;
class LayoutProgress;

class AXProgressIndicator final : public AXLayoutObject {
 public:
  static AXProgressIndicator* Create(LayoutProgress*, AXObjectCacheImpl&);

  AXProgressIndicator(LayoutProgress*, AXObjectCacheImpl&);

 private:
  ax::mojom::Role DetermineAccessibilityRole() final;

  bool IsProgressIndicator() const override { return true; }

  bool ValueForRange(float* out_value) const override;
  bool MaxValueForRange(float* out_value) const override;
  bool MinValueForRange(float* out_value) const override;

  HTMLProgressElement* GetProgressElement() const;
  bool ComputeAccessibilityIsIgnored(IgnoredReasons* = nullptr) const override;

  DISALLOW_COPY_AND_ASSIGN(AXProgressIndicator);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_ACCESSIBILITY_AX_PROGRESS_INDICATOR_H_
