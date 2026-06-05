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
/// Defines the @ref job_interface "job interface".

#pragma once

#include "qdmi/core.h"
#include "qdmi/job/constants.h"
#include "qdmi/job/types.h"

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
// NOLINTBEGIN(modernize-use-using, modernize-redundant-void-arg)

/// @defgroup job_interface QDMI Job Interface
///
/// Defines the functions to create and manage jobs on a device.
///
/// The job interface is split into two parts: - the @ref job_session_interface
/// "job's session interface" for additional functionality on the session level,
/// and
/// - the @ref job_job_interface "actual job interface" for managing jobs on the
///   device.
///
/// @{

/// @defgroup job_session_interface QDMI Job's Session Interface
///
/// Defines functions to query properties related to jobs on the session level
/// and to create jobs.
///
/// @{

/// Query the supported program formats of a device.
///
/// The returned list contains all program formats that the device supports for
/// execution. A client can use this information to determine which program
/// formats can be used when submitting jobs to the device.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] size is the number of elements of type @ref QDMI_Program_Format
/// that the parameter @p value points to. Must be greater than or equal to the
/// number of supported program formats, except when the @p value is @c NULL, in
/// which case it is ignored.
/// @param[out] value is a pointer to the memory location where the supported
/// program formats will be stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret is the actual number of supported program formats. If
/// this is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if, when the @p value is not @c NULL, the
/// supported program formats were retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p session is @c NULL, or
/// - @p value is not @c NULL and @p size is less than the number of supported
///   program formats.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid, and the
/// property cannot be queried in the current state of the session, for example,
/// because the session is not initialized with @ref QDMI_session_initialize.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_program_formats(QDMI_Session session,
                                               size_t size,
                                               QDMI_Program_Format *value,
                                               size_t *size_ret);

/// Query a program format by its ID and version.
///
/// The returned program format corresponds to the specified ID and version. A
/// client can use this information to determine the program format to be used
/// when submitting jobs to the device. See @ref QDMI_Program_Format for a
/// listing of example formats with their corresponding ID.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] id is the ID of the program format to query as a null-terminated
/// string. Must not be @c NULL.
/// @param[in] version The version of the program format to query in packed
/// format using the @ref QDMI_MAKE_VERSION macro. Must not be zero.
/// @param[out] format A pointer to the memory location where the program format
/// will be stored.
///
/// @returns @ref QDMI_SUCCESS if the program format was retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p session or @p id is @c NULL, or
/// - @p version is zero.
/// @returns @ref QDMI_ERROR_NOTFOUND if the arguments are valid, but no program
/// format with the specified ID and version exists.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid, and the
/// program format cannot be queried in the current state of the session, for
/// example, because the session is not initialized with @ref
/// QDMI_session_initialize.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @see QDMI_Program_Format
/// @see QDMI_session_query_program_format_id
/// @see QDMI_session_query_program_format_version
/// @see QDMI_MAKE_VERSION
typedef int QDMI_session_query_program_format_by_id_and_version(
    QDMI_Session session, const char *id, size_t version,
    QDMI_Program_Format *format);

/// Query a program format's ID.
///
/// The ID is a short, null-terminated string of two to eight characters
/// satisfying the regular expression `[a-z][a-z_]{1,7}` that uniquely
/// identifies the program format.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] format is the program format to query the ID for.
/// @param[in] size is the number of `char`s that fit into @p value. Must be
/// greater than or equal to the number of `char`s required for the program
/// format's ID, except when @p value is @c NULL, in which case it is ignored.
/// @param[out] value is a pointer to the memory location where the program
/// format's ID will be stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret is the actual number of `char`s of the ID being
/// queried, including the terminating null character `'\0'`. If this is @c
/// NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if, when the @p value is not @c NULL or @p
/// size_ret is not @c NULL, the program format's ID or the number of `char`s,
/// respectively, was retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p session is @c NULL, or
/// - @p value is not @c NULL and @p size is less than the number of `char`s
///   required for the program format's ID.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid, and the
/// program format's ID cannot be queried in the current state of the session,
/// for example, because the session is not initialized with @ref
/// QDMI_session_initialize.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_program_format_id(QDMI_Session session,
                                                 QDMI_Program_Format format,
                                                 size_t size, char *value,
                                                 size_t *size_ret);

/// Query a program format's version.
///
/// The version is a non-zero integer in packed format using the @ref
/// QDMI_MAKE_VERSION macro. The separate parts of the version can be derived
/// with the corresponding macros: @ref QDMI_MAJOR_VERSION, @ref
/// QDMI_MINOR_VERSION, @ref QDMI_PATCH_VERSION.
///
/// @param session is the session used for the query. Must not be @c NULL.
/// @param format is the program format to query the version for.
/// @param value is a pointer to the memory location where the program format's
/// version will be stored. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the program format's version was retrieved
/// successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session or @p value is @c
/// NULL.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid, and the
/// program format's version cannot be queried in the current state of the
/// session, for example, because the session is not initialized with @ref
/// QDMI_session_initialize.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_program_format_version(
    QDMI_Session session, QDMI_Program_Format format, size_t *value);

