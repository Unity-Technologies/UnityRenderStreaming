// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Disallowed because noparent removes the +allowed from the parent dir.
#include "checkdeps/testdata/allowed/bad.h"

// Same-directory includes are still allowed.
#include "checkdeps/testdata/noparent/self.h"
