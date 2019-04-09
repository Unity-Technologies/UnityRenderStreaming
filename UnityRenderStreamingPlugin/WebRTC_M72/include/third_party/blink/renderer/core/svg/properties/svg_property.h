/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_PROPERTIES_SVG_PROPERTY_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_PROPERTIES_SVG_PROPERTY_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/svg/properties/svg_property_info.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class SVGElement;
class SVGAnimationElement;

class SVGPropertyBase : public GarbageCollectedFinalized<SVGPropertyBase> {
 public:
  // Properties do not have a primitive type by default
  typedef void PrimitiveType;

  virtual ~SVGPropertyBase() = default;

  // FIXME: remove this in WebAnimations transition.
  // This is used from SVGAnimatedNewPropertyAnimator for its animate-by-string
  // implementation.
  virtual SVGPropertyBase* CloneForAnimation(const String&) const = 0;

  virtual String ValueAsString() const = 0;

  // Set the initial value based on a per-type defined (encoded) value. Overload
  // this in the specific subclass to handle initial values, and set
  // kInitialValueBits appropriately.
  static constexpr int kInitialValueBits = 0;
  void SetInitial(unsigned) {}

  // FIXME: remove below and just have this inherit AnimatableValue in
  // WebAnimations transition.
  virtual void Add(SVGPropertyBase*, SVGElement*) = 0;
  virtual void CalculateAnimatedValue(
      SVGAnimationElement*,
      float percentage,
      unsigned repeat_count,
      SVGPropertyBase* from,
      SVGPropertyBase* to,
      SVGPropertyBase* to_at_end_of_duration_value,
      SVGElement*) = 0;
  virtual float CalculateDistance(SVGPropertyBase* to, SVGElement*) = 0;

  virtual AnimatedPropertyType GetType() const = 0;

  SVGPropertyBase* OwnerList() const { return owner_list_; }

  void SetOwnerList(SVGPropertyBase* owner_list) {
    // Previous owner list must be cleared before setting new owner list.
    DCHECK((!owner_list && owner_list_) || (owner_list && !owner_list_));

    owner_list_ = owner_list;
  }

  virtual void Trace(blink::Visitor* visitor) {}

 protected:
  SVGPropertyBase() : owner_list_(nullptr) {}

 private:
  // Oilpan: the back reference to the owner should be a Member, but this can
  // create cycles when SVG properties meet the off-heap InterpolationValue
  // hierarchy.  Not tracing it is safe, albeit an undesirable state of affairs.
  // See http://crbug.com/528275 for the detail.
  UntracedMember<SVGPropertyBase> owner_list_;
  DISALLOW_COPY_AND_ASSIGN(SVGPropertyBase);
};

#define DEFINE_SVG_PROPERTY_TYPE_CASTS(thisType)               \
  DEFINE_TYPE_CASTS(thisType, SVGPropertyBase, value,          \
                    value->GetType() == thisType::ClassType(), \
                    value.GetType() == thisType::ClassType());

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_PROPERTIES_SVG_PROPERTY_H_
