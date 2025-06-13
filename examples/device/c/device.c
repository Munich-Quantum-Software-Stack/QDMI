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
 * @brief A simple example of a device implementation in C.
 * @details This file can be used as a template for implementing a device in C.
 */

#include "c_qdmi/device.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum C_QDMI_DEVICE_SESSION_STATUS { ALLOCATED, INITIALIZED };

typedef struct C_QDMI_Device_Session_impl_d {
  char *token;
  enum C_QDMI_DEVICE_SESSION_STATUS status;
} C_QDMI_Device_Session_impl_t;

typedef struct C_QDMI_Device_Job_impl_d {
  C_QDMI_Device_Session session;
  int id;
  size_t timeout;
  QDMI_Program_Format format;
  void *program;
  QDMI_Job_Status status;
  size_t num_shots;
  char *results;
  size_t results_length; // includes null terminator
  double *state_vec;
  size_t state_vec_length;
} C_QDMI_Device_Job_impl_t;

typedef struct C_QDMI_Site_impl_d {
  size_t id;
} C_QDMI_Site_impl_t;

typedef struct C_QDMI_Operation_impl_d {
  char *name;
} C_QDMI_Operation_impl_t;

typedef struct C_QDMI_EnvironmentSensor_impl_d {
  char *id;
  char *unit;
  int sampling_rate; // in seconds
} C_QDMI_EnvironmentSensor_impl_t;

typedef struct C_QDMI_Device_EnvironmentSensor_Query_impl_d {
  time_t start_time;
  time_t end_time;
  C_QDMI_EnvironmentSensor environment_sensor;
  time_t *result_timestamps;
  float *result_values;
  size_t result_length;
  QDMI_EnvironmentSensor_Query_Status status;
} C_QDMI_Device_EnvironmentSensor_Query_impl_t;

/**
 * @brief Static function to maintain the device status.
 * @return a pointer to the device status.
 * @note This function is considered private and should not be used outside of
 * this file. Hence, it is not part of any header file.
 */
static QDMI_Device_Status *C_QDMI_get_device_status(void) {
  static QDMI_Device_Status device_status = QDMI_DEVICE_STATUS_OFFLINE;
  return &device_status;
}

/**
 * @brief Local function to set the device status.
 * @param status the new device status.
 * @note This function is considered private and should not be used outside of
 * this file. Hence, it is not part of any header file.
 */
void C_QDMI_set_device_status(QDMI_Device_Status status) {
  *C_QDMI_get_device_status() = status;
}

/**
 * @brief Local function to read the device status.
 * @return the current device status.
 * @note This function is considered private and should not be used outside of
 * this file. Hence, it is not part of any header file.
 */
QDMI_Device_Status C_QDMI_read_device_status(void) {
  return *C_QDMI_get_device_status();
}
const C_QDMI_EnvironmentSensor C_DEVICE_ENVIRONMENTSENSORS[] = {
    &(C_QDMI_EnvironmentSensor_impl_t){"t4k", "K", 60},
};

const C_QDMI_Site C_DEVICE_SITES[] = {
    &(C_QDMI_Site_impl_t){0}, &(C_QDMI_Site_impl_t){1},
    &(C_QDMI_Site_impl_t){2}, &(C_QDMI_Site_impl_t){3},
    &(C_QDMI_Site_impl_t){4}};

const C_QDMI_Operation C_DEVICE_OPERATIONS[] = {
    &(C_QDMI_Operation_impl_t){"rx"}, &(C_QDMI_Operation_impl_t){"ry"},
    &(C_QDMI_Operation_impl_t){"rz"}, &(C_QDMI_Operation_impl_t){"cz"}};

#define ADD_SINGLE_VALUE_PROPERTY(prop_name, prop_type, prop_value, prop,      \
                                  size, value, size_ret)                       \
  {                                                                            \
    if ((prop) == (prop_name)) {                                               \
      if ((value) != NULL) {                                                   \
        if ((size) < sizeof(prop_type)) {                                      \
          return QDMI_ERROR_INVALIDARGUMENT;                                   \
        }                                                                      \
        *(prop_type *)(value) = prop_value;                                    \
      }                                                                        \
      if ((size_ret) != NULL) {                                                \
        *(size_ret) = sizeof(prop_type);                                       \
      }                                                                        \
      return QDMI_SUCCESS;                                                     \
    }                                                                          \
  } /// [DOXYGEN MACRO END]

#define ADD_STRING_PROPERTY(prop_name, prop_value, prop, size, value,          \
                            size_ret)                                          \
  {                                                                            \
    if ((prop) == (prop_name)) {                                               \
      if ((value) != NULL) {                                                   \
        if ((size) < strlen(prop_value) + 1) {                                 \
          return QDMI_ERROR_INVALIDARGUMENT;                                   \
        }                                                                      \
        strncpy((char *)(value), prop_value, (size) - 1);                      \
        ((char *)(value))[(size) - 1] = '\0';                                  \
      }                                                                        \
      if ((size_ret) != NULL) {                                                \
        *(size_ret) = strlen(prop_value) + 1;                                  \
      }                                                                        \
      return QDMI_SUCCESS;                                                     \
    }                                                                          \
  } /// [DOXYGEN MACRO END]