/// Query whether a program format supports string-based programs.
///
/// If this function returns @ref QDMI_SUCCESS, the function @ref
/// QDMI_job_set_payload_string can be used to set a program for a job in the
/// specified format. If this function returns @ref QDMI_ERROR_NOTSUPPORTED, the
/// function @ref QDMI_job_set_payload_string must not be used to set a program
/// for a job in the specified format.
///
/// @param session is the session used for the query. Must not be @c NULL.
/// @param format is the program format to query the support for string-based
/// programs for.
///
/// @returns @ref QDMI_SUCCESS if the device supports string-based programs in
/// the specified format.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// device does not support string-based programs in the specified format.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid, and the
/// support for string-based programs cannot be queried in the current state of
/// the session, for example, because the session is not initialized with @ref
/// QDMI_session_initialize.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int
QDMI_session_query_program_format_string_support(QDMI_Session session,
                                                 QDMI_Program_Format format);

/// Query whether a program format supports binary-based programs.
///
/// If this function returns @ref QDMI_SUCCESS, the function @ref
/// QDMI_job_set_payload_binary can be used to set a program for a job in the
/// specified format. If this function returns @ref QDMI_ERROR_NOTSUPPORTED, the
/// function @ref QDMI_job_set_payload_binary must not be used to set a program
/// for a job in the specified format.
///
/// @param session is the session used for the query. Must not be @c NULL.
/// @param format is the program format to query the support for binary-based
/// programs for.
///
/// @returns @ref QDMI_SUCCESS if the device supports binary-based programs in
/// the specified format.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// device does not support binary-based programs in the specified format.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid, and the
/// support for binary-based programs cannot be queried in the current state of
/// the session, for example, because the session is not initialized with @ref
/// QDMI_session_initialize.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int
QDMI_session_query_program_format_binary_support(QDMI_Session session,
                                                 QDMI_Program_Format format);

/// Create a job.
///
/// This is the main entry point for a client to create a job on a device. The
/// returned handle can be used throughout the @ref job_job_interface "job
/// interface" to refer to the job.
///
/// @param[in] session is the session to create the job with. Must not be @c
/// NULL.
/// @param callback is a function with the signature defined by the type @ref
/// QDMI_Log_Callback that will be used for job-specific log messages. If it is
/// @c NULL, the session callback for logging will be used if present, and if
/// the session callback is also @c NULL, the library callback for logging will
/// be used if present.
/// @param user_data is a pointer to user-defined data that will be passed to
/// the callback function whenever it is called, e.g., an instance of a logger.
/// Must be @c NULL if @p callback is @c NULL.
/// @param[out] job A pointer to a handle that will store the created job.
/// Must not be @c NULL. The job must be freed by calling @ref
/// QDMI_job_free when it is no longer used.
///
/// @returns @ref QDMI_SUCCESS if the job was created successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p session @c NULL,
/// - @p job is @c NULL, or
/// - @p callback is @c NULL and @p user_data is not @c NULL.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and the session
/// is not in a state allowing the creation of a job, for example, because the
/// session is not initialized.
/// @returns @ref QDMI_ERROR_OUTOFMEM if memory space ran out.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_create_job(QDMI_Session session,
                                    QDMI_Log_Callback callback, void *user_data,
                                    QDMI_Job *job);

/// Retrieve a job by its ID.
///
/// The returned job corresponds to the specified ID. A client can use this
/// function to retrieve a job that was previously created with @ref
/// QDMI_session_create_job, for example, in an expired session.
///
/// @param[in] session The session to retrieve the job with. Must not be @c
/// NULL.
/// @param callback is a function with the signature defined by the type @ref
/// QDMI_Log_Callback that will be used for job-specific log messages. If it is
/// @c NULL, the session callback for logging will be used if present, and if
/// the session callback is also @c NULL, the library callback for logging will
/// be used if present.
/// @param user_data is a pointer to user-defined data that will be passed to
/// the callback function whenever it is called, e.g., an instance of a logger.
/// Must be @c NULL if @p callback is @c NULL.
/// @param[in] id The ID of the job to retrieve as a null-terminated string.
/// Must not be @c NULL.
/// @param[out] job A pointer to a handle that will store the retrieved job.
/// Must not be @c NULL. The job must be freed by calling @ref QDMI_job_free
/// when it is no longer used.
///
/// @returns @ref QDMI_SUCCESS if the job was retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p session @c NULL,
/// - @p id is @c NULL,
/// - @p job is @c NULL, or
/// - @p callback is @c NULL and @p user_data is not @c NULL.
/// @returns @ref QDMI_ERROR_NOTFOUND if the arguments are valid, but no job
/// with the specified ID exists or can be retrieved.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and the session
/// is not in a state allowing the retrieval of a job, for example, because the
/// session is not initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_retrieve_job_by_id(QDMI_Session session,
                                            QDMI_Log_Callback callback,
                                            void *user_data, const char *id,
                                            QDMI_Job *job);

