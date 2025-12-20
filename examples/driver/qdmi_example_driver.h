/*
 * Copyright (c) 2024 - 2025 Munich Quantum Software Stack Project
 * All rights reserved.
 *
 * Licensed under the Apache License v2.0 with LLVM Exceptions (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE.md
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

/** @file
 * @brief An example driver implementation in C++.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the QDMI driver.
 * @details This function must be called exactly once before any other QDMI
 * function. It performs any necessary initialization of the driver so that a
 * client can allocate sessions (@ref QDMI_Session) and access devices (@ref
 * QDMI_Device).
 * @return @ref QDMI_SUCCESS if the driver was initialized successfully.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 */
int QDMI_driver_init();

/**
 * @brief Shutdown the QDMI driver.
 * @details This function must be called exactly once to cleanly shut down the
 * driver. It should be called after all sessions have been freed and no more
 * QDMI functions will be called.
 * @return @ref QDMI_SUCCESS if the driver was shut down successfully.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 */
int QDMI_driver_shutdown();

#ifdef __cplusplus
} // extern "C"
#endif
