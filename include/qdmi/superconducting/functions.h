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
/// Defines the @ref superconducting_interface "superconducting interface".

#pragma once

#include "qdmi/core.h"
#include "qdmi/superconducting/types.h"

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// NOLINTBEGIN(modernize-use-using)

/// @defgroup superconducting_interface QDMI Superconducting Interface
///
/// Describes the functions to be implemented by a superconducting QPU.
/// @{

/// @defgroup superconducting_session_interface QDMI Superconducting Session
/// Interface
///
/// The superconducting session interface adds functions to the session type
/// for querying superconducting qubits specific properties.
///
/// @{

/// `size_t` A scale factor to interpret all duration values relative to
/// nanoseconds (ns).
///
/// The QPU implementation reports this scale factor. A client must multiply any
/// raw duration value received from the QPU by this factor to obtain the
/// physical duration in nanoseconds.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[out] value is a pointer to the memory location where the value of the
/// property will be stored. If this is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if the QPU supports this property and, when @p
/// value is not @c NULL, the property was successfully retrieved.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// QPU does not support this property.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid, and the
/// property cannot be queried in the current state of the session, for example,
/// because the session is not initialized with @ref QDMI_session_initialize.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @note If querying this property returns @ref QDMI_ERROR_NOTSUPPORTED, a
/// client should assume a default value of `1.0`.
typedef int QDMI_session_query_sc_duration_scale_factor(QDMI_Session session,
                                                        uint64_t *value);

/// Query a list of @ref QDMI_SCQubit "qubits" available on the QPU.
///
/// The returned @ref QDMI_SCQubit handles may be used to query qubit and
/// operation properties. The list need not be sorted based on the their index,
/// see @ref QDMI_session_query_sc_qubit_index.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] size is the number of elements of type @ref QDMI_SCQubit that the
/// parameter @p value points to. Must be greater than or equal to the number of
/// qubits, except when @p value is @c NULL, in which case it is ignored.
/// @param[out] values is a pointer to the memory location where the value of
/// the property will be stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret is the actual number of elements of type @ref
/// QDMI_SCQubit being queried. If this is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if the QPU supports querying the qubits and, when
/// @p values is not @c NULL, the @ref QDMI_SCQubit "qubits" were successfully
/// retrieved.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// QPU does not support querying the qubits.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid, querying
/// qubits is supported, and the property cannot be queried in the current state
/// of the session, for example, because the session is not initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @note Calling this function with the @p value set to @c NULL is expected to
/// allow checking if the QPU supports querying qubits without actually
/// retrieving them and without the need to provide a buffer for it.
/// Additionally, the size of the buffer needed to retrieve the property is
/// returned in @p size_ret if @p size_ret is not @c NULL.
///
/// For example, the following code pattern can be used:
/// ```
/// // Query the size of the list.
/// size_t size;
/// QDMI_session_query_sc_qubits(session, 0, nullptr, &size);
///
/// // Allocate memory for the property.
/// std::vector<QDMI_SCQubit> qubits(size, nullptr);
///
/// // Query the qubits.
/// QDMI_session_query_sc_qubits(session, size, qubits.data(), nullptr);
/// ```
typedef int QDMI_session_query_sc_qubits(QDMI_Session session, size_t size,
                                         QDMI_SCQubit *values,
                                         size_t *size_ret);

/// Query a @ref QDMI_SCQubit "qubit" by its index.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] index is the index of the qubit to query.
/// @param[out] qubit is a pointer to the memory location where the qubit will
/// be stored. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the qubit was successfully retrieved.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session or @p qubit is @c
/// NULL.
/// @returns @ref QDMI_ERROR_NOTFOUND if @p index does not correspond to any
/// qubit for the QPU.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and the qubits
/// can be queried, but the qubit cannot be queried in the current state of the
/// session, for example, because the session is not initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @see QDMI_session_query_sc_qubit_index
typedef int QDMI_session_query_sc_qubit_by_index(QDMI_Session session,
                                                 size_t index,
                                                 QDMI_SCQubit *qubit);

