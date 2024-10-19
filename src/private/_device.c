/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

#include "_device.h"

#include "_types.h"
#include "qdmi/return_codes.h"

#include <dlfcn.h>
#include <stdlib.h>

#define LOAD_SYMBOL(device, symbol)                                            \
  {                                                                            \
    (device)->symbol = NULL;                                                   \
    *(void **)(&((device)->symbol)) =                                          \
        dlsym((device)->lib_handle, #symbol "_dev");                           \
    if ((device)->symbol == NULL) {                                            \
      QDMI_device_close(device);                                               \
      return QDMI_ERROR_NOT_FOUND;                                             \
    }                                                                          \
  }

int QDMI_device_open(const char *lib_name, QDMI_Device *device) {
  // allocate memory for the device
  *device = (QDMI_Device)malloc(sizeof(QDMI_Device_impl_t));
  if (*device == NULL) {
    return QDMI_ERROR_OUT_OF_MEM;
  }
  // open the dynamic library at the location passed in `lib_name`
  (*device)->lib_handle = dlopen(lib_name, RTLD_NOW | RTLD_LOCAL);
  if ((*device)->lib_handle == NULL) {
    free(*device);
    return QDMI_ERROR_LIB_NOT_FOUND;
  }
  // load the function symbols from the dynamic library

  // this has to be the first symbol to be loaded because this method is used
  // to close the device in case of any error.
  LOAD_SYMBOL(*device, QDMI_control_finalize);

  LOAD_SYMBOL(*device, QDMI_query_device_property_string);
  LOAD_SYMBOL(*device, QDMI_query_device_property_double);
  LOAD_SYMBOL(*device, QDMI_query_device_property_int);
  LOAD_SYMBOL(*device, QDMI_query_device_property_string_list);
  LOAD_SYMBOL(*device, QDMI_query_device_property_double_list);
  LOAD_SYMBOL(*device, QDMI_query_device_property_int_list);

  LOAD_SYMBOL(*device, QDMI_query_site_property_string);
  LOAD_SYMBOL(*device, QDMI_query_site_property_double);
  LOAD_SYMBOL(*device, QDMI_query_site_property_int);
  LOAD_SYMBOL(*device, QDMI_query_site_property_string_list);
  LOAD_SYMBOL(*device, QDMI_query_site_property_double_list);
  LOAD_SYMBOL(*device, QDMI_query_site_property_int_list);

  LOAD_SYMBOL(*device, QDMI_query_operation_property_string);
  LOAD_SYMBOL(*device, QDMI_query_operation_property_double);
  LOAD_SYMBOL(*device, QDMI_query_operation_property_int);
  LOAD_SYMBOL(*device, QDMI_query_operation_property_string_list);
  LOAD_SYMBOL(*device, QDMI_query_operation_property_double_list);
  LOAD_SYMBOL(*device, QDMI_query_operation_property_int_list);

  LOAD_SYMBOL(*device, QDMI_control_submit_qasm);
  LOAD_SYMBOL(*device, QDMI_control_submit_qir_string);
  LOAD_SYMBOL(*device, QDMI_control_submit_qir_module);
  LOAD_SYMBOL(*device, QDMI_control_cancel);
  LOAD_SYMBOL(*device, QDMI_control_check);
  LOAD_SYMBOL(*device, QDMI_control_wait);
  LOAD_SYMBOL(*device, QDMI_control_get_hist);
  LOAD_SYMBOL(*device, QDMI_control_get_raw);
  LOAD_SYMBOL(*device, QDMI_control_free_job);
  LOAD_SYMBOL(*device, QDMI_control_initialize);

  // initialize the next pointer to NULL
  (*device)->next = NULL;
  // return success
  return QDMI_SUCCESS;
}

void QDMI_device_close(QDMI_Device device) {
  // Check if QDMI_control_finalize is not NULL before calling it
  if (device->QDMI_control_finalize != NULL) {
    device->QDMI_control_finalize();
  }
  // close the dynamic library
  dlclose(device->lib_handle);
  // free the memory allocated for the device
  free(device);
}
