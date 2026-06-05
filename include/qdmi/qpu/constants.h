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
/// Defines common constants and enums as part of the @ref qpu_interface "QPU
/// interface".

#ifndef QDMI_QPU_CONSTANTS_H
#define QDMI_QPU_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

// The following clang-tidy warnings cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(cppcoreguidelines-use-enum-class, performance-enum-size,
// modernize-use-using)

/// Enum of different statuses the QPU can be in.
typedef enum QDMI_QPU_STATUS_T {
  QDMI_QPU_STATUS_OFFLINE = 0,     ///< The QPU is offline.
  QDMI_QPU_STATUS_IDLE = 1,        ///< The QPU is idle.
  QDMI_QPU_STATUS_BUSY = 2,        ///< The QPU is busy.
  QDMI_QPU_STATUS_ERROR = 3,       ///< The QPU is in an error state.
  QDMI_QPU_STATUS_MAINTENANCE = 4, ///< The QPU is in maintenance.
  QDMI_QPU_STATUS_CALIBRATION = 5, ///< The QPU is in calibration.
} QDMI_QPU_Status;

// NOLINTEND(cppcoreguidelines-use-enum-class, performance-enum-size,
// modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif

#endif // QDMI_QPU_CONSTANTS_H