/// Query a @ref QDMI_SCQubit "qubit" by its name.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] name is the name of the qubit to query as a null-terminated
/// string. Must not be @c NULL.
/// @param[out] qubit is a pointer to the memory location where the qubit will
/// be stored.
///
/// @returns @ref QDMI_SUCCESS if the qubit was successfully retrieved.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session or @p name is @c
/// NULL.
/// @returns @ref QDMI_ERROR_NOTFOUND if no qubit with the specified name exists
/// for the QPU.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the QPU
/// does not support querying qubits by name.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and the qubits
/// can be queried, but the qubit cannot be queried in the current state of the
/// session, for example, because the session is not initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @see QDMI_session_query_sc_qubit_name
typedef int QDMI_session_query_sc_qubit_by_name(QDMI_Session session,
                                                const char *name,
                                                QDMI_SCQubit *qubit);

/// Query a qubit's index.
///
/// The qubit's index is an integer that uniquely identifies the qubit and is
/// used for indexing the qubits of the QPU. The indices do not need to be
/// consecutive.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] qubit is the qubit to query. Must not be @c NULL.
/// @param[out] value is a pointer to the memory location where the index will
/// be stored. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the index was successfully retrieved.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session, @p qubit, or @p
/// value is @c NULL.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and the qubits
/// can be queried, but the index cannot be queried in the current state of the
/// session, for example, because the session is not initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_sc_qubit_index(QDMI_Session session,
                                              QDMI_SCQubit qubit,
                                              size_t *value);

/// Query a qubit's name.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] qubit is the qubit to query. Must not be @c NULL.
/// @param[in] size is the number of `char`s that fit into @p value. Must be
/// greater than or equal to the number of `char`s required for the qubit's
/// name, except when @p value is @c NULL, in which case it is ignored.
/// @param[out] value is a pointer to the memory location where the qubit's name
/// will be stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret is the actual number of `char`s of the name being
/// queried, including the terminating null character `'\0'`. If this is @c
/// NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if, when the @p value is not @c NULL or @p
/// size_ret is not @c NULL, the qubit's name or the number of `char`s,
/// respectively, was retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p session or @p qubit is @c NULL, or
/// - @p value is not @c NULL and @p size is less than the number of `char`s
///   required for the qubit's name.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the QPU
/// does not support querying the qubit's name.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and the qubits
/// can be queried, but the name cannot be queried in the current state of the
/// session, for example, because the session is not initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_sc_qubit_name(QDMI_Session session,
                                             QDMI_SCQubit qubit, size_t size,
                                             char *value, size_t *size_ret);

/// Query a qubit's T1 coherence time.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] qubit is the qubit to query. Must not be @c NULL.
/// @param[out] value is a pointer to the memory location where the T1 coherence
/// time will be stored. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the T1 coherence time was successfully
/// retrieved.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session, @p qubit, or @p
/// value is @c NULL.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and the qubits
/// can be queried, but the T1 coherence time cannot be queried in the current
/// state of the session, for example, because the session is not initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_sc_qubit_coherence_t1(QDMI_Session session,
                                                     QDMI_SCQubit qubit,
                                                     uint64_t *value);

/// Query a qubit's T2 coherence time.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] qubit is the qubit to query. Must not be @c NULL.
/// @param[out] value is a pointer to the memory location where the T2 coherence
/// time will be stored. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the T2 coherence time was successfully
/// retrieved.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session, @p qubit, or @p
/// value is @c NULL.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and the qubits
/// can be queried, but the T2 coherence time cannot be queried in the current
/// state of the session, for example, because the session is not initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_sc_qubit_coherence_t2(QDMI_Session session,
                                                     QDMI_SCQubit qubit,
                                                     uint64_t *value);

/// Query the coupling map of the QPU.
///
/// The returned list contains pairs of qubits that are coupled. The pairs in
/// the list are flattened such that the first qubit of the pair is at index
/// `2n` and the second qubit is at index `2n+1`.
///
/// The qubits returned in that list are represented as @ref QDMI_SCQubit
/// handles. For example, consider a 3-qubit QPU with a coupling map `(0,
/// 1), (1, 2)`. Additionally, assume `qubit_i` is the handle for the i-th
/// qubit. Then, `{qubit_0, qubit_1, qubit_1, qubit_2}` would be returned.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] size is the number of elements of type @ref QDMI_SCQubit that the
/// parameter @p value points to. Must be greater than or equal to twice the
/// number of coupled pairs of qubits, except when @p value is @c NULL, in which
/// case it is ignored.
/// @param[out] values is a pointer to the memory location where the value of
/// the property will be stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret is the actual number of elements of type @ref
/// QDMI_SCQubit being queried. If this is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if the QPU supports querying the coupling map
/// and, when @p value is not @c NULL, the coupling map was successfully
/// retrieved.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p session is @c NULL, or
/// - @p value is not @c NULL and @p size is less than twice the number of
///   coupled pairs of qubits.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// QPU does not support querying the coupling map.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and querying
/// the coupling map is supported, but the coupling map cannot be queried in the
/// current state of the session, for example, because the session is not
/// initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_coupling_map(QDMI_Session session, size_t size,
                                            QDMI_SCQubit *values,
                                            size_t *size_ret);