/** @} */ // end of job_session_interface

/// @defgroup job_job_interface QDMI Job Interface
///
/// Provides functions to manage jobs on a device.
///
/// A job is a task submitted to a device for execution. Most jobs are quantum
/// circuits to be executed on a quantum device. However, jobs can also be a
/// different type of task, such as calibration.
///
/// The typical workflow for a device job is as follows:
/// - Create a job with @ref QDMI_session_create_job.
/// - Submit the job with @ref QDMI_job_submit.
/// - Check the status of the job with @ref QDMI_job_check.
/// - Wait for the job to finish with @ref QDMI_job_wait.
/// - Free the job with @ref QDMI_job_free when it is no longer used.
///
/// @{

/// Set a string-based program as a payload for the job.
///
/// The expected format of the program depends on the program format specified
/// by the @p format parameter.
///
/// @param[in] format is the program format of the program to be set. The format
/// must support string-based programs, which can be checked with @ref
/// QDMI_session_query_program_format_string_support.
/// @param[in] count is the number of programs to be set. Must not be zero,
/// except when @p values is @c NULL, in which case it is ignored.
/// @param[in] values is a pointer to the memory location that contains the
/// programs to be set. The strings must be null-terminated. The data pointed to
/// by the @p values is copied, and the associated memory can be safely reused
/// after this function returns. If this is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if the device supports the program format and,
/// when the @p values is not @c NULL, the program was set successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p job is @c NULL, or
/// - @p values is not @c NULL and @p count is zero.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// program format
/// @p format does not support string-based programs.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and the program
/// cannot be set in the current state of the job, for example, because the job
/// is already submitted.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
/// the function in the current session.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_job_set_payload_string(QDMI_Job job,
                                        QDMI_Program_Format format,
                                        size_t count,
                                        const char *const *values);

/// Set a binary-based program as a payload for the job.
///
/// @param job is a handle to the job to set the program for. Must not be @c
/// NULL.
/// @param format is the program format of the program to be set. The format
/// must support binary-based programs, which can be checked with @ref
/// QDMI_session_query_program_format_binary_support.
/// @param count is the number of programs to be set. Must not be zero, except
/// when @p values is @c NULL, in which case it is ignored.
/// @param sizes are the sizes of the individual binary programs to be set. The
/// number of elements must be equal to @p count. The values must be greater
/// than zero, except when @p values is @c NULL, in which case it is ignored.
/// @param values are the binary programs to be set. The number of elements must
/// be equal to @p count. The data pointed to by the @p values is copied, and
/// the associated memory can be safely reused after this function returns. If
/// this is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if the device supports the program format and,
/// when the @p values is not @c NULL, the program was set successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p job is @c NULL,
/// - @p values is not @c NULL and @p count is zero,
/// - @p values is not @c NULL and @p sizes is @c NULL, or
/// - @p values is not @c NULL and any of the values in @p sizes is zero.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// program format @p format does not support binary-based programs.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and the program
/// cannot be set in the current state of the job, for example, because the job
/// is already submitted.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
/// the function in the current session.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_job_set_payload_binary(QDMI_Job job,
                                        QDMI_Program_Format format,
                                        size_t count, const size_t *sizes,
                                        const void *const *values);

/// Set the number of shots for the job.
///
/// The number of shots is the number of times a quantum circuit is executed on
/// the device. For example, if a circuit is executed with 1024 shots, the
/// device runs the circuit 1024 times and returns the results of all runs. The
/// number of shots is typically used for quantum circuits that involve
/// measurements, where multiple runs are needed to estimate the probabilities
/// of different measurement outcomes.
///
/// @param[in] job A handle to a job for which to set the number of shots. Must
/// not be @c NULL.
/// @param[in] shots The number of shots to set for the job. Must be greater
/// than zero.
///
/// @returns @ref QDMI_SUCCESS if the device supports setting the number of
/// shots and the number of shots was set successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p job is @c NULL, or
/// - @p shots is zero.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// device does not support setting the number of shots or the number of shots
/// cannot be set.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid, the device
/// supports setting the number of shots, and the number of shots cannot be set
/// in the current state of the job, for example, because the job is already
/// submitted.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
/// the function in the current session.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_job_set_shot_count(QDMI_Job job, size_t shots);

/// Set a callback function for job-specific log messages.
///
/// The callback function provided by the @p callback parameter will be used for
/// log messages related to the job, for example, to provide updates on the
/// job's status. If @p callback is @c NULL, the session callback for logging
/// will be used if present, and if the session callback is also @c NULL, the
/// library callback for logging will be used if present.
///
/// @param[in] job is a handle to a job for which to set the log callback. Must
/// not be @c NULL.
/// @param[in] callback is a function with the signature defined by the type
/// @ref QDMI_Log_Callback that will be used for job-specific log messages. If
/// it is
/// @c NULL, the session callback for logging will be used if present, and if
/// the session callback is also @c NULL, the library callback for logging will
/// be used if present.
/// @param user_data is a pointer to user-defined data that will be passed to
/// the callback function whenever it is called, e.g., an instance of a logger.
/// Must be @c NULL if @p callback is @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the log callback was set successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p job is @c NULL, or
/// - @p callback is @c NULL and @p user_data is not @c NULL.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
/// the function in the current session.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_job_set_log_callback(QDMI_Job job, QDMI_Log_Callback callback,
                                      void *user_data);

