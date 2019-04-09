// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef BASE_WIN_ATL_H_
#define BASE_WIN_ATL_H_

// Check no prior poisonous defines were made.
#include "base/win/windows_defines.inc"
// Undefine before windows header will make the poisonous defines
#include "base/win/windows_undefines.inc"

#ifndef _ATL_NO_EXCEPTIONS
#define _ATL_NO_EXCEPTIONS
#endif

#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlhost.h>
#include <atlsecurity.h>
#include <atlwin.h>

// Undefine the poisonous defines
#include "base/win/windows_undefines.inc"
// Check no poisonous defines follow this include
#include "base/win/windows_defines.inc"

#endif  // BASE_WIN_ATL_H_
