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
/// Defines common macros, constants, and enums.

#ifndef QDMI_CORE_CONSTANTS_H
#define QDMI_CORE_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

// The following clang-tidy warnings cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(cppcoreguidelines-use-enum-class, performance-enum-size,
// modernize-use-using)

/// Packs the three parts of a semantic version into one integer.
#define QDMI_MAKE_VERSION(major, minor, patch)                                 \
  (((major) << 22) | ((minor) << 12) | (patch))

/// Extracts the major part of a version in the packed format.
#define QDMI_MAJOR_VERSION(version) ((version) >> 22)

/// Extracts the minor part from a version in the packed format.
#define QDMI_MINOR_VERSION(version) (((version) >> 12) & 0x3FF)

/// Extracts the patch part from a version in the packed format.
#define QDMI_PATCH_VERSION(version) ((version) & 0xFFF)

/// Status codes returned by the API.
enum QDMI_STATUS {
  QDMI_WARN_GENERAL = 1,            ///< A general warning.
  QDMI_SUCCESS = 0,                 ///< The operation was successful.
  QDMI_ERROR_FATAL = -1,            ///< A fatal error.
  QDMI_ERROR_OUTOFMEM = -2,         ///< Out of memory.
  QDMI_ERROR_NOTIMPLEMENTED = -3,   ///< Not implemented.
  QDMI_ERROR_NOTFOUND = -4,         ///< Element not found.
  QDMI_ERROR_OUTOFRANGE = -5,       ///< Out of range.
  QDMI_ERROR_INVALIDARGUMENT = -6,  ///< Invalid argument.
  QDMI_ERROR_PERMISSIONDENIED = -7, ///< Permission denied.
  QDMI_ERROR_NOTSUPPORTED = -8,     ///< Operation is not supported.
  /// Resource is in the wrong state for the operation.
  QDMI_ERROR_BADSTATE = -9,
  QDMI_ERROR_TIMEOUT = -10,         ///< Operation timed out.
  QDMI_ERROR_VERSIONMISMATCH = -11, ///< Version mismatch.
};

/// Enum of authentication options a device supports.
typedef enum QDMI_AUTHENTICATION_OPTION_T {
  /// No authentication required.
  QDMI_AUTHENTICATION_OPTION_NONE = 0,
  /// Authentication via a token is required.
  QDMI_AUTHENTICATION_OPTION_TOKEN = 1,
  /// Authentication via a username and password is required.
  QDMI_AUTHENTICATION_OPTION_PW = 2,
  /// Authentication via an authentication server with the provided URL is
  /// required.
  QDMI_AUTHENTICATION_OPTION_URL = 3,
  /// Authentication via an authentication server with the provided URL and
  /// token is required.
  /// @see QDMI_AUTHENTICATION_OPTION_TOKEN
  /// @see QDMI_AUTHENTICATION_OPTION_URL.
  QDMI_AUTHENTICATION_OPTION_URLTOKEN = 4,
  /// Authentication via an authentication server with the provided URL,
  /// username and password is required.
  /// @see QDMI_AUTHENTICATION_OPTION_URL
  /// @see QDMI_AUTHENTICATION_OPTION_PW
  QDMI_AUTHENTICATION_OPTION_URLPW = 5,
} QDMI_Authentication_Option;

/// Log levels for the QDMI logging callback.
typedef enum QDMI_LOG_LEVEL_T {
  QDMI_LOG_LEVEL_TRACE,   ///< Detailed trace information.
  QDMI_LOG_LEVEL_DEBUG,   ///< Debug-level messages.
  QDMI_LOG_LEVEL_INFO,    ///< Informational messages.
  QDMI_LOG_LEVEL_WARNING, ///< Warning messages.
  QDMI_LOG_LEVEL_ERROR,   ///< Error messages.
  QDMI_LOG_LEVEL_FATAL,   ///< Fatal error messages.
} QDMI_Log_Level;

// NOLINTEND(cppcoreguidelines-use-enum-class, performance-enum-size,
// modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif

#endif // QDMI_CORE_CONSTANTS_H
