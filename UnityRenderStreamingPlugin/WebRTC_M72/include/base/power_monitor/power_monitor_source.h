// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_POWER_MONITOR_POWER_MONITOR_SOURCE_H_
#define BASE_POWER_MONITOR_POWER_MONITOR_SOURCE_H_

#include "base/base_export.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/observer_list_threadsafe.h"
#include "base/synchronization/lock.h"

namespace base {

class PowerMonitor;

// Communicates power state changes to the power monitor.
class BASE_EXPORT PowerMonitorSource {
 public:
  PowerMonitorSource();
  virtual ~PowerMonitorSource();

  // Normalized list of power events.
  enum PowerEvent {
    POWER_STATE_EVENT,  // The Power status of the system has changed.
    SUSPEND_EVENT,      // The system is being suspended.
    RESUME_EVENT        // The system is being resumed.
  };

  // Is the computer currently on battery power. Can be called on any thread.
  bool IsOnBatteryPower();

  // Called by PowerMonitor just before PowerMonitor destroys both itself and
  // this instance). After return from this call it is no longer safe for
  // subclasses to call into PowerMonitor (e.g., via PowerMonitor::Get(). Hence,
  // subclasses should take any necessary actions here to ensure that after
  // return from this invocation they will no longer make any calls on
  // PowerMonitor.
  virtual void Shutdown() = 0;

 protected:
  friend class PowerMonitorTest;

  // Friend function that is allowed to access the protected ProcessPowerEvent.
  friend void ProcessPowerEventHelper(PowerEvent);

  // ProcessPowerEvent should only be called from a single thread, most likely
  // the UI thread or, in child processes, the IO thread.
  static void ProcessPowerEvent(PowerEvent event_id);

  // Platform-specific method to check whether the system is currently
  // running on battery power.  Returns true if running on batteries,
  // false otherwise.
  virtual bool IsOnBatteryPowerImpl() = 0;

  // Sets the initial state for |on_battery_power_|, which defaults to false
  // since not all implementations can provide the value at construction. May
  // only be called before a base::PowerMonitor has been created.
  void SetInitialOnBatteryPowerState(bool on_battery_power);

 private:
  bool on_battery_power_ = false;
  bool suspended_ = false;

  // This lock guards access to on_battery_power_, to ensure that
  // IsOnBatteryPower can be called from any thread.
  Lock battery_lock_;

  DISALLOW_COPY_AND_ASSIGN(PowerMonitorSource);
};

}  // namespace base

#endif  // BASE_POWER_MONITOR_POWER_MONITOR_SOURCE_H_
