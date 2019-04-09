// Copyright 2013 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#ifndef LIBLOUIS_NACL_LIBLOUIS_MODULE_H_
#define LIBLOUIS_NACL_LIBLOUIS_MODULE_H_

#include "base/macros.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"

namespace liblouis_nacl {

// Native Client module which contains liblouis.
class LibLouisModule : public pp::Module {
 public:
  LibLouisModule();
  virtual ~LibLouisModule();

  virtual pp::Instance* CreateInstance(PP_Instance instance);

 private:
  DISALLOW_COPY_AND_ASSIGN(LibLouisModule);
};

}  // namespace liblouis_nacl

#endif  // LIBLOUIS_NACL_LIBLOUIS_MODULE_H_
