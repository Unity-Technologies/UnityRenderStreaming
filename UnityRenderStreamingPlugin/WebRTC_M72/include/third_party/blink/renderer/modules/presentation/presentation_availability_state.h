// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_PRESENTATION_AVAILABILITY_STATE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_PRESENTATION_AVAILABILITY_STATE_H_

#include <memory>
#include <set>
#include <vector>

#include "base/macros.h"
#include "third_party/blink/public/mojom/presentation/presentation.mojom-blink.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/modules/presentation/presentation_availability_callbacks.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class PresentationAvailabilityObserver;

// Maintains the states of PresentationAvailability objects in a frame. It is
// also responsible for querying the availability values from
// PresentationService for the URLs given in a PresentationRequest. As an
// optimization, the result will be cached and shared with other
// PresentationRequests containing the same URL. PresentationAvailabilityState
// is owned by PresentationController in the same frame.
// TODO(crbug.com/780109): Improve encapsulation of PresentationAvailability and
// this class by moving the multiple URL tracking logic to the former, and
// consolidating this class's APIs to take repeating callbacks.
class MODULES_EXPORT PresentationAvailabilityState {
 public:
  explicit PresentationAvailabilityState(mojom::blink::PresentationService*);
  ~PresentationAvailabilityState();

  // Requests availability for the given URLs and invokes the given callbacks
  // with the determined availability value. The callbacks will only be invoked
  // once and will be deleted afterwards.
  void RequestAvailability(const Vector<KURL>&,
                           std::unique_ptr<PresentationAvailabilityCallbacks>);

  // Starts/stops listening for availability with the given observer.
  void AddObserver(PresentationAvailabilityObserver*);
  void RemoveObserver(PresentationAvailabilityObserver*);

  // Updates the availability value for a given URL, and invoking any affected
  // callbacks and observers.
  void UpdateAvailability(const KURL&, mojom::blink::ScreenAvailability);

 private:
  enum class ListeningState {
    INACTIVE,
    WAITING,
    ACTIVE,
  };

  // Tracks listeners of presentation displays availability for
  // |availability_urls|. Shared with PresentationRequest objects with the same
  // set of URLs.
  struct AvailabilityListener {
    explicit AvailabilityListener(const Vector<KURL>& availability_urls);
    ~AvailabilityListener();

    const Vector<KURL> urls;
    std::vector<std::unique_ptr<PresentationAvailabilityCallbacks>>
        availability_callbacks;
    std::set<PresentationAvailabilityObserver*> availability_observers;

    DISALLOW_COPY_AND_ASSIGN(AvailabilityListener);
  };

  // Tracks listening status and screen availability of |availability_url|.
  struct ListeningStatus {
    explicit ListeningStatus(const KURL& availability_url);
    ~ListeningStatus();

    const KURL url;
    mojom::blink::ScreenAvailability last_known_availability;
    ListeningState listening_state;
  };

  // Starts listening for availability for the given URL, and calls
  // PresentationService if needed.
  void StartListeningToURL(const KURL&);

  // Stops listening for availability for the given URL if there are no
  // remaining callbacks or observers registered to it, and calls
  // PresentationService if needed.
  void MaybeStopListeningToURL(const KURL&);

  // Returns AVAILABLE if any url in |urls| has screen availability AVAILABLE;
  // otherwise returns DISABLED if at least one url in |urls| has screen
  // availability DISABLED;
  // otherwise, returns SOURCE_NOT_SUPPORTED if any url in |urls| has screen
  // availability SOURCE_NOT_SUPPORTED;
  // otherwise, returns UNAVAILABLE if any url in |urls| has screen
  // availability UNAVAILABLE;
  // otherwise returns UNKNOWN.
  mojom::blink::ScreenAvailability GetScreenAvailability(
      const Vector<KURL>&) const;

  // Returns nullptr if there is no AvailabilityListener for the given URLs.
  AvailabilityListener* GetAvailabilityListener(const Vector<KURL>&) const;

  // Removes the given listener from |availability_set_| if it has no callbacks
  // and no observers.
  void TryRemoveAvailabilityListener(AvailabilityListener*);

  // Returns nullptr if there is no status for the given URL.
  ListeningStatus* GetListeningStatus(const KURL&) const;

  // ListeningStatus for known URLs.
  std::vector<std::unique_ptr<ListeningStatus>> availability_listening_status_;

  // Set of AvailabilityListener for known PresentationRequests.
  std::vector<std::unique_ptr<AvailabilityListener>> availability_listeners_;

  // A pointer to PresentationService owned by PresentationController.
  mojom::blink::PresentationService* const presentation_service_;

  DISALLOW_COPY_AND_ASSIGN(PresentationAvailabilityState);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_PRESENTATION_AVAILABILITY_STATE_H_
