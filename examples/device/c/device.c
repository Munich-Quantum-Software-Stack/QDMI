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
#include <stdlib.h>
#include <string.h>

typedef struct C_QDMI_Job_impl_d {
  int id;
  QDMI_Job_Status status;
  size_t num_shots;
  char *results;
  size_t results_length; // includes null terminator
  double *state_vec;
  size_t state_vec_length;
} C_QDMI_Job_impl_t;

typedef struct C_QDMI_Site_impl_d {
  size_t id;
} C_QDMI_Site_impl_t;

typedef struct C_QDMI_Operation_impl_d {
  char *name;
} C_QDMI_Operation_impl_t;

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

const C_QDMI_Site DEVICE_SITES[] = {
    &(C_QDMI_Site_impl_t){0}, &(C_QDMI_Site_impl_t){1},
    &(C_QDMI_Site_impl_t){2}, &(C_QDMI_Site_impl_t){3},
    &(C_QDMI_Site_impl_t){4}};

const C_QDMI_Operation DEVICE_OPERATIONS[] = {
    &(C_QDMI_Operation_impl_t){"rx"}, &(C_QDMI_Operation_impl_t){"ry"},
    &(C_QDMI_Operation_impl_t){"rz"}, &(C_QDMI_Operation_impl_t){"cx"}};

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

