// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_CONTENT_SETTINGS_CLIENT_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_CONTENT_SETTINGS_CLIENT_H_

#include "base/time/time.h"
#include "third_party/blink/public/platform/web_client_hints_type.h"
#include "third_party/blink/public/platform/web_content_setting_callbacks.h"

namespace blink {

class WebSecurityOrigin;
class WebString;
class WebURL;

// This class provides the content settings information which tells
// whether each feature is allowed or not.
class WebContentSettingsClient {
 public:
  // Only used if this is a WebContentSettingsClient on a worker thread. Clones
  // this WebContentSettingsClient so it can be used by another worker thread.
  virtual std::unique_ptr<WebContentSettingsClient> Clone() { return nullptr; }

  // Controls whether access to Web Databases is allowed for this frame.
  virtual bool AllowDatabase(const WebString& name,
                             const WebString& display_name,
                             unsigned estimated_size) {
    return true;
  }

  // Controls whether access to File System is allowed for this frame.
  virtual bool RequestFileSystemAccessSync() { return true; }

  // Controls whether access to File System is allowed for this frame.
  virtual void RequestFileSystemAccessAsync(
      const WebContentSettingCallbacks& callbacks) {
    WebContentSettingCallbacks permission_callbacks(callbacks);
    permission_callbacks.DoAllow();
  }

  // Controls whether images are allowed for this frame.
  virtual bool AllowImage(bool enabled_per_settings, const WebURL& image_url) {
    return enabled_per_settings;
  }

  // Controls whether access to Indexed DB are allowed for this frame.
  virtual bool AllowIndexedDB(const WebSecurityOrigin&) { return true; }

  // Controls whether scripts are allowed to execute for this frame.
  virtual bool AllowScript(bool enabled_per_settings) {
    return enabled_per_settings;
  }

  // Controls whether scripts loaded from the given URL are allowed to execute
  // for this frame.
  virtual bool AllowScriptFromSource(bool enabled_per_settings,
                                     const WebURL& script_url) {
    return enabled_per_settings;
  }

  // Retrieves the client hints that should be attached to the request for the
  // given URL.
  virtual void GetAllowedClientHintsFromSource(const blink::WebURL& url,
                                               WebEnabledClientHints*) const {}

  // Controls whether insecure scripts are allowed to execute for this frame.
  virtual bool AllowRunningInsecureContent(bool enabled_per_settings,
                                           const WebSecurityOrigin&,
                                           const WebURL&) {
    return enabled_per_settings;
  }

  // Controls whether HTML5 Web Storage is allowed for this frame.
  // If local is true, then this is for local storage, otherwise it's for
  // session storage.
  virtual bool AllowStorage(bool local) { return true; }

  // Controls whether access to read the clipboard is allowed for this frame.
  virtual bool AllowReadFromClipboard(bool default_value) {
    return default_value;
  }

  // Controls whether access to write the clipboard is allowed for this frame.
  virtual bool AllowWriteToClipboard(bool default_value) {
    return default_value;
  }

  // Controls whether enabling Web Components API for this frame.
  virtual bool AllowWebComponents(bool default_value) { return default_value; }

  // Controls whether to enable MutationEvents for this frame.
  // The common use case of this method is actually to selectively disable
  // MutationEvents, but it's been named for consistency with the rest of the
  // interface.
  virtual bool AllowMutationEvents(bool default_value) { return default_value; }

  // Controls whether autoplay is allowed for this frame.
  virtual bool AllowAutoplay(bool default_value) { return default_value; }

  virtual bool AllowPopupsAndRedirects(bool default_value) {
    return default_value;
  }

  // Reports that passive mixed content was found at the provided URL.
  virtual void PassiveInsecureContentFound(const WebURL&) {}

  // Notifies the client that the frame would have instantiated a plugin if
  // plugins were enabled.
  virtual void DidNotAllowPlugins() {}

  // Notifies the client that the frame would have executed script if script
  // were enabled.
  virtual void DidNotAllowScript() {}

  // Called to persist the received client hint preferences when |url| was
  // fetched. The preferences should be persisted for |duration|.
  virtual void PersistClientHints(
      const WebEnabledClientHints& enabled_client_hints,
      base::TimeDelta duration,
      const blink::WebURL& url) {}

  virtual ~WebContentSettingsClient() = default;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_CONTENT_SETTINGS_CLIENT_H_
