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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_KEYFRAME_EFFECT_MODEL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_KEYFRAME_EFFECT_MODEL_H_

#include <memory>
#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/core/animation/animation_effect.h"
#include "third_party/blink/renderer/core/animation/effect_model.h"
#include "third_party/blink/renderer/core/animation/interpolation_effect.h"
#include "third_party/blink/renderer/core/animation/property_handle.h"
#include "third_party/blink/renderer/core/animation/string_keyframe.h"
#include "third_party/blink/renderer/core/animation/transition_keyframe.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/animation/timing_function.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"
#include "third_party/blink/renderer/platform/wtf/hash_set.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class ComputedStyle;
class Element;
class KeyframeEffectModelTest;

class CORE_EXPORT KeyframeEffectModelBase : public EffectModel {
 public:
  // FIXME: Implement accumulation.

  using PropertySpecificKeyframeVector =
      HeapVector<Member<Keyframe::PropertySpecificKeyframe>>;
  class PropertySpecificKeyframeGroup
      : public GarbageCollected<PropertySpecificKeyframeGroup> {
   public:
    void AppendKeyframe(Keyframe::PropertySpecificKeyframe*);
    const PropertySpecificKeyframeVector& Keyframes() const {
      return keyframes_;
    }

    void Trace(Visitor* visitor) { visitor->Trace(keyframes_); }

   private:
    void RemoveRedundantKeyframes();
    bool AddSyntheticKeyframeIfRequired(
        scoped_refptr<TimingFunction> zero_offset_easing);

    PropertySpecificKeyframeVector keyframes_;

    friend class KeyframeEffectModelBase;
  };

  bool AffectedByUnderlyingAnimations() const final { return !IsReplaceOnly(); }
  bool IsReplaceOnly() const;

  PropertyHandleSet Properties() const;

  using KeyframeVector = HeapVector<Member<Keyframe>>;
  const KeyframeVector& GetFrames() const { return keyframes_; }
  bool HasFrames() const { return !keyframes_.IsEmpty(); }
  template <class K>
  void SetFrames(HeapVector<K>& keyframes);

  CompositeOperation Composite() const { return composite_; }
  void SetComposite(CompositeOperation composite) { composite_ = composite; }

  const PropertySpecificKeyframeVector& GetPropertySpecificKeyframes(
      const PropertyHandle& property) const {
    EnsureKeyframeGroups();
    return keyframe_groups_->at(property)->Keyframes();
  }

  using KeyframeGroupMap =
      HeapHashMap<PropertyHandle, Member<PropertySpecificKeyframeGroup>>;
  const KeyframeGroupMap& GetPropertySpecificKeyframeGroups() const {
    EnsureKeyframeGroups();
    return *keyframe_groups_;
  }

  // EffectModel implementation.
  bool Sample(int iteration,
              double fraction,
              AnimationTimeDelta iteration_duration,
              HeapVector<Member<Interpolation>>&) const override;

  bool IsKeyframeEffectModel() const override { return true; }

  virtual bool IsStringKeyframeEffectModel() const { return false; }
  virtual bool IsTransitionKeyframeEffectModel() const { return false; }

  bool HasSyntheticKeyframes() const {
    EnsureKeyframeGroups();
    return has_synthetic_keyframes_;
  }

  void InvalidateCompositorKeyframesSnapshot() const {
    needs_compositor_keyframes_snapshot_ = true;
  }

  bool SnapshotNeutralCompositorKeyframes(
      Element&,
      const ComputedStyle& old_style,
      const ComputedStyle& new_style,
      const ComputedStyle* parent_style) const;

  bool SnapshotAllCompositorKeyframesIfNecessary(
      Element&,
      const ComputedStyle& base_style,
      const ComputedStyle* parent_style) const;

  template <class K>
  static Vector<double> GetComputedOffsets(const HeapVector<K>& keyframes);

  bool Affects(const PropertyHandle& property) const override {
    EnsureKeyframeGroups();
    return keyframe_groups_->Contains(property);
  }

  bool IsTransformRelatedEffect() const override;

  virtual KeyframeEffectModelBase* Clone() = 0;

  void Trace(Visitor*) override;

 protected:
  KeyframeEffectModelBase(CompositeOperation composite,
                          scoped_refptr<TimingFunction> default_keyframe_easing)
      : interpolation_effect_(new InterpolationEffect),
        last_iteration_(0),
        last_fraction_(std::numeric_limits<double>::quiet_NaN()),
        last_iteration_duration_(AnimationTimeDelta()),
        composite_(composite),
        default_keyframe_easing_(std::move(default_keyframe_easing)),
        has_synthetic_keyframes_(false),
        needs_compositor_keyframes_snapshot_(true) {}

  // Lazily computes the groups of property-specific keyframes.
  void EnsureKeyframeGroups() const;
  void EnsureInterpolationEffectPopulated() const;

  using ShouldSnapshotPropertyCallback =
      std::function<bool(const PropertyHandle&)>;
  using ShouldSnapshotKeyframeCallback =
      std::function<bool(const PropertySpecificKeyframe&)>;

