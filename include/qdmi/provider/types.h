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
/// Defines types used across the @ref provider_interface "provider interface".

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// NOLINTBEGIN(modernize-use-using)

/// The function table of the @ref provider_interface "provider interface".
typedef struct QDMI_Provider_Interface_impl_d QDMI_Provider_Interface;

// Forward-declare opaque handles to prevent exposing internal structs

/// A handle for a managed device.
///
/// An opaque pointer to a type defined by the provider that encapsulates all
/// information about a device managed by the provider. This includes, for
/// example, the function table for the device's core interface.
typedef struct QDMI_Device_impl_d *QDMI_Device;

// NOLINTEND(modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