/// Query the job's ID.
///
/// The ID must uniquely identify a job for the specific device. It should
/// generally be universally unique (such as a UUID), to avoid conflicts with
/// other devices' job IDs. It may be used to recover a @ref QDMI_Job handle
/// upon device failure via @ref QDMI_session_retrieve_job_by_id.
///
/// @param[in] job is a handle to a job for which to query the ID. Must not be
/// @c NULL.
/// @param[in] size is the number of `char`s the parameter @p id points to. Must
/// be greater than or equal to the size of the returned id, except when the @p
/// id is
/// @c NULL, in which case it is ignored.
/// @param[out] value is a pointer to the memory location where the ID will be
/// stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret The actual number of `char`s of the id. If this is @c
/// NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if the device supports querying a job ID, and,
/// when the @p id is not @c NULL, the id was retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p job is @c NULL, or
/// - @p id is not @c NULL and @p size is less than the size of the data being
///   queried.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid, and the job ID
/// cannot be queried in the current state of the job, for example, because the
/// job is not submitted yet.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
/// the function in the current session.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_job_query_id(QDMI_Job job, size_t size, char *value,
                              size_t *size_ret);

/// Query the program format of a job.
///
/// The program format of a job is the format of the program that is set for the
/// job. It can be set with @ref QDMI_job_set_payload_string or @ref
/// QDMI_job_set_payload_binary. The expected format of the program depends on
/// the program format specified by the @p format parameter.
///
/// @param[in] job is a handle to a job for which to query the program format.
/// Must not be @c NULL.
/// @param[out] value is a pointer to the memory location where the program
/// format will be stored. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the program format was retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p job or @p value is @c NULL.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid, and the job's
/// program format cannot be queried in the current state of the job, for
/// example, because no program has been set for the job yet.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
/// the function in the current session.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_job_query_payload_program_format(QDMI_Job job,
                                                  QDMI_Program_Format *value);

/// Query a string-based payload of a job.
///
/// The expected format of the returned payload depends on the program format of
/// the job, which can be queried with @ref
/// QDMI_job_query_payload_program_format.
///
/// @param[in] job is a handle to a job for which to query the payload. Must not
/// be
/// @c NULL.
/// @param[in] index is the index of the program to query. Must be less than the
/// number of programs set for the job.
/// @param[in] size is the number of `char`s of the buffer pointed to by @p
/// value. Must be greater than or equal to the size required to retrieve the
/// program, except when @p value is @c NULL, in which case it is ignored.
/// @param[out] value A pointer to the memory location where the program will be
/// stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret The actual number of `char`s of the program being
/// queried, including the terminating null character `'\0'`. If this is @c
/// NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if, when the @p value is not @c NULL or @p
/// size_ret is not @c NULL, the program or the number of `char`s, respectively,
/// was retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p job is @c NULL, or
/// - @p value is not @c NULL and @p size is less than the number of `char`s
///   required for the program.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// job's program format was not set with @ref QDMI_job_set_payload_string.
/// @returns @ref QDMI_ERROR_OUTOFRANGE if the arguments are valid and the index
/// is greater than or equal to the number of programs set for the job.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid, and the
/// program cannot be queried in the current state of the job, for example,
/// because no payload has been set for the job yet.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
/// the function in the current session.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @note Calling this function with the @p value and @p size_ret set to @c NULL
/// is expected to allow checking whether the job's payload was set with @ref
/// QDMI_job_set_payload_string. Additionally, the size of the buffer needed to
/// retrieve the program is returned in @p size_ret if @p size_ret is not @c
/// NULL. See @ref QDMI_context_query_name for an example.
typedef int QDMI_job_query_payload_string(QDMI_Job job, size_t index,
                                          size_t size, char *value,
                                          size_t *size_ret);

