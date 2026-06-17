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
/// @brief Defines types of the @ref core_interface "core interface".

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// The following clang-tidy warning cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(modernize-use-using)

// Forward-declare interface function tables to avoid circular dependencies.

/// The function table of the @ref core_interface "core interface".
///
/// A struct with function pointers to the functions of the @ref core_interface
/// "core interface".
///
/// @note The struct is defined in the @ref core/functions.h header.
typedef struct QDMI_Core_Interface_impl_d QDMI_Core_Interface;

/// The function table of the @ref library_interface "library interface".
///
/// A struct with function pointers to the functions of the @ref
/// library_interface "library interface". In contrast to the @ref
/// core_interface "core interface function table", these functions are not
/// meant to be exposed to any third-party device.
///
/// @note The struct is defined in the @ref core/functions.h header.
typedef struct QDMI_Library_impl_d QDMI_Library;

// Forward-declare opaque pointers to prevent exposing internal structs

/// The context handle.
///
/// An opaque pointer to a type defined by the device that encapsulates all
/// information about a context. This may include, for example, global
/// information about the device, such as the available modules.
typedef struct QDMI_Context_impl_d *QDMI_Context;

/// The module handle.
///
/// An opaque pointer to a type defined by the device that encapsulates all
/// information about a module.
///
/// It follows a list of standard modules for different devices that are
/// provided with the default QDMI preceded by their corresponding IDs:
/// - `"qpu"`: The module for a quantum processing unit (QPU).
/// - `"provider"`: The module for a provider that manages multiple devices.
/// - `"ol"`: The module for an orchestration layer (ol) that is the entry point
///   to another HPC center's software stack.
///
/// The following is a list of technology-specific modules:
/// - `"sc"`: The module for superconducting qubit QPUs.
/// - `"na"`: The module for neutral atom QPUs.
typedef struct QDMI_Module_impl_d *QDMI_Module;

/// The session handle.
///
/// An opaque pointer to a type defined by the device that encapsulates all
/// information about a session between a device and its client.
typedef struct QDMI_Session_impl_d *QDMI_Session;

/// Logging callback function pointer type.
///
/// @param[in] level is the log level of the message.
/// @param[in] message is the null-terminated log message.
/// @param[in] user_data is a pointer to user-defined data, provided when the
/// callback was set.
///
/// @note These functions must usually be thread-safe on the client side, as
/// they may be called asynchronously by the device.
typedef void (*QDMI_Log_Callback)(QDMI_Log_Level level, const char *message,
                                  void *user_data);

// NOLINTEND(modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
