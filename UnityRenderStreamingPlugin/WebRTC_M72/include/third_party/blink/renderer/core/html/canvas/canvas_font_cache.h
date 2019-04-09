// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CANVAS_CANVAS_FONT_CACHE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CANVAS_CANVAS_FONT_CACHE_H_

#include <memory>
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/css/css_property_value_set.h"
#include "third_party/blink/renderer/platform/fonts/font.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/scheduler/public/thread.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"
#include "third_party/blink/renderer/platform/wtf/linked_hash_set.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class ComputedStyle;
class Document;
class FontCachePurgePreventer;

class CORE_EXPORT CanvasFontCache final
    : public GarbageCollectedFinalized<CanvasFontCache>,
      public Thread::TaskObserver {
  USING_PRE_FINALIZER(CanvasFontCache, Dispose);

 public:
  static CanvasFontCache* Create(Document& document) {
    return MakeGarbageCollected<CanvasFontCache>(document);
  }

  explicit CanvasFontCache(Document&);

  MutableCSSPropertyValueSet* ParseFont(const String&);
  void PruneAll();
  unsigned size();

  virtual void Trace(blink::Visitor*);

  static unsigned MaxFonts();
  unsigned HardMaxFonts();

  void WillUseCurrentFont() { SchedulePruningIfNeeded(); }
  bool GetFontUsingDefaultStyle(const String&, Font&);

  // TaskObserver implementation
  void DidProcessTask(const base::PendingTask&) override;
  void WillProcessTask(const base::PendingTask&) override {}

  // For testing
  bool IsInCache(const String&);

  ~CanvasFontCache() override;

 private:
  void Dispose();
  void SchedulePruningIfNeeded();
  typedef HeapHashMap<String, Member<MutableCSSPropertyValueSet>>
      MutableStylePropertyMap;

  HashMap<String, Font> fonts_resolved_using_default_style_;
  MutableStylePropertyMap fetched_fonts_;
  LinkedHashSet<String> font_lru_list_;
  std::unique_ptr<FontCachePurgePreventer> main_cache_purge_preventer_;
  Member<Document> document_;
  scoped_refptr<ComputedStyle> default_font_style_;
  bool pruning_scheduled_;
};

}  // namespace blink

#endif
