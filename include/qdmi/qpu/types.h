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
/// @brief Defines types of the @ref qpu_interface "QPU interface".

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// The following clang-tidy warning cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(modernize-use-using)

/// The function table of the @ref qpu_interface "QPU interface".
///
/// A struct with function pointers to the functions of the @ref qpu_interface
/// "QPU interface". This also includes the @ref job_interface "job interface".
///
/// @note The struct is defined in the @ref qpu/functions.h header.
typedef struct QDMI_QPU_Interface_impl_d QDMI_QPU_Interface;

// NOLINTEND(modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
