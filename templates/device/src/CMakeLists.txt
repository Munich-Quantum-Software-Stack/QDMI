# ------------------------------------------------------------------------------
# Copyright 2024 Munich Quantum Software Stack Project
#
# Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
# "License"); you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# ------------------------------------------------------------------------------
add_library(my_device SHARED my_device.cpp)
target_link_libraries(my_device PRIVATE qdmi::qdmi qdmi::qdmi_project_warnings)
generate_prefixed_qdmi_headers(${QDMI_PREFIX})
target_include_directories(my_device PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)
if(NOT CXX_DEVICE)
  # set c++ standard
  target_compile_features(my_device PRIVATE cxx_std_17)
endif()
add_library(qdmi::my_device ALIAS my_device)
