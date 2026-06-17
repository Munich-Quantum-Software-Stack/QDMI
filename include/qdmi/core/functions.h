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
/// @brief Defines the @ref core_interface "core interface" of QDMI.

#pragma once

#include "qdmi/core/constants.h"
#include "qdmi/core/types.h"

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// The following clang-tidy warning cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(modernize-use-using)

/// @defgroup library_interface QDMI Library Interface
///
/// The library interface controls the library's state and provides access to
/// the @ref core_interface "core interface".
///
/// @remark The library interface is only meant for the device loading this
/// library and not to be shared with third-party devices.
///
/// @{

/// Get the @ref core_interface "core interface" of the device.
/// @param[in] interface_ptr is the pointer to a variable where the core
/// interface pointer will be stored. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the core interface was retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p interface_ptr is @c NULL.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_get_interface(QDMI_Core_Interface const **interface_ptr);

/// Set a logging callback for the device.
///
/// This callback will be used for library-wide log messages.
/// @param[in] context is the context used to initialize the library with in
/// @ref QDMI_initialize. Must not be @c NULL.
/// @param[in] callback is the callback function to use for logging. If it is @c
/// NULL, logging is disabled.
/// @param[in] user_data is a pointer to user-defined data that will be passed
/// to the callback function, e.g., an instance of a logger. Must be @c NULL if
/// @p callback is @c NULL.
///
/// @returns @ref QDMI_SUCCESS on success.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p context is @c NULL or @p
/// user_data is @c NULL but @p callback is not @c NULL.

/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_context_set_log_callback(QDMI_Context context,
                                          QDMI_Log_Callback callback,
                                          void *user_data);

/// Finalize a device.
///
/// A device can expect that this function is called exactly once for every @p
/// context.
///
/// @remark Using the context after it has been finalized results in undefined
/// behavior.
///
/// @param[in] context is the context used to initialize the library with in
/// @ref QDMI_initialize. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the device was finalized successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p context is @c NULL.
/// @returns @ref QDMI_ERROR_FATAL if the finalization failed, this could, for
/// example, due to a job that is still running.
typedef int QDMI_context_finalize(QDMI_Context context);

struct QDMI_Library_impl_d {
  /// @see QDMI_get_interface
  QDMI_get_interface *get_interface;
  /// @see QDMI_create_context
  QDMI_context_set_log_callback *context_set_log_callback;
  /// @see QDMI_context_finalize
  QDMI_context_finalize *context_finalize;
};

/** @} */ // end of library_interface

/// @defgroup core_interface QDMI Core Interface
///
/// The core interface provides basic functionality implemented by all devices.
///
/// It includes functions to manage sessions, an authenticated connection
/// between the device and its client, and to query basic properties of the
/// device.
///
/// The device interface is split into two parts:
/// - The @ref core_context_interface "context interface" for device-wide
///   functionality like loading modules or querying the interface version.
/// - The @ref core_session_interface "session interface" for managing sessions
///   between a device and its client.
///
/// @note The function pointers to all functions of the core interface can be
/// retrieved via @ref QDMI_get_interface.
/// @note All devices must implement the core interface.
///
/// @{

/// @defgroup core_context_interface QDMI Context Interface
///
/// The context interface provides functions for device-wide functionality.
///
/// It includes functions to query basic properties of the device, such as its
/// ID, name, and version, and to manage modules that extend the core interface
/// with additional functionality.
///
/// @{

