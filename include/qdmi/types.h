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

/** @file
 * @brief Defines all types used within QDMI across the @ref client_interface
 * and the @ref device_interface.
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
 * Those atoms are then used as qubits. To this end, sites are generalizations
 * of qubits that denote locations where qubits can be placed on a device.
 * Each implementation of the @ref device_interface "QDMI Device Interface"
 * defines the actual implementation of the concept.
 *
 * A simple example of an implementation is a struct that merely contains the
 * site ID, which can be used to identify the site.
 * @code{.cpp}
 * struct QDMI_Site_impl_d {
 *   size_t id;
 * };
 * @endcode
 */
typedef struct QDMI_Site_impl_d *QDMI_Site;

/**
 * @brief A handle for an operation.
 * @details An opaque pointer to an implementation of the QDMI operation
 * concept. An operation generally represents any instruction that can be
 * executed on a device. This includes gates, measurements, classical control
 * flow elements, movement of qubits, pulse-level instructions, etc.
 * Each implementation of the @ref device_interface "QDMI Device Interface"
 * defines the actual implementation of the concept.
 *
 * A simple example of an implementation is a struct that merely contains the
 * name of the operation, which can be used to identify the operation.
 * @code{.cpp}
 * struct QDMI_Operation_impl_d {
 *   std::string name;
 * };
 * @endcode
 */
typedef struct QDMI_Operation_impl_d *QDMI_Operation;

/**
 * @brief A handle for a child device.
 * @details An opaque pointer to an implementation of the QDMI child device
 * concept. A child device generally represents a core or processing unit
 * of a multi-core device.
 * Each implementation of the @ref device_interface "QDMI Device Interface"
 * defines the actual implementation of the concept.
 *
 * A simple example of an implementation is a struct that merely contains an
 * index, which can be used to identify the respective core / processing unit.
 * @code{.cpp}
 * struct QDMI_Child_Device_impl_d {
 *   size_t id;
 * };
 * @endcode
 * @see QDMI_DEVICE_PROPERTY_CHILDDEVICES
 * @see QDMI_DEVICE_SESSION_PARAMETER_CHILDDEVICE
 */
typedef struct QDMI_Child_Device_impl_d *QDMI_Child_Device;

// NOLINTEND(modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
