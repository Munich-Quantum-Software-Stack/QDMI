/*------------------------------------------------------------------------------
Copyright 2024 Munich Quantum Software Stack Project

Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
"License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at

https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations under
the License.

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

/** @file
 * @brief Defines all types used within QDMI that are shared across the client
 * and device interfaces.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// The following clang-tidy warning cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(modernize-use-using)

/**
 * @brief A handle for a site.
 * @details An opaque pointer to an implementation of the QDMI site concept.
 * A site is a place that can potentially hold a qubit. In case of
 * superconducting qubits, sites can be used synonymously with qubits. In case
 * of neutral atoms, sites represent individual traps that can confine atoms.
 * Those atoms are then used as qubits. To this end, sites are a generalization
 * of qubits that denote locations where qubits can be placed on a device.
 * The actual implementation of the concept is defined by each device.
 */
typedef struct QDMI_Site_impl_d *QDMI_Site;

/**
 * @brief A handle for an operation.
 * @details An opaque pointer to an implementation of the QDMI operation
 * concept. An operation generally represents any instruction that can be
 * executed on a device. This includes gates, measurements, classical control
 * flow elements, movement of qubits, pulse-level instructions, etc.
 * The actual implementation of the concept is defined by each device.
 */
typedef struct QDMI_Operation_impl_d *QDMI_Operation;

// NOLINTEND(modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
