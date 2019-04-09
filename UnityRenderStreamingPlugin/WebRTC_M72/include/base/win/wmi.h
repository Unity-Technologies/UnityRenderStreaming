// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// WMI (Windows Management and Instrumentation) is a big, complex, COM-based
// API that can be used to perform all sorts of things. Sometimes is the best
// way to accomplish something under windows but its lack of an approachable
// C++ interface prevents its use. This collection of functions is a step in
// that direction.
// There are two classes; WMIUtil and WMIProcessUtil. The first
// one contains generic helpers and the second one contains the only
// functionality that is needed right now which is to use WMI to launch a
// process.
// To use any function on this header you must call CoInitialize or
// CoInitializeEx beforehand.
//
// For more information about WMI programming:
// http://msdn2.microsoft.com/en-us/library/aa384642(VS.85).aspx

#ifndef BASE_WIN_WMI_H_
#define BASE_WIN_WMI_H_

#include <wbemidl.h>
#include <wrl/client.h>

#include "base/base_export.h"
#include "base/strings/string16.h"
#include "base/strings/string_piece.h"

namespace base {
namespace win {

// Creates an instance of the WMI service connected to the local computer and
// returns its COM interface. If |set_blanket| is set to true, the basic COM
// security blanket is applied to the returned interface. This is almost
// always desirable unless you set the parameter to false and apply a custom
// COM security blanket.
// Returns true if succeeded and |wmi_services|: the pointer to the service.
BASE_EXPORT bool CreateLocalWmiConnection(
    bool set_blanket,
    Microsoft::WRL::ComPtr<IWbemServices>* wmi_services);

// Creates a WMI method using from a WMI class named |class_name| that
// contains a method named |method_name|. Only WMI classes that are CIM
// classes can be created using this function.
// Returns true if succeeded and |class_instance| returns a pointer to the
// WMI method that you can fill with parameter values using SetParameter.
BASE_EXPORT bool CreateWmiClassMethodObject(
    IWbemServices* wmi_services,
    const StringPiece16& class_name,
    const StringPiece16& method_name,
    Microsoft::WRL::ComPtr<IWbemClassObject>* class_instance);

// Fills a single parameter given an instanced |class_method|. Returns true
// if the operation succeeded. When all the parameters are set the method can
// be executed using IWbemServices::ExecMethod().
BASE_EXPORT bool SetWmiClassMethodParameter(IWbemClassObject* class_method,
                                            const StringPiece16& parameter_name,
                                            VARIANT* parameter);

// Creates a new process from |command_line|. The advantage over CreateProcess
// is that it allows you to always break out from a Job object that the caller
// is attached to even if the Job object flags prevent that.
// Returns true and the process id in process_id if the process is launched
// successful. False otherwise.
// Note that a fully qualified path must be specified in most cases unless
// the program is not in the search path of winmgmt.exe.
// Processes created this way are children of wmiprvse.exe and run with the
// caller credentials.
// More info: http://msdn2.microsoft.com/en-us/library/aa394372(VS.85).aspx
BASE_EXPORT bool WmiLaunchProcess(const string16& command_line,
                                  int* process_id);

// This class contains functionality of the WMI class 'Win32_ComputerSystem'.
// More info: http://msdn.microsoft.com/en-us/library/aa394102(VS.85).aspx
class BASE_EXPORT WmiComputerSystemInfo {
 public:
  static WmiComputerSystemInfo Get();

  const string16& manufacturer() const { return manufacturer_; }
  const string16& model() const { return model_; }

 private:
  string16 manufacturer_;
  string16 model_;
};

}  // namespace win
}  // namespace base

#endif  // BASE_WIN_WMI_H_
