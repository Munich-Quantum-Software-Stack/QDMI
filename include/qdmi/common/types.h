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
 * @brief Defines all common types shared between all components.
 */

#pragma once

#include "qdmi/common/enums.h"

#ifdef __cplusplus
extern "C" {
#endif

// The following disables the clang-tidy warning modernize-use-using.
// Since this is C code, we cannot use the using keyword.
// NOLINTBEGIN(modernize-use-using)

/// Type of the device properties.
typedef enum QDMI_DEVICE_PROPERTY_T QDMI_Device_Property;

/// Type of the device status.
typedef enum QDMI_DEVICE_STATUS_T QDMI_Device_Status;

/// Type of the site properties.
typedef enum QDMI_SITE_PROPERTY_T QDMI_Site_Property;

/// Type of the operation properties.
typedef enum QDMI_OPERATION_PROPERTY_T QDMI_Operation_Property;

/// Type of the device status.
typedef enum QDMI_JOB_STATUS_T QDMI_Job_Status;

/// Type of the device mode used in opening a device.
typedef enum QDMI_DEVICE_MODE_T QDMI_Device_Mode;

/// Type of the program format.
typedef enum QDMI_PROGRAM_FORMAT_T QDMI_Program_Format;

/// Type of the job parameter.
typedef enum QDMI_JOB_PARAMETER_T QDMI_Job_Parameter;

/// Type of the job result.
typedef enum QDMI_JOB_RESULT_T QDMI_Job_Result;

/// Type of the session parameter.
typedef enum QDMI_SESSION_PARAMETER_T QDMI_Session_Parameter;

// NOLINTEND(modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
