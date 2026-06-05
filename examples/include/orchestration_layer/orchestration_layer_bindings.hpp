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
/// Orchestration layer interface bindings.

#pragma once

#include "o_qdmi/orchestration_layer.h"

namespace qdmi::orchestration_layer {
/// @returns the @ref O_QDMI_OrchestrationLayer_Interface "orchestration layer
/// interface" of the orchestration layer.
[[nodiscard]] auto Get_orchestration_layer_interface()
    -> const O_QDMI_OrchestrationLayer_Interface &;
} // namespace qdmi::orchestration_layer