/// Query the device's ID.
///
/// The ID is a short string of two to eight characters satisfying the regular
/// expression `[a-z][a-z_]{1,7}` that uniquely identifies the device.
///
/// @param[in] context is the context used to initialize the library with in
/// @ref QDMI_initialize. Must not be @c NULL.
/// @param[in] size is the number of `char`s that fit into @p value. Must be
/// greater than or equal to the size of the returned ID, except when @p value
/// is
/// @c NULL, in which case it is ignored.
/// @param[out] value is a `char` pointer to the memory location where the
/// device's ID will be stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret is the actual number of `char`s of the ID being
/// queried, including the terminating null character `'\0'`. If this is @c
/// NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if, when @p value is not @c NULL or @p size_ret
/// is not @c NULL, the ID or the number of `char`s, respectively, was retrieved
/// successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p context is @c NULL or @p
/// value is not @c NULL and @p size is less than the number of `char`s required
/// for the device's ID.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @note See @ref QDMI_context_query_name for an example of first querying the
/// required size of the buffer @p value, and then safely querying the actual
/// value in a second step.
typedef int QDMI_context_query_id(QDMI_Context context, size_t size,
                                  char *value, size_t *size_ret);

/// Query the device's name.
///
/// The device's name is a descriptive string providing a human-readable name of
/// the device. It may contain spaces and special characters.
///
/// @param[in] context is the context used to initialize the library with in
/// @ref QDMI_initialize. Must not be @c NULL.
/// @param[in] size is the number of `char`s that fit into @p value. Must be
/// greater than or equal to the size of the returned name, except when @p value
/// is @c NULL, in which case it is ignored.
/// @param[out] value is a `char` pointer to the memory location where the
/// device's name will be stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret is the actual number of `char`s of the name being
/// queried, including the terminating null character `'\0'`. If this is @c
/// NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if, when @p value is not @c NULL or @p size_ret
/// is not @c NULL, the name or the number of `char`s, respectively, was
/// retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p context is @c NULL or @p
/// value is not @c NULL and @p size is less than the number of `char`s required
/// for the device's name.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @note Calling this function with @p value set to @c NULL is expected to
/// allow querying the size of the buffer needed to retrieve the device's name.
/// It is returned in @p size_ret if @p size_ret is not @c NULL.
///
/// For example, the following code pattern can be used to safely query the
/// device's name.
/// ```
/// // Query the size of the device's name.
/// size_t size = 0;
/// QDMI_context_query_name(ctx, 0, nullptr, &size)
///
/// // Allocate memory for the device's name accordingly.
/// std::string name(size - 1, '\0')
///
/// // Query the device's name.
/// QDMI_context_query_name(ctx, size, name.data(), nullptr);
/// ```
typedef int QDMI_context_query_name(QDMI_Context context, size_t size,
                                    char *value, size_t *size_ret);

/// Query the device's version.
///
/// A version `x.y.z` is transformed to an integer via @code (x << 22) | (y <<
/// 12) | z @endcode using the macro @ref QDMI_MAKE_VERSION. The separate parts
/// of the version can be derived with the corresponding macros: @ref
/// QDMI_MAJOR_VERSION, @ref QDMI_MINOR_VERSION, @ref QDMI_PATCH_VERSION.
///
/// @param[in] context is the context used to initialize the library with in
/// @ref QDMI_initialize. Must not be @c NULL.
/// @param[out] value is a pointer to a variable where the device's version will
/// be stored. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the device's version was retrieved
/// successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p context or @p value is @c
/// NULL.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_context_query_version(QDMI_Context context, size_t *value);

/// Query the device's authentication options.
///
/// The returned @ref QDMI_Authentication_Option "authentication options" define
/// different combinations of credentials that are required for successful
/// authentication.
///
/// @param[in] context is the context used to initialize the library with in
/// @ref QDMI_initialize. Must not be @c NULL.
/// @param[in] size is the number of @ref QDMI_Authentication_Option elements
/// that fit into
/// @p values. Must be greater than or equal to the number of authentication
/// options supported by the device, except when @p values is @c NULL, in which
/// case it is ignored.
/// @param[out] values is a pointer to the memory location where the device's
/// authentication options will be stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret The actual number of @ref QDMI_Authentication_Option
/// elements of the authentication options being queried. If this is @c NULL, it
/// is ignored.
///
/// @returns @ref QDMI_SUCCESS if, when @p values is not @c NULL or @p size_ret
/// is not @c NULL, the authentication options, or the number of @ref
/// QDMI_Authentication_Option elements, respectively, were retrieved
/// successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p context is @c NULL or @p
/// values is not @c NULL and @p size is less than the number of @ref
/// QDMI_Authentication_Option elements required for the device's authentication
/// options.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @note See @ref QDMI_context_query_name for an example of first querying the
/// required size of the buffer @p values, and then safely querying the actual
/// values in a second step.
typedef int
QDMI_context_query_authentication_options(QDMI_Context context, size_t size,
                                          QDMI_Authentication_Option *values,
                                          size_t *size_ret);

