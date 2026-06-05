/*
 * Copyright (c) 2024 - 2026 QDMI Maintainers
 * All rights reserved.
 *
 * Licensed under the Apache License v2.0 with LLVM Exceptions (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://llvm.org/LICENSE.txt
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "qdmi/core.h"

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif // _WIN32

#ifdef _WIN32
#define DL_OPEN(lib) LoadLibraryA((lib))
#define DL_SYM(lib, sym)                                                       \
  reinterpret_cast<void *>(GetProcAddress(static_cast<HMODULE>((lib)), (sym)))
#define DL_CLOSE(lib) FreeLibrary(static_cast<HMODULE>((lib)))
#else
#define DL_OPEN(lib, flags) dlopen((lib), (flags))
#define DL_SYM(lib, sym) dlsym((lib), (sym))
#define DL_CLOSE(lib) dlclose((lib))
#endif

#define ANSI_RED "\x1b[0;31m"
#define ANSI_GREEN "\x1b[0;32m"
#define ANSI_RESET "\x1b[0m"

// use `static` to enforce internal linkage
static void Log_callback(const QDMI_Log_Level level, const char *message,
                         void *user_data) {
  printf("[%s] ", (char *)user_data);
  switch (level) {
  case QDMI_LOG_LEVEL_ERROR:
    printf("[" ANSI_RED "error" ANSI_RESET "]");
    break;
  case QDMI_LOG_LEVEL_INFO:
    printf("[" ANSI_GREEN "info" ANSI_RESET "]");
    break;
  default:
    break;
  }
  printf(" %s\n", message);
}

int main(void) {
  //===--------------------------------------------------------------------===//
  //                              Listing 1
  //===--------------------------------------------------------------------===//
  // dynamically load the shared library
  void *dylib = DL_OPEN(QDMI_DYLIB, RTLD_NOW | RTLD_LOCAL);
  QDMI_initialize_t *init_fn =
      (QDMI_initialize_t *)dlsym(dylib, "Q_QDMI_initialize");

  // initialize the library
  QDMI_Context ctx = NULL;
  const QDMI_Library *lib = NULL;
  init_fn(QDMI_VERSION, &Log_callback, "qdmi   ", &ctx, &lib);

  // get the core interface function table
  const QDMI_Core_Interface *core = NULL;
  lib->get_interface(&core);

  // open and authenticate a session
  QDMI_Session session = NULL;
  core->context_allocate_session(ctx, &Log_callback, "session", &session);
  core->session_set_token(session, "abc...");
  core->session_initialize(session);
  //===--------------------------------------------------------------------===//
  // free resources
  core->session_free(session);
  lib->context_finalize(ctx);
  DL_CLOSE(dylib);

  return 0;
}

#undef DL_CLOSE
#undef DL_SYM
#undef DL_OPEN
