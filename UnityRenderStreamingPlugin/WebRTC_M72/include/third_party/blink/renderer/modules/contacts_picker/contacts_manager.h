// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_CONTACTS_PICKER_CONTACTS_MANAGER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_CONTACTS_PICKER_CONTACTS_MANAGER_H_

#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/modules/contacts_picker/contacts_select_options.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/thread_state.h"

namespace blink {

class ScriptState;

// Represents an the ContactManager, providing access to Contacts.
class ContactsManager final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  ContactsManager();
  ~ContactsManager() override;

  // Web-exposed function defined in the IDL file.
  ScriptPromise select(ScriptState* script_state,
                       ContactsSelectOptions* options);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_CONTACTS_PICKER_CONTACTS_MANAGER_H_
