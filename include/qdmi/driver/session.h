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
 * @brief The QDMI session interface.
 * @details The driver manages resources provided to clients in sessions. This
 * header provides the interface for these sessions.
 */

#pragma once

#include "qdmi/common/types.h"
#include "qdmi/driver/types.h"

#ifdef __cplusplus
#include <cstddef>

extern "C" {
#else
#include <stddef.h>
#endif

/**
 * @brief Allocate a new QDMI session.
 * @details The returned handle can be used in subsequent calls to @ref
 * QDMI_session_get_devices to get the devices that are available to the client.
 * @param[out] session a handle to the session that is allocated. The session
 * must be freed by calling @ref QDMI_session_free when it is no longer needed.
 * @return @ref QDMI_SUCCESS if the session was allocated successfully.
 * @return @ref QDMI_ERROR_OUTOFMEM if memory space ran out.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 */
int QDMI_session_alloc(QDMI_Session *session);

/**
 * @brief Set a parameter for a session.
 * @details This function sets a parameter for a session. The parameter can be
 * one of the predefined parameters in @ref QDMI_Session_Parameter.
 * @param[in] session refers to the session to set the parameter for.
 * @param[in] param the parameter to set.
 * @param[in] size the size of the value in bytes.
 * @param[in] value the value to set the parameter to.
 * @return @ref QDMI_SUCCESS if the parameter was set successfully.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if the session does not exist, is in
 * an invalid state or the parameter is invalid.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the device does not support the
 * parameter.
 * @return @ref QDMI_ERROR_FATAL if the parameter could not be set.
 */
int QDMI_session_set_parameter(QDMI_Session session,
                               QDMI_Session_Parameter param, size_t size,
                               const void *value);
/**
 * @brief Get the devices associated with @p session.
 * @param[in] session refers to the session returned by @ref QDMI_session_alloc
 * or can be `NULL`. If @p session is `NULL`, the behavior is
 * implementation-defined.
 * @param[in] num_entries the number of entries that can be added to @p devices.
 * If @p devices is not `NULL`, @p num_entries must be greater than zero.
 * @param[out] devices returns a list of devices found. The @ref QDMI_Device
 * values returned in @p devices can be used to identify a specific @ref
 * QDMI_Device. If @p devices is `NULL`, this argument is ignored. The number of
 * @ref QDMI_Device values returned is the minimum of the value specified by @p
 * num_entries and the number of devices found.
 * @param[out] num_devices returns the number of devices found. If @p
 * num_devices is `NULL`, this argument is ignored.
 * @return @ref QDMI_SUCCESS if the function is executed successfully.
 * Otherwise, it returns one of the following error codes:
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p num_entries is zero and
 * @p devices is not `NULL` or if both @p devices and @p num_devices are `NULL`.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 */
int QDMI_session_get_devices(QDMI_Session session, size_t num_entries,
                             QDMI_Device *devices, size_t *num_devices);

/**
 * @brief Free a QDMI session.
 * @details This function frees the memory that was allocated for the session.
 * Accessing a (dangling) handle to a device that was attached to the session
 * after the session was freed is undefined behavior.
 * @param[in] session the session to free.
 */
void QDMI_session_free(QDMI_Session session);

#ifdef __cplusplus
} // extern "C"
#endif
