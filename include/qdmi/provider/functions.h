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
/// Defines the @ref provider_interface "provider interface".

#pragma once

// EXTRANAMESHIFTNEXTLINE
#include "qdmi/core.h"
// NOLINTNEXTLINE(readability-duplicate-include)
#include "qdmi/core.h"
#include "qdmi/provider/types.h"

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// NOLINTBEGIN(modernize-use-using)

/// @defgroup provider_interface QDMI Provider Interface
///
/// Describes the functions to be implemented by a provider managing multiple
/// devices.
///
/// @{

/// @defgroup provider_session_interface QDMI Provider's Session Interface
///
/// The provider's session interface includes functions to query properties
/// related to the provider on the session level.
///
/// The provider's session interface includes functions to query the devices
/// managed by the provider, and to query provider-side properties of a specific
/// device.
///
/// @{

/// Query the devices managed by a device.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] size is the number of @ref QDMI_Device "`QDMI_Device`'s" that fit
/// into @p value. Must be greater than or equal to the number of devices
/// returned, except when the @p value is @c NULL, in which case it is ignored.
/// @param[out] value is a pointer to the memory location where the list of
/// devices will be stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret is the actual number of @ref QDMI_Device
/// "`QDMI_Device`'s" of the list being queried. If this is @c NULL, it is
/// ignored.
///
/// @returns @ref QDMI_SUCCESS if, when the @p value is not @c NULL or @p
/// size_ret is not @c NULL, the list of devices or the number of devices,
/// respectively, was retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p value is not @c NULL and @p
/// size is less than the number of @ref QDMI_Device "`QDMI_Device`'s" returned
/// by the query.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @note Calling this function with the @p value set to @c NULL is expected to
/// allow querying the size of the buffer needed to retrieve the list of
/// devices. It is returned in @p size_ret if @p size_ret is not @c NULL.
///
/// For example, the following code pattern can be used to safely query the list
/// of devices.
/// ```
/// // Query the number of devices.
/// size_t size = 0;
/// QDMI_session_query_devices(session, 0, nullptr, &size);
///
/// // Allocate a buffer for the devices.
/// std::vector<QDMI_Device> devices(size, nullptr);
///
/// // Query the devices.
/// QDMI_session_query_devices(session, size, devices.data(), nullptr);
/// ```
typedef int QDMI_session_query_devices(QDMI_Session session, size_t size,
                                       QDMI_Device *value, size_t *size_ret);

/// Query a device by its identifier.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] id is the identifier of the device to query as a null-terminated
/// string. Must not be @c NULL. The expected format of the identifier depends
/// on the provider and must be documented in the provider's documentation. For
/// example, it could be a human-readable name of the device, or a unique
/// identifier such as a UUID.
/// @param[out] device is a pointer to the memory location where the queried
/// device handle will be stored. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the device with the specified identifier was
/// found and retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session or @p id is @c NULL,
/// or if @p device is @c NULL.
/// @returns @ref QDMI_ERROR_NOTFOUND if the arguments are valid and no device
/// with the specified identifier exists.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_device_by_id(QDMI_Session session,
                                            const char *id,
                                            QDMI_Device *device);

/// Get the core interface of a device.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] device is the device for which to get the core interface. Must
/// not be @c NULL.
/// @param[out] interface_ptr is a pointer to the memory location where the
/// pointer to the core interface of the device will be stored. Must not be @c
/// NULL.
///
/// @returns @ref QDMI_SUCCESS if the core interface of the device was retrieved
/// successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session, @p device, or @p
/// interface_ptr is @c NULL.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int
QDMI_session_get_device_interface(QDMI_Session session, QDMI_Device device,
                                  // EXTRANAMESHIFTNEXTLINE
                                  QDMI_Core_Interface const **interface_ptr);

/// Query the @ref QDMI_Context handle to be used with the interface returned
/// by @ref QDMI_session_get_device_interface for a device.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] device is the device for which to query the context. Must not
/// be @c NULL.
/// @param[out] context is a pointer to the memory location where the context of
/// the device will be stored. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the context of the device was retrieved
/// successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session, @p device, or @p
/// context is @c NULL.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_device_context(QDMI_Session session,
                                              QDMI_Device device,
                                              // EXTRANAMESHIFTNEXTLINE
                                              QDMI_Context *context);

/** @} */ // end of provider_session_interface

struct QDMI_Provider_Interface_impl_d {
  /// @see QDMI_session_query_devices
  QDMI_session_query_devices *session_query_devices;
  /// @see QDMI_session_query_device_by_id
  QDMI_session_query_device_by_id *session_query_device_by_id;
  /// @see QDMI_session_get_device_interface
  QDMI_session_get_device_interface *session_get_device_interface;
  /// @see QDMI_session_query_device_context
  QDMI_session_query_device_context *session_query_device_context;
};

/** @} */ // end of provider_interface

// NOLINTEND(modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
