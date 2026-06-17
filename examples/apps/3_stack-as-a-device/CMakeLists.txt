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

include(MakeVersion)

set(TARGET_NAME 3_stack-as-a-device)

# create executable target
add_executable(${TARGET_NAME})

# link to qdmi core library
target_link_libraries(${TARGET_NAME} PRIVATE qdmi::qdmi
                                             qdmi::qdmi_project_warnings)

# set C standard to C11
target_compile_features(${TARGET_NAME} PRIVATE c_std_11)

# define version macros
get_target_property(qdmi_library_VERSION qdmi::qdmi VERSION)
qdmi_make_version(qdmi_packed_version ${qdmi_library_VERSION})
target_compile_definitions(
  ${TARGET_NAME} PRIVATE QDMI_VERSION=${qdmi_packed_version}
                         QDMI_DYLIB="$<TARGET_FILE:orchestration_layer_o1>")

# add sources to target
target_sources(${TARGET_NAME} PRIVATE stack-as-a-device.c)

# set the folder of the target for IDEs
set_target_properties(
  ${TARGET_NAME} PROPERTIES FOLDER "examples/apps" RUNTIME_OUTPUT_DIRECTORY
                                                   "${CMAKE_BINARY_DIR}/bin")

# add qdmi::orchestration_layer_o1 as a dependency to ensure it is built before
# the executable
add_dependencies(${TARGET_NAME} qdmi::orchestration_layer_o1)
