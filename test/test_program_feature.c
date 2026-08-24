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

#include "qdmi/client.h"

_Static_assert(QDMI_CLIENT_ABI_VERSION == QDMI_MAKE_VERSION(1, 4, 0),
               "unexpected QDMI Client ABI version");

/// C uses void for a no-argument prototype. The C++ test calls this function.
// NOLINTNEXTLINE(misc-use-internal-linkage, modernize-redundant-void-arg)
QDMI_Program_Feature QDMI_test_unconstrained_program_feature(void) {
  const QDMI_Program_Feature feature = QDMI_PROGRAM_FEATURE_UNCONSTRAINED(
      QDMI_PROGRAM_FEATURE_INTEGER_COMPUTATION, 64U);
  return feature;
}
