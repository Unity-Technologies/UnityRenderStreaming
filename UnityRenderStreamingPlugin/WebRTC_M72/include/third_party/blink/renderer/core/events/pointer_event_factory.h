// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EVENTS_POINTER_EVENT_FACTORY_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EVENTS_POINTER_EVENT_FACTORY_H_

#include "third_party/blink/public/platform/web_pointer_event.h"
#include "third_party/blink/public/platform/web_pointer_properties.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/events/pointer_event.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"

namespace blink {

// Helper class for tracking the pointer ids for each type of PointerEvents.
// Using id re-mapping at this layer, this class makes sure that regardless of
// the domain of the id (i.e. in touch or pen) the corresponding pointer event
// will have a unique id amongst all pointer events as per pointer events'
// specification. This class intended to behave the same as existing browsers as
// much as possible for compatibility reasons. Particularly it behaves very
// similar to MS Edge to have pointer event ids generated by mouse always equal
// 1 and those that are generated by touch and pen will have increasing ids from
// 2.
class CORE_EXPORT PointerEventFactory {
  DISALLOW_NEW();

 public:
  PointerEventFactory();
  ~PointerEventFactory();

  PointerEvent* Create(const WebPointerEvent& web_pointer_event,
                       const Vector<WebPointerEvent>& coalesced_events,
                       const Vector<WebPointerEvent>& predicted_events,
                       LocalDOMWindow* view);

  PointerEvent* CreatePointerCancelEvent(
      const int pointer_id,
      TimeTicks platfrom_time_stamp);

  // For creating raw move events in chorded button case.
  PointerEvent* CreatePointerRawMoveEvent(PointerEvent*);

  // For creating capture events (i.e got/lostpointercapture).
  PointerEvent* CreatePointerCaptureEvent(PointerEvent*, const AtomicString&);

  // For creating boundary events (i.e pointerout/leave/over/enter).
  PointerEvent* CreatePointerBoundaryEvent(PointerEvent*,
                                           const AtomicString&,
                                           EventTarget*);

  // Clear all the existing ids.
  void Clear();

  // When a particular pointerId is removed, the id is considered free even
  // though there might have been other PointerEvents that were generated with
  // the same id before.
  bool Remove(const int);

  // Returns all ids of pointers that are not hovering.
  Vector<int> GetPointerIdsOfNonHoveringPointers() const;

  // Returns whether a pointer id exists and active.
  bool IsActive(const int) const;

  // Returns whether a pointer id exists and has at least one pressed button.
  bool IsActiveButtonsState(const int) const;

  // Returns the id of the pointer event corresponding to the given pointer
  // properties if exists otherwise s_invalidId.
  int GetPointerEventId(const WebPointerProperties&) const;

  // Returns pointerType of for the given pointerId if such id is active.
  // Otherwise it returns WebPointerProperties::PointerType::Unknown.
  WebPointerProperties::PointerType GetPointerType(int pointer_id) const;

  // Returns whether a WebPoinerProperties is primary pointer.
  bool IsPrimary(const WebPointerProperties&) const;

  static const int kMouseId;

  // Removes pointer_id from the map.
  void RemoveLastPosition(const int pointer_id);

  // Returns last_position of for the given pointerId if such id is active.
  // Otherwise it returns the PositionInScreen of the given events, so we will
  // get movement = 0 when there is no last position.
  FloatPoint GetLastPointerPosition(int pointer_id,
                                    const WebPointerProperties& event) const;

 private:
  typedef WTF::UnsignedWithZeroKeyHashTraits<int> UnsignedHash;
  typedef struct IncomingId : public std::pair<int, int> {
    IncomingId() = default;
    IncomingId(WebPointerProperties::PointerType pointer_type, int raw_id)
        : std::pair<int, int>(static_cast<int>(pointer_type), raw_id) {}
    int PointerTypeInt() const { return first; }
    WebPointerProperties::PointerType GetPointerType() const {
      return static_cast<WebPointerProperties::PointerType>(first);
    }
    int RawId() const { return second; }
  } IncomingId;
  typedef struct PointerAttributes {
    IncomingId incoming_id;
    bool is_active_buttons;
    bool hovering;
    PointerAttributes()
        : incoming_id(), is_active_buttons(false), hovering(true) {}
    PointerAttributes(IncomingId incoming_id,
                      bool is_active_buttons,
                      bool hovering)
        : incoming_id(incoming_id),
          is_active_buttons(is_active_buttons),
          hovering(hovering) {}
  } PointerAttributes;

  int AddIdAndActiveButtons(const IncomingId,
                            bool is_active_buttons,
                            bool hovering);
  bool IsPrimary(const int) const;
  PointerEventInit* ConvertIdTypeButtonsEvent(const WebPointerEvent&);
  void SetEventSpecificFields(PointerEventInit*, const AtomicString& type);

  // Creates pointerevents like boundary and capture events from another
  // pointerevent (i.e. up/down/move events).
  PointerEvent* CreatePointerEventFrom(PointerEvent*,
                                       const AtomicString&,
                                       EventTarget*);

  HeapVector<Member<PointerEvent>> CreateEventSequence(
      const WebPointerEvent& web_pointer_event,
      const PointerEventInit* pointer_event_init,
      const Vector<WebPointerEvent>& event_list,
      LocalDOMWindow* view);

  void SetLastPosition(int pointer_id, const WebPointerProperties& event);

  static const int kInvalidId;

  int current_id_;
  HashMap<IncomingId,
          int,
          WTF::PairHash<int, int>,
          WTF::PairHashTraits<UnsignedHash, UnsignedHash>>
      pointer_incoming_id_mapping_;
  HashMap<int, PointerAttributes, WTF::IntHash<int>, UnsignedHash>
      pointer_id_mapping_;
  int primary_id_[static_cast<int>(
                      WebPointerProperties::PointerType::kLastEntry) +
                  1];
  int id_count_[static_cast<int>(
                    WebPointerProperties::PointerType::kLastEntry) +
                1];

  HashMap<int, FloatPoint, WTF::IntHash<int>, UnsignedHash>
      pointer_id_last_position_mapping_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EVENTS_POINTER_EVENT_FACTORY_H_
