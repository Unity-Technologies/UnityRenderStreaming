// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_WEBRTC_OVERRIDES_INIT_WEBRTC_H_
#define THIRD_PARTY_WEBRTC_OVERRIDES_INIT_WEBRTC_H_

// Initialize WebRTC. Call this explicitly to initialize WebRTC module
// (before initializing the sandbox in Chrome) and hook up Chrome+WebRTC
// integration such as common logging and tracing.
bool InitializeWebRtcModule();

#endif // THIRD_PARTY_WEBRTC_OVERRIDES_INIT_WEBRTC_H_
