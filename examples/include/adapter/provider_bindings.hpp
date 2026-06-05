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
/// Provider interface bindings for the provider.

#pragma once

#include "v2_qdmi/provider.h"

namespace qdmi::adapter {
/// @returns the @ref V2_QDMI_Provider_Interface "provider interface" of the
/// provider.
[[nodiscard]] auto Get_provider_interface() -> V2_QDMI_Provider_Interface;
} // namespace qdmi::adapter