/// Query the device's modules.
///
/// The returned @ref QDMI_Module handles can be used to retrieve an object with
/// the module's function pointers via @ref QDMI_context_get_module_interface.
///
/// @param[in] context is the context used to initialize the library with in
/// @ref QDMI_initialize. Must not be @c NULL.
/// @param[in] size is the number of @ref QDMI_Module elements that fit into @p
/// values. Must be greater than or equal to the number of modules supported by
/// the device, except when @p values is @c NULL, in which case it is ignored.
/// @param[out] values is a pointer to the memory location where the library's
/// available modules will be stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret is the actual number of @ref QDMI_Module elements of
/// the available modules being queried. If this is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if, when @p values is not @c NULL or @p size_ret
/// is not @c NULL, the available modules, or the number of @ref QDMI_Module
/// elements, respectively, were retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p context is @c NULL or @p
/// values is not @c NULL and @p size is less than the number of @ref
/// QDMI_Module elements required for the library's authentication options.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @note See @ref QDMI_context_query_name for an example of first querying the
/// required size of the buffer @p values, and then safely querying the actual
/// values in a second step.
typedef int QDMI_context_query_modules(QDMI_Context context, size_t size,
                                       QDMI_Module *values, size_t *size_ret);

/// Query a module handle by its ID.
///
/// @param[in] context is the context used to initialize the library with in
/// @ref QDMI_initialize. Must not be @c NULL.
/// @param[in] id is the ID of the module as a null-terminated string. Must not
/// be @c NULL.
/// @param[out] module is the module handle corresponding to the provided @p id.
///
/// @returns @ref QDMI_SUCCESS if a module with the specified @p id exists, and,
/// when the @p module was not @c NULL, the module was returned successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p context, @p id, or @p module
/// is
/// @c NULL.
/// @returns @ref QDMI_ERROR_NOTFOUND if the arguments are valid and no module
/// with the specified @p id is available.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @see QDMI_context_query_module_id
typedef int QDMI_context_query_module_by_id(QDMI_Context context,
                                            const char *id,
                                            QDMI_Module *module);

/// Allocate a new session.
///
/// This is the main entry point for a client to establish a session with a
/// device. The returned handle can be used throughout the @ref
/// core_session_interface "session interface" to refer to the session.
///
/// @param[in] context is the context used to initialize the library with in
/// @ref QDMI_initialize. Must not be @c NULL.
/// @param[in] callback is a function with the signature defined by the type
/// @ref QDMI_Log_Callback that will be used for session-specific log messages.
/// If it is @c NULL, the library callback for logging will be used if present.
/// @param[in] user_data is a pointer to user-defined data that will be passed
/// to the callback function whenever it is called, e.g., an instance of a
/// logger. Must be @c NULL if @p callback is @c NULL.
/// @param[out] session is a handle to the session that is allocated. Must not
/// be
/// @c NULL. The session must be freed by calling @ref QDMI_session_free
/// when it is no longer used.
///
/// @returns @ref QDMI_SUCCESS if the session was allocated successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p context or @p session is @c
/// NULL or when @p callback is @c NULL and @p user_data is not @c NULL.
/// @returns @ref QDMI_ERROR_OUTOFMEM if memory space ran out.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @see QDMI_session_set_parameter
/// @see QDMI_session_initialize
typedef int QDMI_context_allocate_session(QDMI_Context context,
                                          QDMI_Log_Callback callback,
                                          void *user_data,
                                          QDMI_Session *session);

