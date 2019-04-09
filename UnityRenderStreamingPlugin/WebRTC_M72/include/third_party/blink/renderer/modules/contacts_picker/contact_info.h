// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_CONTACTS_PICKER_CONTACT_INFO_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_CONTACTS_PICKER_CONTACT_INFO_H_

#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/thread_state.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

// Represents an individual Contact in the Contacts Picker.
class ContactInfo final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  ContactInfo(base::Optional<Vector<String>> name,
              base::Optional<Vector<String>> email,
              base::Optional<Vector<String>> tel);
  ~ContactInfo() override;

  // Web-exposed attributes defined in the IDL file.
  const Vector<String> name(bool& is_null) const;
  const Vector<String> email(bool& is_null) const;
  const Vector<String> tel(bool& is_null) const;

 private:
  base::Optional<Vector<String>> name_;
  base::Optional<Vector<String>> email_;
  base::Optional<Vector<String>> tel_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_CONTACTS_PICKER_CONTACT_INFO_H_