/// Query a binary-based payload of a job.
///
/// The expected format of the returned payload depends on the program format of
/// the job, which can be queried with @ref
/// QDMI_job_query_payload_program_format.
///
/// @param[in] job is a handle to a job for which to query the payload. Must not
/// be
/// @c NULL.
/// @param[in] index is the index of the program to query. Must be less than the
/// number of
/// @param[in] size is the number of `char`s of the buffer pointed to by @p
/// value. Must be greater than or equal to the size required to retrieve the
/// program, except when @p value is @c NULL, in which case it is ignored.
/// @param[out] value is a pointer to the memory location where the program will
/// be stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret is the actual number of `char`s of the program being
/// queried. If this is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if, when the @p value is not @c NULL or @p
/// size_ret is not @c NULL, the program or the number of `char`s, respectively,
/// was retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p job is @c NULL, or
/// - @p value is not @c NULL and @p size is less than the number of `char`s
///   required for the program.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// job's program format was not set with @ref QDMI_job_set_payload_binary.
/// @returns @ref QDMI_ERROR_OUTOFRANGE if the arguments are valid and the index
/// is greater than or equal to the number of programs set for the job.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid, and the
/// program cannot be queried in the current state of the job, for example,
/// because no payload has been set for the job yet.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
/// the function in the current session.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @note Calling this function with the @p value and @p size_ret set to @c NULL
/// is expected to allow checking whether the job's payload was set with @ref
/// QDMI_job_set_payload_binary. Additionally, the size of the buffer needed to
/// retrieve the program is returned in @p size_ret if @p size_ret is not @c
/// NULL. See @ref QDMI_context_query_name for an example.
typedef int QDMI_job_query_payload_binary(QDMI_Job job, size_t index,
                                          size_t size, void *value,
                                          size_t *size_ret);

/// Query the number of shots set for a job.
/// @param[in] job is a handle to a job for which to query the number of shots.
/// Must not be @c NULL.
/// @param[out] value is a pointer to the memory location where the number of
/// shots will be stored.
///
/// @returns @ref QDMI_SUCCESS if the number of shots was retrieved
/// successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p job is @c NULL.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// device does not support setting or querying the number of shots.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid, the device
/// supports setting and querying the number of shots, and the number of shots
/// cannot be queried in the current state of the job, for example, because no
/// number of shots has been set for the job yet.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
/// the function in the current session.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_job_query_shot_count(QDMI_Job job, size_t *value);

/// Submit a job to the device.
///
/// This function can either be blocking until the job is finished or
/// non-blocking and return while the job is running. In the latter case, the
/// functions @ref QDMI_job_check and @ref QDMI_job_wait can be
/// used to check the status and wait for the job to finish.
///
/// @param[in] job is the job to submit. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the job was successfully submitted.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p job is @c NULL.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the argument is valid and the
/// device does not allow using the @ref job_interface "job interface" for the
/// current session.
/// @returns @ref QDMI_ERROR_FATAL if the job submission failed.
typedef int QDMI_job_submit(QDMI_Job job);

/// Cancel an already submitted job.
///
/// Remove the job from the queue of waiting jobs. This changes the status of
/// the job to @ref QDMI_JOB_STATUS_CANCELED.
///
/// @param[in] job is the job to cancel. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the job was successfully canceled.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if the @p job is @c NULL.
/// @returns @ref QDMI_ERROR_BADSTATE of the job already has the status @ref
/// QDMI_JOB_STATUS_DONE or QDMI_JOB_STATUS_CANCELED.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the argument is valid and the
/// device does not allow using the @ref job_interface "job interface" for the
/// current session.
/// @returns @ref QDMI_ERROR_FATAL if the job could not be canceled.
typedef int QDMI_job_cancel(QDMI_Job job);

/// Check the status of a job.
///
/// This function is non-blocking and returns immediately with the job status.
///
/// @param[in] job is the job to check the status of. Must not be @c NULL.
/// @param[out] status is the status of the job. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the job status was successfully checked.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p job or @p status is @c NULL.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the argument is valid and the
/// device does not allow using the @ref job_interface "job interface" for the
/// current session.
/// @returns @ref QDMI_ERROR_FATAL if the job status could not be checked.
typedef int QDMI_job_check(QDMI_Job job, QDMI_Job_Status *status);

/// Wait for a job to finish.
///
/// This function blocks until the job has either finished, has been canceled,
/// or the timeout has been reached. If @p timeout is not zero, this function
/// returns latest after the specified number of seconds.
///
/// @param[in] job is the job to wait for. Must not be @c NULL.
/// @param[in] timeout is the timeout in seconds.
/// If this is zero, the function waits indefinitely until the job has finished.
///
/// @returns @ref QDMI_SUCCESS if the job is finished or canceled.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p job is @c NULL.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the argument is valid and the
/// device does not allow using the @ref job_interface "job interface" for the
/// current session.
/// @returns @ref QDMI_ERROR_TIMEOUT if the argument is valid, the permission is
/// granted, and the @p timeout is not zero, and the job did not finish within
/// the specified time.
/// @returns @ref QDMI_ERROR_FATAL if the job could not be waited for and this
/// function returns before the job has finished or has been canceled.
typedef int QDMI_job_wait(QDMI_Job job, size_t timeout);

