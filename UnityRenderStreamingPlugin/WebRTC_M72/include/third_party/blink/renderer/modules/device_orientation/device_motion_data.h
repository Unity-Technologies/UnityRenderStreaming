/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_ORIENTATION_DEVICE_MOTION_DATA_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_ORIENTATION_DEVICE_MOTION_DATA_H_

#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class DeviceAcceleration;
class DeviceRotationRate;
class DeviceMotionEventInit;

class MODULES_EXPORT DeviceMotionData final
    : public GarbageCollected<DeviceMotionData> {
 public:
  static DeviceMotionData* Create();
  static DeviceMotionData* Create(
      DeviceAcceleration*,
      DeviceAcceleration* acceleration_including_gravity,
      DeviceRotationRate*,
      double interval);
  static DeviceMotionData* Create(const DeviceMotionEventInit*);

  DeviceMotionData();
  DeviceMotionData(DeviceAcceleration*,
                   DeviceAcceleration* acceleration_including_gravity,
                   DeviceRotationRate*,
                   double interval);

  void Trace(blink::Visitor*);

  DeviceAcceleration* GetAcceleration() const { return acceleration_.Get(); }
  DeviceAcceleration* GetAccelerationIncludingGravity() const {
    return acceleration_including_gravity_.Get();
  }
  DeviceRotationRate* GetRotationRate() const { return rotation_rate_.Get(); }

  double Interval() const { return interval_; }

  bool CanProvideEventData() const;

 private:
  Member<DeviceAcceleration> acceleration_;
  Member<DeviceAcceleration> acceleration_including_gravity_;
  Member<DeviceRotationRate> rotation_rate_;
  double interval_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_ORIENTATION_DEVICE_MOTION_DATA_H_
