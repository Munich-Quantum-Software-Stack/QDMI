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

/// @file
/// QPU interface bindings for the device.

#pragma once

#include "q_qdmi/core.h"

namespace qdmi::qpu {
/// @returns the @ref Q_QDMI_Core_Interface "core interface" of the quantum
/// processing unit.
[[nodiscard]] auto Get_core_interface() -> Q_QDMI_Core_Interface;

/// @returns the @ref Q_QDMI_Library "library" of the quantum processing unit.
[[nodiscard]] auto Get_library() -> Q_QDMI_Library;
} // namespace qdmi::qpu
