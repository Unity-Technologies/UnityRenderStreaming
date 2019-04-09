// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_SCOPED_FAKE_PLUGIN_REGISTRY_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_SCOPED_FAKE_PLUGIN_REGISTRY_H_

namespace blink {

// Simulates the browser process serving a list of plugins that includes just a
// fake PDF plugin.
class ScopedFakePluginRegistry {
 public:
  ScopedFakePluginRegistry();
  ~ScopedFakePluginRegistry();
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_SCOPED_FAKE_PLUGIN_REGISTRY_H_
