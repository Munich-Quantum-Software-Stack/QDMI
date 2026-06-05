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
/// Includes all elements necessary for the @ref core_interface "core interface"
/// and defines the entry point to the library.

#pragma once

#include "qdmi/core/constants.h" // IWYU pragma: export
#include "qdmi/core/functions.h" // IWYU pragma: export
#include "qdmi/core/types.h"     // IWYU pragma: export

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// NOLINTBEGIN(modernize-use-using)

/// Retrieve a handle to the function pointers of the @ref core_interface "core
/// interface".
///
/// This is the *only* function that needs to be loaded via `dlsym` (or
/// similar). All other functions of QDMI are provided with the retrieved
/// interface handle.
///
/// @param version The QDMI version to request the library for. The version must
/// be provided in packed format. A version `x.y.z` is transformed to an integer
/// via @code (x << 22) | (y << 12) | z@endcode using the macro @ref
/// QDMI_MAKE_VERSION.
/// @param[in] callback is the callback function to use for logging.
/// @param[in] user_data is a pointer to user-defined data that will be passed
/// to the callback function.
/// @param context is a pointer to a variable where the context handle will be
/// stored. Must not be @c NULL.
/// @param interface_ptr The pointer to a variable where the core interface
/// pointer will be stored. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the core interface was retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p interface_ptr is @c NULL.
/// @returns @ref QDMI_ERROR_VERSIONMISMATCH if the arguments are valid and the
/// requested version is not supported by the library.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @note This is the only function that must be loaded via @c dlsym (or
/// similar). Then, this function facilitates the retrieval of function pointers
/// to all other functions of the interface. This saves additional lookups in
/// the symbol table of the dynamic library and returns handles to the interface
/// functions in constant time.
///
/// As a consequence, this provides good encapsulation of the interface's
/// implementation. In particular, this approach only requires one function to
/// be exported by the dynamic library. All other functions can have internal
/// linkage (declared as @c static or with hidden visibility). Additionally,
/// this prevents the dynamic symbol table from being cluttered with dozens or
/// hundreds of function names, keeping it minimal and clean. Hence, when
/// multiple libraries are loaded, having all functions with internal linkage
/// means there's no risk of name conflicts between different libraries. Only
/// the single entry point needs a globally unique name.
///
/// Furthermore, to prevent the need to copy the struct with the function
/// pointers and enable it to reside in the dynamic library's memory space, the
/// parameter @p interface_ptr is a pointer to a pointer. This facilitates
/// writing the pointer to the @ref QDMI_Core_Interface_impl_d struct stored in
/// the dynamic library to the user's memory space.
typedef int QDMI_initialize_t(size_t version, QDMI_Log_Callback callback,
                              void *user_data, QDMI_Context *context,
                              QDMI_Library const **interface_ptr);
extern QDMI_initialize_t QDMI_initialize;

// only define the get_prefix function if this is not a static library, as it is
// only necessary for dynamic libraries to retrieve the prefix for name-shifting
// the function symbols.
#ifndef QDMI_STATIC_DEFINE
/// Retrieve the prefix used for name-shifting the @ref QDMI_initialize
/// function.
///
/// @remark This function is purposefully not name-shifted.
///
/// @returns The prefix used for name-shifting the @ref QDMI_initialize
/// function.
// NOLINTNEXTLINE(modernize-redundant-void-arg)
typedef const char *QDMI_get_prefix_t(void);
extern QDMI_get_prefix_t QDMI_get_prefix;
#endif // QDMI_STATIC_DEFINE

// NOLINTEND(modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
