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
#include "qdmi/qpu.h"

#include <stdio.h>
#include <stdlib.h>

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

#define VERSION QDMI_MAKE_VERSION

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
  //                 Listing 1 (identical to bootstrapping.cpp)
  //===--------------------------------------------------------------------===//
  // dynamically load the shared library
  void *dylib = DL_OPEN(QDMI_DYLIB, RTLD_NOW | RTLD_LOCAL);
  QDMI_initialize_t *init_fn =
      (QDMI_initialize_t *)dlsym(dylib, "Q_QDMI_initialize");

  // initialize the library
  QDMI_Context ctx = NULL;
  const QDMI_Library *lib = NULL;
  init_fn(QDMI_VERSION, Log_callback, "qdmi   ", &ctx, &lib);

  // get the core interface function table
  const QDMI_Core_Interface *core = NULL;
  lib->get_interface(&core);

  // open and authenticate a session
  QDMI_Session session = NULL;
  core->context_allocate_session(ctx, Log_callback, "session", &session);
  core->session_set_token(session, "abc...");
  core->session_initialize(session);
  //===--------------------------------------------------------------------===//
  //===--------------------------------------------------------------------===//
  //                              Listing 2
  //===--------------------------------------------------------------------===//
  // query 'qpu' module and get its function table
  QDMI_Module m = NULL;
  core->context_query_module_by_id(ctx, "qpu", &m);
  const QDMI_QPU_Interface *qpu = NULL;
  core->context_get_module_interface(ctx, m, (const void **)&qpu);

  // negotiate program format
  QDMI_Program_Format fmt = NULL;
  if (qpu->session_query_program_format_by_id_and_version(
          session, "qasm", VERSION(3, 0, 0), &fmt) != QDMI_SUCCESS) {
    return -1; // abort with error
  }

  // create, configure, and submit a job
  QDMI_Job job = NULL;
  qpu->session_create_job(session, Log_callback, "job    ", &job);
  const char *ps[] = {
      "include \"stdgates.inc\";\nqubit[2] q;\nh q[0];\ncx q[0],q[1];"};
  qpu->job_set_payload_string(job, fmt, 1, ps);
  qpu->job_set_shot_count(job, 100);
  qpu->job_submit(job);

  // wait and retrieve results
  qpu->job_wait(job, /*timeout_s=*/10);
  size_t nq = 0; // num. qubits
  qpu->session_query_qubit_count(session, &nq);
  char *result = malloc((nq + 1) * /*n_shots=*/100);
  qpu->job_get_shots(job, /*prog_idx=*/0, (nq + 1) * 100, result, NULL);
  printf("%s\n", result); // '11000,00000,11000,...'
  //===--------------------------------------------------------------------===//
  // free resources
  free(result);
  qpu->job_free(job);
  core->session_free(session);
  lib->context_finalize(ctx);
  DL_CLOSE(dylib);

  return 0;
}

#undef VERSION
#undef DL_CLOSE
#undef DL_SYM
#undef DL_OPEN
