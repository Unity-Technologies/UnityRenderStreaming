/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_NETWORK_NETWORK_STATE_NOTIFIER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_NETWORK_NETWORK_STATE_NOTIFIER_H_

#include <memory>

#include "base/optional.h"
#include "base/rand_util.h"
#include "base/single_thread_task_runner.h"
#include "third_party/blink/public/platform/web_connection_type.h"
#include "third_party/blink/public/platform/web_effective_connection_type.h"
#include "third_party/blink/renderer/platform/cross_thread_copier.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/threading_primitives.h"
#include "third_party/blink/renderer/platform/wtf/time.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class PLATFORM_EXPORT NetworkStateNotifier {
  WTF_MAKE_NONCOPYABLE(NetworkStateNotifier);
  USING_FAST_MALLOC(NetworkStateNotifier);

 public:
  struct NetworkState {
    static const int kInvalidMaxBandwidth = -1;
    bool on_line_initialized = false;
    bool on_line = true;
    bool connection_initialized = false;
    WebConnectionType type = kWebConnectionTypeOther;
    double max_bandwidth_mbps = kInvalidMaxBandwidth;
    WebEffectiveConnectionType effective_type =
        WebEffectiveConnectionType::kTypeUnknown;
    base::Optional<TimeDelta> http_rtt;
    base::Optional<TimeDelta> transport_rtt;
    base::Optional<double> downlink_throughput_mbps;
    bool save_data = false;

    // If set, then network quality corresponding to
    // |network_quality_web_holdback| should be returned to the web consumers.
    // Consumers within Blink should still receive the actual network quality
    // values.
    base::Optional<WebEffectiveConnectionType> network_quality_web_holdback;
  };

  class NetworkStateObserver {
   public:
    // Will be called on the task runner that is passed in add*Observer.
    virtual void ConnectionChange(
        WebConnectionType,
        double max_bandwidth_mbps,
        WebEffectiveConnectionType,
        const base::Optional<TimeDelta>& http_rtt,
        const base::Optional<TimeDelta>& transport_rtt,
        const base::Optional<double>& downlink_throughput_mbps,
        bool save_data) {}
    virtual void OnLineStateChange(bool on_line) {}
  };

  enum class ObserverType {
    kOnLineState,
    kConnectionType,
  };

  class PLATFORM_EXPORT NetworkStateObserverHandle {
    USING_FAST_MALLOC(NetworkStateObserverHandle);

   public:
    NetworkStateObserverHandle(NetworkStateNotifier*,
                               ObserverType,
                               NetworkStateObserver*,
                               scoped_refptr<base::SingleThreadTaskRunner>);
    ~NetworkStateObserverHandle();

   private:
    NetworkStateNotifier* notifier_;
    ObserverType type_;
    NetworkStateObserver* observer_;
    scoped_refptr<base::SingleThreadTaskRunner> task_runner_;

    DISALLOW_COPY_AND_ASSIGN(NetworkStateObserverHandle);
  };

  NetworkStateNotifier() : has_override_(false) {}

  ~NetworkStateNotifier() {
    DCHECK(connection_observers_.IsEmpty());
    DCHECK(on_line_state_observers_.IsEmpty());
  }

  // Can be called on any thread.
  bool OnLine() const {
    MutexLocker locker(mutex_);
    const NetworkState& state = has_override_ ? override_ : state_;
    DCHECK(state.on_line_initialized);
    return state.on_line;
  }

  // Returns the current effective connection type, which is the connection type
  // whose typical performance is most similar to the measured performance of
  // the network in use.
  WebEffectiveConnectionType EffectiveType() const {
    MutexLocker locker(mutex_);
    const NetworkState& state = has_override_ ? override_ : state_;
    // TODO (tbansal): Add a DCHECK to check that |state.on_line_initialized| is
    // true once https://crbug.com/728771 is fixed.
    return state.effective_type;
  }

  // Returns the current HTTP RTT estimate. If the estimate is unavailable, the
  // returned optional value is null.
  base::Optional<TimeDelta> HttpRtt() const {
    MutexLocker locker(mutex_);
    const NetworkState& state = has_override_ ? override_ : state_;
    // TODO (tbansal): Add a DCHECK to check that |state.on_line_initialized| is
    // true once https://crbug.com/728771 is fixed.
    return state.http_rtt;
  }

  // Returns the current transport RTT estimate. If the estimate is unavailable,
  // the returned optional value is null.
  base::Optional<TimeDelta> TransportRtt() const {
    MutexLocker locker(mutex_);
    const NetworkState& state = has_override_ ? override_ : state_;
    DCHECK(state.on_line_initialized);
    return state.transport_rtt;
  }

  // Returns the current throughput estimate (in megabits per second). If the
  // estimate is unavailable, the returned optional value is null.
  base::Optional<double> DownlinkThroughputMbps() const {
    MutexLocker locker(mutex_);
    const NetworkState& state = has_override_ ? override_ : state_;
    // TODO (tbansal): Add a DCHECK to check that |state.on_line_initialized| is
    // true once https://crbug.com/728771 is fixed.
    return state.downlink_throughput_mbps;
  }

  // Returns if the save data functionality has been enabled by the user.
  // The returned value does not account for any holdback experiments that may
  // be enabled.
  bool SaveDataEnabled() const {
    MutexLocker locker(mutex_);
    const NetworkState& state = has_override_ ? override_ : state_;
    // TODO (tbansal): Add a DCHECK to check that |state.on_line_initialized| is
    // true once https://crbug.com/728771 is fixed.
    return state.save_data;
  }

  void SetOnLine(bool);

  // Can be called on any thread.
  WebConnectionType ConnectionType() const {
    MutexLocker locker(mutex_);
    const NetworkState& state = has_override_ ? override_ : state_;
    DCHECK(state.connection_initialized);
    return state.type;
  }

  // Can be called on any thread.
  bool IsCellularConnectionType() const {
    switch (ConnectionType()) {
      case kWebConnectionTypeCellular2G:
      case kWebConnectionTypeCellular3G:
      case kWebConnectionTypeCellular4G:
        return true;
      case kWebConnectionTypeBluetooth:
      case kWebConnectionTypeEthernet:
      case kWebConnectionTypeWifi:
      case kWebConnectionTypeWimax:
      case kWebConnectionTypeOther:
      case kWebConnectionTypeNone:
      case kWebConnectionTypeUnknown:
        return false;
    }
    NOTREACHED();
    return false;
  }

  // Can be called on any thread.
  double MaxBandwidth() const {
    MutexLocker locker(mutex_);
    const NetworkState& state = has_override_ ? override_ : state_;
    DCHECK(state.connection_initialized);
    return state.max_bandwidth_mbps;
  }

  void SetWebConnection(WebConnectionType, double max_bandwidth_mbps);
  void SetNetworkQuality(WebEffectiveConnectionType,
                         TimeDelta http_rtt,
                         TimeDelta transport_rtt,
                         int downlink_throughput_kbps);
  void SetNetworkQualityWebHoldback(WebEffectiveConnectionType);
  void SetSaveDataEnabled(bool enabled);

  // When called, successive setWebConnectionType/setOnLine calls are stored,
  // and supplied overridden values are used instead until clearOverride() is
  // called.  This is used for layout tests (see crbug.com/377736) and inspector
  // emulation.
  // If |effective_type| is null, its value is computed using |http_rtt_msec|.
  // |max_bandwidth_mbps| is used to override both the |max_bandwidth_mbps| and
  // |downlink_throughput_mbps|.
  //
  // Since this class is a singleton, tests must clear override when completed
  // to avoid indeterminate state across the test harness.
  void SetNetworkConnectionInfoOverride(
      bool on_line,
      WebConnectionType,
      base::Optional<WebEffectiveConnectionType> effective_type,
      unsigned long http_rtt_msec,
      double max_bandwidth_mbps);
  void SetSaveDataEnabledOverride(bool enabled);
  void ClearOverride();

  // Must be called on the given task runner. An added observer must be removed
  // before the observer or its execution context goes away. It's possible for
  // an observer to be called twice for the same event if it is first removed
  // and then added during notification.
  std::unique_ptr<NetworkStateObserverHandle> AddConnectionObserver(
      NetworkStateObserver*,
      scoped_refptr<base::SingleThreadTaskRunner>);
  std::unique_ptr<NetworkStateObserverHandle> AddOnLineObserver(
      NetworkStateObserver*,
      scoped_refptr<base::SingleThreadTaskRunner>);

  // Returns the String equivalent for a given WebEffectiveCOnnectionType.
  static String EffectiveConnectionTypeToString(WebEffectiveConnectionType);

  // Returns |rtt| after adding host-specific random noise, and rounding it as
  // per the NetInfo spec to improve privacy.
  unsigned long RoundRtt(const String& host,
                         const base::Optional<TimeDelta>& rtt) const;

  // Returns |downlink_mbps| after adding host-specific random noise, and
  // rounding it as per the NetInfo spec and to improve privacy.
  double RoundMbps(const String& host,
                   const base::Optional<double>& downlink_mbps) const;

  // Returns the randomization salt (weak and insecure) that should be used when
  // adding noise to the network quality metrics. This is known only to the
  // device, and is generated only once. This makes it possible to add the same
  // amount of noise for a given origin.
  uint8_t RandomizationSalt() const { return randomization_salt_; }

  // Returns the overriding effective connection type that should be returned to
  // the web consumers. If the returned value is null, then the actual network
  // quality value should be returned to the web consumers.
  // Consumers within Blink should not call this API.
  base::Optional<WebEffectiveConnectionType> GetWebHoldbackEffectiveType()
      const;

  // Returns the overriding HTTP RTT estimate that should be returned to
  // the web consumers. If the returned value is null, then the actual network
  // quality value should be returned to the web consumers.
  // Consumers within Blink should not call this API.
  base::Optional<TimeDelta> GetWebHoldbackHttpRtt() const;

  // Returns the overriding HTTP RTT estimate that should be returned to
  // the web consumers. If the returned value is null, then the actual network
  // quality value should be returned to the web consumers.
  // Consumers within Blink should not call this API.
  base::Optional<double> GetWebHoldbackDownlinkThroughputMbps() const;

  // Sets the metrics of all the values while taking into account any network
  // quality web holdbacks in place. The caller must guarantee that all pointers
  // are non-null.
  void GetMetricsWithWebHoldback(WebConnectionType* type,
                                 double* downlink_max_mbps,
                                 WebEffectiveConnectionType* effective_type,
                                 base::Optional<TimeDelta>* http_rtt,
                                 base::Optional<double>* downlink_mbps,
                                 bool* save_data) const;

 private:
  friend class NetworkStateObserverHandle;

  struct ObserverList {
    ObserverList() : iterating(false) {}
    bool iterating;
    Vector<NetworkStateObserver*> observers;
    Vector<wtf_size_t> zeroed_observers;  // Indices in observers that are 0.
  };

  // This helper scope issues required notifications when mutating the state if
  // something has changed.  It's only possible to mutate the state on the main
  // thread.  Note that ScopedNotifier must be destroyed when not holding a lock
  // so that onLine notifications can be dispatched without a deadlock.
  class ScopedNotifier {
   public:
    explicit ScopedNotifier(NetworkStateNotifier&);
    ~ScopedNotifier();

   private:
    NetworkStateNotifier& notifier_;
    NetworkState before_;
  };

  // The ObserverListMap is cross-thread accessed, adding/removing Observers
  // running on a task runner.
  using ObserverListMap = HashMap<scoped_refptr<base::SingleThreadTaskRunner>,
                                  std::unique_ptr<ObserverList>>;

  void NotifyObservers(ObserverListMap&, ObserverType, const NetworkState&);
  void NotifyObserversOnTaskRunner(ObserverListMap*,
                                   ObserverType,
                                   scoped_refptr<base::SingleThreadTaskRunner>,
                                   const NetworkState&);

  void AddObserverToMap(ObserverListMap&,
                        NetworkStateObserver*,
                        scoped_refptr<base::SingleThreadTaskRunner>);
  void RemoveObserver(ObserverType,
                      NetworkStateObserver*,
                      scoped_refptr<base::SingleThreadTaskRunner>);
  void RemoveObserverFromMap(ObserverListMap&,
                             NetworkStateObserver*,
                             scoped_refptr<base::SingleThreadTaskRunner>);

  ObserverList* LockAndFindObserverList(
      ObserverListMap&,
      scoped_refptr<base::SingleThreadTaskRunner>);

  // Removed observers are nulled out in the list in case the list is being
  // iterated over. Once done iterating, call this to clean up nulled
  // observers.
  void CollectZeroedObservers(ObserverListMap&,
                              ObserverList*,
                              scoped_refptr<base::SingleThreadTaskRunner>);

  // A random number by which the RTT and downlink estimates are multiplied
  // with. The returned random multiplier is a function of the hostname.
  // Adding this noise reduces the chances of cross-origin fingerprinting.
  double GetRandomMultiplier(const String& host) const;

  mutable Mutex mutex_;
  NetworkState state_;
  bool has_override_;
  NetworkState override_;

  ObserverListMap connection_observers_;
  ObserverListMap on_line_state_observers_;

  const uint8_t randomization_salt_ = base::RandInt(1, 20);
};

PLATFORM_EXPORT NetworkStateNotifier& GetNetworkStateNotifier();

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_NETWORK_NETWORK_STATE_NOTIFIER_H_
