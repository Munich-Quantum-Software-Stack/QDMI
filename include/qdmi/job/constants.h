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

/// @file
/// Defines common constants and enums as part of the @ref job_interface "job
/// interface".

#ifndef QDMI_JOB_CONSTANTS_H
#define QDMI_JOB_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

// The following clang-tidy warnings cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(cppcoreguidelines-use-enum-class, performance-enum-size,
// modernize-use-using)

/// Enum of the status a job can have. @details See also @ref job_interface
/// "job interface" for a description of
/// the job's lifecycle.
typedef enum QDMI_JOB_STATUS_T {
  /// The job was created.
  QDMI_JOB_STATUS_CREATED = 0,
  /// The job was submitted.
  QDMI_JOB_STATUS_SUBMITTED = 1,
  /// The job was received and is waiting to be executed.
  QDMI_JOB_STATUS_QUEUED = 2,
  /// The job is running, and the result is not yet available.
  QDMI_JOB_STATUS_RUNNING = 3,
  /// The job is done, and the result can be retrieved.
  QDMI_JOB_STATUS_DONE = 4,
  /// The job was canceled, and the result is not available.
  QDMI_JOB_STATUS_CANCELED = 5,
  /// An error occurred in the job's lifecycle.
  QDMI_JOB_STATUS_FAILED = 6,
} QDMI_Job_Status;

// NOLINTEND(cppcoreguidelines-use-enum-class, performance-enum-size,
// modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif

#endif // QDMI_JOB_CONSTANTS_H
