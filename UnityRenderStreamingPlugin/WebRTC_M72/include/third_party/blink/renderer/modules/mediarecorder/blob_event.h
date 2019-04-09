// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIARECORDER_BLOB_EVENT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIARECORDER_BLOB_EVENT_H_

#include "third_party/blink/renderer/core/dom/dom_high_res_time_stamp.h"
#include "third_party/blink/renderer/core/fileapi/blob.h"
#include "third_party/blink/renderer/modules/event_modules.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"

namespace blink {

class Blob;
class BlobEventInit;

class MODULES_EXPORT BlobEvent final : public Event {
  DEFINE_WRAPPERTYPEINFO();

 public:
  ~BlobEvent() override = default;

  static BlobEvent* Create(const AtomicString& type,
                           const BlobEventInit* initializer);
  static BlobEvent* Create(const AtomicString& type,
                           Blob* blob,
                           double timecode);

  Blob* data() const { return blob_.Get(); }
  DOMHighResTimeStamp timecode() const { return timecode_; }

  // Event
  const AtomicString& InterfaceName() const final;

  void Trace(blink::Visitor* visitor) override;

 private:
  BlobEvent(const AtomicString& type, const BlobEventInit* initializer);
  BlobEvent(const AtomicString& type, Blob* blob, double timecode);

  Member<Blob> blob_;
  DOMHighResTimeStamp timecode_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIARECORDER_BLOB_EVENT_H_