int C_QDMI_query_get_sites_dev(const size_t num_entries, C_QDMI_Site *sites,
                               size_t *num_sites) {
  if ((sites != NULL && num_entries == 0) ||
      (sites == NULL && num_sites == NULL)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  const size_t device_sites_size =
      sizeof(DEVICE_SITES) / sizeof(DEVICE_SITES[0]);
  if (sites != NULL) {
    const size_t copy_size =
        (num_entries < device_sites_size ? num_entries : device_sites_size);
    memcpy((void *)sites, (const void *)DEVICE_SITES,
           copy_size * sizeof(C_QDMI_Site));
  }
  if (num_sites != NULL) {
    *num_sites = device_sites_size;
  }
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_query_get_operations_dev(const size_t num_entries,
                                    C_QDMI_Operation *operations,
                                    size_t *num_operations) {
  if ((operations != NULL && num_entries == 0) ||
      (operations == NULL && num_operations == NULL)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  const size_t device_operations_size =
      sizeof(DEVICE_OPERATIONS) / sizeof(DEVICE_OPERATIONS[0]);
  if (operations != NULL) {
    const size_t copy_size =
        (num_entries < device_operations_size ? num_entries
                                              : device_operations_size);
    memcpy((void *)operations, (void *)DEVICE_OPERATIONS,
           copy_size * sizeof(C_QDMI_Operation));
  }
  if (num_operations != NULL) {
    *num_operations = device_operations_size;
  }
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_query_device_property_dev(const QDMI_Device_Property prop,
                                     const size_t size, void *value,
                                     size_t *size_ret) {
  if (prop >= QDMI_DEVICE_PROPERTY_MAX || (value == NULL && size_ret == NULL)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  ADD_STRING_PROPERTY(QDMI_DEVICE_PROPERTY_NAME, "C Device with 5 qubits", prop,
                      size, value, size_ret)
  ADD_STRING_PROPERTY(QDMI_DEVICE_PROPERTY_VERSION, "0.1.0", prop, size, value,
                      size_ret)
  ADD_STRING_PROPERTY(QDMI_DEVICE_PROPERTY_LIBRARYVERSION, "1.0.0", prop, size,
                      value, size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_DEVICE_PROPERTY_QUBITSNUM, size_t, 5, prop,
                            size, value, size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_DEVICE_PROPERTY_STATUS, QDMI_Device_Status,
                            C_QDMI_read_device_status(), prop, size, value,
                            size_ret)
  ADD_LIST_PROPERTY(
      QDMI_DEVICE_PROPERTY_COUPLINGMAP, C_QDMI_Site,
      ((C_QDMI_Site[]){
          DEVICE_SITES[0], DEVICE_SITES[1], DEVICE_SITES[1], DEVICE_SITES[0],
          DEVICE_SITES[1], DEVICE_SITES[2], DEVICE_SITES[2], DEVICE_SITES[1],
          DEVICE_SITES[2], DEVICE_SITES[3], DEVICE_SITES[3], DEVICE_SITES[2],
          DEVICE_SITES[3], DEVICE_SITES[4], DEVICE_SITES[4], DEVICE_SITES[3],
          DEVICE_SITES[4], DEVICE_SITES[0], DEVICE_SITES[0], DEVICE_SITES[4]}),
      20, prop, size, value, size_ret)
  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_query_site_property_dev(C_QDMI_Site site,
                                   const QDMI_Site_Property prop,
                                   const size_t size, void *value,
                                   size_t *size_ret) {
  if (site == NULL || prop >= QDMI_SITE_PROPERTY_MAX ||
      (value == NULL && size_ret == NULL)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  ADD_SINGLE_VALUE_PROPERTY(QDMI_SITE_PROPERTY_TIME_T1, double, 1000.0, prop,
                            size, value, size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_SITE_PROPERTY_TIME_T2, double, 100000.0, prop,
                            size, value, size_ret)
  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_query_operation_property_dev(C_QDMI_Operation operation,
                                        const size_t num_sites,
                                        const C_QDMI_Site *sites,
                                        const QDMI_Operation_Property prop,
                                        const size_t size, void *value,
                                        size_t *size_ret) {
  if (prop >= QDMI_OPERATION_PROPERTY_MAX || operation == NULL ||
      (sites != NULL && num_sites == 0) ||
      (value == NULL && size_ret == NULL)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  // General properties
  ADD_STRING_PROPERTY(QDMI_OPERATION_PROPERTY_NAME, operation->name, prop, size,
                      value, size_ret)
  // Two-qubit gates
  if (strcmp(operation->name, "cx") == 0) {
    if (sites != NULL && num_sites != 2) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
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
    if ((sites[0]->id == 0 && sites[1]->id == 1) ||
        (sites[0]->id == 1 && sites[1]->id == 0)) {
      ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_FIDELITY, double, 0.99,
                                prop, size, value, size_ret)
    }
    if ((sites[0]->id == 1 && sites[1]->id == 2) ||
        (sites[0]->id == 2 && sites[1]->id == 1)) {
      ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_FIDELITY, double, 0.98,
                                prop, size, value, size_ret)
    }
    if ((sites[0]->id == 2 && sites[1]->id == 3) ||
        (sites[0]->id == 3 && sites[1]->id == 2)) {
      ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_FIDELITY, double, 0.97,
                                prop, size, value, size_ret)
    }
    if ((sites[0]->id == 3 && sites[1]->id == 4) ||
        (sites[0]->id == 4 && sites[1]->id == 3)) {
      ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_FIDELITY, double, 0.96,
                                prop, size, value, size_ret)
    }
    if ((sites[0]->id == 4 && sites[1]->id == 0) ||
        (sites[0]->id == 0 && sites[1]->id == 4)) {
      ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_FIDELITY, double, 0.95,
                                prop, size, value, size_ret)
    }
    if (prop == QDMI_OPERATION_PROPERTY_FIDELITY) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
  }
  // Single-qubit gates
  else if (strcmp(operation->name, "rx") == 0 ||
           strcmp(operation->name, "ry") == 0 ||
           strcmp(operation->name, "rz") == 0) {
    if (sites != NULL && num_sites != 1) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_DURATION, double, 0.01,
                              prop, size, value, size_ret)
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_QUBITSNUM, size_t, 1,
                              prop, size, value, size_ret)
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_FIDELITY, double, 0.999,
                              prop, size, value, size_ret)
  }
  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_control_create_job_dev(const QDMI_Program_Format format,
                                  const size_t size, const void *prog,
                                  C_QDMI_Job *job) {
  if (format != QDMI_PROGRAM_FORMAT_QASM2 &&
      format != QDMI_PROGRAM_FORMAT_QIRBASESTRING &&
      format != QDMI_PROGRAM_FORMAT_QIRBASEMODULE) {
    if (prog != NULL && (size <= 0 || job == NULL)) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    return QDMI_ERROR_NOTSUPPORTED;
  }
  if (prog == NULL) {
    return QDMI_SUCCESS;
  }
  if (size == 0 || job == NULL) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (C_QDMI_read_device_status() != QDMI_DEVICE_STATUS_IDLE) {
    return QDMI_ERROR_FATAL;
  }
  *job = (C_QDMI_Job)malloc(sizeof(C_QDMI_Job_impl_t));
  // set job id to random number for demonstration purposes
  (*job)->id = rand();
  (*job)->status = QDMI_JOB_STATUS_CREATED;
  (*job)->num_shots = 0;
  (*job)->results = NULL;
  (*job)->state_vec = NULL;
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_control_set_parameter_dev(C_QDMI_Job job,
                                     const QDMI_Job_Parameter param,
                                     const size_t size, const void *value) {
  if (job == NULL || param >= QDMI_JOB_PARAMETER_MAX || size == 0 ||
      job->status != QDMI_JOB_STATUS_CREATED) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (param == QDMI_JOB_PARAMETER_SHOTS_NUM) {
    job->num_shots = *(const size_t *)value;
    return QDMI_SUCCESS;
  }
  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_control_submit_job_dev(C_QDMI_Job job) {
  if (job == NULL || job->status != QDMI_JOB_STATUS_CREATED) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  C_QDMI_set_device_status(QDMI_DEVICE_STATUS_BUSY);
  job->status = QDMI_JOB_STATUS_SUBMITTED;
  // here, the actual submission of the problem to the device would happen
  // ...
  // set job status to running for demonstration purposes
  job->status = QDMI_JOB_STATUS_RUNNING;
  // generate random result data
  size_t num_qubits = 0;
  C_QDMI_query_device_property_dev(QDMI_DEVICE_PROPERTY_QUBITSNUM,
                                   sizeof(size_t), &num_qubits, NULL);
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

int C_QDMI_control_cancel_dev(C_QDMI_Job job) {
  // cannot cancel a job that is already done
  if (job->status == QDMI_JOB_STATUS_DONE) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }

  job->status = QDMI_JOB_STATUS_CANCELLED;
  C_QDMI_set_device_status(QDMI_DEVICE_STATUS_IDLE);
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_control_check_dev(C_QDMI_Job job, QDMI_Job_Status *status) {
  // randomly decide whether job is done or not
  if (job->status == QDMI_JOB_STATUS_RUNNING && rand() % 2 == 0) {
    C_QDMI_set_device_status(QDMI_DEVICE_STATUS_IDLE);
    job->status = QDMI_JOB_STATUS_DONE;
  }
  *status = job->status;
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_control_wait_dev(C_QDMI_Job job) {
  // in a real implementation, this would wait for the job to finish
  job->status = QDMI_JOB_STATUS_DONE;
  C_QDMI_set_device_status(QDMI_DEVICE_STATUS_IDLE);
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

// Comparison function for qsort
int C_QDMI_Compare_results(const void *a, const void *b) {
  return strcmp(*(char **)a, *(char **)b);
} /// [DOXYGEN FUNCTION END]

int C_QDMI_control_get_data_dev(C_QDMI_Job job, const QDMI_Job_Result result,
                                const size_t size, void *data,
                                size_t *size_ret) {
  if (job->status != QDMI_JOB_STATUS_DONE) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (result == QDMI_JOB_RESULT_SHOTS) {
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
  }
  if (result == QDMI_JOB_RESULT_HIST_KEYS ||
      result == QDMI_JOB_RESULT_HIST_VALUES) {
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
          C_QDMI_Compare_results);
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
  }
  if (result == QDMI_JOB_RESULT_STATEVECTOR_DENSE) {
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
  }
  if (result == QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS ||
      result == QDMI_JOB_RESULT_STATEVECTOR_SPARSE_VALUES ||
      result == QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS ||
      result == QDMI_JOB_RESULT_PROBABILITIES_SPARSE_VALUES) {
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
    if (result == QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS ||
        result == QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS) {
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
    if (result == QDMI_JOB_RESULT_STATEVECTOR_SPARSE_VALUES) {
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
    } else {
      const size_t req_size = count * sizeof(double);
      if (data != NULL) {
        if (size < req_size) {
          return QDMI_ERROR_INVALIDARGUMENT;
        }
        double *data_ptr = data;
        for (size_t i = 0; i < length; ++i) {
          if (vec[2 * i] != 0.0 || vec[(2 * i) + 1] != 0.0) {
            *data_ptr++ = (vec[2 * i] * vec[2 * i]) +
                          (vec[(2 * i) + 1] * vec[(2 * i) + 1]);
          }
        }
      }
      if ((size_ret) != NULL) {
        *(size_ret) = req_size;
      }
    }
    return QDMI_SUCCESS;
  }
  if (result == QDMI_JOB_RESULT_PROBABILITIES_DENSE) {
    const size_t length = job->state_vec_length / 2;
    const size_t req_size = length * sizeof(double);
    if (data != NULL) {
      if (size < req_size) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      double *data_ptr = data;
      for (size_t i = 0; i < length; ++i) {
        // Calculate the probability of the state
        *data_ptr++ =
            (job->state_vec[2 * i] * job->state_vec[2 * i]) +
            (job->state_vec[(2 * i) + 1] * job->state_vec[(2 * i) + 1]);
      }
    }
    if ((size_ret) != NULL) {
      *(size_ret) = req_size;
    }
    return QDMI_SUCCESS;
  }
  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

void C_QDMI_control_free_job_dev(C_QDMI_Job job) {
  // this method should free all resources associated with the job
  if (job->results != NULL) {
    free(job->results);
    free(job->state_vec);
    job->results = NULL;
  }
  free(job);
} /// [DOXYGEN FUNCTION END]

int C_QDMI_control_initialize_dev(void) {
  C_QDMI_set_device_status(QDMI_DEVICE_STATUS_IDLE);
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int C_QDMI_control_finalize_dev(void) {
  C_QDMI_set_device_status(QDMI_DEVICE_STATUS_OFFLINE);
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]
