# Copyright (c) 2024 - 2025 Munich Quantum Software Stack Project
# All rights reserved.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE.md
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

option(ENABLE_CACHE "Enable compiler cache if available" ON)
if(NOT ENABLE_CACHE)
  return()
endif()

set(CACHE_OPTION_VALUES "ccache" "sccache")
set(CACHE_OPTION
    "ccache"
    CACHE STRING "Compiler cache to use")
set_property(CACHE CACHE_OPTION PROPERTY STRINGS ${CACHE_OPTION_VALUES})
list(FIND CACHE_OPTION_VALUES ${CACHE_OPTION} CACHE_OPTION_INDEX)
if(CACHE_OPTION_INDEX EQUAL -1)
  message(
    NOTICE
    "Unknown compiler cache '${CACHE_OPTION}'. Available options are: ${CACHE_OPTION_VALUES}"
  )
endif()

find_program(CACHE_BINARY ${CACHE_OPTION})
if(CACHE_BINARY)
  message(STATUS "Compiler cache '${CACHE_OPTION}' found and enabled")
  set(CMAKE_C_COMPILER_LAUNCHER ${CACHE_BINARY})
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CACHE_BINARY})
else()
  message(NOTICE "${CACHE_OPTION} is enabled but was not found. Not using it")
endif()