/// Query the supported operations of the QPU.
///
/// The returned list contains all operations that the QPU supports for
/// execution. A client can use this information to determine which operations
/// can be used when constructing programs for the QPU.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] size is the number of elements of type @ref QDMI_SCOperation that
/// the parameter @p value points to. Must be greater than or equal to the
/// number of supported operations, except when @p value is @c NULL, in which
/// case it is ignored.
/// @param[out] values is a pointer to the memory location where the supported
/// operations will be stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret is the actual number of supported operations. If this
/// is
/// @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if, when the @p value is not @c NULL, the
/// supported operations were retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p session is @c NULL, or
/// - @p value is not @c NULL and @p size is less than the number of supported
///   operations.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid, and the
/// property cannot be queried in the current state of the session, for example,
/// because the session is not initialized with @ref QDMI_session_initialize.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_sc_operations(QDMI_Session session, size_t size,
                                             QDMI_SCOperation *values,
                                             size_t *size_ret);

/// Query an operation by its unique identifier.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] id is the unique identifier of the operation to query as a
/// null-terminated string. Must not be @c NULL.
/// @param[out] operation is a pointer to the memory location where the
/// operation will be stored.
///
/// @returns @ref QDMI_SUCCESS if the operation was retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session or @p id is @c NULL.
/// @returns @ref QDMI_ERROR_NOTFOUND if no operation with the specified ID
/// exists for the QPU.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and operations
/// can be queried, but the operation cannot be queried in the current state of
/// the session, for example, because the session is not initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_sc_operation_by_id(QDMI_Session session,
                                                  const char *id,
                                                  QDMI_SCOperation *operation);

/// Query an operation's unique identifier.
///
/// The operation's ID is a short string that uniquely identifies the operation.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] operation is the operation to query. Must not be @c NULL.
/// @param[in] size is the number of `char`s that fit into @p value. Must be
/// greater than or equal to the number of `char`s required for the operation's
/// unique identifier, except when @p value is @c NULL, in which case it is
/// ignored.
/// @param[out] value is a pointer to the memory location where the operation's
/// unique identifier will be stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret is the actual number of `char`s of the unique
/// identifier being queried, including the terminating null character `'\0'`.
/// If this is
/// @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if, when the @p value is not @c NULL or @p
/// size_ret is not @c NULL, the operation's unique identifier or the number of
/// `char`s, respectively, was retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p session or @p operation is @c NULL, or
/// - @p value is not @c NULL and @p size is less than the number of `char`s
///   required for the operation's unique identifier.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the QPU
/// does not support querying the operation's unique identifier.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and operations
/// can be queried, but the unique identifier cannot be queried in the current
/// state of the session, for example, because the session is not initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_sc_operation_id(QDMI_Session session,
                                               QDMI_SCOperation operation,
                                               size_t size, char *value,
                                               size_t *size_ret);

/// Query an operation's name.
///
/// The operation's name is a human-readable string that describes the
/// operation.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] operation is the operation to query. Must not be @c NULL.
/// @param[in] size is the number of `char`s that fit into @p value. Must be
/// greater than or equal to the number of `char`s required for the operation's
/// name, except when @p value is @c NULL, in which case it is ignored.
/// @param[out] value is a pointer to the memory location where the operation's
/// name will be stored. If this is @c NULL, it is ignored.
/// @param[out] size_ret is the actual number of `char`s of the name being
/// queried, including the terminating null character `'\0'`. If this is @c
/// NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if, when the @p value is not @c NULL or @p
/// size_ret is not @c NULL, the operation's name or the number of `char`s,
/// respectively, was retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p session or @p operation is @c NULL, or
/// - @p value is not @c NULL and @p size is less than the number of `char`s
///   required for the operation's name.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the QPU
/// does not support querying the operation's name.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and operations
/// can be queried, but the name cannot be queried in the current state of the
/// session, for example, because the session is not initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_sc_operation_name(QDMI_Session session,
                                                 QDMI_SCOperation operation,
                                                 size_t size, char *value,
                                                 size_t *size_ret);

