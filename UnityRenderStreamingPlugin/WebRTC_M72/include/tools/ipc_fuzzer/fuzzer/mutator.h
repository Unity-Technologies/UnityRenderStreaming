// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TOOLS_IPC_FUZZER_MUTATE_MUTATOR_H_
#define TOOLS_IPC_FUZZER_MUTATE_MUTATOR_H_

#include <stddef.h>
#include <stdint.h>

#include <string>

#include "tools/ipc_fuzzer/fuzzer/fuzzer.h"

namespace ipc_fuzzer {

class Mutator : public Fuzzer {
 public:
  explicit Mutator(unsigned int frequency) : frequency_(frequency) {}
  virtual ~Mutator() {}

  void FuzzBool(bool* value) override;
  void FuzzInt(int* value) override;
  void FuzzLong(long* value) override;
  void FuzzSize(size_t* value) override;
  void FuzzUChar(unsigned char* value) override;
  void FuzzWChar(wchar_t* value) override;
  void FuzzUInt16(uint16_t* value) override;
  void FuzzUInt32(uint32_t* value) override;
  void FuzzInt64(int64_t* value) override;
  void FuzzUInt64(uint64_t* value) override;
  void FuzzFloat(float* value) override;
  void FuzzDouble(double* value) override;
  void FuzzString(std::string* value) override;
  void FuzzString16(base::string16* value) override;
  void FuzzData(char* data, int length) override;
  void FuzzBytes(void* data, int data_len) override;
  bool ShouldGenerate() override;

 private:
  // TODO(mbarbella): Use double frequencies.
  unsigned int frequency_;
};

}  // namespace ipc_fuzzer

#endif  // TOOLS_IPC_FUZZER_MUTATE_MUTATOR_H_
