/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_SRTPTESTUTIL_H_
#define PC_SRTPTESTUTIL_H_

#include <string>

namespace rtc {

extern const char CS_AES_CM_128_HMAC_SHA1_32[];
extern const char CS_AEAD_AES_128_GCM[];
extern const char CS_AEAD_AES_256_GCM[];

static const uint8_t kTestKey1[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234";
static const uint8_t kTestKey2[] = "4321ZYXWVUTSRQPONMLKJIHGFEDCBA";
static const int kTestKeyLen = 30;

static int rtp_auth_tag_len(const std::string& cs) {
  if (cs == CS_AES_CM_128_HMAC_SHA1_32) {
    return 4;
  } else if (cs == CS_AEAD_AES_128_GCM || cs == CS_AEAD_AES_256_GCM) {
    return 16;
  } else {
    return 10;
  }
}
static int rtcp_auth_tag_len(const std::string& cs) {
  if (cs == CS_AEAD_AES_128_GCM || cs == CS_AEAD_AES_256_GCM) {
    return 16;
  } else {
    return 10;
  }
}

}  // namespace rtc

#endif  // PC_SRTPTESTUTIL_H_