/// Query the number of parameters that an operation takes.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] operation is the operation to query. Must not be @c NULL.
/// @param[out] value is a pointer to the memory location where the number of
/// parameters will be stored. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the number of parameters was successfully
/// retrieved.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session, @p operation, or @p
/// value is @c NULL.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and operations
/// can be queried, but the
/// number of parameters cannot be queried in the current state of the session,
/// for example, because the session is not initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_sc_operation_parameter_count(
    QDMI_Session session, QDMI_SCOperation operation, size_t *value);

/// Query the number of qubits that an operation is applied to.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] operation is the operation to query. Must not be @c NULL.
/// @param[out] value is a pointer to the memory location where the number of
/// qubits will be stored. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the number of qubits was successfully
/// retrieved.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session, @p operation, or @p
/// value is @c NULL.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and operations
/// can be queried, but the number of qubits cannot be queried in the current
/// state of the session, for example, because the session is not initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_sc_operation_qubit_count(
    QDMI_Session session, QDMI_SCOperation operation, size_t *value);

/// Query the qubits that an operation can be applied to.
///
/// The returned list of @ref QDMI_SCQubit "qubits" is a list of tuples. Each
/// tuple represents a valid combination of qubits for the operation. The number
/// of qubits in each tuple matches the value returned by
/// @ref QDMI_session_query_sc_operation_qubit_count for the operation. The
/// tuples in the list are flattened such that the qubits of the n-th tuple are
/// at indices `m*n` to `m*n+m-1`, where `m` is the number of qubits that the
/// operation is applied to.
///
/// @ param[in] session is the session used for the query. Must not be @ c NULL.
/// @ param[in] operation is the operation to query. Must not be @ c NULL.
/// @ param[in] size is the number of elements of type @ ref QDMI_SCQubit that
/// the parameter @ p value points to. Must be greater than or equal to the
/// number of qubits that the operation is applied to, except when @ p value is
/// @ c NULL, in which case it is ignored.
/// @ param[out] value is a pointer to the memory location where the qubits that
/// the operation can be applied to will be stored. If this is @ c NULL, it is
/// ignored.
/// @ param[out] size_ret is the actual number of elements of type @ ref
/// QDMI_SCQubit being queried. If this is @ c NULL, it is ignored.
///
/// @ return @ ref QDMI_SUCCESS if, when the @ p value is not @ c NULL, the
/// qubits that the operation is applied to were retrieved successfully.
/// @ return @ ref QDMI_ERROR_INVALIDARGUMENT if
/// - @ p session or @ p operation is @ c NULL, or
/// - @ p value is not @ c NULL and @ p size is less than the number of qubits
///   that the operation is applied to.
/// @ return @ ref QDMI_ERROR_BADSTATE if the arguments are valid and operations
/// can be queried, but the qubits cannot be queried in the current state of the
/// session, for example, because the session is not initialized.
/// @ return @ ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_sc_operation_qubits(QDMI_Session session,
                                                   QDMI_SCOperation operation,
                                                   size_t size,
                                                   QDMI_SCQubit *value,
                                                   size_t *size_ret);

/// Query the duration of an operation.
///
/// The duration is returned in units of the scale factor returned by @ref
/// QDMI_session_query_sc_duration_scale_factor. For example, if the scale
/// factor is `0.5`, and the duration of an operation is `20`, then the physical
/// duration of the operation is `20 * 0.5 = 10` nanoseconds.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] operation is the operation to query. Must not be @c NULL.
/// @param[in] qubits is a pointer to an array of @ref QDMI_SCQubit "qubits"
/// that the operation is applied to. The qubits must match one of the tuples
/// returned by @ref QDMI_session_query_sc_operation_qubits for the operation.
/// @param[in] params is a pointer to an array of parameter values for the
/// operation. The parameters must be in the same order as defined by the QPU
/// implementation for the operation.
/// @param[out] value is a pointer to the memory location where the duration
/// will be stored. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the duration was successfully retrieved.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p session or @p operation is @c NULL,
/// - @p qubits does not match one of the tuples returned by @ref
///   QDMI_session_query_sc_operation_qubits for the operation, or
/// - @p params does not match the expected parameters for the operation.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and operations
/// can be queried, but the duration cannot be queried in the current state of
/// the session, for example, because the session is not initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_sc_operation_duration(QDMI_Session session,
                                                     QDMI_SCOperation operation,
                                                     const QDMI_SCQubit *qubits,
                                                     const double *params,
                                                     uint64_t *value);

