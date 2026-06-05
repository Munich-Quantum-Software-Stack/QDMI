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
/// Core interface bindings for the devices managed by the orchestration layer.

#pragma once

#include "od_qdmi/core.h"

namespace qdmi::orchestration_layer {
/// Retrieves the core interface of the devices managed by the orchestration
/// layer.
///
/// This @ref OD_QDMI_Core_Interface "core interface" substitutes the one from
/// the devices directly. Hence, the orchestration layer can hide some
/// functionality from the client if it wants to prevent access to it.
///
/// @returns the @ref OD_QDMI_Core_Interface "core interface" of the devices
/// managed by the orchestration layer.
[[nodiscard]] auto Get_device_core_interface() -> OD_QDMI_Core_Interface;
} // namespace qdmi::orchestration_layer
