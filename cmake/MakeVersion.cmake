# Copyright (c) 2024 - 2026 QDMI Maintainers
# All rights reserved.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# https://llvm.org/LICENSE.txt
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# function to pack version strings into one integer according to QDMI's
# specification
function(qdmi_make_version output input)
  set(result 0)
  # Split the version string into major, minor, and patch components
  string(REGEX MATCHALL "[0-9]+" qdmi_version_components "${input}")
  list(GET qdmi_version_components 0 qdmi_version_major)
  list(GET qdmi_version_components 1 qdmi_version_minor)
  list(GET qdmi_version_components 2 qdmi_version_patch)
  # Calculate the packed integer version
  math(
    EXPR
    result
    "(${qdmi_version_major} << 22) | (${qdmi_version_minor} << 12) | ${qdmi_version_patch}"
  )
  set(${output}
      ${result}
      PARENT_SCOPE)
endfunction()