/// Query a module's ID.
///
/// The ID is a short string of two to eight characters satisfying the regular
/// expression `[a-z][a-z_]{1,7}` that uniquely identifies the module. It can be
/// used to get a handle on the module via @ref QDMI_context_query_module_by_id.
/// For a list of standard modules with their corresponding IDs, see @ref
/// QDMI_Module.
///
/// @param[in] context is the context used to initialize the library with in
/// @ref QDMI_initialize. Must not be @c NULL.
/// @param[in] module is the @ref QDMI_Module to retrieve the id for.
/// @param[in] size is the number of `char`s that fit into @p value. Must be
/// greater than or equal to the size of the returned ID, except when the @p
/// value is @c NULL, in which case it is ignored.
/// @param[out] value is a `char` pointer to the memory location where the
/// module's ID will be stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret is the actual number of `char`s of the ID being
/// queried. If this is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if, when the @p value is not @c NULL or @p
/// size_ret is not @c NULL, the module's ID or the number of `char`s,
/// respectively, was retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p context or @p module is @c
/// NULL, @p value is not @c NULL and @p size is less than the number of `char`s
/// required for the module's ID.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @note See @ref QDMI_context_query_name for an example of first querying the
/// required size of the buffer @p value, and then safely querying the actual
/// value in a second step.
typedef int QDMI_context_query_module_id(QDMI_Context context,
                                         QDMI_Module module, size_t size,
                                         char *value, size_t *size_ret);

/// Retrieve the function table for a module handle.
///
/// @param[in] context is the context used to initialize the library with in
/// @ref QDMI_initialize. Must not be @c NULL.
/// @param[in] module is the @p module to get the function table for.
/// @param[out] interface_ptr is a pointer to a variable where a pointer to the
/// function table will be stored. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if, when the @p module is not @c NULL, the
/// object with function pointers was retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p context, @p module, or @p
/// interface_pr is @c NULL.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @note To prevent the need to copy the struct with the function pointers and
/// enable it to reside in the dynamic library's memory space, the parameter @p
/// interface_ptr is a pointer to a pointer. This facilitates writing the
/// pointer to the struct stored in the dynamic library to the user's memory
/// space.
typedef int QDMI_context_get_module_interface(QDMI_Context context,
                                              QDMI_Module module,
                                              void const **interface_ptr);

/** @} */ // end of core_context_interface

/// @defgroup core_session_interface QDMI Session Interface
///
/// The session interface provides functions to manage sessions between the
/// client and the device.
///
/// A session is a connection between a client and a device that allows the
/// client to interact with the device. Sessions are used to authenticate with
/// the device and to manage resources required for the interaction with the
/// device.
///
/// The typical workflow for a session is as follows:
/// - Allocate a session with @ref QDMI_context_allocate_session.
/// - Set parameters for the session with, e.g., @ref QDMI_session_set_token.
/// - Initialize the session with @ref QDMI_session_initialize.
/// - Run code to interact with the device using functions like @ref
///   QDMI_session_create_job from the @ref job_interface "job interface", or
///   any other interface defined by available modules.
/// - Free the session with @ref QDMI_session_free when it is no longer
///   necessary.
///
/// @{

/// Set the token for the session.
///
/// A token could be an API key. If the device requires authentication via a
/// token, it must be set before calling @ref QDMI_session_initialize.
///
/// @param[in] session is a handle to the session to set the token for. Must not
/// be @c NULL.
/// @param[in] value is a pointer to a `char` array that contains the token to
/// be set for the session. The string must be null-terminated. The data pointed
/// to by the @p value is copied and the associated memory can be safely reused
/// after this function returns. If this is @c NULL, the token is unset, i.e.,
/// any previously set token is cleared.
///
/// @returns @ref QDMI_SUCCESS if the device supports setting a token, and, when
/// the @p value is not @c NULL, the value for the token was set successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// device does not support a token or the value of the token.
/// @returns @ref QDMI_ERROR_BADSTATE if arguments are valid and the token
/// cannot be set in the current state of the session, for example, because the
/// session is already initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_set_token(QDMI_Session session, const char *value);

