/*
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2005, 2006, 2007, 2008, 2010 Apple Inc. All rights
 * reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_STYLE_CONTENT_DATA_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_STYLE_CONTENT_DATA_H_

#include <memory>
#include <utility>

#include "third_party/blink/renderer/core/style/counter_content.h"
#include "third_party/blink/renderer/core/style/style_image.h"

namespace blink {

class ComputedStyle;
class LayoutObject;
class PseudoElement;

class ContentData : public GarbageCollectedFinalized<ContentData> {
 public:
  static ContentData* Create(StyleImage*);
  static ContentData* Create(const String&);
  static ContentData* Create(std::unique_ptr<CounterContent>);
  static ContentData* Create(QuoteType);

  virtual ~ContentData() = default;

  virtual bool IsCounter() const { return false; }
  virtual bool IsImage() const { return false; }
  virtual bool IsQuote() const { return false; }
  virtual bool IsText() const { return false; }

  virtual LayoutObject* CreateLayoutObject(PseudoElement&,
                                           ComputedStyle&) const = 0;

  virtual ContentData* Clone() const;

  ContentData* Next() const { return next_.Get(); }
  void SetNext(ContentData* next) { next_ = next; }

  virtual bool Equals(const ContentData&) const = 0;

  virtual void Trace(blink::Visitor*);

 private:
  virtual ContentData* CloneInternal() const = 0;

  Member<ContentData> next_;
};

#define DEFINE_CONTENT_DATA_TYPE_CASTS(typeName)                 \
  DEFINE_TYPE_CASTS(typeName##ContentData, ContentData, content, \
                    content->Is##typeName(), content.Is##typeName())

class ImageContentData final : public ContentData {
  friend class ContentData;

 public:
  ImageContentData(StyleImage* image) : image_(image) { DCHECK(image_); }

  const StyleImage* GetImage() const { return image_.Get(); }
  StyleImage* GetImage() { return image_.Get(); }
  void SetImage(StyleImage* image) {
    DCHECK(image);
    image_ = image;
  }

  bool IsImage() const override { return true; }
  LayoutObject* CreateLayoutObject(PseudoElement&,
                                   ComputedStyle&) const override;

  bool Equals(const ContentData& data) const override {
    if (!data.IsImage())
      return false;
    return *static_cast<const ImageContentData&>(data).GetImage() ==
           *GetImage();
  }

  void Trace(blink::Visitor*) override;

 private:
  ContentData* CloneInternal() const override {
    StyleImage* image = const_cast<StyleImage*>(this->GetImage());
    return Create(image);
  }

  Member<StyleImage> image_;
};

DEFINE_CONTENT_DATA_TYPE_CASTS(Image);

class TextContentData final : public ContentData {
  friend class ContentData;

 public:
  const String& GetText() const { return text_; }
  void SetText(const String& text) { text_ = text; }

  bool IsText() const override { return true; }
  LayoutObject* CreateLayoutObject(PseudoElement&,
                                   ComputedStyle&) const override;

  bool Equals(const ContentData& data) const override {
    if (!data.IsText())
      return false;
    return static_cast<const TextContentData&>(data).GetText() == GetText();
  }

 private:
  TextContentData(const String& text) : text_(text) {}

  ContentData* CloneInternal() const override { return Create(GetText()); }

  String text_;
};

DEFINE_CONTENT_DATA_TYPE_CASTS(Text);

class CounterContentData final : public ContentData {
  friend class ContentData;

 public:
  const CounterContent* Counter() const { return counter_.get(); }
  void SetCounter(std::unique_ptr<CounterContent> counter) {
    counter_ = std::move(counter);
  }

  bool IsCounter() const override { return true; }
  LayoutObject* CreateLayoutObject(PseudoElement&,
                                   ComputedStyle&) const override;

 private:
  CounterContentData(std::unique_ptr<CounterContent> counter)
      : counter_(std::move(counter)) {}

  ContentData* CloneInternal() const override {
    std::unique_ptr<CounterContent> counter_data =
        std::make_unique<CounterContent>(*Counter());
    return Create(std::move(counter_data));
  }

  bool Equals(const ContentData& data) const override {
    if (!data.IsCounter())
      return false;
    return *static_cast<const CounterContentData&>(data).Counter() ==
           *Counter();
  }

  std::unique_ptr<CounterContent> counter_;
};

DEFINE_CONTENT_DATA_TYPE_CASTS(Counter);

class QuoteContentData final : public ContentData {
  friend class ContentData;

 public:
  QuoteType Quote() const { return quote_; }
  void SetQuote(QuoteType quote) { quote_ = quote; }

  bool IsQuote() const override { return true; }
  LayoutObject* CreateLayoutObject(PseudoElement&,
                                   ComputedStyle&) const override;

  bool Equals(const ContentData& data) const override {
    if (!data.IsQuote())
      return false;
    return static_cast<const QuoteContentData&>(data).Quote() == Quote();
  }

 private:
  QuoteContentData(QuoteType quote) : quote_(quote) {}

  ContentData* CloneInternal() const override { return Create(Quote()); }

  QuoteType quote_;
};

DEFINE_CONTENT_DATA_TYPE_CASTS(Quote);

inline bool operator==(const ContentData& a, const ContentData& b) {
  const ContentData* ptr_a = &a;
  const ContentData* ptr_b = &b;

  while (ptr_a && ptr_b && ptr_a->Equals(*ptr_b)) {
    ptr_a = ptr_a->Next();
    ptr_b = ptr_b->Next();
  }

  return !ptr_a && !ptr_b;
}

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_STYLE_CONTENT_DATA_H_
