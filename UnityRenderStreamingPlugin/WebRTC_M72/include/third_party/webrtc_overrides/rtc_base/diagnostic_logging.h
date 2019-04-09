// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_WEBRTC_OVERRIDES_WEBRTC_RTC_BASE_DIAGNOSTIC_LOGGING_H_
#define THIRD_PARTY_WEBRTC_OVERRIDES_WEBRTC_RTC_BASE_DIAGNOSTIC_LOGGING_H_

#include <sstream>
#include <string>

#include "third_party/webrtc/rtc_base/checks.h"
#include "third_party/webrtc/rtc_base/scoped_ref_ptr.h"

namespace rtc {

///////////////////////////////////////////////////////////////////////////////
// ConstantLabel can be used to easily generate string names from constant
// values.  This can be useful for logging descriptive names of error messages.
// Usage:
//   const ConstantLabel LIBRARY_ERRORS[] = {
//     KLABEL(SOME_ERROR),
//     KLABEL(SOME_OTHER_ERROR),
//     ...
//     LASTLABEL
//   }
//
//   int err = LibraryFunc();
//   LOG(LS_ERROR) << "LibraryFunc returned: "
//                 << ErrorName(err, LIBRARY_ERRORS);

struct ConstantLabel {
  int value;
  const char* label;
};
#define KLABEL(x) \
  { x, #x }
#define LASTLABEL \
  { 0, 0 }

const char* FindLabel(int value, const ConstantLabel entries[]);
std::string ErrorName(int err, const ConstantLabel* err_table);

//////////////////////////////////////////////////////////////////////
// Note that the non-standard LoggingSeverity aliases exist because they are
// still in broad use.  The meanings of the levels are:
//  LS_SENSITIVE: Information which should only be logged with the consent
//   of the user, due to privacy concerns.
//  LS_VERBOSE: This level is for data which we do not want to appear in the
//   normal debug log, but should appear in diagnostic logs.
//  LS_INFO: Chatty level used in debugging for all sorts of things, the default
//   in debug builds.
//  LS_WARNING: Something that may warrant investigation.
//  LS_ERROR: Something that should not have occurred.
//  LS_NONE: Set this as minimum severity to disable logging.
// Note that LoggingSeverity is mapped over to chromiums verbosity levels where
// anything lower than or equal to the current verbosity level is written to
// file which is the opposite of logging severity in libjingle where higher
// severity numbers than or equal to the current severity level are written to
// file. Also, note that the values are explicitly defined here for convenience
// since the command line flag must be set using numerical values.
// TODO(tommi): To keep things simple, we should just use the same values for
// these constants as Chrome does.
enum LoggingSeverity {
  LS_NONE = 0,
  LS_ERROR = 1,
  LS_WARNING = 2,
  LS_INFO = 3,
  LS_VERBOSE = 4,
  LS_SENSITIVE = 5,
  INFO = LS_INFO,
  WARNING = LS_WARNING,
  LERROR = LS_ERROR
};

// LogErrorContext assists in interpreting the meaning of an error value.
enum LogErrorContext {
  ERRCTX_NONE,
  ERRCTX_ERRNO,     // System-local errno
  ERRCTX_HRESULT,   // Windows HRESULT
  ERRCTX_OSSTATUS,  // MacOS OSStatus

  // Abbreviations for LOG_E macro
  ERRCTX_EN = ERRCTX_ERRNO,     // LOG_E(sev, EN, x)
  ERRCTX_HR = ERRCTX_HRESULT,   // LOG_E(sev, HR, x)
  ERRCTX_OS = ERRCTX_OSSTATUS,  // LOG_E(sev, OS, x)
};

// Class that writes a log message to the logging delegate ("WebRTC logging
// stream" in Chrome) and to Chrome's logging stream.
class DiagnosticLogMessage {
 public:
  DiagnosticLogMessage(const char* file,
                       int line,
                       LoggingSeverity severity,
                       LogErrorContext err_ctx,
                       int err);
  DiagnosticLogMessage(const char* file,
                       int line,
                       LoggingSeverity severity,
                       LogErrorContext err_ctx,
                       int err,
                       const char* module);
  ~DiagnosticLogMessage();

  void CreateTimestamp();

  std::ostream& stream() { return print_stream_; }

 private:
  const char* file_name_;
  const int line_;
  const LoggingSeverity severity_;
  const LogErrorContext err_ctx_;
  const int err_;
  const char* const module_;
  const bool log_to_chrome_;

  std::ostringstream print_stream_;
};

// This class is used to explicitly ignore values in the conditional
// logging macros.  This avoids compiler warnings like "value computed
// is not used" and "statement has no effect".
class LogMessageVoidify {
 public:
  LogMessageVoidify() {}
  // This has to be an operator with a precedence lower than << but
  // higher than ?:
  void operator&(std::ostream&) {}
};

//////////////////////////////////////////////////////////////////////
// Logging Helpers
//////////////////////////////////////////////////////////////////////

class LogMessage {
 public:
  static void LogToDebug(int min_sev);
};

// TODO(grunell): Change name to InitDiagnosticLoggingDelegate or
// InitDiagnosticLogging. Change also in init_webrtc.h/cc.
// TODO(grunell): typedef the delegate function.
void InitDiagnosticLoggingDelegateFunction(
    void (*delegate)(const std::string&));

void SetExtraLoggingInit(
    void (*function)(void (*delegate)(const std::string&)));

}  // namespace rtc

#endif  // THIRD_PARTY_WEBRTC_OVERRIDES_WEBRTC_RTC_BASE_DIAGNOSTIC_LOGGING_H_