/// Set the authentication file for the session.
///
/// The file may contain a token or other authentication information required
/// for the session. If the device requires authentication via an authentication
/// file, the device documentation *must* document what kind of authentication
/// information is expected in this file.
///
/// @param[in] session is a handle to the session to set the authentication file
/// for. Must not be @c NULL.
/// @param[in] value is a pointer to a `char` array that contains the
/// authentication file path to be set for the session. The string must be
/// null-terminated. The data pointed to by the @p value is copied and the
/// associated memory can be safely reused after this function returns. If this
/// is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if the device supports setting an authentication
/// file, and, when the @p value is not @c NULL, the value for the
/// authentication file was set successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL or, when
/// @p value is not @c NULL, and the file pointed to by @p value cannot be
/// accessed, e.g., because it does not exist.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// device does not support an authentication file, or the content of the
/// authentication file.
/// @returns @ref QDMI_ERROR_BADSTATE if arguments are valid and the
/// authentication file cannot be set in the current state of the session, for
/// example, because the session is already initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_set_authentication_file(QDMI_Session session,
                                                 const char *value);

/// Set the authentication URL for the session.
///
/// This parameter might be used as part of an authentication scheme where an
/// API token is received from an authentication server. This may, additionally,
/// require a username and a password, which can be set via @ref
/// QDMI_session_set_username and @ref QDMI_session_set_password.
///
/// @param session is a handle to the session to set the authentication URL for.
/// Must not be @c NULL.
/// @param[in] value is a pointer to a `char` array that contains the
/// authentication URL to be set for the session. The string must be
/// null-terminated. The data pointed to by the @p value is copied, and the
/// associated memory can be safely reused after this function returns. If this
/// is @c NULL, the authentication URL is unset, i.e., any previously set
/// authentication URL is cleared.
///
/// @returns @ref QDMI_SUCCESS if the device supports setting an authentication
/// URL, and, when the @p value is not @c NULL, the value for the authentication
/// URL was set successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// device does not support an authentication URL or the value of the
/// authentication URL.
/// @returns @ref QDMI_ERROR_BADSTATE if arguments are valid and the
/// authentication URL cannot be set in the current state of the session, for
/// example, because the session is already initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_set_authentication_url(QDMI_Session session,
                                                const char *value);

/// Set the username for the session.
///
/// The username is used for authentication within the session.
/// @param session is a handle to the session to set the username for. Must not
/// be @c NULL.
/// @param[in] value is a pointer to a `char` array that contains the username
/// to be set for the session. The string must be null-terminated. The data
/// pointed to by the @p value is copied, and the associated memory can be
/// safely reused after this function returns. If this is @c NULL, the username
/// is unset, i.e., any previously set username is cleared.
///
/// @returns @ref QDMI_SUCCESS if the device supports setting a username, and,
/// when the @p value is not @c NULL, the value for the username was set
/// successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// device does not support a username or the value of the username.
/// @returns @ref QDMI_ERROR_BADSTATE if arguments are valid and the username
/// cannot be set in the current state of the session, for example, because the
/// session is already initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_set_username(QDMI_Session session, const char *value);

/// Set the password for the session.
///
/// The password is used for authentication within the session.
/// @param session is a handle to the session to set the password for. Must not
/// be @c NULL.
/// @param[in] value is a pointer to a `char` array that contains the password
/// to be set for the session. The string must be null-terminated. The data
/// pointed to by @p value is copied, and the associated memory can be safely
/// reused after this function returns. If this is @c NULL, the password is
/// unset, i.e., any previously set password is cleared.
///
/// @returns @ref QDMI_SUCCESS if the device supports setting a password, and,
/// when the @p value is not @c NULL, the value for the password was set
/// successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session @c NULL.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// device does not support a password or the value of the password.
/// @returns @ref QDMI_ERROR_BADSTATE if arguments are valid and the password
/// cannot be set in the current state of the session, for example, because the
/// session is already initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_set_password(QDMI_Session session, const char *value);

