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
#include "qdmi/orchestration_layer.h"

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
      (QDMI_initialize_t *)dlsym(dylib, "O_QDMI_initialize");

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
  int command = 0;
  while (printf("Which QPU should the job run on [?,1,2,3,4,q]? ") &&
         ((command = getchar()) != EOF)) {
    // discard the rest of the line (including the '\n' from pressing Enter)
    int ch = 0;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
    const char *id = NULL;
    if (command == '1') {
      id = "q1";
    } else if (command == '2') {
      id = "q2";
    } else if (command == '3') {
      id = "q3";
    } else if (command == '4') {
      id = "V1";
    } else if (command == 'q') {
      break;
    } else if (command == '?') {
      printf("You have an active session with an orchestration layer that has\n"
             "access to three QPUs:\n"
             "1 - a QPU (ID: \"q1\") that is directly connected to the\n"
             "    orchestration layer loaded via `dlopen` by the\n"
             "    orchestration itself. This QPU corresponds to the CMake\n"
             "    target `qpu_q1`.\n"
             "2 - a QPU (ID: \"q2\") that is connected to the orchestration\n"
             "    layer via a provider. The QPU with CMake target `qpu_q2` is\n"
             "    loaded via `dlopen` by the provider. The provider\n"
             "    corresponding to CMake target `provider_p1`, in turn, is\n"
             "    loaded by the orchestration layer via `dlopen`.\n"
             "3 - a QPU (ID: \"q3\") that is part of another orchestration\n"
             "    layer corresponding to CMake target\n"
             "    `orchestration_layer_o2`. This separate orchestration layer\n"
             "    loads the QPU corresponding to CMake target `qpu_q3` and\n"
             "    exposes it to this orchestration layer.\n"
             "4 - a QPU (ID: \"V1\") that is to the orchestration layer\n"
             "    through two nested providers. The directly connected\n"
             "    provider is the same as for '2'. The second provider is an"
             "    adapter that corresponds to the CMake target `adapter`. It\n"
             "    loads a QDMIv1 device and exposes it as a QDMIv2 device.\n"
             "After selecting the QPU, the same job is created with the\n"
             "target device set accordingly. The job is submitted and\n"
             "(random) results are retrieved and printed to the console.\n");
      continue;
    } else {
      printf("Invalid command. Type '?' for help, '1', '2', '3', or '4' to "
             "select the a QPU, or 'q' to quit.\n");
      continue;
    }
    char qpu_prefix[8];
    snprintf(qpu_prefix, sizeof(qpu_prefix), "qpu%c   ", command);
    char job_prefix[8];
    snprintf(job_prefix, sizeof(job_prefix), "job%c   ", command);
    //===------------------------------------------------------------------===//
    //                             Listing 3
    //===------------------------------------------------------------------===//
    // query orchestration layer module 'ol'
    // and get its function table
    QDMI_Module m = NULL;
    core->context_query_module_by_id(ctx, "ol", &m);
    const QDMI_OrchestrationLayer_Interface *ol = NULL;
    core->context_get_module_interface(ctx, m, (const void **)&ol);

    // query QPU, get its context and function table
    QDMI_Device q = NULL;
    QDMI_Context q_ctx = NULL;
    const QDMI_Core_Interface *q_core = NULL;
    ol->session_query_device_by_id(session, id, &q);
    ol->session_query_device_context(session, q, &q_ctx);
    ol->session_get_device_interface(session, q, &q_core);

    // open and authenticate a session
    QDMI_Session q_session = NULL;
    q_core->context_allocate_session(q_ctx, Log_callback, qpu_prefix,
                                     &q_session);
    // q_core->session_set_token(q_session, "abc..."); // not required here
    q_core->session_initialize(q_session);
    //===----------------------------------------------------------------===//
    //===------------------------------------------------------------------===//
    //          Listing 2 (adapted for the orchestration layer)
    //===------------------------------------------------------------------===//
    // negotiate program format
    QDMI_Program_Format fmt = NULL;
    if (ol->session_query_program_format_by_id_and_version(
            session, "qasm", VERSION(3, 0, 0), &fmt) != QDMI_SUCCESS) {
      return -1; // abort with error
    }

    // create, configure, and submit a job
    QDMI_Job job = NULL;
    ol->session_create_job(session, Log_callback, job_prefix, &job);
    const char *ps[] = {
        "include \"stdgates.inc\";\nqubit[2] q;\nh q[0];\ncx q[0],q[1];"};
    ol->job_set_payload_string(job, fmt, 1, ps);
    ol->job_set_shot_count(job, 100);
    ol->job_set_device(job, q);
    ol->job_submit(job);

    // wait and retrieve results
    ol->job_wait(job, /*timeout_s=*/10);
    size_t result_size = 0;
    ol->job_get_shots(job, /*prog_idx=*/0, 0, NULL, &result_size);
    char *result = malloc(result_size);
    ol->job_get_shots(job, /*prog_idx=*/0, result_size, result, NULL);
    printf("%s\n", result); // '11000,00000,11000,...'
    //===------------------------------------------------------------------===//
    // free resources
    free(result);
    ol->job_free(job);
    q_core->session_free(q_session);
  }
  // free resources
  core->session_free(session);
  lib->context_finalize(ctx);
  DL_CLOSE(dylib);

  return 0;
}

#undef VERSION
#undef DL_CLOSE
#undef DL_SYM
#undef DL_OPEN