/// Retrieve the results of a job.
///
/// The results of the individual shots as a comma-separated list, for example,
/// "0010,1101,0101,1100,1001,1100" for four qubits and six shots.
///
/// @param[in] job is the job to retrieve the results from. Must not be @c NULL.
/// @param[in] index is the index of the result to query. Must be less than the
/// number of results available for the job.
/// @param[in] size is the number of `char`s of the buffer pointed to by @p
/// values. Must be greater than or equal to the size required to retrieve the
/// results, except when @p values is @c NULL, in which case it is ignored.
/// @param[out] values A pointer to the memory location where the results will
/// be stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret The actual number of `char`s of the data being queried.
/// If this is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if the device supports the specified result and,
/// when @p values is not @c NULL, the results were successfully retrieved.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p job is @c NULL, or
/// - @p values is not @c NULL and @p size is smaller than the size of the data
///   being queried.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// device does not support retrieving the results in this format.
/// @returns @ref QDMI_ERROR_OUTOFRANGE if the arguments are valid and the index
/// is greater than or equal to the number of results available for the job.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and the results
/// cannot be queried in the current state of the job, for example, because the
/// job has not finished yet.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
/// the function in the current session.
/// @returns @ref QDMI_ERROR_FATAL if an error occurred during the retrieval.
///
/// @note Calling this function with the @p value set to @c NULL is expected to
/// allow checking if the job supports retrieving the results in this format and
/// without the need to provide a buffer for it. Additionally, the size of the
/// buffer needed to retrieve the property is returned in @p size_ret if @p
/// size_ret is not @c NULL.
///
/// For example, to query the measurement results of a quantum circuit job,
/// the following code pattern can be used:
/// ```
/// // Query the size of the result.
/// size_t size;
/// auto ret = QDMI_job_get_shots(job, 0, nullptr, &size);
///
/// // Allocate memory for the result.
/// std::string shots(size - 1, '\0');
///
/// // Query the result.
/// QDMI_job_get_shots(job, size, shots.data(), nullptr);
/// ```
typedef int QDMI_job_get_shots(QDMI_Job job, size_t index, size_t size,
                               char *values, size_t *size_ret);

/// Retrieve the results as a histogram.
///
/// The histogram of the measurement results is represented as a key-value
/// mapping. This mapping is returned as a list of keys and an equal-length list
/// of values. The corresponding partners of keys and values can be found at the
/// same index in the lists. For example, if the keys are
/// `["00", "01", "10", "11"]` and the values are `[10, 20, 30, 40]`, the
/// histogram represents the mapping `{"00": 10, "01": 20, "10": 30, "11": 40}`.
///
/// @param[in] job is the job to retrieve the results from. Must not be @c NULL.
/// @param[in] index is the index of the result to query. Must be less than the
/// number of results available for the job.
/// @param[in] size_keys is the number of `char`s of the buffer pointed to by @p
/// keys. Must be greater or equal to the size required to retrieve the keys,
/// except when @p keys is @c NULL, in which case it is ignored.
/// @param[out] keys is a pointer to the memory location where the keys will be
/// stored. The list of keys are stored as a comma-separated list, i.e., in the
/// example above, the value of @p keys would be `"00,01,10,11"`. If this is @c
/// NULL, it is ignored.
/// @param[out] size_keys_ret Tís the actual number of `char`s of the keys being
/// queried. If this is @c NULL, it is ignored.
/// @param[in] size_values is the number of `int`s of the buffer pointed to by
/// @p values. Must be greater or equal to the size required to retrieve the
/// values, except when @p values is @c NULL, in which case it is ignored.
/// @param[out] values is a pointer to the memory location where the values will
/// be stored. This value must be interpreted as a list of integers. If this is
/// @c NULL, it is ignored.
/// @param[out] size_values_ret is the actual number of `int`s of the values
/// being queried. If this is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if the device supports the specified result and,
/// when @p keys and @p values are not @c NULL, the results were successfully
/// retrieved.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p job is @c NULL,
/// - @p keys is not @c NULL and @p size_keys is smaller than the size of the
///   keys being queried, or
/// - @p values is not @c NULL and @p size_values is smaller than the size of
///   the values being queried.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// device does not support retrieving the results in this format.
/// @returns @ref QDMI_ERROR_OUTOFRANGE if the arguments are valid and the index
/// is greater than or equal to the number of results available for the job.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and the results
/// cannot be queried in the current state of the job, for example, because the
/// job has not finished yet.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
/// the function in the current session.
/// @returns @ref QDMI_ERROR_FATAL if an error occurred during the retrieval.
///
/// @note Calling this function with the @p keys and @p values set to @c NULL is
/// expected to allow checking if the job supports retrieving the results in
/// this format and without the need to provide buffers for them. Additionally,
/// the size of the buffers needed to retrieve the properties is returned in @p
/// size_keys_ret and @p size_values_ret if they are not @c NULL.
///
/// For example, to query the histogram of the measurement results of a quantum
/// circuit job, the following code pattern can be used:
/// ```
/// // Query the size of the keys and values.
/// size_t size_keys, size_values;
/// auto ret = QDMI_job_get_histogram(job, 0, nullptr, &size_keys, 0,
/// nullptr, &size_values);
///
/// // Allocate memory for the keys and values.
/// std::string keys(size_keys - 1, '\0');
/// std::vector<size_t> values(size_values, 0);
///
/// // Query the keys and values.
/// QDMI_job_get_histogram(job,  size_keys, keys.data(), nullptr,
/// size_values, values.data(), nullptr);
/// ```
typedef int QDMI_job_get_histogram(QDMI_Job job, size_t index, size_t size_keys,
                                   char *keys, size_t *size_keys_ret,
                                   size_t size_values, size_t *values,
                                   size_t *size_values_ret);

