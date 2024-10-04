/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

#include "qdmi/private/_device.h"

#include "qdmi/private/_types.h"
#include "qdmi/return_codes.h"

#include <dlfcn.h>
#include <stdlib.h>
#include <qdmi/backend/query.h>

#define LOAD_SYMBOL(device, symbol)                                            \
  {                                                                            \
    *(void **)(&((device)->symbol)) = dlsym((device)->lib_handle, #symbol);    \
    if ((device)->symbol == NULL) {                                            \
      QDMI_device_close(device);                                               \
      return QDMI_ERROR_NOT_FOUND;                                             \
    }                                                                          \
  }

int QDMI_device_open(const char *lib_name, QDMI_Device *device) {
  // allocate memory for the device
  *device = (QDMI_Device_impl_t *)malloc(sizeof(QDMI_Device_impl_t));
  if (*device == NULL) {
    return QDMI_ERROR_OUT_OF_MEM;
  }
  // open the dynamic library at the location passed in `lib_name`
  (*device)->lib_handle = dlopen(lib_name, RTLD_NOW);
  if ((*device)->lib_handle == NULL) {
    free(*device);
    return QDMI_ERROR_LIB_NOT_FOUND;
  }
  // load the function symbols from the dynamic library
  LOAD_SYMBOL(*device, QDMI_query_device_property_char);
  LOAD_SYMBOL(*device, QDMI_query_device_property_double);
  LOAD_SYMBOL(*device, QDMI_query_device_property_float);
  LOAD_SYMBOL(*device, QDMI_query_device_property_int);
  LOAD_SYMBOL(*device, QDMI_query_device_property_char_list);
  LOAD_SYMBOL(*device, QDMI_query_device_property_double_list);
  LOAD_SYMBOL(*device, QDMI_query_device_property_float_list);
  LOAD_SYMBOL(*device, QDMI_query_device_property_int_list);

  LOAD_SYMBOL(*device, QDMI_query_all_sites);
  LOAD_SYMBOL(*device, QDMI_query_site_by_index);
  LOAD_SYMBOL(*device, QDMI_query_site_property_char);
  LOAD_SYMBOL(*device, QDMI_query_site_property_double);
  LOAD_SYMBOL(*device, QDMI_query_site_property_float);
  LOAD_SYMBOL(*device, QDMI_query_site_property_int);
  LOAD_SYMBOL(*device, QDMI_query_site_property_char_list);
  LOAD_SYMBOL(*device, QDMI_query_site_property_double_list);
  LOAD_SYMBOL(*device, QDMI_query_site_property_float_list);
  LOAD_SYMBOL(*device, QDMI_query_site_property_int_list);

  LOAD_SYMBOL(*device, QDMI_query_all_operations);
  LOAD_SYMBOL(*device, QDMI_query_operation_by_name);
  LOAD_SYMBOL(*device, QDMI_query_operation_property_char);
  LOAD_SYMBOL(*device, QDMI_query_operation_property_double);
  LOAD_SYMBOL(*device, QDMI_query_operation_property_float);
  LOAD_SYMBOL(*device, QDMI_query_operation_property_int);
  LOAD_SYMBOL(*device, QDMI_query_operation_property_char_list);
  LOAD_SYMBOL(*device, QDMI_query_operation_property_double_list);
  LOAD_SYMBOL(*device, QDMI_query_operation_property_float_list);
  LOAD_SYMBOL(*device, QDMI_query_operation_property_int_list);
  LOAD_SYMBOL(*device, QDMI_query_operation_at_site_property_char);
  LOAD_SYMBOL(*device, QDMI_query_operation_at_site_property_double);
  LOAD_SYMBOL(*device, QDMI_query_operation_at_site_property_float);
  LOAD_SYMBOL(*device, QDMI_query_operation_at_site_property_int);
  LOAD_SYMBOL(*device, QDMI_query_operation_at_site_property_char_list);
  LOAD_SYMBOL(*device, QDMI_query_operation_at_site_property_double_list);
  LOAD_SYMBOL(*device, QDMI_query_operation_at_site_property_float_list);
  LOAD_SYMBOL(*device, QDMI_query_operation_at_site_property_int_list);
  // initialize the next pointer to NULL
  (*device)->next = NULL;
  // return success
  return QDMI_SUCCESS;
}

void QDMI_device_close(QDMI_Device device) {
  // close the dynamic library
  dlclose(device->lib_handle);
  // free the memory allocated for the device
  free(device);
}