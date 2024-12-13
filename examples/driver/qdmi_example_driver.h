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
 * @brief A drive implementation for QDMI.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the QDMI driver.
 * @details This function should be called before any other QDMI function. It
 * performs any necessary initialization of the driver so that a client can
 * allocate sessions (@ref QDMI_Session) and access devices (@ref QDMI_Device).
 * @note This function should be called only once.
 * @return @ref QDMI_SUCCESS if the driver was initialized successfully.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 */
int QDMI_driver_init();

/**
 * @brief Shutdown the QDMI driver.
 * @details This function should be called to perform any necessary cleanup of
 * the driver. It should be called after all sessions have been freed and no
 * more QDMI functions will be called.
 * @note This function should be called only once.
 * @return @ref QDMI_SUCCESS if the driver was shut down successfully.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 */
int QDMI_driver_shutdown();

#ifdef __cplusplus
} // extern "C"
#endif