/// Retrieve the state vector of a job.
///
/// The complex amplitudes are stored as a list of real and imaginary parts. The
/// real part of the amplitude is at index `2n` and the imaginary part is at
/// index `2n+1`. For example, the state vector of a 2-qubit system with
/// amplitudes `(0.5, 0.5), (0.5, -0.5), (-0.5, 0.5), (-0.5, -0.5)` would be
/// represented as `{0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5}`.
/// @param[in] job is the job to retrieve the state vector from. Must not be @c
/// NULL.
/// @param[in] index is the index of the state vector to query. Must be less
/// than the number of state vectors available for the job.
/// @param[in] size is the number of `double`s of the buffer pointed to by @p
/// values. Must be greater than or equal to the size required to retrieve the
/// state vector, except when @p values is @c NULL, in which case it is ignored.
/// @param[out] values is a pointer to the memory location where the state
/// vector will be stored. Its value must be interpreted as a list of complex
/// amplitudes, where the real and imaginary parts are interleaved as described
/// above. If this is @c NULL, it is ignored.
/// @param[out] size_ret The actual number of `double`s of the state vector
/// being queried. If this is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if the device supports the specified result and,
/// when the @p values is not @c NULL, the state vector was successfully
/// retrieved.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p job is @c NULL, or
/// - @p values is not @c NULL and @p size is smaller than the size of the
///   state vector being queried.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// device does not support retrieving the state vector in this format.
/// @returns @ref QDMI_ERROR_OUTOFRANGE if the arguments are valid and the index
/// is greater than or equal to the number of state vectors available for the
/// job.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and the state
/// vector cannot be queried in the current state of the job, for example,
/// because the job has not finished yet.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
/// the function in the current session.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @note Calling this function with the @p values set to @c NULL is
/// expected to allow checking if the job supports retrieving the results in
/// this format and without the need to provide a buffer for it. Additionally,
/// the size of the buffer needed to retrieve the results is returned in @p
/// size_ret if it is not @c NULL.
///
/// @remark This function is only relevant for simulators.
typedef int QDMI_job_get_state_vector_dense(QDMI_Job job, size_t index,
                                            size_t size, double *values,
                                            size_t *size_ret);

/// Retrieve the probabilities of a job.
///
/// The probabilities are stored as a list of real numbers. The probability of
/// the state with index `n` is at index `n` in the list. For example, the
/// probabilities of a 2-qubit system with states `00, 01, 10, 11` would be
/// represented as `{0.25, 0.25, 0.25, 0.25}`.
///
/// @param[in] job is the job to retrieve the probabilities from. Must not be @c
/// NULL.
/// @param index is the index of the probabilities to query. Must be less than
/// the number of probabilities available for the job.
/// @param[in] size is the number of `double`s of the buffer pointed to by @p
/// values. Must be greater or equal to the size required to retrieve the
/// probabilities, except when @p values is @c NULL, in which case it is
/// ignored.
/// @param[out] values A pointer to the memory location where the probabilities
/// will be stored. Its value must be interpreted as a list of real numbers,
/// where the probability of the state with index `n` is at index `n` in the
/// list. If this is @c NULL, it is ignored.
/// @param[out] size_ret The actual number of `double`s of the probabilities
/// being queried. If this is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if the device supports the specified result and,
/// when the @p values is not @c NULL, the probabilities were successfully
/// retrieved.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p job is @c NULL, or
/// - @p values is not @c NULL and @p size is smaller than the size of the
///   probabilities being queried.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
/// the function in the current session.
/// @returns @ref QDMI_ERROR_FATAL if an error occurred during the retrieval.
///
/// @note Calling this function with the @p values set to @c NULL is expected to
/// allow checking if the job supports retrieving the results in this format and
/// without the need to provide a buffer for it. Additionally, the size of the
/// buffer needed to retrieve the results is returned in @p size_ret if it is
/// not @c NULL.
///
/// @remark This function is only relevant for simulators.
typedef int QDMI_job_get_probabilities_dense(QDMI_Job job, size_t index,
                                             size_t size, double *values,
                                             size_t *size_ret);
