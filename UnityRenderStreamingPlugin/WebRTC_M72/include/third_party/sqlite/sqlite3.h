// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_SQLITE_SQLITE3_H_
#define THIRD_PARTY_SQLITE_SQLITE3_H_

// This is a shim header to include the right sqlite3 headers.
// Use this instead of referencing sqlite3 headers directly.

// We prefix chrome_ to SQLite's exported symbols, so that we don't clash with
// other SQLite libraries loaded by the system libraries. This only matters when
// using the component build, where our SQLite's symbols are visible to the
// dynamic library loader.
#include "third_party/sqlite/amalgamation/rename_exports.h"
#include "third_party/sqlite/amalgamation/sqlite3.h"

#endif  // THIRD_PARTY_SQLITE_SQLITE3_H_
