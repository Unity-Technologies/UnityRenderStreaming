// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_BACKGROUND_FETCH_BACKGROUND_FETCH_ICON_LOADER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_BACKGROUND_FETCH_BACKGROUND_FETCH_ICON_LOADER_H_

#include <memory>

#include "third_party/blink/renderer/core/loader/threadable_loader.h"
#include "third_party/blink/renderer/core/loader/threadable_loader_client.h"
#include "third_party/blink/renderer/modules/background_fetch/background_fetch_type_converters.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/shared_buffer.h"
#include "third_party/skia/include/core/SkBitmap.h"

namespace blink {

class BackgroundFetchBridge;
class SegmentReader;
struct WebSize;

class MODULES_EXPORT BackgroundFetchIconLoader final
    : public GarbageCollectedFinalized<BackgroundFetchIconLoader>,
      public ThreadableLoaderClient {
  USING_GARBAGE_COLLECTED_MIXIN(BackgroundFetchIconLoader);

 public:
  // The bitmap may be empty if the request failed or the image data could not
  // be decoded. The int64_t returned is the scale of the ideal to chosen icon,
  // before resizing. This is -1 if the ideal icon size is empty, or if no icon
  // provided was suitable.
  using IconCallback = base::OnceCallback<void(const SkBitmap&, int64_t)>;

  BackgroundFetchIconLoader();
  ~BackgroundFetchIconLoader() override;

  // Asynchronously download an icon from the given url, decodes the loaded
  // data, and passes the bitmap to the given callback.
  void Start(BackgroundFetchBridge* bridge,
             ExecutionContext* execution_context,
             HeapVector<Member<ManifestImageResource>> icons,
             IconCallback icon_callback);

  // Cancels the pending load, if there is one. The |icon_callback_| will not
  // be run.
  void Stop();

  // ThreadableLoaderClient interface.
  void DidReceiveData(const char* data, unsigned length) override;
  void DidFinishLoading(unsigned long resource_identifier) override;
  void DidFail(const ResourceError& error) override;
  void DidFailRedirectCheck() override;

  void Trace(blink::Visitor* visitor) override {
    visitor->Trace(threadable_loader_);
    visitor->Trace(icons_);
    ThreadableLoaderClient::Trace(visitor);
  }

 private:
  friend class BackgroundFetchIconLoaderTest;

  void RunCallback(int64_t ideal_to_chosen_icon_size_times_hundred);
  void RunCallbackWithEmptyBitmap();

  // Callback for BackgroundFetchBridge::GetIconDisplaySize()
  void DidGetIconDisplaySizeIfSoLoadIcon(
      ExecutionContext* execution_context,
      IconCallback callback,
      const WebSize& icon_display_size_pixels);

  // Picks the best icon from the list of developer provided icons, for current
  // display, given the ideal |icon_display_size_pixels_|, and returns its KURL.
  // If none of the icons is appropriate, this returns an empty URL.
  KURL PickBestIconForDisplay(ExecutionContext* execution_context);

  // Decodes the |data| to a SkBitmap using the image decoders and resizes it to
  // be no larger than |icon_display_size_pixels_|.
  void DecodeAndResizeImageOnBackgroundThread(
      scoped_refptr<base::SingleThreadTaskRunner> task_runner,
      scoped_refptr<SegmentReader> data);

  // Called when the image has been decoded and resized on a background thread.
  void DidFinishDecoding();

  HeapVector<Member<ManifestImageResource>> icons_;
  IconCallback icon_callback_;

  Member<ThreadableLoader> threadable_loader_;

  // Size at which the icon will be presented to the user.
  WebSize icon_display_size_pixels_;

  // Data received from the ThreadableLoader. Will be invalidated when decoding
  // of the image data starts.
  scoped_refptr<SharedBuffer> data_;

  // Result of decoding the icon on the background thread.
  SkBitmap decoded_icon_;

  // Whether the icon loading operation has been stopped. The process should
  // be aborted then, and |icon_callback_| must not be invoked anymore.
  bool stopped_ = false;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_BACKGROUND_FETCH_BACKGROUND_FETCH_ICON_LOADER_H_
