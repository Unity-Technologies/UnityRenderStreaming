/*
 *  Copyright 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SDK_ANDROID_SRC_JNI_ANDROIDNETWORKMONITOR_H_
#define SDK_ANDROID_SRC_JNI_ANDROIDNETWORKMONITOR_H_

#include <stdint.h>
#include <map>
#include <string>
#include <vector>

#include "rtc_base/networkmonitor.h"
#include "rtc_base/thread_checker.h"
#include "sdk/android/src/jni/jni_helpers.h"

namespace webrtc {
namespace jni {

typedef int64_t NetworkHandle;

// c++ equivalent of java NetworkMonitorAutoDetect.ConnectionType.
enum NetworkType {
  NETWORK_UNKNOWN,
  NETWORK_ETHERNET,
  NETWORK_WIFI,
  NETWORK_4G,
  NETWORK_3G,
  NETWORK_2G,
  NETWORK_UNKNOWN_CELLULAR,
  NETWORK_BLUETOOTH,
  NETWORK_VPN,
  NETWORK_NONE
};

// The information is collected from Android OS so that the native code can get
// the network type and handle (Android network ID) for each interface.
struct NetworkInformation {
  std::string interface_name;
  NetworkHandle handle;
  NetworkType type;
  NetworkType underlying_type_for_vpn;
  std::vector<rtc::IPAddress> ip_addresses;

  NetworkInformation();
  NetworkInformation(const NetworkInformation&);
  NetworkInformation(NetworkInformation&&);
  ~NetworkInformation();
  NetworkInformation& operator=(const NetworkInformation&);
  NetworkInformation& operator=(NetworkInformation&&);

  std::string ToString() const;
};

class AndroidNetworkMonitor : public rtc::NetworkMonitorBase,
                              public rtc::NetworkBinderInterface {
 public:
  explicit AndroidNetworkMonitor(JNIEnv* env,
                                 const JavaRef<jobject>& j_application_context);
  ~AndroidNetworkMonitor() override;

  // TODO(sakal): Remove once down stream dependencies have been updated.
  static void SetAndroidContext(JNIEnv* jni, jobject context) {}

  void Start() override;
  void Stop() override;

  rtc::NetworkBindingResult BindSocketToNetwork(
      int socket_fd,
      const rtc::IPAddress& address) override;
  rtc::AdapterType GetAdapterType(const std::string& if_name) override;
  rtc::AdapterType GetVpnUnderlyingAdapterType(
      const std::string& if_name) override;
  void OnNetworkConnected(const NetworkInformation& network_info);
  void OnNetworkDisconnected(NetworkHandle network_handle);
  // Always expected to be called on the network thread.
  void SetNetworkInfos(const std::vector<NetworkInformation>& network_infos);

  void NotifyConnectionTypeChanged(JNIEnv* env,
                                   const JavaRef<jobject>& j_caller);
  void NotifyOfNetworkConnect(JNIEnv* env,
                              const JavaRef<jobject>& j_caller,
                              const JavaRef<jobject>& j_network_info);
  void NotifyOfNetworkDisconnect(JNIEnv* env,
                                 const JavaRef<jobject>& j_caller,
                                 jlong network_handle);
  void NotifyOfActiveNetworkList(JNIEnv* env,
                                 const JavaRef<jobject>& j_caller,
                                 const JavaRef<jobjectArray>& j_network_infos);

 private:
  void OnNetworkConnected_w(const NetworkInformation& network_info);
  void OnNetworkDisconnected_w(NetworkHandle network_handle);

  const int android_sdk_int_;
  ScopedJavaGlobalRef<jobject> j_application_context_;
  ScopedJavaGlobalRef<jobject> j_network_monitor_;
  rtc::ThreadChecker thread_checker_;
  bool started_ = false;
  std::map<std::string, rtc::AdapterType> adapter_type_by_name_;
  std::map<std::string, rtc::AdapterType> vpn_underlying_adapter_type_by_name_;
  std::map<rtc::IPAddress, NetworkHandle> network_handle_by_address_;
  std::map<NetworkHandle, NetworkInformation> network_info_by_handle_;
};

class AndroidNetworkMonitorFactory : public rtc::NetworkMonitorFactory {
 public:
  // Deprecated. Pass in application context to this class.
  AndroidNetworkMonitorFactory();

  AndroidNetworkMonitorFactory(JNIEnv* env,
                               const JavaRef<jobject>& j_application_context);

  ~AndroidNetworkMonitorFactory() override;

  rtc::NetworkMonitorInterface* CreateNetworkMonitor() override;

 private:
  ScopedJavaGlobalRef<jobject> j_application_context_;
};

}  // namespace jni
}  // namespace webrtc

// TODO(magjed): Remove once external clients are updated.
namespace webrtc_jni {

using webrtc::jni::AndroidNetworkMonitor;
using webrtc::jni::AndroidNetworkMonitorFactory;

}  // namespace webrtc_jni

#endif  // SDK_ANDROID_SRC_JNI_ANDROIDNETWORKMONITOR_H_
