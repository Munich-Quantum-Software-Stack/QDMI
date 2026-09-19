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

cmake_minimum_required(VERSION 3.24)
project(qdmi_package_test LANGUAGES C CXX)

set(expected_launcher "${CMAKE_COMMAND};-E;env")
set(CMAKE_C_COMPILER_LAUNCHER "${expected_launcher}")
set(CMAKE_CXX_COMPILER_LAUNCHER "${expected_launcher}")

find_package(qdmi 1.3.3 CONFIG REQUIRED)

foreach(language IN ITEMS C CXX)
  if(NOT "${CMAKE_${language}_COMPILER_LAUNCHER}" STREQUAL
     "${expected_launcher}")
    message(FATAL_ERROR "QDMI changed the ${language} compiler launcher")
  endif()
endforeach()

get_target_property(compile_options qdmi::qdmi INTERFACE_COMPILE_OPTIONS)
if("-g" IN_LIST compile_options)
  message(FATAL_ERROR "QDMI requires debug information in consumer targets")
endif()

# Exercise the package version check without requiring a cross compiler.
set(consumer_pointer_size "${CMAKE_SIZEOF_VOID_P}")
foreach(pointer_size IN ITEMS 4 8)
  set(CMAKE_SIZEOF_VOID_P "${pointer_size}")
  find_package(qdmi 1.3.3 CONFIG REQUIRED)
endforeach()
set(CMAKE_SIZEOF_VOID_P "${consumer_pointer_size}")
