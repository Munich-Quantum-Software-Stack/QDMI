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

// Declaration shapes that the export helper must recognise.
#pragma once

#include <stddef.h>

#define QDMI_EXPORT __attribute__((visibility("default")))
#define QDMI_DEPRECATED_EXPORT QDMI_EXPORT __attribute__((deprecated))

QDMI_DEPRECATED_EXPORT int QDMI_device_deprecated(void);
QDMI_EXPORT const char *QDMI_device_pointer(void);
QDMI_EXPORT size_t QDMI_device_wrapped(void);