/// Set a logging callback for the session.
///
/// This callback will be used for session-specific log messages. If it is @c
/// NULL, the library callback for logging will be used if present.
///
/// @param[in] session is the session to set the logging callback for. Must not
/// be @c NULL.
/// @param[in] callback is a function with the signature defined by the type
/// @ref QDMI_Log_Callback that will be used for session-specific log messages.
/// If it is @c NULL, the library callback for logging will be used if present.
/// @param[in] user_data is a pointer to user-defined data that will be passed
/// to the callback function whenever it is called, e.g., an instance of a
/// logger. Must be @c NULL if @p callback is @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the logging callback was set successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL or when @p
/// callback is @c NULL and @p user_data is not @c NULL.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @note This setter may be called independent of the current session's state,
/// i.e., whether it is initialized or not.
typedef int QDMI_session_set_log_callback(QDMI_Session session,
                                          QDMI_Log_Callback callback,
                                          void *user_data);

/// Initialize a session.
///
/// This function initializes the session and prepares it for use. The session
/// must be initialized before it can be used as part of the @ref
/// core_session_interface "session interface". If a device requires
/// authentication, the required credentials must be set using
/// `QDMI_session_set_*`, e.g., @ref QDMI_session_set_token before calling this
/// function. A session may only be initialized successfully once.
///
/// @param[in] session is the session to initialize. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the session was initialized successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if the @p session is @c NULL.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the @p session is not @c NULL
/// and the @p session could not be initialized due to missing permissions. This
/// could be due to missing credentials that must be set for this device.
/// @returns @ref QDMI_ERROR_BADSTATE if the @p session is not @c NULL and the
/// @p session is not in a state allowing initialization, for example, because
/// the session is already initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_initialize(QDMI_Session session);

/// Free a QDMI session.
///
/// This function frees the memory allocated for the session. Using a session
/// handle after it was freed is undefined behavior.
///
/// @param[in] session The session to free.
typedef void QDMI_session_free(QDMI_Session session);

/** @} */ // end of core_session_interface

struct QDMI_Core_Interface_impl_d {
  //===--------------------------------------------------------------------===//
  // Context interface
  //===--------------------------------------------------------------------===//
  /// @see QDMI_context_query_id
  QDMI_context_query_id *context_query_id;
  /// @see QDMI_context_query_name
  QDMI_context_query_name *context_query_name;
  /// @see QDMI_context_query_version
  QDMI_context_query_version *context_query_version;
  /// @see QDMI_context_query_authentication_options
  QDMI_context_query_authentication_options
      *context_query_authentication_options;
  /// @see QDMI_context_query_modules
  QDMI_context_query_modules *context_query_modules;
  /// @see QDMI_context_query_module_by_id
  QDMI_context_query_module_by_id *context_query_module_by_id;
  /// @see QDMI_context_query_module_id
  QDMI_context_query_module_id *context_query_module_id;
  /// @see QDMI_context_get_module_interface
  QDMI_context_get_module_interface *context_get_module_interface;
  /// @see QDMI_context_allocate_session
  QDMI_context_allocate_session *context_allocate_session;
  //===--------------------------------------------------------------------===//
  // Session interface
  //===--------------------------------------------------------------------===//
  /// @see QDMI_session_set_token
  QDMI_session_set_token *session_set_token;
  /// @see QDMI_session_set_authentication_file
  QDMI_session_set_authentication_file *session_set_authentication_file;
  /// @see QDMI_session_set_authentication_url
  QDMI_session_set_authentication_url *session_set_authentication_url;
  /// @see QDMI_session_set_username
  QDMI_session_set_username *session_set_username;
  /// @see QDMI_session_set_password
  QDMI_session_set_password *session_set_password;
  /// @see QDMI_session_set_log_callback
  QDMI_session_set_log_callback *session_set_log_callback;
  /// @see QDMI_session_initialize
  QDMI_session_initialize *session_initialize;
  /// @see QDMI_session_free
  QDMI_session_free *session_free;
};

/** @} */ // end of core_interface

// NOLINTEND(modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
