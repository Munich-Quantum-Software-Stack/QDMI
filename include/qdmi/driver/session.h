/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

/** @file
 * @brief The QDMI session interface.
 * @details The driver manages resources provided to clients in sessions. This
 * header provides the interface for these sessions.
 */

#pragma once

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
