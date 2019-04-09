/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CORE_INITIALIZER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CORE_INITIALIZER_H_

#include "base/macros.h"
#include "services/service_manager/public/cpp/binder_registry.h"
#include "third_party/blink/public/common/dom_storage/session_storage_namespace_id.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

class DevToolsSession;
class Document;
class HTMLMediaElement;
class InspectedFrames;
class InspectorDOMAgent;
class LocalFrame;
class MediaControls;
class Page;
class PictureInPictureController;
class Settings;
class ShadowRoot;
class WebLocalFrameClient;
class WebLayerTreeView;
class WebMediaPlayer;
class WebMediaPlayerClient;
class WebMediaPlayerSource;
class WebRemotePlaybackClient;
class WebViewClient;
class WorkerClients;

class CORE_EXPORT CoreInitializer {
  USING_FAST_MALLOC(CoreInitializer);
  DISALLOW_COPY_AND_ASSIGN(CoreInitializer);

 public:
  // Initialize must be called before GetInstance.
  static CoreInitializer& GetInstance() {
    DCHECK(instance_);
    return *instance_;
  }

  virtual ~CoreInitializer() = default;

  // Should be called by clients before trying to create Frames.
  virtual void Initialize();

  // Called on startup to register Mojo interfaces that for control messages,
  // e.g. messages that are not routed to a specific frame.
  virtual void RegisterInterfaces(service_manager::BinderRegistry&) = 0;
  // Methods defined in CoreInitializer and implemented by ModulesInitializer to
  // bypass the inverted dependency from core/ to modules/.
  // Mojo Interfaces registered with LocalFrame
  virtual void InitLocalFrame(LocalFrame&) const = 0;
  // Supplements installed on a frame using ChromeClient
  virtual void InstallSupplements(LocalFrame&) const = 0;
  virtual void ProvideLocalFileSystemToWorker(WorkerClients&) const = 0;
  virtual void ProvideIndexedDBClientToWorker(WorkerClients&) const = 0;
  virtual MediaControls* CreateMediaControls(HTMLMediaElement&,
                                             ShadowRoot&) const = 0;
  virtual PictureInPictureController* CreatePictureInPictureController(
      Document&) const = 0;
  // Session Initializers for Inspector Agents in modules/
  // These methods typically create agents and append them to a session.
  // TODO(nverne): remove this and restore to WebDevToolsAgentImpl once that
  // class is a controller/ crbug:731490
  virtual void InitInspectorAgentSession(DevToolsSession*,
                                         bool,
                                         InspectorDOMAgent*,
                                         InspectedFrames*,
                                         Page*) const = 0;

  virtual void OnClearWindowObjectInMainWorld(Document&,
                                              const Settings&) const = 0;

  virtual std::unique_ptr<WebMediaPlayer> CreateWebMediaPlayer(
      WebLocalFrameClient*,
      HTMLMediaElement&,
      const WebMediaPlayerSource&,
      WebMediaPlayerClient*,
      WebLayerTreeView*) const = 0;

  virtual WebRemotePlaybackClient* CreateWebRemotePlaybackClient(
      HTMLMediaElement&) const = 0;

  virtual void ProvideModulesToPage(Page&, WebViewClient*) const = 0;
  virtual void ForceNextWebGLContextCreationToFail() const = 0;

  virtual void CollectAllGarbageForAnimationAndPaintWorklet() const = 0;

  virtual void CloneSessionStorage(
      Page* clone_from_page,
      const SessionStorageNamespaceId& clone_to_namespace) = 0;

 protected:
  // CoreInitializer is only instantiated by subclass ModulesInitializer.
  CoreInitializer() = default;

 private:
  static CoreInitializer* instance_;
  void RegisterEventFactory();
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CORE_INITIALIZER_H_
