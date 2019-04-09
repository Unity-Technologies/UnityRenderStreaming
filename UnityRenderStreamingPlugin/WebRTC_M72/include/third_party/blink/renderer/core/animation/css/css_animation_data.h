// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_CSS_CSS_ANIMATION_DATA_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_CSS_CSS_ANIMATION_DATA_H_

#include <memory>

#include "base/memory/ptr_util.h"
#include "third_party/blink/renderer/core/animation/css/css_timing_data.h"
#include "third_party/blink/renderer/core/animation/timing.h"
#include "third_party/blink/renderer/core/style/computed_style_constants.h"

namespace blink {

class CSSAnimationData final : public CSSTimingData {
 public:
  static std::unique_ptr<CSSAnimationData> Create() {
    return base::WrapUnique(new CSSAnimationData);
  }

  std::unique_ptr<CSSAnimationData> Clone() const {
    return base::WrapUnique(new CSSAnimationData(*this));
  }

  bool AnimationsMatchForStyleRecalc(const CSSAnimationData& other) const;
  bool operator==(const CSSAnimationData& other) const {
    return AnimationsMatchForStyleRecalc(other);
  }

  Timing ConvertToTiming(size_t index) const;

  const Vector<AtomicString>& NameList() const { return name_list_; }
  const Vector<double>& IterationCountList() const {
    return iteration_count_list_;
  }
  const Vector<Timing::PlaybackDirection>& DirectionList() const {
    return direction_list_;
  }
  const Vector<Timing::FillMode>& FillModeList() const {
    return fill_mode_list_;
  }
  const Vector<EAnimPlayState>& PlayStateList() const {
    return play_state_list_;
  }

  Vector<AtomicString>& NameList() { return name_list_; }
  Vector<double>& IterationCountList() { return iteration_count_list_; }
  Vector<Timing::PlaybackDirection>& DirectionList() { return direction_list_; }
  Vector<Timing::FillMode>& FillModeList() { return fill_mode_list_; }
  Vector<EAnimPlayState>& PlayStateList() { return play_state_list_; }

  static const AtomicString& InitialName();
  static Timing::PlaybackDirection InitialDirection() {
    return Timing::PlaybackDirection::NORMAL;
  }
  static Timing::FillMode InitialFillMode() { return Timing::FillMode::NONE; }
  static double InitialIterationCount() { return 1.0; }
  static EAnimPlayState InitialPlayState() { return EAnimPlayState::kPlaying; }

 private:
  CSSAnimationData();
  explicit CSSAnimationData(const CSSAnimationData&);

  Vector<AtomicString> name_list_;
  Vector<double> iteration_count_list_;
  Vector<Timing::PlaybackDirection> direction_list_;
  Vector<Timing::FillMode> fill_mode_list_;
  Vector<EAnimPlayState> play_state_list_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_CSS_CSS_ANIMATION_DATA_H_
