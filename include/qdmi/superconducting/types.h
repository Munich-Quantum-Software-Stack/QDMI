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
/// Defines types used across the @ref superconducting_interface
/// "superconducting interface".

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// NOLINTBEGIN(modernize-use-using)

/// The function table of the @ref superconducting_interface "superconducting
/// interface".
///
/// A struct with function pointers to the functions of the @ref
/// superconducting_interface "superconducting interface". Each device
/// implementing the superconducting interface defines the actual implementation
/// of the struct.
///
/// @note The struct is defined in the @ref superconducting/functions.h header.
typedef struct QDMI_SC_Interface_impl_d QDMI_SC_Interface;

// Forward-declare opaque handles to prevent exposing internal structs

/// A handle for a qubit.
///
/// An opaque pointer to an implementation of the QDMI qubit concept. Each
/// implementation of the @ref superconducting_interface "superconducting
/// interface" defines the actual implementation of the concept.
///
/// A simple example of an implementation is a struct that merely contains the
/// qubit ID, which can be used to identify the qubit on the device.
/// ```
/// struct QDMI_SC_Qubit_impl_d {
/// size_t id;
/// };
/// ```
typedef struct QDMI_SCQubit_impl_d *QDMI_SCQubit;

/// A handle for an operation.
///
/// An opaque pointer to an implementation of the QDMI operation concept. An
/// operation generally represents any instruction that can be executed on a
/// device. This includes gates, measurements, classical control flow elements,
/// movement of qubits, pulse-level instructions, etc. Each implementation of
/// the @ref superconducting_interface "superconducting interface" defines the
/// actual implementation of the concept.
///
/// A simple example of an implementation is a struct that merely contains the
/// id of the operation, which is used to identify the operation.
/// ```
/// struct QDMI_SC_Operation_impl_d {
/// std::string id;
/// };
/// ```
typedef struct QDMI_SCOperation_impl_d *QDMI_SCOperation;

// NOLINTEND(modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