  bool SnapshotCompositableProperties(
      Element& element,
      const ComputedStyle& computed_style,
      const ComputedStyle* parent_style,
      ShouldSnapshotPropertyCallback should_process_property_callback,
      ShouldSnapshotKeyframeCallback should_process_keyframe_callback) const;

  bool SnapshotCompositorKeyFrames(
      const PropertyHandle& property,
      Element& element,
      const ComputedStyle& computed_style,
      const ComputedStyle* parent_style,
      ShouldSnapshotPropertyCallback should_process_property_callback,
      ShouldSnapshotKeyframeCallback should_process_keyframe_callback) const;

  KeyframeVector keyframes_;
  // The spec describes filtering the normalized keyframes at sampling time
  // to get the 'property-specific keyframes'. For efficiency, we cache the
  // property-specific lists.
  mutable Member<KeyframeGroupMap> keyframe_groups_;
  mutable Member<InterpolationEffect> interpolation_effect_;
  mutable int last_iteration_;
  mutable double last_fraction_;
  mutable AnimationTimeDelta last_iteration_duration_;
  CompositeOperation composite_;
  scoped_refptr<TimingFunction> default_keyframe_easing_;

  mutable bool has_synthetic_keyframes_;
  mutable bool needs_compositor_keyframes_snapshot_;

  friend class KeyframeEffectModelTest;
};

// Time independent representation of an Animation's keyframes.
template <class K>
class KeyframeEffectModel final : public KeyframeEffectModelBase {
 public:
  using KeyframeVector = HeapVector<Member<K>>;
  static KeyframeEffectModel<K>* Create(
      const KeyframeVector& keyframes,
      CompositeOperation composite = kCompositeReplace,
      scoped_refptr<TimingFunction> default_keyframe_easing = nullptr) {
    return MakeGarbageCollected<KeyframeEffectModel<K>>(
        keyframes, composite, std::move(default_keyframe_easing));
  }

  KeyframeEffectModel(const KeyframeVector& keyframes,
                      CompositeOperation composite,
                      scoped_refptr<TimingFunction> default_keyframe_easing)
      : KeyframeEffectModelBase(composite, std::move(default_keyframe_easing)) {
    keyframes_.AppendVector(keyframes);
  }

  KeyframeEffectModelBase* Clone() override {
    KeyframeVector keyframes;
    for (const auto& keyframe : GetFrames()) {
      Keyframe* new_keyframe = keyframe->Clone();
      keyframes.push_back(static_cast<K*>(new_keyframe));
    }
    return Create(keyframes, composite_, default_keyframe_easing_);
  }

 private:
  bool IsStringKeyframeEffectModel() const override { return false; }
  bool IsTransitionKeyframeEffectModel() const override { return false; }
};

using KeyframeVector = KeyframeEffectModelBase::KeyframeVector;
using PropertySpecificKeyframeVector =
    KeyframeEffectModelBase::PropertySpecificKeyframeVector;

using StringKeyframeEffectModel = KeyframeEffectModel<StringKeyframe>;
using StringKeyframeVector = StringKeyframeEffectModel::KeyframeVector;
using StringPropertySpecificKeyframeVector =
    StringKeyframeEffectModel::PropertySpecificKeyframeVector;

using TransitionKeyframeEffectModel = KeyframeEffectModel<TransitionKeyframe>;
using TransitionKeyframeVector = TransitionKeyframeEffectModel::KeyframeVector;
using TransitionPropertySpecificKeyframeVector =
    TransitionKeyframeEffectModel::PropertySpecificKeyframeVector;

DEFINE_TYPE_CASTS(KeyframeEffectModelBase,
                  EffectModel,
                  value,
                  value->IsKeyframeEffectModel(),
                  value.IsKeyframeEffectModel());
DEFINE_TYPE_CASTS(StringKeyframeEffectModel,
                  KeyframeEffectModelBase,
                  value,
                  value->IsStringKeyframeEffectModel(),
                  value.IsStringKeyframeEffectModel());
DEFINE_TYPE_CASTS(TransitionKeyframeEffectModel,
                  KeyframeEffectModelBase,
                  value,
                  value->IsTransitionKeyframeEffectModel(),
                  value.IsTransitionKeyframeEffectModel());

inline const StringKeyframeEffectModel* ToStringKeyframeEffectModel(
    const EffectModel* base) {
  return ToStringKeyframeEffectModel(ToKeyframeEffectModelBase(base));
}

inline StringKeyframeEffectModel* ToStringKeyframeEffectModel(
    EffectModel* base) {
  return ToStringKeyframeEffectModel(ToKeyframeEffectModelBase(base));
}

inline TransitionKeyframeEffectModel* ToTransitionKeyframeEffectModel(
    EffectModel* base) {
  return ToTransitionKeyframeEffectModel(ToKeyframeEffectModelBase(base));
}

template <>
inline bool KeyframeEffectModel<StringKeyframe>::IsStringKeyframeEffectModel()
    const {
  return true;
}

template <>
inline bool KeyframeEffectModel<
    TransitionKeyframe>::IsTransitionKeyframeEffectModel() const {
  return true;
}

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_KEYFRAME_EFFECT_MODEL_H_
