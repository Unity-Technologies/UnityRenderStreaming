// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_SCRIPT_RUN_ITERATOR_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_SCRIPT_RUN_ITERATOR_H_

#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/deque.h"
#include "third_party/blink/renderer/platform/wtf/dtoa/utils.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

#include <unicode/uchar.h>
#include <unicode/uscript.h>

namespace blink {

class ScriptData;

class PLATFORM_EXPORT ScriptRunIterator {
  USING_FAST_MALLOC(ScriptRunIterator);
  WTF_MAKE_NONCOPYABLE(ScriptRunIterator);

 public:
  ScriptRunIterator(const UChar* text, wtf_size_t length);

  // This maintains a reference to data. It must exist for the lifetime of
  // this object. Typically data is a singleton that exists for the life of
  // the process.
  ScriptRunIterator(const UChar* text, wtf_size_t length, const ScriptData*);

  bool Consume(unsigned& limit, UScriptCode&);

  static constexpr int kMaxScriptCount = 20;
  using UScriptCodeList = Vector<UScriptCode, kMaxScriptCount>;

 private:
  struct BracketRec {
    DISALLOW_NEW();
    UChar32 ch;
    UScriptCode script;
  };
  void OpenBracket(UChar32);
  void CloseBracket(UChar32);
  bool MergeSets();
  void FixupStack(UScriptCode resolved_script);
  bool Fetch(wtf_size_t* pos, UChar32*);

  UScriptCode ResolveCurrentScript() const;

  const UChar* text_;
  const wtf_size_t length_;

  Deque<BracketRec> brackets_;
  wtf_size_t brackets_fixup_depth_;
  // Limit max brackets so that the bracket tracking buffer does not grow
  // excessively large when processing long runs of text.
  static const int kMaxBrackets = 32;

  UScriptCodeList current_set_;
  // Because next_set_ and ahead_set_ are swapped as we consume characters, and
  // swapping inlined vector is not cheap, next_set_ and ahead_set_ are
  // pointers.
  std::unique_ptr<UScriptCodeList> next_set_;
  std::unique_ptr<UScriptCodeList> ahead_set_;

  UChar32 ahead_character_;
  wtf_size_t ahead_pos_;

  UScriptCode common_preferred_;

  const ScriptData* script_data_;
};

// ScriptData is a wrapper which returns a set of scripts for a particular
// character retrieved from the character's primary script and script
// extensions, as per ICU / Unicode data. ScriptData maintains a certain
// priority order of the returned values, which are essential for mergeSets
// method to work correctly.
class PLATFORM_EXPORT ScriptData {
  USING_FAST_MALLOC(ScriptData);
  WTF_MAKE_NONCOPYABLE(ScriptData);

 protected:
  ScriptData() = default;

 public:
  virtual ~ScriptData();

  enum PairedBracketType {
    kBracketTypeNone,
    kBracketTypeOpen,
    kBracketTypeClose,
    kBracketTypeCount
  };

  static constexpr int kMaxScriptCount = ScriptRunIterator::kMaxScriptCount;
  using UScriptCodeList = ScriptRunIterator::UScriptCodeList;

  virtual void GetScripts(UChar32, UScriptCodeList& dst) const = 0;

  virtual UChar32 GetPairedBracket(UChar32) const = 0;

  virtual PairedBracketType GetPairedBracketType(UChar32) const = 0;
};

class PLATFORM_EXPORT ICUScriptData : public ScriptData {
 public:
  ~ICUScriptData() override = default;

  static const ICUScriptData* Instance();

  void GetScripts(UChar32, UScriptCodeList& dst) const override;

  UChar32 GetPairedBracket(UChar32) const override;

  PairedBracketType GetPairedBracketType(UChar32) const override;
};
}  // namespace blink

#endif
