/*
 * Copyright (C) 2001 Peter Kelly (pmk@post.com)
 * Copyright (C) 2001 Tobias Anton (anton@stud.fbi.fh-darmstadt.de)
 * Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights
 * reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EVENTS_KEYBOARD_EVENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EVENTS_KEYBOARD_EVENT_H_

#include <memory>
#include "third_party/blink/public/platform/web_keyboard_event.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/events/keyboard_event_init.h"
#include "third_party/blink/renderer/core/events/ui_event_with_key_state.h"

namespace blink {

class CORE_EXPORT KeyboardEvent final : public UIEventWithKeyState {
  DEFINE_WRAPPERTYPEINFO();

 public:
  enum KeyLocationCode {
    kDomKeyLocationStandard = 0x00,
    kDomKeyLocationLeft = 0x01,
    kDomKeyLocationRight = 0x02,
    kDomKeyLocationNumpad = 0x03
  };

  static KeyboardEvent* Create() {
    return MakeGarbageCollected<KeyboardEvent>();
  }

  static KeyboardEvent* Create(const WebKeyboardEvent& web_event,
                               LocalDOMWindow* dom_window) {
    return MakeGarbageCollected<KeyboardEvent>(web_event, dom_window);
  }

  static KeyboardEvent* Create(ScriptState*,
                               const AtomicString& type,
                               const KeyboardEventInit*);

  KeyboardEvent();
  KeyboardEvent(const WebKeyboardEvent&, LocalDOMWindow*);
  KeyboardEvent(const AtomicString&, const KeyboardEventInit*);
  ~KeyboardEvent() override;

  void initKeyboardEvent(ScriptState*,
                         const AtomicString& type,
                         bool bubbles,
                         bool cancelable,
                         AbstractView*,
                         const String& key_identifier,
                         unsigned location,
                         bool ctrl_key,
                         bool alt_key,
                         bool shift_key,
                         bool meta_key);

  const String& code() const { return code_; }
  const String& key() const { return key_; }

  unsigned location() const { return location_; }

  const WebKeyboardEvent* KeyEvent() const { return key_event_.get(); }

  int keyCode()
      const;  // key code for keydown and keyup, character for keypress
  int charCode() const;  // character code for keypress, 0 for keydown and keyup
  bool repeat() const { return GetModifiers() & WebInputEvent::kIsAutoRepeat; }

  const AtomicString& InterfaceName() const override;
  bool IsKeyboardEvent() const override;
  unsigned which() const override;
  bool isComposing() const { return is_composing_; }

  void Trace(blink::Visitor*) override;

 private:
  void InitLocationModifiers(unsigned location);

  std::unique_ptr<WebKeyboardEvent> key_event_;
  String code_;
  String key_;
  unsigned location_;
  bool is_composing_ = false;
  unsigned char_code_ = 0;
  unsigned key_code_ = 0;
};

DEFINE_EVENT_TYPE_CASTS(KeyboardEvent);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EVENTS_KEYBOARD_EVENT_H_
