// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_VIDEO_FRAME_SUBMITTER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_VIDEO_FRAME_SUBMITTER_H_

#include <memory>
#include <utility>

#include "base/memory/weak_ptr.h"
#include "base/threading/thread_checker.h"
#include "base/time/time.h"
#include "components/viz/client/shared_bitmap_reporter.h"
#include "components/viz/common/gpu/context_provider.h"
#include "components/viz/common/resources/shared_bitmap.h"
#include "components/viz/common/surfaces/child_local_surface_id_allocator.h"
#include "mojo/public/cpp/bindings/binding.h"
#include "mojo/public/cpp/system/buffer.h"
#include "services/viz/public/interfaces/compositing/compositor_frame_sink.mojom-blink.h"
#include "third_party/blink/public/platform/modules/frame_sinks/embedded_frame_sink.mojom-blink.h"
#include "third_party/blink/public/platform/web_video_frame_submitter.h"
#include "third_party/blink/renderer/platform/graphics/video_frame_resource_provider.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/functional.h"

namespace blink {

// This single-threaded class facilitates the communication between the media
// stack and browser renderer, providing compositor frames containing video
// frames and corresponding resources to the |compositor_frame_sink_|. This
// class has dependencies on classes that use the media thread's OpenGL
// ContextProvider, and thus, besides construction, should be consistently ran
// from the same media SingleThreadTaskRunner.
class PLATFORM_EXPORT VideoFrameSubmitter
    : public WebVideoFrameSubmitter,
      public viz::ContextLostObserver,
      public viz::SharedBitmapReporter,
      public viz::mojom::blink::CompositorFrameSinkClient {
 public:
  explicit VideoFrameSubmitter(WebContextProviderCallback,
                               std::unique_ptr<VideoFrameResourceProvider>);

  ~VideoFrameSubmitter() override;

  bool Rendering() { return is_rendering_; }
  cc::VideoFrameProvider* Provider() { return video_frame_provider_; }
  mojo::Binding<viz::mojom::blink::CompositorFrameSinkClient>* Binding() {
    return &binding_;
  }

  void OnReceivedContextProvider(bool, scoped_refptr<viz::ContextProvider>);

  // cc::VideoFrameProvider::Client implementation.
  void StopUsingProvider() override;
  void StartRendering() override;
  void StopRendering() override;
  void DidReceiveFrame() override;
  bool IsDrivingFrameUpdates() const override;

  // WebVideoFrameSubmitter implementation.
  void Initialize(cc::VideoFrameProvider*) override;
  void SetRotation(media::VideoRotation) override;
  void SetIsOpaque(bool) override;
  void EnableSubmission(viz::SurfaceId,
                        base::TimeTicks local_surface_id_allocation_time,
                        WebFrameSinkDestroyedCallback) override;
  void UpdateSubmissionState(bool is_visible) override;
  void SetForceSubmit(bool) override;

  // viz::ContextLostObserver implementation.
  void OnContextLost() override;

  // cc::mojom::CompositorFrameSinkClient implementation.
  void DidReceiveCompositorFrameAck(
      const WTF::Vector<viz::ReturnedResource>& resources) override;
  void OnBeginFrame(
      const viz::BeginFrameArgs&,
      WTF::HashMap<uint32_t, ::gfx::mojom::blink::PresentationFeedbackPtr>)
      override;
  void OnBeginFramePausedChanged(bool paused) override {}
  void ReclaimResources(
      const WTF::Vector<viz::ReturnedResource>& resources) override;

  // viz::SharedBitmapReporter implementation.
  void DidAllocateSharedBitmap(mojo::ScopedSharedBufferHandle,
                               const viz::SharedBitmapId&) override;
  void DidDeleteSharedBitmap(const viz::SharedBitmapId&) override;

  void SetCompositorFrameSinkPtrForTesting(
      viz::mojom::blink::CompositorFrameSinkPtr* sink) {
    compositor_frame_sink_ = std::move(*sink);
  }
  void SetSurfaceEmbedderPtrForTesting(
      mojom::blink::SurfaceEmbedderPtr embedder) {
    surface_embedder_ = std::move(embedder);
  }
  void SetSurfaceIdForTesting(const viz::SurfaceId&, base::TimeTicks);

 private:
  FRIEND_TEST_ALL_PREFIXES(VideoFrameSubmitterTest, ContextLostDuringSubmit);
  FRIEND_TEST_ALL_PREFIXES(VideoFrameSubmitterTest,
                           ShouldSubmitPreventsSubmission);
  FRIEND_TEST_ALL_PREFIXES(VideoFrameSubmitterTest,
                           SetForceSubmitForcesSubmission);
  FRIEND_TEST_ALL_PREFIXES(VideoFrameSubmitterTest,
                           FrameSizeChangeUpdatesLocalSurfaceId);
  FRIEND_TEST_ALL_PREFIXES(VideoFrameSubmitterTest,
                           StopUsingProviderDuringContextLost);

  void StartSubmitting();
  void UpdateSubmissionStateInternal();
  // Returns whether a frame was submitted.
  bool SubmitFrame(const viz::BeginFrameAck&, scoped_refptr<media::VideoFrame>);
  void SubmitEmptyFrame();

  // Pulls frame and submits it to compositor.
  // Used in cases like PaintSingleFrame, which occurs before video rendering
  // has started to post a poster image, or to submit a final frame before
  // ending rendering.
  void SubmitSingleFrame();

  // Return whether the submitter should submit frames based on its current
  // state.
  bool ShouldSubmit() const;

  cc::VideoFrameProvider* video_frame_provider_ = nullptr;
  scoped_refptr<viz::ContextProvider> context_provider_;
  viz::mojom::blink::CompositorFrameSinkPtr compositor_frame_sink_;
  mojom::blink::SurfaceEmbedderPtr surface_embedder_;
  mojo::Binding<viz::mojom::blink::CompositorFrameSinkClient> binding_;
  WebContextProviderCallback context_provider_callback_;
  std::unique_ptr<VideoFrameResourceProvider> resource_provider_;
  WebFrameSinkDestroyedCallback frame_sink_destroyed_callback_;
  bool waiting_for_compositor_ack_ = false;

  bool is_rendering_ = false;
  // If we are not on screen, we should not submit.
  bool should_submit_internal_ = false;
  // Whether frames should always be submitted, even if we're not visible.
  bool force_submit_ = false;
  // Needs to be initialized in implementation because media isn't a public_dep
  // of blink/platform.
  media::VideoRotation rotation_;
  bool is_opaque_ = true;

  viz::FrameSinkId frame_sink_id_;

  // Size of the video frame being submitted. It is set the first time a frame
  // is submitted. Every time there is a change in the video frame size, the
  // child component of the LocalSurfaceId will be updated.
  gfx::Size frame_size_;

  // Used to updated the LocalSurfaceId when detecting a change in video frame
  // size.
  viz::ChildLocalSurfaceIdAllocator child_local_surface_id_allocator_;

  const bool enable_surface_synchronization_;

  THREAD_CHECKER(media_thread_checker_);

  base::WeakPtrFactory<VideoFrameSubmitter> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(VideoFrameSubmitter);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_VIDEO_FRAME_SUBMITTER_H_
