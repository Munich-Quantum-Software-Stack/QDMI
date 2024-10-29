/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

/** @file
 * @brief The control interface implemented by a QDMI device.
 * @see qdmi/client/control.h for the client interface.
 */
#pragma once

#include "qdmi/common/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a job on a device.
 * @details Create a job consisting of a circuit. Other parameters like the
 * number of shots (@ref QDMI_JOB_PARAMETER_SHOTS_NUM) can be set via @ref
 * QDMI_control_submit_job. After setting all parameters, the job must be
 * submitted for execution through @ref QDMI_control_submit_job.
 * @param[in] format is the format of the program, see @ref
 * QDMI_PROGRAM_FORMAT_T for available options. Note that the availability also
 * depends on the device.
 * @param[in] size is the number of bytes of the program.
 * @param[in] prog is the program to run.
 * @param[out] job is a handle to the job created.
 * @return @ref QDMI_SUCCESS if the job was successfully created.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p size is less than or equal to
 * zero or the program @p prog is invalid, e.g. @c NULL.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the device does not support the
 * program format.
 * @return @ref QDMI_ERROR_FATAL if the job creation failed.
 */
int QDMI_control_create_job_dev(QDMI_Program_Format format, int size,
                                const void *prog, QDMI_Job *job);

/**
 * @brief Set a parameter for a job.
 * @details Set a parameter for a job. The parameter can be one of the
 * predefined parameters in @ref QDMI_Job_Parameter. The value of the parameter
 * is passed as a pointer to the value and the size of the value.
 * @param[in] job The job to set the parameter for.
 * @param[in] param is the parameter to set.
 * @param[in] size is the size of the value.
 * @param[in] value is the value to set.
 * @return @ref QDMI_SUCCESS if the parameter was successfully set.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if the job does not exist, is in an
 * invalid state or the parameter is invalid.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the device does not support the
 * parameter.
 * @return @ref QDMI_ERROR_FATAL if the parameter could not be set.
 */
int QDMI_control_set_parameter_dev(QDMI_Job job, QDMI_Job_Parameter param,
                                   int size, const void *value);

/**
 * @brief Submit a job to the device.
 * @details Submit a via @ref QDMI_control_create_job previously created job to
 * the device. The job can be configured via @ref QDMI_control_set_parameter
 * before submission.
 * @note This function can either be blocking until the job is finished or
 * non-blocking and return while the job is running. In the latter case, there
 * are the functions @ref QDMI_control_check_dev and @ref QDMI_control_wait_dev
 * to check the status and wait for the job to finish.
 * @param[in] job The job to submit.
 * @return @ref QDMI_SUCCESS if the job was successfully submitted.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if the job is in an invalid state
 * @return @ref QDMI_ERROR_FATAL if the job submission failed.
 */
int QDMI_control_submit_job_dev(QDMI_Job job);

/**
 * @brief Cancel an already submitted job.
 * @details Remove the job from the queue of waiting jobs. This changes the
 * status of the job to @ref QDMI_JOB_STATUS_CANCELLED.
 * @param[in] job The job to cancel.
 * @return @ref QDMI_SUCCESS if the job was successfully cancelled.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if the job has already the status
 * QDMI_JOB_STATUS_DONE or the job does not exist.
 * @return @ref QDMI_ERROR_FATAL if the job could not be cancelled.
 */
int QDMI_control_cancel_dev(QDMI_Job job);

/**
 * @brief Check the status of a job.
 * @note This function is non-blocking and returns immediately with the job
 * status.
 * @note It is *not* necessary to call this function before calling
 * @ref QDMI_control_get_data_dev.
 * @param[in] job The job to check the status of.
 * @param[out] status The status of the job.
 * @return @ref QDMI_SUCCESS if the job status was successfully checked.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if the job does not exist.
 */
int QDMI_control_check_dev(QDMI_Job job, QDMI_Job_Status *status);

/**
 * @brief Wait for a job to finish.
 * @details This function blocks until the job has either finished or has been
 * cancelled.
 * @param[in] job The job to wait for.
 * @return @ref QDMI_SUCCESS if the job is finished or cancelled.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if the job does not exist.
 * @return @ref QDMI_ERROR_FATAL if the job could not be waited for and this
 * function returns before the job has finished or has been cancelled.
 */
int QDMI_control_wait_dev(QDMI_Job job);

/**
 * @brief Retrieve the results of a job.
 * @details The results of a job can vary
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
int QDMI_control_get_data_dev(QDMI_Job job, QDMI_Job_Result result, int size,
                              void *data, int *size_ret);

/**
 * @brief Free a job.
 * @details Free the resources associated with a job.
 * @param[in] job The job to free.
 */
void QDMI_control_free_job_dev(QDMI_Job job);

/**
 * @brief Initialize a device.
 * @details A device can expect that this function is called once in the
 * beginning and has returned before any other functions are invoked on that
 * device.
 * @return @ref QDMI_SUCCESS if the initialization was successful.
 * @return @ref QDMI_ERROR_FATAL if the initialization failed.
 */
int QDMI_control_initialize_dev(void);

/**
 * @brief Finalize a device.
 * @details A device can expect that this function is called once at the end of
 * a session and no other functions are invoked on that device after that
 * anymore.
 * @return @ref QDMI_SUCCESS if the finalization was successful.
 * @return @ref QDMI_ERROR_FATAL if the finalization failed, this could, e.g.,
 * be due to a job that is still running.
 */
int QDMI_control_finalize_dev(void);

#ifdef __cplusplus
} // extern "C"
#endif
