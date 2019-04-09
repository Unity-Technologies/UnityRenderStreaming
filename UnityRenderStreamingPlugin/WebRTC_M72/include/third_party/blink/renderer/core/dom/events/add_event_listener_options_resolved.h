// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_DOM_EVENTS_ADD_EVENT_LISTENER_OPTIONS_RESOLVED_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_DOM_EVENTS_ADD_EVENT_LISTENER_OPTIONS_RESOLVED_H_

#include "third_party/blink/renderer/core/dom/events/add_event_listener_options.h"

namespace blink {

// AddEventListenerOptionsResolved class represents resolved event listener
// options. An application requests AddEventListenerOptions and the user
// agent may change ('resolve') these settings (based on settings or policies)
// and the result and the reasons why changes occurred are stored in this class.
class CORE_EXPORT AddEventListenerOptionsResolved
    : public AddEventListenerOptions {
 public:
  static AddEventListenerOptionsResolved* Create() {
    return new AddEventListenerOptionsResolved();
  }
  static AddEventListenerOptionsResolved* Create(
      const AddEventListenerOptions* options) {
    return new AddEventListenerOptionsResolved(options);
  }
  ~AddEventListenerOptionsResolved() override;

  void SetPassiveForcedForDocumentTarget(bool forced) {
    passive_forced_for_document_target_ = forced;
  }
  bool PassiveForcedForDocumentTarget() const {
    return passive_forced_for_document_target_;
  }

  // Set whether passive was specified when the options were
  // created by callee.
  void SetPassiveSpecified(bool specified) { passive_specified_ = specified; }
  bool PassiveSpecified() const { return passive_specified_; }

  void Trace(blink::Visitor*) override;

 private:
  AddEventListenerOptionsResolved();
  AddEventListenerOptionsResolved(const AddEventListenerOptions*);

  bool passive_forced_for_document_target_;
  bool passive_specified_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_DOM_EVENTS_ADD_EVENT_LISTENER_OPTIONS_RESOLVED_H_
