/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 * Copyright (C) 2011, 2012, 2013 Apple Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_TRACK_TEXT_TRACK_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_TRACK_TEXT_TRACK_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/events/event_target.h"
#include "third_party/blink/renderer/core/html/track/track_base.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_member.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class CueTimeline;
class ExceptionState;
class HTMLMediaElement;
class TextTrack;
class TextTrackCue;
class TextTrackCueList;
class TextTrackList;

class CORE_EXPORT TextTrack : public EventTargetWithInlineData,
                              public TrackBase {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(TextTrack);

 public:
  static TextTrack* Create(const AtomicString& kind,
                           const AtomicString& label,
                           const AtomicString& language) {
    return new TextTrack(kind, label, language, g_empty_atom, kAddTrack);
  }
  ~TextTrack() override;

  virtual void SetTrackList(TextTrackList*);
  TextTrackList* TrackList() { return track_list_; }

  bool IsVisualKind() const;

  static const AtomicString& SubtitlesKeyword();
  static const AtomicString& CaptionsKeyword();
  static const AtomicString& DescriptionsKeyword();
  static const AtomicString& ChaptersKeyword();
  static const AtomicString& MetadataKeyword();
  static bool IsValidKindKeyword(const String&);

  static const AtomicString& DisabledKeyword();
  static const AtomicString& HiddenKeyword();
  static const AtomicString& ShowingKeyword();

  void SetKind(const AtomicString& kind) { kind_ = kind; }
  void SetLabel(const AtomicString& label) { label_ = label; }
  void SetLanguage(const AtomicString& language) { language_ = language; }
  void SetId(const String& id) { id_ = id; }

  AtomicString mode() const { return mode_; }
  virtual void setMode(const AtomicString&);

  enum ReadinessState {
    kNotLoaded = 0,
    kLoading = 1,
    kLoaded = 2,
    kFailedToLoad = 3
  };
  ReadinessState GetReadinessState() const { return readiness_state_; }
  void SetReadinessState(ReadinessState state) { readiness_state_ = state; }

  TextTrackCueList* cues();
  TextTrackCueList* activeCues();

  HTMLMediaElement* MediaElement() const;
  Node* Owner() const;

  void addCue(TextTrackCue*);
  void removeCue(TextTrackCue*, ExceptionState&);

  void CueWillChange(TextTrackCue*);
  void CueDidChange(TextTrackCue*, bool update_cue_index);

  DEFINE_ATTRIBUTE_EVENT_LISTENER(cuechange, kCuechange);

  enum TextTrackType { kTrackElement, kAddTrack, kInBand };
  TextTrackType TrackType() const { return track_type_; }

  int TrackIndex();
  void InvalidateTrackIndex();

  bool IsRendered() const;
  bool CanBeRendered() const;
  int TrackIndexRelativeToRenderedTracks();

  bool HasBeenConfigured() const { return has_been_configured_; }
  void SetHasBeenConfigured(bool flag) { has_been_configured_ = flag; }

  virtual bool IsDefault() const { return false; }

  void RemoveAllCues();

  // EventTarget methods
  const AtomicString& InterfaceName() const override;
  ExecutionContext* GetExecutionContext() const override;

  void Trace(blink::Visitor*) override;

 protected:
  TextTrack(const AtomicString& kind,
            const AtomicString& label,
            const AtomicString& language,
            const AtomicString& id,
            TextTrackType);

  void AddListOfCues(HeapVector<Member<TextTrackCue>>&);

 private:
  CueTimeline* GetCueTimeline() const;

  TextTrackCueList* EnsureTextTrackCueList();
  TraceWrapperMember<TextTrackCueList> cues_;
  Member<TextTrackCueList> active_cues_;

  Member<TextTrackList> track_list_;
  AtomicString mode_;
  TextTrackType track_type_;
  ReadinessState readiness_state_;
  int track_index_;
  int rendered_track_index_;
  bool has_been_configured_;
};

DEFINE_TRACK_TYPE_CASTS(TextTrack, WebMediaPlayer::kTextTrack);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_TRACK_TEXT_TRACK_H_
