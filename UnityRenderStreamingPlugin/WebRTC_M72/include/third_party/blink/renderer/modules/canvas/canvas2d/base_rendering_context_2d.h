// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_CANVAS_CANVAS2D_BASE_RENDERING_CONTEXT_2D_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_CANVAS_CANVAS2D_BASE_RENDERING_CONTEXT_2D_H_

#include "third_party/blink/renderer/bindings/modules/v8/canvas_image_source.h"
#include "third_party/blink/renderer/bindings/modules/v8/string_or_canvas_gradient_or_canvas_pattern.h"
#include "third_party/blink/renderer/core/geometry/dom_matrix.h"
#include "third_party/blink/renderer/core/html/canvas/image_data.h"
#include "third_party/blink/renderer/modules/canvas/canvas2d/canvas_gradient.h"
#include "third_party/blink/renderer/modules/canvas/canvas2d/canvas_path.h"
#include "third_party/blink/renderer/modules/canvas/canvas2d/canvas_rendering_context_2d_state.h"
#include "third_party/blink/renderer/modules/canvas/canvas2d/canvas_style.h"
#include "third_party/blink/renderer/platform/graphics/canvas_heuristic_parameters.h"

namespace blink {
class CanvasImageSource;
class Color;
class Image;
class Path2D;

typedef CSSImageValueOrHTMLImageElementOrSVGImageElementOrHTMLVideoElementOrHTMLCanvasElementOrImageBitmapOrOffscreenCanvas
    CanvasImageSourceUnion;

class MODULES_EXPORT BaseRenderingContext2D : public GarbageCollectedMixin,
                                              public CanvasPath {
  WTF_MAKE_NONCOPYABLE(BaseRenderingContext2D);

 public:
  ~BaseRenderingContext2D() override;

  void Reset();

  void strokeStyle(StringOrCanvasGradientOrCanvasPattern&) const;
  void setStrokeStyle(const StringOrCanvasGradientOrCanvasPattern&);

  void fillStyle(StringOrCanvasGradientOrCanvasPattern&) const;
  void setFillStyle(const StringOrCanvasGradientOrCanvasPattern&);

  double lineWidth() const;
  void setLineWidth(double);

  String lineCap() const;
  void setLineCap(const String&);

  String lineJoin() const;
  void setLineJoin(const String&);

  double miterLimit() const;
  void setMiterLimit(double);

  const Vector<double>& getLineDash() const;
  void setLineDash(const Vector<double>&);

  double lineDashOffset() const;
  void setLineDashOffset(double);

  virtual double shadowOffsetX() const;
  virtual void setShadowOffsetX(double);

  virtual double shadowOffsetY() const;
  virtual void setShadowOffsetY(double);

  virtual double shadowBlur() const;
  virtual void setShadowBlur(double);

  String shadowColor() const;
  void setShadowColor(const String&);

  double globalAlpha() const;
  void setGlobalAlpha(double);

  String globalCompositeOperation() const;
  void setGlobalCompositeOperation(const String&);

  String filter() const;
  void setFilter(const ExecutionContext*, const String&);

  void save();
  void restore();

  void scale(double sx, double sy);
  void rotate(double angle_in_radians);
  void translate(double tx, double ty);
  void transform(double m11,
                 double m12,
                 double m21,
                 double m22,
                 double dx,
                 double dy);
  void setTransform(double m11,
                    double m12,
                    double m21,
                    double m22,
                    double dx,
                    double dy);
  void setTransform(DOMMatrix2DInit*, ExceptionState&);
  DOMMatrix* getTransform();
  void resetTransform();

  void beginPath();

  void fill(const String& winding = "nonzero");
  void fill(Path2D*, const String& winding = "nonzero");
  void stroke();
  void stroke(Path2D*);
  void clip(const String& winding = "nonzero");
  void clip(Path2D*, const String& winding = "nonzero");

  bool isPointInPath(const double x,
                     const double y,
                     const String& winding = "nonzero");
  bool isPointInPath(Path2D*,
                     const double x,
                     const double y,
                     const String& winding = "nonzero");
  bool isPointInStroke(const double x, const double y);
  bool isPointInStroke(Path2D*, const double x, const double y);

  void clearRect(double x, double y, double width, double height);
  void fillRect(double x, double y, double width, double height);
  void strokeRect(double x, double y, double width, double height);

  void drawImage(ScriptState*,
                 const CanvasImageSourceUnion&,
                 double x,
                 double y,
                 ExceptionState&);
  void drawImage(ScriptState*,
                 const CanvasImageSourceUnion&,
                 double x,
                 double y,
                 double width,
                 double height,
                 ExceptionState&);
  void drawImage(ScriptState*,
                 const CanvasImageSourceUnion&,
                 double sx,
                 double sy,
                 double sw,
                 double sh,
                 double dx,
                 double dy,
                 double dw,
                 double dh,
                 ExceptionState&);
  void drawImage(ScriptState*,
                 CanvasImageSource*,
                 double sx,
                 double sy,
                 double sw,
                 double sh,
                 double dx,
                 double dy,
                 double dw,
                 double dh,
                 ExceptionState&);

  CanvasGradient* createLinearGradient(double x0,
                                       double y0,
                                       double x1,
                                       double y1);
  CanvasGradient* createRadialGradient(double x0,
                                       double y0,
                                       double r0,
                                       double x1,
                                       double y1,
                                       double r1,
                                       ExceptionState&);
  CanvasPattern* createPattern(ScriptState*,
                               const CanvasImageSourceUnion&,
                               const String& repetition_type,
                               ExceptionState&);
  CanvasPattern* createPattern(ScriptState*,
                               CanvasImageSource*,
                               const String& repetition_type,
                               ExceptionState&);

  ImageData* createImageData(ImageData*, ExceptionState&) const;
  ImageData* createImageData(int width, int height, ExceptionState&) const;
  ImageData* createImageData(unsigned,
                             unsigned,
                             ImageDataColorSettings*,
                             ExceptionState&) const;
  ImageData* createImageData(ImageDataArray&,
                             unsigned,
                             unsigned,
                             ExceptionState&) const;
  ImageData* createImageData(ImageDataArray&,
                             unsigned,
                             unsigned,
                             ImageDataColorSettings*,
                             ExceptionState&) const;

  ImageData* getImageData(int sx, int sy, int sw, int sh, ExceptionState&);
  void putImageData(ImageData*, int dx, int dy, ExceptionState&);
  void putImageData(ImageData*,
                    int dx,
                    int dy,
                    int dirty_x,
                    int dirty_y,
                    int dirty_width,
                    int dirty_height,
                    ExceptionState&);

  bool imageSmoothingEnabled() const;
  void setImageSmoothingEnabled(bool);
  String imageSmoothingQuality() const;
  void setImageSmoothingQuality(const String&);

  virtual bool OriginClean() const = 0;
  virtual void SetOriginTainted() = 0;
  virtual bool WouldTaintOrigin(CanvasImageSource*, ExecutionContext*) = 0;

  virtual int Width() const = 0;
  virtual int Height() const = 0;

  virtual bool IsAccelerated() const {
    NOTREACHED();
    return false;
  }
  virtual bool CanCreateCanvas2dResourceProvider() const = 0;

  virtual bool ParseColorOrCurrentColor(Color&,
                                        const String& color_string) const = 0;

  virtual cc::PaintCanvas* DrawingCanvas() const = 0;
  virtual cc::PaintCanvas* ExistingDrawingCanvas() const = 0;
  virtual void DisableDeferral(DisableDeferralReason) = 0;

  virtual void DidDraw(const SkIRect& dirty_rect) = 0;

  virtual bool StateHasFilter() = 0;
  virtual sk_sp<PaintFilter> StateGetFilter() = 0;
  virtual void SnapshotStateForFilter() = 0;

  virtual void ValidateStateStack() const = 0;

  virtual bool HasAlpha() const = 0;

  virtual bool isContextLost() const = 0;

  virtual void WillDrawImage(CanvasImageSource*) const {}

  virtual String ColorSpaceAsString() const {
    return kSRGBCanvasColorSpaceName;
  }
  virtual CanvasPixelFormat PixelFormat() const {
    return kRGBA8CanvasPixelFormat;
  }

  void RestoreMatrixClipStack(cc::PaintCanvas*) const;

  String textAlign() const;
  void setTextAlign(const String&);

  String textBaseline() const;
  void setTextBaseline(const String&);

  void Trace(blink::Visitor*) override;

  enum DrawCallType {
    kStrokePath = 0,
    kFillPath,
    kDrawVectorImage,
    kDrawBitmapImage,
    kFillText,
    kStrokeText,
    kFillRect,
    kStrokeRect,
    kDrawCallTypeCount  // used to specify the size of storage arrays
  };

  enum PathFillType {
    kColorFillType,
    kLinearGradientFillType,
    kRadialGradientFillType,
    kPatternFillType,
    kPathFillTypeCount  // used to specify the size of storage arrays
  };

  struct UsageCounters {
    int num_draw_calls[kDrawCallTypeCount];  // use DrawCallType enum as index
    float bounding_box_perimeter_draw_calls[kDrawCallTypeCount];
    float bounding_box_area_draw_calls[kDrawCallTypeCount];
    float bounding_box_area_fill_type[kPathFillTypeCount];
    int num_non_convex_fill_path_calls;
    float non_convex_fill_path_area;
    int num_radial_gradients;
    int num_linear_gradients;
    int num_patterns;
    int num_draw_with_complex_clips;
    int num_blurred_shadows;
    float bounding_box_area_times_shadow_blur_squared;
    float bounding_box_perimeter_times_shadow_blur_squared;
    int num_filters;
    int num_get_image_data_calls;
    float area_get_image_data_calls;
    int num_put_image_data_calls;
    float area_put_image_data_calls;
    int num_clear_rect_calls;
    int num_draw_focus_calls;
    int num_frames_since_reset;

    UsageCounters();
  };

  const UsageCounters& GetUsage();

 protected:
  BaseRenderingContext2D();

  CanvasRenderingContext2DState& ModifiableState();
  const CanvasRenderingContext2DState& GetState() const {
    return *state_stack_.back();
  }

  bool ComputeDirtyRect(const FloatRect& local_bounds, SkIRect*);
  bool ComputeDirtyRect(const FloatRect& local_bounds,
                        const SkIRect& transformed_clip_bounds,
                        SkIRect*);

  template <typename DrawFunc, typename ContainsFunc>
  void Draw(const DrawFunc&,
            const ContainsFunc&,
            const SkRect& bounds,
            CanvasRenderingContext2DState::PaintType,
            CanvasRenderingContext2DState::ImageType =
                CanvasRenderingContext2DState::kNoImage);

  void InflateStrokeRect(FloatRect&) const;

  void UnwindStateStack();

  virtual CanvasColorParams ColorParams() const { return CanvasColorParams(); };
  virtual bool WritePixels(const SkImageInfo& orig_info,
                           const void* pixels,
                           size_t row_bytes,
                           int x,
                           int y) {
    NOTREACHED();
    return false;
  }
  virtual scoped_refptr<StaticBitmapImage> GetImage(AccelerationHint) const {
    NOTREACHED();
    return nullptr;
  }

  enum DrawType {
    kClipFill,  // Fill that is already known to cover the current clip
    kUntransformedUnclippedFill
  };

  void CheckOverdraw(const SkRect&,
                     const PaintFlags*,
                     CanvasRenderingContext2DState::ImageType,
                     DrawType);

  HeapVector<Member<CanvasRenderingContext2DState>> state_stack_;
  AntiAliasingMode clip_antialiasing_;

  mutable UsageCounters usage_counters_;

  virtual void NeedsFinalizeFrame() {}

  float GetFontBaseline(const SimpleFontData&) const;

  static const char kDefaultFont[];
  static const char kInheritDirectionString[];
  static const char kRtlDirectionString[];
  static const char kLtrDirectionString[];
  // Canvas is device independent
  static const double kCDeviceScaleFactor;

  virtual void DisableAcceleration() {}
  virtual void DidInvokeGPUReadbackInCurrentFrame() {}

  virtual bool IsPaint2D() const { return false; }
  virtual void WillOverwriteCanvas() {}

 private:
  void RealizeSaves();

  bool ShouldDrawImageAntialiased(const FloatRect& dest_rect) const;

  void DrawPathInternal(const Path&,
                        CanvasRenderingContext2DState::PaintType,
                        SkPath::FillType = SkPath::kWinding_FillType);
  void DrawImageInternal(cc::PaintCanvas*,
                         CanvasImageSource*,
                         Image*,
                         const FloatRect& src_rect,
                         const FloatRect& dst_rect,
                         const PaintFlags*);
  void ClipInternal(const Path&, const String& winding_rule_string);

  bool IsPointInPathInternal(const Path&,
                             const double x,
                             const double y,
                             const String& winding_rule_string);
  bool IsPointInStrokeInternal(const Path&, const double x, const double y);

  static bool IsFullCanvasCompositeMode(SkBlendMode);

  template <typename DrawFunc>
  void CompositedDraw(const DrawFunc&,
                      cc::PaintCanvas*,
                      CanvasRenderingContext2DState::PaintType,
                      CanvasRenderingContext2DState::ImageType);

  void ClearCanvas();
  bool RectContainsTransformedRect(const FloatRect&, const SkIRect&) const;
  // Sets the origin to be tainted by the content of the canvas, such
  // as a cross-origin image. This is as opposed to some other reason
  // such as tainting from a filter applied to the canvas.
  void SetOriginTaintedByContent();

  ImageDataColorSettings* GetColorSettingsAsImageDataColorSettings() const;

  void PutByteArray(const unsigned char* source,
                    const IntSize& source_size,
                    const IntRect& source_rect,
                    const IntPoint& dest_point);
  virtual bool IsCanvas2DBufferValid() const {
    NOTREACHED();
    return false;
  }

  int getScaledElapsedTime(int width, int height, base::TimeTicks start_time);

  bool origin_tainted_by_content_;
};

template <typename DrawFunc, typename ContainsFunc>
void BaseRenderingContext2D::Draw(
    const DrawFunc& draw_func,
    const ContainsFunc& draw_covers_clip_bounds,
    const SkRect& bounds,
    CanvasRenderingContext2DState::PaintType paint_type,
    CanvasRenderingContext2DState::ImageType image_type) {
  if (!GetState().IsTransformInvertible())
    return;

  SkIRect clip_bounds;
  if (!DrawingCanvas() || !DrawingCanvas()->getDeviceClipBounds(&clip_bounds))
    return;

  if (IsFullCanvasCompositeMode(GetState().GlobalComposite()) ||
      StateHasFilter()) {
    CompositedDraw(draw_func, DrawingCanvas(), paint_type, image_type);
    DidDraw(clip_bounds);
  } else if (GetState().GlobalComposite() == SkBlendMode::kSrc) {
    ClearCanvas();  // takes care of checkOverdraw()
    const PaintFlags* flags =
        GetState().GetFlags(paint_type, kDrawForegroundOnly, image_type);
    draw_func(DrawingCanvas(), flags);
    DidDraw(clip_bounds);
  } else {
    SkIRect dirty_rect;
    if (ComputeDirtyRect(bounds, clip_bounds, &dirty_rect)) {
      const PaintFlags* flags =
          GetState().GetFlags(paint_type, kDrawShadowAndForeground, image_type);
      if (paint_type != CanvasRenderingContext2DState::kStrokePaintType &&
          draw_covers_clip_bounds(clip_bounds))
        CheckOverdraw(bounds, flags, image_type, kClipFill);
      draw_func(DrawingCanvas(), flags);
      DidDraw(dirty_rect);
    }
  }
}

template <typename DrawFunc>
void BaseRenderingContext2D::CompositedDraw(
    const DrawFunc& draw_func,
    cc::PaintCanvas* c,
    CanvasRenderingContext2DState::PaintType paint_type,
    CanvasRenderingContext2DState::ImageType image_type) {
  sk_sp<PaintFilter> filter = StateGetFilter();
  DCHECK(IsFullCanvasCompositeMode(GetState().GlobalComposite()) || filter);
  SkMatrix ctm = c->getTotalMatrix();
  c->setMatrix(SkMatrix::I());
  PaintFlags composite_flags;
  composite_flags.setBlendMode((SkBlendMode)GetState().GlobalComposite());
  if (GetState().ShouldDrawShadows()) {
    // unroll into two independently composited passes if drawing shadows
    PaintFlags shadow_flags =
        *GetState().GetFlags(paint_type, kDrawShadowOnly, image_type);
    int save_count = c->getSaveCount();
    c->save();
    if (filter) {
      PaintFlags foreground_flags =
          *GetState().GetFlags(paint_type, kDrawForegroundOnly, image_type);
      foreground_flags.setImageFilter(sk_make_sp<ComposePaintFilter>(
          sk_make_sp<ComposePaintFilter>(foreground_flags.getImageFilter(),
                                         shadow_flags.getImageFilter()),
          filter));
      c->setMatrix(ctm);
      draw_func(c, &foreground_flags);
    } else {
      DCHECK(IsFullCanvasCompositeMode(GetState().GlobalComposite()));
      c->saveLayer(nullptr, &composite_flags);
      shadow_flags.setBlendMode(SkBlendMode::kSrcOver);
      c->setMatrix(ctm);
      draw_func(c, &shadow_flags);
    }
    c->restoreToCount(save_count);
  }

  composite_flags.setImageFilter(std::move(filter));
  c->saveLayer(nullptr, &composite_flags);
  PaintFlags foreground_flags =
      *GetState().GetFlags(paint_type, kDrawForegroundOnly, image_type);
  foreground_flags.setBlendMode(SkBlendMode::kSrcOver);
  c->setMatrix(ctm);
  draw_func(c, &foreground_flags);
  c->restore();
  c->setMatrix(ctm);
}

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_CANVAS_CANVAS2D_BASE_RENDERING_CONTEXT_2D_H_
