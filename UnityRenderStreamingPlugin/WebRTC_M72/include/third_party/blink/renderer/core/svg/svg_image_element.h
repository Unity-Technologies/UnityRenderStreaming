/*
 * Copyright (C) 2004, 2005, 2006, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
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
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_IMAGE_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_IMAGE_ELEMENT_H_

#include "third_party/blink/renderer/bindings/core/v8/active_script_wrappable.h"
#include "third_party/blink/renderer/core/html/canvas/image_element_base.h"
#include "third_party/blink/renderer/core/svg/svg_animated_length.h"
#include "third_party/blink/renderer/core/svg/svg_animated_preserve_aspect_ratio.h"
#include "third_party/blink/renderer/core/svg/svg_graphics_element.h"
#include "third_party/blink/renderer/core/svg/svg_image_loader.h"
#include "third_party/blink/renderer/core/svg/svg_uri_reference.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class CORE_EXPORT SVGImageElement final
    : public SVGGraphicsElement,
      public ImageElementBase,
      public SVGURIReference,
      public ActiveScriptWrappable<SVGImageElement> {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(SVGImageElement);

 public:
  DECLARE_NODE_FACTORY(SVGImageElement);
  void Trace(blink::Visitor*) override;

  bool CurrentFrameHasSingleSecurityOrigin() const;

  SVGAnimatedLength* x() const { return x_.Get(); }
  SVGAnimatedLength* y() const { return y_.Get(); }
  SVGAnimatedLength* width() const { return width_.Get(); }
  SVGAnimatedLength* height() const { return height_.Get(); }
  SVGAnimatedPreserveAspectRatio* preserveAspectRatio() {
    return preserve_aspect_ratio_.Get();
  }

  IntSize GetOverriddenIntrinsicSize() const {
    return overridden_intrinsic_size_;
  }

  bool HasPendingActivity() const final {
    return GetImageLoader().HasPendingActivity();
  }

  ScriptPromise decode(ScriptState*, ExceptionState&);

  // Exposed for testing.
  ImageResourceContent* CachedImage() const {
    return GetImageLoader().GetContent();
  }

  bool IsDefaultIntrinsicSize() const {
    return is_default_overridden_intrinsic_size_;
  }

  void SetImageForTest(ImageResourceContent* content) {
    GetImageLoader().SetImageForTest(content);
  }

 private:
  explicit SVGImageElement(Document&);

  bool IsStructurallyExternal() const override {
    return !HrefString().IsNull();
  }

  void CollectStyleForPresentationAttribute(
      const QualifiedName&,
      const AtomicString&,
      MutableCSSPropertyValueSet*) override;

  void SvgAttributeChanged(const QualifiedName&) override;
  void ParseAttribute(const AttributeModificationParams&) override;

  void AttachLayoutTree(AttachContext&) override;
  InsertionNotificationRequest InsertedInto(ContainerNode&) override;

  LayoutObject* CreateLayoutObject(const ComputedStyle&) override;

  const AtomicString ImageSourceURL() const override;

  bool HaveLoadedRequiredResources() override;

  bool SelfHasRelativeLengths() const override;
  void DidMoveToNewDocument(Document& old_document) override;
  SVGImageLoader& GetImageLoader() const override { return *image_loader_; }

  IntSize overridden_intrinsic_size_;
  bool is_default_overridden_intrinsic_size_;

  Member<SVGAnimatedLength> x_;
  Member<SVGAnimatedLength> y_;
  Member<SVGAnimatedLength> width_;
  Member<SVGAnimatedLength> height_;
  Member<SVGAnimatedPreserveAspectRatio> preserve_aspect_ratio_;

  Member<SVGImageLoader> image_loader_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_IMAGE_ELEMENT_H_
