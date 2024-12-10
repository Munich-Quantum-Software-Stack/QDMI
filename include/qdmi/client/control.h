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
 * @brief Defines the functions for controlling a device.
 * @details The control interface allows submitting jobs to a device, managing
 * the jobs, and retrieving the results.
 * @see qdmi/device/control.h for the device interface.
 */

#pragma once

#include "qdmi/common/types.h"
#include "qdmi/device/types.h"
#include "qdmi/driver/types.h"

#ifdef __cplusplus
#include <cstddef>

extern "C" {
#else
#include <stddef.h>
#endif

/**
 * @brief Create a job on a device.
 * @details This function creates a job that consists of a circuit.
 * Additional parameters, such as the number of shots, can be set using @ref
 * QDMI_control_set_parameter. After setting all necessary parameters, the
 * job must be submitted for execution using @ref QDMI_control_submit_job.
 * @param[in] dev identifies the device to create the job on.
 * @param[in] format is the format of the program. Refer to @ref
 * QDMI_PROGRAM_FORMAT_T for available options. Note that the availability of
 * formats depends on the device.
 * @param[in] size is the size of the program in bytes.
 * @param[in] prog is the program to run. If this is @c NULL, it is ignored.
 * @param[out] job is a pointer to a handle that will store the created job.
 * Must not be @c NULL, except when @p prog is @c NULL, in which case it is
 * ignored.
 * @return @ref QDMI_SUCCESS if the device supports the specified @ref
 * QDMI_Program_Format @p format and, when @p prog is not @c NULL, the job was
 * successfully created.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p prog is not @c NULL and @p size
 * is equal to zero, or the program @p prog is invalid (e.g.,
 * contains a syntax error).
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the device does not support the
 * specified program format @p format.
 * @return @ref QDMI_ERROR_FATAL if the job creation failed due to a fatal
 * error.
 *
 * @note By calling this function with @p prog and @p job set to @c NULL, the
 * function can be used to check if the device supports the specified program
 * format without creating a job and without the need to provide a program.
 */
int QDMI_control_create_job(QDMI_Device dev, QDMI_Program_Format format,
                            size_t size, const void *prog, QDMI_Job *job);

/**
 * @brief Set a parameter for a job.
 * @details The parameter must be one of the parameters defined in @ref
 * QDMI_Job_Parameter. It is specified by a @p value pointer and the @p
 * size of the value being pointed to in bytes.
 * @param[in] dev is a handle to the device which the @p job belongs to.
 * @param[in] job is a handle to a job for which to set @p param.
 * @param[in] param identifies the parameter whose value will be set.
 * @param[in] size specifies the size in bytes of the data pointed to by @p
 * value.
 * @param[in] value is a pointer to the memory location that contains the value
 * of the parameter to be set. The data pointed to by @p value is copied and
 * can be safely reused after this function returns.
 * @return @ref QDMI_SUCCESS if the parameter was successfully set.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if the job does not exist, is in an
 * invalid state, the parameter is invalid, @p value is @c NULL, or if @p size
 * is not the expected size for the parameter (if specified by the parameter).
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the device does not support the
 * parameter.
 * @return @ref QDMI_ERROR_FATAL if the parameter could not be set.
 */
int QDMI_control_set_parameter(QDMI_Device dev, QDMI_Job job,
                               QDMI_Job_Parameter param, size_t size,
                               const void *value);

/**
 * @brief Submit a job to the device.
 * @details Submit a via @ref QDMI_control_create_job previously created job to
 * the device. The job can be configured via @ref QDMI_control_set_parameter
 * before submission.
 * @note This function can either be blocking until the job is finished or
 * non-blocking and return while the job is running. In the latter case, there
 * are the functions @ref QDMI_control_check_dev and @ref QDMI_control_wait_dev
 * to check the status and wait for the job to finish.
 * @param[in] dev The device to submit the job to.
 * @param[in] job The job to submit.
 * @return @ref QDMI_SUCCESS if the job was successfully submitted.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if the job is in an invalid state.
 * @return @ref QDMI_ERROR_FATAL if the job submission failed.
 */
int QDMI_control_submit_job(QDMI_Device dev, QDMI_Job job);

/**
 * @brief Cancel an already submitted job.
 * @details Remove the job from the queue of waiting jobs. This changes the
 * status of the job to @ref QDMI_JOB_STATUS_CANCELLED.
 * @param[in] dev The device to cancel the job on.
 * @param[in] job The job to cancel.
 * @return @ref QDMI_SUCCESS if the job was successfully cancelled.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if the job has already the status
 * @ref QDMI_JOB_STATUS_DONE or the job does not exist.
 * @return @ref QDMI_ERROR_FATAL if the job could not be cancelled.
 */
int QDMI_control_cancel(QDMI_Device dev, QDMI_Job job);

/**
 * @brief Check the status of a job.
 * @note This function is non-blocking and returns immediately with the job
 * status.
 * @note It is *not* necessary to call this function before calling
 * @ref QDMI_control_get_data_dev.
 * @param[in] dev The device to check the status on.
 * @param[in] job The job to check the status of.
 * @param[out] status The status of the job.
 * @return @ref QDMI_SUCCESS if the job status was successfully checked.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if the job does not exist.
 */
int QDMI_control_check(QDMI_Device dev, QDMI_Job job, QDMI_Job_Status *status);

/**
 * @brief Wait for a job to finish.
 * @details This function blocks until the job has either finished or has been
 * cancelled.
 * @param[in] dev The device to wait on.
 * @param[in] job The job to wait for.
 * @return @ref QDMI_SUCCESS if the job is finished or cancelled.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if the job does not exist.
 * @return @ref QDMI_ERROR_FATAL if the job could not be waited for and this
 * function returns before the job has finished or has been cancelled.
 */
int QDMI_control_wait(QDMI_Device dev, QDMI_Job job);

/**
 * @brief Retrieve the results of a job.
 * @details The results of a job can vary
 * @param[in] dev The device to retrieve the results from.
 * @param[in] job The job to retrieve the results from.
 * @param[in] result The result to retrieve.
 * @param[in] size The size of the data buffer in bytes.
 * @param[out] data The data of the result.
 * @param[out] size_ret The size of the returned data in bytes.
 * @return @ref QDMI_SUCCESS if the results were successfully retrieved.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if the job has not finished yet, was
 * cancelled, or does not exist.
 * @return @ref QDMI_ERROR_FATAL if an error occurred during the retrieval.
 */
int QDMI_control_get_data(QDMI_Device dev, QDMI_Job job, QDMI_Job_Result result,
                          size_t size, void *data, size_t *size_ret);

/**
 * @brief Free a job.
 * @details Free the resources associated with a job.
 * @param[in] dev The device to free the job on.
 * @param[in] job The job to free.
 */
void QDMI_control_free_job(QDMI_Device dev, QDMI_Job job);

#ifdef __cplusplus
} // extern "C"
#endif
