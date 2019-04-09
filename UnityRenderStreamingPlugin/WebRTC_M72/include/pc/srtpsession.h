/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_SRTPSESSION_H_
#define PC_SRTPSESSION_H_

#include <vector>

#include "rtc_base/scoped_ref_ptr.h"
#include "rtc_base/thread_checker.h"

// Forward declaration to avoid pulling in libsrtp headers here
struct srtp_event_data_t;
struct srtp_ctx_t_;

namespace cricket {

// Class that wraps a libSRTP session.
class SrtpSession {
 public:
  SrtpSession();
  ~SrtpSession();

  // Configures the session for sending data using the specified
  // cipher-suite and key. Receiving must be done by a separate session.
  bool SetSend(int cs,
               const uint8_t* key,
               size_t len,
               const std::vector<int>& extension_ids);
  bool UpdateSend(int cs,
                  const uint8_t* key,
                  size_t len,
                  const std::vector<int>& extension_ids);

  // Configures the session for receiving data using the specified
  // cipher-suite and key. Sending must be done by a separate session.
  bool SetRecv(int cs,
               const uint8_t* key,
               size_t len,
               const std::vector<int>& extension_ids);
  bool UpdateRecv(int cs,
                  const uint8_t* key,
                  size_t len,
                  const std::vector<int>& extension_ids);

  // Encrypts/signs an individual RTP/RTCP packet, in-place.
  // If an HMAC is used, this will increase the packet size.
  bool ProtectRtp(void* data, int in_len, int max_len, int* out_len);
  // Overloaded version, outputs packet index.
  bool ProtectRtp(void* data,
                  int in_len,
                  int max_len,
                  int* out_len,
                  int64_t* index);
  bool ProtectRtcp(void* data, int in_len, int max_len, int* out_len);
  // Decrypts/verifies an invidiual RTP/RTCP packet.
  // If an HMAC is used, this will decrease the packet size.
  bool UnprotectRtp(void* data, int in_len, int* out_len);
  bool UnprotectRtcp(void* data, int in_len, int* out_len);

  // Helper method to get authentication params.
  bool GetRtpAuthParams(uint8_t** key, int* key_len, int* tag_len);

  int GetSrtpOverhead() const;

  // If external auth is enabled, SRTP will write a dummy auth tag that then
  // later must get replaced before the packet is sent out. Only supported for
  // non-GCM cipher suites and can be checked through "IsExternalAuthActive"
  // if it is actually used. This method is only valid before the RTP params
  // have been set.
  void EnableExternalAuth();
  bool IsExternalAuthEnabled() const;

  // A SRTP session supports external creation of the auth tag if a non-GCM
  // cipher is used. This method is only valid after the RTP params have
  // been set.
  bool IsExternalAuthActive() const;

 private:
  bool DoSetKey(int type,
                int cs,
                const uint8_t* key,
                size_t len,
                const std::vector<int>& extension_ids);
  bool SetKey(int type,
              int cs,
              const uint8_t* key,
              size_t len,
              const std::vector<int>& extension_ids);
  bool UpdateKey(int type,
                 int cs,
                 const uint8_t* key,
                 size_t len,
                 const std::vector<int>& extension_ids);
  // Returns send stream current packet index from srtp db.
  bool GetSendStreamPacketIndex(void* data, int in_len, int64_t* index);

  // These methods are responsible for initializing libsrtp (if the usage count
  // is incremented from 0 to 1) or deinitializing it (when decremented from 1
  // to 0).
  //
  // Returns true if successful (will always be successful if already inited).
  static bool IncrementLibsrtpUsageCountAndMaybeInit();
  static void DecrementLibsrtpUsageCountAndMaybeDeinit();

  void HandleEvent(const srtp_event_data_t* ev);
  static void HandleEventThunk(srtp_event_data_t* ev);

  rtc::ThreadChecker thread_checker_;
  srtp_ctx_t_* session_ = nullptr;
  int rtp_auth_tag_len_ = 0;
  int rtcp_auth_tag_len_ = 0;
  bool inited_ = false;
  static rtc::GlobalLockPod lock_;
  int last_send_seq_num_ = -1;
  bool external_auth_active_ = false;
  bool external_auth_enabled_ = false;
  int decryption_failure_count_ = 0;
  RTC_DISALLOW_COPY_AND_ASSIGN(SrtpSession);
};

}  // namespace cricket

#endif  // PC_SRTPSESSION_H_
