/*
 * Copyright (C) 2006, 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Justin Haygood (jhaygood@reaktix.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBDATABASE_SQLITE_SQLITE_DATABASE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBDATABASE_SQLITE_SQLITE_DATABASE_H_

#include "base/macros.h"
#include "build/build_config.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"
#include "third_party/blink/renderer/platform/wtf/text/cstring.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/threading.h"
#include "third_party/blink/renderer/platform/wtf/threading_primitives.h"

struct sqlite3;

namespace blink {

class DatabaseAuthorizer;
class SQLiteTransaction;

extern const int kSQLResultDone;
extern const int kSQLResultOk;
extern const int kSQLResultRow;
extern const int kSQLResultFull;
extern const int kSQLResultInterrupt;
extern const int kSQLResultConstraint;

class SQLiteDatabase {
  DISALLOW_NEW();
  friend class SQLiteTransaction;

 public:
  SQLiteDatabase();
  ~SQLiteDatabase();

  bool Open(const String& filename);
  bool IsOpen() const { return db_; }
  void Close();

  void UpdateLastChangesCount();

  bool ExecuteCommand(const String&);

  bool TableExists(const String&);
  int RunVacuumCommand();
  int RunIncrementalVacuumCommand();

  bool TransactionInProgress() const { return transaction_in_progress_; }

  int64_t LastInsertRowID();
  int LastChanges();

  void SetBusyTimeout(int ms);

  // Sets the maximum size in bytes.
  // Depending on per-database attributes, the size will only be settable in
  // units that are the page size of the database, which is established at
  // creation.  These chunks will never be anything other than 512, 1024, 2048,
  // 4096, 8192, 16384, or 32768 bytes in size.  setMaximumSize() will round the
  // size down to the next smallest chunk if the passed size doesn't align.
  void SetMaximumSize(int64_t);

  // Gets the number of unused bytes in the database file.
  int64_t FreeSpaceSize();
  int64_t TotalSize();

  int LastError();
  const char* LastErrorMsg();

  sqlite3* Sqlite3Handle() const {
#if DCHECK_IS_ON()
    DCHECK_EQ(!!CurrentThread(), opening_thread_ || !db_);
#endif
    return db_;
  }

  void SetAuthorizer(DatabaseAuthorizer*);

  bool IsAutoCommitOn() const;

  // The SQLite AUTO_VACUUM pragma can be either NONE, FULL, or INCREMENTAL.
  // NONE - SQLite does not do any vacuuming
  // FULL - SQLite moves all empty pages to the end of the DB file and truncates
  //        the file to remove those pages after every transaction. This option
  //        requires SQLite to store additional information about each page in
  //        the database file.
  // INCREMENTAL - SQLite stores extra information for each page in the database
  //               file, but removes the empty pages only when PRAGMA
  //               INCREMANTAL_VACUUM is called.
  enum AutoVacuumPragma {
    kAutoVacuumNone = 0,
    kAutoVacuumFull = 1,
    kAutoVacuumIncremental = 2
  };
  bool TurnOnIncrementalAutoVacuum();

  void Trace(blink::Visitor* visitor) {}

 private:
  static int AuthorizerFunction(void*,
                                int,
                                const char*,
                                const char*,
                                const char*,
                                const char*);

  void EnableAuthorizer(bool enable);

  int PageSize();

  sqlite3* db_;
  int page_size_;

  bool transaction_in_progress_;

  Mutex authorizer_lock_;
  CrossThreadPersistent<DatabaseAuthorizer> authorizer_;

  ThreadIdentifier opening_thread_;

  Mutex database_closing_mutex_;

  int open_error_;
  CString open_error_message_;

  int last_changes_count_;

  DISALLOW_COPY_AND_ASSIGN(SQLiteDatabase);
};

}  // namespace blink

#endif