/// Query the fidelity of an operation.
///
/// @param[in] session is the session used for the query. Must not be @c NULL.
/// @param[in] operation is the operation to query. Must not be @c NULL.
/// @param[in] qubits is a pointer to an array of @ref QDMI_SCQubit "qubits"
/// that the operation is applied to. The qubits must match one of the tuples
/// returned by @ref QDMI_session_query_sc_operation_qubits for the
/// operation.
/// @param[in] params is a pointer to an array of parameter values for the
/// operation. The parameters must be in the same order as defined by the QPU
/// implementation for the operation.
/// @param[out] value is a pointer to the memory location where the fidelity
/// will be stored. Must not be @c NULL.
///
/// @returns @ref QDMI_SUCCESS if the fidelity was successfully retrieved.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if
/// - @p session or @p operation is @c NULL,
/// - @p qubits does not match one of the tuples returned by @ref
///   QDMI_session_query_sc_operation_qubits for the operation, or
/// - @p params does not match the expected parameters for the operation.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid and operations
/// can be queried, but the fidelity cannot be queried in the current state of
/// the session, for example, because the session is not initialized.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_session_query_sc_operation_fidelity(QDMI_Session session,
                                                     QDMI_SCOperation operation,
                                                     const QDMI_SCQubit *qubits,
                                                     const double *params,
                                                     double *value);

/** @} */ // end of superconducting_session_interface

struct QDMI_SC_Interface_impl_d {
  /// @see QDMI_session_query_sc_duration_scale_factor
  QDMI_session_query_sc_duration_scale_factor
      *session_query_sc_duration_scale_factor{};
  /// @see QDMI_session_query_sc_qubits
  QDMI_session_query_sc_qubits *session_query_sc_qubits{};
  /// @see QDMI_session_query_sc_qubit_by_index
  QDMI_session_query_sc_qubit_by_index *session_query_sc_qubit_by_index{};
  /// @see QDMI_session_query_sc_qubit_by_name
  QDMI_session_query_sc_qubit_by_name *session_query_sc_qubit_by_name{};
  /// @see QDMI_session_sc_query_qubit_index
  QDMI_session_query_sc_qubit_index *session_query_sc_qubit_index{};
  /// @see QDMI_session_query_sc_qubit_name
  QDMI_session_query_sc_qubit_name *session_query_sc_qubit_name{};
  /// @see QDMI_session_query_sc_qubit_coherence_t1
  QDMI_session_query_sc_qubit_coherence_t1
      *session_query_sc_qubit_coherence_t1{};
  /// @see QDMI_session_query_sc_qubit_coherence_t2
  QDMI_session_query_sc_qubit_coherence_t2
      *session_query_sc_qubit_coherence_t2{};
  /// @see QDMI_session_query_coupling_map
  QDMI_session_query_coupling_map *session_query_coupling_map{};
  /// @see QDMI_session_query_sc_operations
  QDMI_session_query_sc_operations *session_query_sc_operations{};
  /// @see QDMI_session_query_sc_operation_by_id
  QDMI_session_query_sc_operation_by_id *session_query_sc_operation_by_id{};
  /// @see QDMI_session_query_sc_operation_id
  QDMI_session_query_sc_operation_id *session_query_sc_operation_id{};
  /// @see QDMI_session_query_operation_name
  QDMI_session_query_sc_operation_name *session_query_sc_operation_name{};
  /// @see QDMI_session_query_sc_operation_parameter_count
  QDMI_session_query_sc_operation_parameter_count
      *session_query_sc_operation_parameter_count{};
  /// @see QDMI_session_query_sc_operation_qubit_count
  QDMI_session_query_sc_operation_qubit_count
      *session_query_sc_operation_qubit_count{};
  /// @see QDMI_session_query_sc_operation_qubits
  QDMI_session_query_sc_operation_qubits *session_query_sc_operation_qubits{};
  /// @see QDMI_session_query_sc_operation_duration
  QDMI_session_query_sc_operation_duration
      *session_query_sc_operation_duration{};
  /// @see QDMI_session_query_sc_operation_fidelity
  QDMI_session_query_sc_operation_fidelity
      *session_query_sc_operation_fidelity{};
};

/** @} */ // end of superconducting_interface

// NOLINTEND(modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