#define ADD_LIST_PROPERTY(prop_name, prop_type, prop_values, prop_length,      \
                          prop, size, value, size_ret)                         \
  {                                                                            \
    if ((prop) == (prop_name)) {                                               \
      if ((value) != NULL) {                                                   \
        if ((size) < (prop_length) * sizeof(prop_type)) {                      \
          return QDMI_ERROR_INVALIDARGUMENT;                                   \
        }                                                                      \
        memcpy((void *)(value), (const void *)(prop_values),                   \
               (prop_length) * sizeof(prop_type));                             \
      }                                                                        \
      if ((size_ret) != NULL) {                                                \
        *(size_ret) = (prop_length) * sizeof(prop_type);                       \
      }                                                                        \
      return QDMI_SUCCESS;                                                     \
    }                                                                          \
  } /// [DOXYGEN MACRO END]

int C_QDMI_device_initialize(void) {
  C_QDMI_set_device_status(QDMI_DEVICE_STATUS_IDLE);
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_finalize(void) {
  C_QDMI_set_device_status(QDMI_DEVICE_STATUS_OFFLINE);
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_session_alloc(C_QDMI_Device_Session *session) {
  if (session == NULL) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  *session =
      (C_QDMI_Device_Session)malloc(sizeof(C_QDMI_Device_Session_impl_t));
  (*session)->token = NULL;
  (*session)->status = ALLOCATED;
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_session_init(C_QDMI_Device_Session session) {
  if (session == NULL) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  switch (C_QDMI_read_device_status()) {
  case QDMI_DEVICE_STATUS_ERROR:
  case QDMI_DEVICE_STATUS_OFFLINE:
  case QDMI_DEVICE_STATUS_MAINTENANCE:
    return QDMI_ERROR_FATAL;
  default:
    break;
  }
  if (session->token == NULL) {
    return QDMI_ERROR_PERMISSIONDENIED;
  }
  session->status = INITIALIZED;
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

void C_QDMI_device_session_free(C_QDMI_Device_Session session) {
  if (session != NULL) {
    free(session->token);
    free(session);
  }
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_session_set_parameter(
    C_QDMI_Device_Session session, const QDMI_Device_Session_Parameter param,
    const size_t size, const void *value) {
  if (session == NULL || (value != NULL && size == 0) ||
      (param >= QDMI_DEVICE_SESSION_PARAMETER_MAX &&
       param != QDMI_DEVICE_SESSION_PARAMETER_CUSTOM1 &&
       param != QDMI_DEVICE_SESSION_PARAMETER_CUSTOM2 &&
       param != QDMI_DEVICE_SESSION_PARAMETER_CUSTOM3 &&
       param != QDMI_DEVICE_SESSION_PARAMETER_CUSTOM4 &&
       param != QDMI_DEVICE_SESSION_PARAMETER_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (session->status != ALLOCATED) {
    return QDMI_ERROR_BADSTATE;
  }
  if (param != QDMI_DEVICE_SESSION_PARAMETER_TOKEN) {
    return QDMI_ERROR_NOTSUPPORTED;
  }

  if (value != NULL) {
    session->token = (char *)malloc(size);
    strncpy(session->token, (const char *)value, size);
  }
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_session_create_device_job(C_QDMI_Device_Session session,
                                            C_QDMI_Device_Job *job) {
  if (session == NULL || job == NULL) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (session->status != INITIALIZED) {
    return QDMI_ERROR_BADSTATE;
  }

  *job = (C_QDMI_Device_Job)malloc(sizeof(C_QDMI_Device_Job_impl_t));
  (*job)->session = session;
  // set job id to random number for demonstration purposes
  (*job)->id = rand();
  (*job)->status = QDMI_JOB_STATUS_CREATED;
  (*job)->num_shots = 0;
  (*job)->results = NULL;
  (*job)->state_vec = NULL;
  (*job)->program = NULL;
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

void C_QDMI_device_job_free(C_QDMI_Device_Job job) {
  // this method should free all resources associated with the job
  free(job->results);
  job->results = NULL;
  free(job->state_vec);
  job->state_vec = NULL;
  free(job->program);
  job->program = NULL;
  free(job);
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_job_set_parameter(C_QDMI_Device_Job job,
                                    const QDMI_Device_Job_Parameter param,
                                    const size_t size, const void *value) {
  if (job == NULL || (value != NULL && size == 0) ||
      (param >= QDMI_DEVICE_JOB_PARAMETER_MAX &&
       param != QDMI_DEVICE_JOB_PARAMETER_CUSTOM1 &&
       param != QDMI_DEVICE_JOB_PARAMETER_CUSTOM2 &&
       param != QDMI_DEVICE_JOB_PARAMETER_CUSTOM3 &&
       param != QDMI_DEVICE_JOB_PARAMETER_CUSTOM4 &&
       param != QDMI_DEVICE_JOB_PARAMETER_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (job->status != QDMI_JOB_STATUS_CREATED) {
    return QDMI_ERROR_BADSTATE;
  }
  switch (param) {
  case QDMI_DEVICE_JOB_PARAMETER_PROGRAMFORMAT:
    if (value != NULL) {
      QDMI_Program_Format format = *(const QDMI_Program_Format *)value;
      if (format >= QDMI_PROGRAM_FORMAT_MAX &&
          format != QDMI_PROGRAM_FORMAT_CUSTOM1 &&
          format != QDMI_PROGRAM_FORMAT_CUSTOM2 &&
          format != QDMI_PROGRAM_FORMAT_CUSTOM3 &&
          format != QDMI_PROGRAM_FORMAT_CUSTOM4 &&
          format != QDMI_PROGRAM_FORMAT_CUSTOM5) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      if (format != QDMI_PROGRAM_FORMAT_QASM2 &&
          format != QDMI_PROGRAM_FORMAT_QIRBASESTRING &&
          format != QDMI_PROGRAM_FORMAT_QIRBASEMODULE &&
          format != QDMI_PROGRAM_FORMAT_CALIBRATION) {
        return QDMI_ERROR_NOTSUPPORTED;
      }
      job->format = format;
    }
    return QDMI_SUCCESS;
  case QDMI_DEVICE_JOB_PARAMETER_PROGRAM:
    if (value != NULL) {
      job->program = malloc(size);
      memcpy(job->program, value, size);
    }
    return QDMI_SUCCESS;
  case QDMI_DEVICE_JOB_PARAMETER_SHOTSNUM:
    if (value != NULL) {
      job->num_shots = *(const size_t *)value;
    }
    return QDMI_SUCCESS;
  default:
    return QDMI_ERROR_NOTSUPPORTED;
  }
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_job_query_property(C_QDMI_Device_Job job,
                                     const QDMI_Device_Job_Property prop,
                                     const size_t size, void *value,
                                     size_t *size_ret) {
  if (job == NULL || (value != NULL && size == 0) ||
      (prop >= QDMI_DEVICE_JOB_PROPERTY_MAX &&
       prop != QDMI_DEVICE_JOB_PROPERTY_CUSTOM1 &&
       prop != QDMI_DEVICE_JOB_PROPERTY_CUSTOM2 &&
       prop != QDMI_DEVICE_JOB_PROPERTY_CUSTOM3 &&
       prop != QDMI_DEVICE_JOB_PROPERTY_CUSTOM4 &&
       prop != QDMI_DEVICE_JOB_PROPERTY_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  char str[20];
  sprintf(str, "%d", job->id);
  ADD_STRING_PROPERTY(QDMI_DEVICE_JOB_PROPERTY_ID, str, prop, size, value,
                      size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_DEVICE_JOB_PROPERTY_PROGRAMFORMAT,
                            QDMI_Program_Format, job->format, prop, size, value,
                            size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_DEVICE_JOB_PROPERTY_SHOTSNUM, size_t,
                            job->num_shots, prop, size, value, size_ret)
  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_job_submit(C_QDMI_Device_Job job) {
  if (job == NULL || job->status != QDMI_JOB_STATUS_CREATED) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }

  // Calibration jobs complete immediately
  if (job->format == QDMI_PROGRAM_FORMAT_CALIBRATION) {
    job->status = QDMI_JOB_STATUS_DONE;
    return QDMI_SUCCESS;
  }

  C_QDMI_set_device_status(QDMI_DEVICE_STATUS_BUSY);
  job->status = QDMI_JOB_STATUS_SUBMITTED;
  // here, the actual submission of the problem to the device would happen
  // ...
  // set job status to running for demonstration purposes
  job->status = QDMI_JOB_STATUS_RUNNING;
  // generate random result data
  size_t num_qubits = 0;
  C_QDMI_device_session_query_device_property(
      job->session, QDMI_DEVICE_PROPERTY_QUBITSNUM, sizeof(size_t), &num_qubits,
      NULL);
  job->results_length = job->num_shots * (num_qubits + 1);
  job->results = (char *)malloc(job->results_length);
  for (size_t i = 0; i < job->num_shots; ++i) {
    // generate random bitstring
    for (size_t j = 0; j < num_qubits; ++j) {
      *(job->results + (i * (num_qubits + 1) + j)) = (rand() % 2) ? '1' : '0';
    }
    if (i < job->num_shots - 1) {
      *(job->results + ((i + 1) * (num_qubits + 1) - 1)) = ',';
    }
  }
  *(job->results + (job->results_length - 1)) = '\0';
  // Generate random complex numbers and calculate the norm
  job->state_vec_length = 2ULL << num_qubits;
  job->state_vec = (double *)malloc(job->state_vec_length * sizeof(double));
  double norm = 0.0;
  for (size_t i = 0; i < job->state_vec_length / 2; ++i) {
    const double real_part = (((double)rand() / RAND_MAX) * 2.0) - 1.0;
    const double imag_part = (((double)rand() / RAND_MAX) * 2.0) - 1.0;
    norm += real_part * real_part + imag_part * imag_part;
    job->state_vec[2UL * i] = real_part;
    job->state_vec[(2UL * i) + 1] = imag_part;
  }
  // Normalize the vector
  norm = sqrt(norm);
  for (size_t i = 0; i < job->state_vec_length; ++i) {
    // NOLINTNEXTLINE(*-core.UndefinedBinaryOperatorResult)
    job->state_vec[i] = job->state_vec[i] / norm;
  }
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_job_cancel(C_QDMI_Device_Job job) {
  // cannot cancel a job that is already done
  if (job == NULL || job->status == QDMI_JOB_STATUS_DONE) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }

  job->status = QDMI_JOB_STATUS_CANCELED;
  C_QDMI_set_device_status(QDMI_DEVICE_STATUS_IDLE);
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_job_check(C_QDMI_Device_Job job, QDMI_Job_Status *status) {
  if (job == NULL || status == NULL) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  // randomly decide whether job is done or not
  if (job->status == QDMI_JOB_STATUS_RUNNING && rand() % 2 == 0) {
    C_QDMI_set_device_status(QDMI_DEVICE_STATUS_IDLE);
    job->status = QDMI_JOB_STATUS_DONE;
  }
  *status = job->status;
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_job_wait(C_QDMI_Device_Job job, const size_t timeout) {
  if (job == NULL) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  // in a real implementation, this would wait for the job to finish
  job->status = QDMI_JOB_STATUS_DONE;
  job->timeout = timeout;
  C_QDMI_set_device_status(QDMI_DEVICE_STATUS_IDLE);
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

// Comparison function for qsort
int C_QDMI_compare_results(const void *a, const void *b) {
  return strcmp(*(char **)a, *(char **)b);
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_job_get_results_shots(C_QDMI_Device_Job job,
                                        const size_t size, void *data,
                                        size_t *size_ret) {
  const size_t req_size = job->results_length;
  if (data != NULL) {
    if (size < req_size) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    strncpy((char *)data, job->results, req_size);
  }
  if ((size_ret) != NULL) {
    *(size_ret) = req_size;
  }
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_job_get_results_hist(C_QDMI_Device_Job job,
                                       const QDMI_Job_Result result,
                                       const size_t size, void *data,
                                       size_t *size_ret) {
  char *raw_data = malloc(job->results_length);
  strncpy(raw_data, job->results, job->results_length);
  // split the string at the commas
  char **raw_data_split = (char **)malloc(sizeof(char *) * job->num_shots);
  char *token = strtok(raw_data, ",");
  int i = 0;
  while (token != NULL) {
    raw_data_split[i] = token;
    token = strtok(NULL, ",");
    ++i;
  }
  // Sort the array
  qsort((void *)raw_data_split, job->num_shots, sizeof(char *),
        C_QDMI_compare_results);
  // Count unique elements
  const size_t num_qubits = strlen(raw_data_split[0]);

  size_t count = 1; // First element is always unique
  for (size_t j = 1; j < job->num_shots; ++j) {
    if (strncmp(raw_data_split[j], raw_data_split[j - 1], num_qubits) != 0) {
      count++;
    }
  }
  if (result == QDMI_JOB_RESULT_HIST_KEYS) {
    const size_t req_size = count * (num_qubits + 1);
    if (size_ret != NULL) {
      *size_ret = req_size;
    }
    if (data != NULL) {
      if (size < req_size) {
        free((void *)raw_data_split);
        free(raw_data);
        return QDMI_ERROR_INVALIDARGUMENT;
      }

      char *data_ptr = data;
      strncpy(data_ptr, raw_data_split[0], num_qubits);
      data_ptr += num_qubits;
      for (size_t j = 1; j < job->num_shots; ++j) {
        if (strncmp(raw_data_split[j], raw_data_split[j - 1], num_qubits) !=
            0) {
          *data_ptr++ = ',';
          strncpy(data_ptr, raw_data_split[j], num_qubits);
          data_ptr += num_qubits;
        }
      }
      *data_ptr = '\0';
    }
  } else {
    // case QDMI_JOB_RESULT_HIST_VALUES:
    const size_t req_size = count * sizeof(size_t);
    if (size_ret != NULL) {
      *size_ret = req_size;
    }
    if (data != NULL) {
      if (size < req_size) {
        free((void *)raw_data_split);
        free(raw_data);
        return QDMI_ERROR_INVALIDARGUMENT;
      }

      size_t *data_ptr = data;
      size_t n = 1;
      for (size_t j = 1; j < job->num_shots; ++j) {
        if (strcmp(raw_data_split[j], raw_data_split[j - 1]) != 0) {
          *data_ptr++ = n;
          n = 1;
        } else {
          ++n;
        }
      }
      *data_ptr = n;
    }
  }
  free((void *)raw_data_split);
  free(raw_data);
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_job_get_results_statevector(C_QDMI_Device_Job job,
                                              const size_t size, void *data,
                                              size_t *size_ret) {
  const size_t req_size = job->state_vec_length * sizeof(double);
  if (data != NULL) {
    if (size < req_size) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    memcpy(data, job->state_vec, req_size);
  }
  if ((size_ret) != NULL) {
    *(size_ret) = req_size;
  }
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_job_get_results_sparse(C_QDMI_Device_Job job,
                                         const QDMI_Job_Result result,
                                         const size_t size, void *data,
                                         size_t *size_ret) {
  const size_t length = job->state_vec_length / 2;
  const size_t num_qubits = (size_t)log2((double)length);
  const double *vec = job->state_vec;
  // count non-zero elements
  size_t count = 0;
  for (size_t i = 0; i < length; ++i) {
    if (vec[2 * i] != 0.0 || vec[(2 * i) + 1] != 0.0) {
      count++;
    }
  }
  switch (result) {
  case QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS:
  case QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS: {
    const size_t req_size = count * (num_qubits + 1);
    if (data != NULL) {
      if (size < req_size) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      char *data_ptr = data;
      for (size_t i = 0; i < length; ++i) {
        if (vec[2 * i] != 0.0 || vec[(2 * i) + 1] != 0.0) {
          for (size_t j = 0; j < num_qubits; j++) {
            *data_ptr++ = (i & (1ULL << (num_qubits - j - 1))) ? '1' : '0';
          }
          *data_ptr++ = ',';
        }
      }
      *(data_ptr - 1) = '\0'; // replace the last comma with a null terminator
    }
    if ((size_ret) != NULL) {
      *(size_ret) = req_size;
    }
    return QDMI_SUCCESS;
  }
  case QDMI_JOB_RESULT_STATEVECTOR_SPARSE_VALUES: {
    const size_t req_size = count * 2 * sizeof(double);
    if (data != NULL) {
      if (size < req_size) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      double *data_ptr = data;
      for (size_t i = 0; i < length; ++i) {
        if (vec[2 * i] != 0.0 || vec[(2 * i) + 1] != 0.0) {
          *data_ptr++ = vec[2 * i];
          *data_ptr++ = vec[(2 * i) + 1];
        }
      }
    }
    if ((size_ret) != NULL) {
      *(size_ret) = req_size;
    }
    return QDMI_SUCCESS;
  }
  default: {
    // case QDMI_JOB_RESULT_PROBABILITIES_SPARSE_VALUES:
    const size_t req_size = count * sizeof(double);
    if (data != NULL) {
      if (size < req_size) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      double *data_ptr = data;
      for (size_t i = 0; i < length; ++i) {
        if (vec[2 * i] != 0.0 || vec[(2 * i) + 1] != 0.0) {
          *data_ptr++ =
              (vec[2 * i] * vec[2 * i]) + (vec[(2 * i) + 1] * vec[(2 * i) + 1]);
        }
      }
    }
    if ((size_ret) != NULL) {
      *(size_ret) = req_size;
    }
  }
  }
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_job_get_results_probability(C_QDMI_Device_Job job,
                                              const size_t size, void *data,
                                              size_t *size_ret) {
  const size_t length = job->state_vec_length / 2;
  const size_t req_size = length * sizeof(double);
  if (data != NULL) {
    if (size < req_size) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    double *data_ptr = data;
    for (size_t i = 0; i < length; ++i) {
      // Calculate the probability of the state
      *data_ptr++ = (job->state_vec[2 * i] * job->state_vec[2 * i]) +
                    (job->state_vec[(2 * i) + 1] * job->state_vec[(2 * i) + 1]);
    }
  }
  if ((size_ret) != NULL) {
    *(size_ret) = req_size;
  }
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_job_get_results(C_QDMI_Device_Job job,
                                  const QDMI_Job_Result result,
                                  const size_t size, void *data,
                                  size_t *size_ret) {
  if (job == NULL || job->status != QDMI_JOB_STATUS_DONE ||
      (data != NULL && size == 0) ||
      (result >= QDMI_JOB_RESULT_MAX && result != QDMI_JOB_RESULT_CUSTOM1 &&
       result != QDMI_JOB_RESULT_CUSTOM2 && result != QDMI_JOB_RESULT_CUSTOM3 &&
       result != QDMI_JOB_RESULT_CUSTOM4 &&
       result != QDMI_JOB_RESULT_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  switch (result) {
  case QDMI_JOB_RESULT_SHOTS:
    return C_QDMI_device_job_get_results_shots(job, size, data, size_ret);
  case QDMI_JOB_RESULT_HIST_KEYS:
  case QDMI_JOB_RESULT_HIST_VALUES:
    return C_QDMI_device_job_get_results_hist(job, result, size, data,
                                              size_ret);
  case QDMI_JOB_RESULT_STATEVECTOR_DENSE:
    return C_QDMI_device_job_get_results_statevector(job, size, data, size_ret);
  case QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS:
  case QDMI_JOB_RESULT_STATEVECTOR_SPARSE_VALUES:
  case QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS:
  case QDMI_JOB_RESULT_PROBABILITIES_SPARSE_VALUES:
    return C_QDMI_device_job_get_results_sparse(job, result, size, data,
                                                size_ret);
  case QDMI_JOB_RESULT_PROBABILITIES_DENSE:
    return C_QDMI_device_job_get_results_probability(job, size, data, size_ret);
  default:
    return QDMI_ERROR_NOTSUPPORTED;
  }
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_session_query_device_property(C_QDMI_Device_Session session,
                                                const QDMI_Device_Property prop,
                                                const size_t size, void *value,
                                                size_t *size_ret) {
  if (session == NULL || (value != NULL && size == 0) ||
      (prop >= QDMI_DEVICE_PROPERTY_MAX &&
       prop != QDMI_DEVICE_PROPERTY_CUSTOM1 &&
       prop != QDMI_DEVICE_PROPERTY_CUSTOM2 &&
       prop != QDMI_DEVICE_PROPERTY_CUSTOM3 &&
       prop != QDMI_DEVICE_PROPERTY_CUSTOM4 &&
       prop != QDMI_DEVICE_PROPERTY_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (session->status != INITIALIZED) {
    return QDMI_ERROR_BADSTATE;
  }
  ADD_STRING_PROPERTY(QDMI_DEVICE_PROPERTY_NAME, "C Device with 5 qubits", prop,
                      size, value, size_ret)
  ADD_STRING_PROPERTY(QDMI_DEVICE_PROPERTY_VERSION, "0.1.0", prop, size, value,
                      size_ret)
  ADD_STRING_PROPERTY(QDMI_DEVICE_PROPERTY_LIBRARYVERSION, "1.1.0", prop, size,
                      value, size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_DEVICE_PROPERTY_STATUS, QDMI_Device_Status,
                            C_QDMI_read_device_status(), prop, size, value,
                            size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_DEVICE_PROPERTY_QUBITSNUM, size_t, 5, prop,
                            size, value, size_ret)
  ADD_LIST_PROPERTY(QDMI_DEVICE_PROPERTY_SITES, C_QDMI_Site, C_DEVICE_SITES, 5,
                    prop, size, value, size_ret)
  ADD_LIST_PROPERTY(QDMI_DEVICE_PROPERTY_OPERATIONS, C_QDMI_Operation,
                    C_DEVICE_OPERATIONS, 4, prop, size, value, size_ret)
  ADD_LIST_PROPERTY(
      QDMI_DEVICE_PROPERTY_COUPLINGMAP, C_QDMI_Site,
      ((C_QDMI_Site[]){C_DEVICE_SITES[0], C_DEVICE_SITES[1], C_DEVICE_SITES[1],
                       C_DEVICE_SITES[0], C_DEVICE_SITES[1], C_DEVICE_SITES[2],
                       C_DEVICE_SITES[2], C_DEVICE_SITES[1], C_DEVICE_SITES[2],
                       C_DEVICE_SITES[3], C_DEVICE_SITES[3], C_DEVICE_SITES[2],
                       C_DEVICE_SITES[3], C_DEVICE_SITES[4], C_DEVICE_SITES[4],
                       C_DEVICE_SITES[3], C_DEVICE_SITES[4], C_DEVICE_SITES[0],
                       C_DEVICE_SITES[0], C_DEVICE_SITES[4]}),
      20, prop, size, value, size_ret)

  // The example device never requires calibration
  ADD_SINGLE_VALUE_PROPERTY(QDMI_DEVICE_PROPERTY_NEEDSCALIBRATION, size_t, 0,
                            prop, size, value, size_ret)
  ADD_LIST_PROPERTY(QDMI_DEVICE_PROPERTY_ENVIRONMENTSENSORS,
                    C_QDMI_EnvironmentSensor, C_DEVICE_ENVIRONMENTSENSORS, 1, prop, size,
                    value, size_ret)

  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_session_query_site_property(C_QDMI_Device_Session session,
                                              C_QDMI_Site site,
                                              const QDMI_Site_Property prop,
                                              const size_t size, void *value,
                                              size_t *size_ret) {
  if (session == NULL || site == NULL || (value != NULL && size == 0) ||
      (prop >= QDMI_SITE_PROPERTY_MAX && prop != QDMI_SITE_PROPERTY_CUSTOM1 &&
       prop != QDMI_SITE_PROPERTY_CUSTOM2 &&
       prop != QDMI_SITE_PROPERTY_CUSTOM3 &&
       prop != QDMI_SITE_PROPERTY_CUSTOM4 &&
       prop != QDMI_SITE_PROPERTY_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  ADD_SINGLE_VALUE_PROPERTY(QDMI_SITE_PROPERTY_INDEX, uint64_t, site->id, prop,
                            size, value, size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_SITE_PROPERTY_T1, double, 1000.0, prop, size,
                            value, size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_SITE_PROPERTY_T2, double, 100000.0, prop, size,
                            value, size_ret)
  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_session_query_operation_property(
    C_QDMI_Device_Session session, C_QDMI_Operation operation,
    const size_t num_sites, const C_QDMI_Site *sites, const size_t num_params,
    const double *params, const QDMI_Operation_Property prop, const size_t size,
    void *value, size_t *size_ret) {
  if (session == NULL || operation == NULL ||
      (sites != NULL && num_sites == 0) ||
      (params != NULL && num_params == 0) || (value != NULL && size == 0) ||
      (prop >= QDMI_OPERATION_PROPERTY_MAX &&
       prop != QDMI_OPERATION_PROPERTY_CUSTOM1 &&
       prop != QDMI_OPERATION_PROPERTY_CUSTOM2 &&
       prop != QDMI_OPERATION_PROPERTY_CUSTOM3 &&
       prop != QDMI_OPERATION_PROPERTY_CUSTOM4 &&
       prop != QDMI_OPERATION_PROPERTY_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (operation == C_DEVICE_OPERATIONS[0]) {
    // Two-qubit operation properties
    ADD_STRING_PROPERTY(QDMI_OPERATION_PROPERTY_NAME, "cx", prop, size, value,
                        size_ret)
    if (sites != NULL && num_sites != 2) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_PARAMETERSNUM, size_t, 0,
                              prop, size, value, size_ret)
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_DURATION, double, 0.01,
                              prop, size, value, size_ret)
    if (sites == NULL) {
      ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_QUBITSNUM, size_t, 2,
                                prop, size, value, size_ret)
      return QDMI_ERROR_NOTSUPPORTED;
    }
    if (sites[0] == sites[1]) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    if ((sites[0] == C_DEVICE_SITES[0] && sites[1] == C_DEVICE_SITES[1]) ||
        (sites[0] == C_DEVICE_SITES[1] && sites[1] == C_DEVICE_SITES[0])) {
      ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_FIDELITY, double, 0.99,
                                prop, size, value, size_ret)
    }
    if ((sites[0] == C_DEVICE_SITES[1] && sites[1] == C_DEVICE_SITES[2]) ||
        (sites[0] == C_DEVICE_SITES[2] && sites[1] == C_DEVICE_SITES[1])) {
      ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_FIDELITY, double, 0.98,
                                prop, size, value, size_ret)
    }
    if ((sites[0] == C_DEVICE_SITES[2] && sites[1] == C_DEVICE_SITES[3]) ||
        (sites[0] == C_DEVICE_SITES[3] && sites[1] == C_DEVICE_SITES[2])) {
      ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_FIDELITY, double, 0.97,
                                prop, size, value, size_ret)
    }
    if ((sites[0] == C_DEVICE_SITES[3] && sites[1] == C_DEVICE_SITES[4]) ||
        (sites[0] == C_DEVICE_SITES[4] && sites[1] == C_DEVICE_SITES[3])) {
      ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_FIDELITY, double, 0.96,
                                prop, size, value, size_ret)
    }
    if ((sites[0] == C_DEVICE_SITES[4] && sites[1] == C_DEVICE_SITES[0]) ||
        (sites[0] == C_DEVICE_SITES[0] && sites[1] == C_DEVICE_SITES[4])) {
      ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_FIDELITY, double, 0.95,
                                prop, size, value, size_ret)
    }
    if (prop == QDMI_OPERATION_PROPERTY_FIDELITY) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
  } else {
    if (operation == C_DEVICE_OPERATIONS[1]) {
      ADD_STRING_PROPERTY(QDMI_OPERATION_PROPERTY_NAME, "rx", prop, size, value,
                          size_ret)
    } else if (operation == C_DEVICE_OPERATIONS[2]) {
      ADD_STRING_PROPERTY(QDMI_OPERATION_PROPERTY_NAME, "ry", prop, size, value,
                          size_ret)
    } else if (operation == C_DEVICE_OPERATIONS[3]) {
      ADD_STRING_PROPERTY(QDMI_OPERATION_PROPERTY_NAME, "rz", prop, size, value,
                          size_ret)
    } else {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    // Common properties for all single-qubit operations
    if ((sites != NULL && num_sites != 1) ||
        (params != NULL && num_params != 1)) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_DURATION, double, 0.01,
                              prop, size, value, size_ret)
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_QUBITSNUM, size_t, 1,
                              prop, size, value, size_ret)
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_PARAMETERSNUM, size_t, 1,
                              prop, size, value, size_ret)
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_FIDELITY, double, 0.999,
                              prop, size, value, size_ret)
  }
  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_device_session_query_environmentsensor_property(
    C_QDMI_Device_Session session, C_QDMI_EnvironmentSensor environment_sensor,
    QDMI_EnvironmentSensor_Property prop, size_t size, void *value,
    size_t *size_ret) {
  if (session == NULL || environment_sensor == NULL || (value != NULL && size == 0) ||
      (prop >= QDMI_ENVIRONMENTSENSOR_PROPERTY_MAX &&
       prop != QDMI_ENVIRONMENTSENSOR_PROPERTY_CUSTOM1 &&
       prop != QDMI_ENVIRONMENTSENSOR_PROPERTY_CUSTOM2 &&
       prop != QDMI_ENVIRONMENTSENSOR_PROPERTY_CUSTOM3 &&
       prop != QDMI_ENVIRONMENTSENSOR_PROPERTY_CUSTOM4 &&
       prop != QDMI_ENVIRONMENTSENSOR_PROPERTY_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }

  ADD_STRING_PROPERTY(QDMI_ENVIRONMENTSENSOR_PROPERTY_ID, environment_sensor->id, prop, size,
                      value, size_ret)
  ADD_STRING_PROPERTY(QDMI_ENVIRONMENTSENSOR_PROPERTY_UNIT, environment_sensor->unit, prop,
                      size, value, size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_ENVIRONMENTSENSOR_PROPERTY_SAMPLINGRATE, int,
                            environment_sensor->sampling_rate, prop, size, value,
                            size_ret)
  return QDMI_ERROR_NOTSUPPORTED;
}

int C_QDMI_device_session_create_environmentsensor_query(
    C_QDMI_Device_Session session, C_QDMI_Device_EnvironmentSensor_Query *query) {

  if (session == NULL || query == NULL) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (session->status != INITIALIZED) {
    return QDMI_ERROR_BADSTATE;
  }
  *query = malloc(sizeof(C_QDMI_Device_EnvironmentSensor_Query_impl_t));
  (*query)->environment_sensor = malloc(sizeof(C_QDMI_EnvironmentSensor));
  (*query)->start_time = time(NULL);
  (*query)->end_time = time(NULL);

  return QDMI_SUCCESS;
}

int C_QDMI_device_environmentsensor_query_set_parameter(
    C_QDMI_Device_EnvironmentSensor_Query query,
    QDMI_Device_EnvironmentSensor_Query_Parameter param, size_t size,
    const void *value) {

  if (query == NULL || (value != NULL && size == 0) ||
      (param >= QDMI_DEVICE_ENVIRONMENTSENSOR_QUERY_PARAMETER_MAX &&
       param != QDMI_DEVICE_ENVIRONMENTSENSOR_QUERY_PARAMETER_CUSTOM1 &&
       param != QDMI_DEVICE_ENVIRONMENTSENSOR_QUERY_PARAMETER_CUSTOM2 &&
       param != QDMI_DEVICE_ENVIRONMENTSENSOR_QUERY_PARAMETER_CUSTOM3 &&
       param != QDMI_DEVICE_ENVIRONMENTSENSOR_QUERY_PARAMETER_CUSTOM4 &&
       param != QDMI_DEVICE_ENVIRONMENTSENSOR_QUERY_PARAMETER_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  switch (param) {

  case QDMI_DEVICE_ENVIRONMENTSENSOR_QUERY_PARAMETER_START_TIME:
    query->start_time = *(time_t *)(value);
    return QDMI_SUCCESS;
  case QDMI_DEVICE_ENVIRONMENTSENSOR_QUERY_PARAMETER_END_TIME:
    query->end_time = *(time_t *)(value);
    return QDMI_SUCCESS;
  case QDMI_DEVICE_ENVIRONMENTSENSOR_QUERY_PARAMETER_ENVIRONMENTSENSOR:
    query->environment_sensor = *(C_QDMI_EnvironmentSensor *)(value);
    return QDMI_SUCCESS;
  default:
    return QDMI_ERROR_NOTSUPPORTED;
  }
}

int C_QDMI_device_environmentsensor_query_submit(
    C_QDMI_Device_EnvironmentSensor_Query query) {

  if (query == NULL || query->environment_sensor == NULL) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }

  long time_difference = query->end_time - query->start_time;
  if (time_difference <= 0) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  // here, the actual submission.

  // for demonstration purposes
  int sampling_rate = query->environment_sensor->sampling_rate;

  size_t result_length = (size_t)(time_difference / sampling_rate);

  query->result_timestamps = malloc(sizeof(time_t) * result_length);
  query->result_values = malloc(sizeof(float) * result_length);
  query->result_length = result_length;

  for (size_t i = 0; i < result_length; i++) {
    query->result_timestamps[i] = query->start_time + sampling_rate * (long)i;
    query->result_values[i] = ((float)rand() / (float)(RAND_MAX)) * 100;
  }

  return QDMI_SUCCESS;
}

int C_QDMI_device_environmentsensor_query_get_results(
    C_QDMI_Device_EnvironmentSensor_Query query, QDMI_EnvironmentSensor_Query_Result result,
    size_t size, void *data, size_t *size_ret) {

  if (query == NULL || (data != NULL && size == 0) ||
      (result >= QDMI_ENVIRONMENTSENSOR_QUERY_RESULT_MAX &&
       result != QDMI_ENVIRONMENTSENSOR_QUERY_RESULT_CUSTOM1 &&
       result != QDMI_ENVIRONMENTSENSOR_QUERY_RESULT_CUSTOM2 &&
       result != QDMI_ENVIRONMENTSENSOR_QUERY_RESULT_CUSTOM3 &&
       result != QDMI_ENVIRONMENTSENSOR_QUERY_RESULT_CUSTOM4 &&
       result != QDMI_ENVIRONMENTSENSOR_QUERY_RESULT_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  size_t req_size = query->result_length;
  switch (result) {
  case QDMI_ENVIRONMENTSENSOR_QUERY_RESULT_TIMESTAMPS:
    req_size *= sizeof(time_t);
    if (data != NULL) {
      if (size < req_size) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      memcpy(data, query->result_timestamps, req_size);
    }

    if ((size_ret) != NULL) {
      *(size_ret) = req_size;
    }
    return QDMI_SUCCESS;
  case QDMI_ENVIRONMENTSENSOR_QUERY_RESULT_VALUES:

    req_size *= sizeof(float);
    if (data != NULL) {
      if (size < req_size) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      memcpy(data, (query->result_values), req_size);
    }
    if ((size_ret) != NULL) {
      *(size_ret) = req_size;
    }
    return QDMI_SUCCESS;

  default:
    return QDMI_ERROR_NOTSUPPORTED;
  }

  return QDMI_SUCCESS;
}

int C_QDMI_device_environmentsensor_query_check_status(
    C_QDMI_Device_EnvironmentSensor_Query query,
    QDMI_EnvironmentSensor_Query_Status *status) {
  if (query == NULL || status == NULL) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }

  // randomly decide whether job is done or not
  if (query->status == QDMI_ENVIRONMENTSENSOR_QUERY_STATUS_RUNNING &&
      rand() % 2 == 0) {
    query->status = QDMI_ENVIRONMENTSENSOR_QUERY_STATUS_DONE;
  }

  *status = query->status;
  return QDMI_SUCCESS;
}

int C_QDMI_device_environmentsensor_query_wait(
    C_QDMI_Device_EnvironmentSensor_Query query) {

  if (query == NULL) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }

  query->status = QDMI_ENVIRONMENTSENSOR_QUERY_STATUS_DONE;
  return QDMI_SUCCESS;
}

int C_QDMI_device_environmentsensor_query_cancel(
    C_QDMI_Device_EnvironmentSensor_Query query) {

  if (query == NULL || query->status == QDMI_ENVIRONMENTSENSOR_QUERY_STATUS_DONE) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }

  query->status = QDMI_ENVIRONMENTSENSOR_QUERY_STATUS_CANCELED;

  return QDMI_SUCCESS;
}

void C_QDMI_device_environmentsensor_query_free(
    C_QDMI_Device_EnvironmentSensor_Query query) {
  free(query->result_timestamps);
  query->result_timestamps = NULL;

  free(query->result_values);
  query->result_values = NULL;

  free(query);
}
