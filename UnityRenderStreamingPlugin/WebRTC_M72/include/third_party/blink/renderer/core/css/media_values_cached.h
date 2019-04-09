// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_MEDIA_VALUES_CACHED_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_MEDIA_VALUES_CACHED_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/css/media_values.h"
#include "third_party/blink/renderer/platform/cross_thread_copier.h"

namespace blink {

class CORE_EXPORT MediaValuesCached final : public MediaValues {
 public:
  struct CORE_EXPORT MediaValuesCachedData final {
    DISALLOW_NEW();
    // Members variables must be thread safe, since they're copied to the parser
    // thread
    double viewport_width;
    double viewport_height;
    int device_width;
    int device_height;
    float device_pixel_ratio;
    int color_bits_per_component;
    int monochrome_bits_per_component;
    PointerType primary_pointer_type;
    int available_pointer_types;
    HoverType primary_hover_type;
    int available_hover_types;
    int default_font_size;
    bool three_d_enabled;
    bool immersive_mode;
    bool strict_mode;
    String media_type;
    WebDisplayMode display_mode;
    DisplayShape display_shape;
    ColorSpaceGamut color_gamut;

    MediaValuesCachedData();
    explicit MediaValuesCachedData(Document&);

    MediaValuesCachedData DeepCopy() const {
      MediaValuesCachedData data;
      data.viewport_width = viewport_width;
      data.viewport_height = viewport_height;
      data.device_width = device_width;
      data.device_height = device_height;
      data.device_pixel_ratio = device_pixel_ratio;
      data.color_bits_per_component = color_bits_per_component;
      data.monochrome_bits_per_component = monochrome_bits_per_component;
      data.primary_pointer_type = primary_pointer_type;
      data.available_pointer_types = available_pointer_types;
      data.primary_hover_type = primary_hover_type;
      data.available_hover_types = available_hover_types;
      data.default_font_size = default_font_size;
      data.three_d_enabled = three_d_enabled;
      data.immersive_mode = immersive_mode;
      data.strict_mode = strict_mode;
      data.media_type = media_type.IsolatedCopy();
      data.display_mode = display_mode;
      data.display_shape = display_shape;
      data.color_gamut = color_gamut;
      return data;
    }
  };

  static MediaValuesCached* Create();
  static MediaValuesCached* Create(const MediaValuesCachedData&);

  MediaValuesCached();
  MediaValuesCached(LocalFrame*);
  MediaValuesCached(const MediaValuesCachedData&);

  MediaValues* Copy() const override;
  bool ComputeLength(double value,
                     CSSPrimitiveValue::UnitType,
                     int& result) const override;
  bool ComputeLength(double value,
                     CSSPrimitiveValue::UnitType,
                     double& result) const override;

  double ViewportWidth() const override;
  double ViewportHeight() const override;
  int DeviceWidth() const override;
  int DeviceHeight() const override;
  float DevicePixelRatio() const override;
  int ColorBitsPerComponent() const override;
  int MonochromeBitsPerComponent() const override;
  PointerType PrimaryPointerType() const override;
  int AvailablePointerTypes() const override;
  HoverType PrimaryHoverType() const override;
  int AvailableHoverTypes() const override;
  bool ThreeDEnabled() const override;
  bool InImmersiveMode() const override;
  bool StrictMode() const override;
  Document* GetDocument() const override;
  bool HasValues() const override;
  const String MediaType() const override;
  WebDisplayMode DisplayMode() const override;
  DisplayShape GetDisplayShape() const override;
  ColorSpaceGamut ColorGamut() const override;

  void OverrideViewportDimensions(double width, double height) override;

 protected:
  MediaValuesCachedData data_;
};

template <>
struct CrossThreadCopier<MediaValuesCached::MediaValuesCachedData> {
  typedef MediaValuesCached::MediaValuesCachedData Type;
  static Type Copy(const MediaValuesCached::MediaValuesCachedData& data) {
    return data.DeepCopy();
  }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_MEDIA_VALUES_CACHED_H_