/// Retrieve the state vector of a job.
///
/// The sparse state vector is represented as a key-value mapping. This mapping
/// is returned as a list of keys and an equal-length list of values. The
/// corresponding partners of keys and values can be found at the same index in
/// the lists.
///
/// @param[in] job is the job to retrieve the state vector from. Must not be @c
/// NULL.
/// @param index is the index of the state vector to query. Must be less than
/// the number of state vectors available for the job.
/// @param[in] size_keys is the number of `char`s of the buffer pointed to by @p
/// keys. Must be greater or equal to the size required to retrieve the keys,
/// except when @p keys is @c NULL, in which case it is ignored.
/// @param[out] keys is a pointer to the memory location where the keys will be
/// stored. The list of keys are stored as a comma-separated list, i.e., if the
/// keys are `["00", "01", "10", "11"]`, the value of @p keys would be
/// `"00,01,10,11"`. If this is @c NULL, it is ignored.
/// @param[out] size_keys_ret is the actual number of `char`s of the keys being
/// queried. If this is @c NULL, it is ignored.
/// @param[in] size_values is the number of `double`s of the buffer pointed to
/// by
/// @p values. Must be greater or equal to the size required to retrieve the
/// values, except when @p values is @c NULL, in which case it is ignored.
/// @param[out] values is a pointer to the memory location where the values will
/// be stored. This value must be interpreted as a list of complex amplitudes,
/// where the real and imaginary parts are interleaved as described for @ref
/// QDMI_job_get_state_vector_dense. If this is @c NULL, it is ignored.
/// @param[out] size_values_ret is the actual number of `double`s of the values
/// being queried. If this is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if the device supports the specified result and,
/// when the @p keys and @p values are not @c NULL, the state vector was
/// successfully retrieved.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p job is @c NULL,
/// - @p keys is not @c NULL and @p size_keys is smaller than the size of the
///   keys being queried, or
/// - @p values is not @c NULL and @p size_values is smaller than the size of
///   the values being queried.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// device does not support retrieving the state vector in this format.
/// @returns @ref QDMI_ERROR_OUTOFRANGE if the arguments are valid and the index
/// is greater than or equal to the number of state vectors available for the
/// job.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and the state
/// vector cannot be queried in the current state of the job, for example,
/// because the job has not finished yet.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
/// the function in the current session.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_job_get_state_vector_sparse(QDMI_Job job, size_t index,
                                             size_t size_keys, char *keys,
                                             size_t *size_keys_ret,
                                             size_t size_values, double *values,
                                             size_t *size_values_ret);

/// Retrieve the probabilities of a job.
///
/// The sparse probabilities are represented as a key-value mapping. This
/// mapping is returned as a list of keys and an equal-length list of values.
/// The corresponding partners of keys and values can be found at the same index
/// in the lists.
///
/// @param[in] job is the job to retrieve the probabilities from. Must not be @c
/// NULL.
/// @param index is the index of the probabilities to query. Must be less than
/// the number of probabilities available for the job.
/// @param[in] size_keys is the number of `char`s of the buffer pointed to by @p
/// keys. Must be greater or equal to the size required to retrieve the keys,
/// except when @p keys is @c NULL, in which case it is ignored.
/// @param[out] keys is a pointer to the memory location where the keys will be
/// stored. The list of keys are stored as a comma-separated list, i.e., if the
/// keys are `["00", "01", "10", "11"]`, the value of @p keys would be
/// `"00,01,10,11"`. If this is @c NULL, it is ignored.
/// @param[out] size_keys_ret is the actual number of `char`s of the keys being
/// queried. If this is @c NULL, it is ignored.
/// @param[in] size_values is the number of `double`s of the buffer pointed to
/// by
/// @p values. Must be greater or equal to the size required to retrieve the
/// values, except when @p values is @c NULL, in which case it is ignored.
/// @param[out] values is a pointer to the memory location where the values will
/// be stored. This value must be interpreted as a list of real numbers, where
/// the probability of the state with index `n` is at index `n` in the list. If
/// this is @c NULL, it is ignored.
/// @param[out] size_values_ret is the actual number of `double`s of the values
/// being queried. If this is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if the device supports the specified result and,
/// when the @p keys and @p values are not @c NULL, the probabilities were
/// successfully retrieved.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p job is @c NULL,
/// - @p keys is not @c NULL and @p size_keys is smaller than the size of the
///   keys being queried, or
/// - @p values is not @c NULL and @p size_values is smaller than the size of
///   the values being queried.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// device does not support retrieving the probabilities in this format.
/// @returns @ref QDMI_ERROR_OUTOFRANGE if the arguments are valid and the index
/// is greater than or equal to the number of probabilities available for the
/// job.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and the
/// probabilities cannot be queried in the current state of the job, for
/// example, because the job has not finished yet.
/// @returns @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
/// the function in the current session.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_job_get_probabilities_sparse(QDMI_Job job, size_t index,
                                              size_t size_keys, char *keys,
                                              size_t *size_keys_ret,
                                              size_t size_values,
                                              double *values,
                                              size_t *size_values_ret);

/// Free a job.
///
/// Free the resources associated with a job. Using a job handle after it was
/// freed is undefined behavior.
///
/// @param[in] job The job to free.
///
/// @remark The device implementation may choose to delete the job entirely or
/// to just destroy the handle, but to save the job itself, to later be able to
/// retrieve a new handle for the job with @ref QDMI_session_retrieve_job_by_id.
typedef void QDMI_job_free(QDMI_Job job);

/** @} */ // end of job_job_interface

/** @} */ // end of job_interface

// NOLINTEND(modernize-use-using, modernize-redundant-void-arg)

#ifdef __cplusplus
} // extern "C"
#endif
