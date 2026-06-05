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
/// Defines types for the @ref job_interface "job interface".

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// The following clang-tidy warning cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(modernize-use-using)

// Forward-declare opaque handles to prevent exposing internal structs

/// A handle for a program format. @details An opaque pointer to a type defined
/// by the device that encapsulates
/// all information about a program format supported by the device. Example
/// program formats are listed below with their ID:
/// - OpenQASM 2.0: `id = "qasm"`, `version = QDMI_MAKE_VERSION(2, 0, 0)`
/// - OpenQASM 3.0: `id = "qasm"`, `version = QDMI_MAKE_VERSION(3, 0, 0)`
/// - QIR: `id = "qir"`, `version = QDMI_MAKE_VERSION(1, 0, 0)`
typedef struct QDMI_Program_Format_impl_d *QDMI_Program_Format;

/// A handle for a job. @details An opaque pointer to a type defined by the
/// device that encapsulates
/// all information about a job on a device.
/// @remark Implementations of the underlying type will want to store the
/// session handle used to create the job in the job handle to be able to access
/// the session information when needed.
typedef struct QDMI_Job_impl_d *QDMI_Job;

// NOLINTEND(modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
