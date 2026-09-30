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

# Restrict a shared library to the selected QDMI interface and explicit
# additions. Usage: configure_qdmi_exports(TARGET my_device INTERFACE device
# PREFIX MY) configure_qdmi_exports(TARGET my_driver INTERFACE client
# EXTRA_SYMBOLS QDMI_driver_init QDMI_driver_shutdown)
function(configure_qdmi_exports)
  cmake_parse_arguments(PARSE_ARGV 0 ARG "" "TARGET;INTERFACE;PREFIX"
                        "EXTRA_SYMBOLS")
  if(ARG_UNPARSED_ARGUMENTS
     OR ARG_KEYWORDS_MISSING_VALUES
     OR NOT TARGET "${ARG_TARGET}"
     OR NOT ARG_INTERFACE MATCHES "^(device|client)$")
    message(
      FATAL_ERROR
        "configure_qdmi_exports requires TARGET and INTERFACE device or client")
  endif()
  if(ARG_INTERFACE STREQUAL "device" AND NOT "${ARG_PREFIX}" MATCHES
                                         "^[A-Za-z_][A-Za-z0-9_]*$")
    message(FATAL_ERROR "The device interface requires a C identifier PREFIX")
  elseif(ARG_INTERFACE STREQUAL "client" AND DEFINED ARG_PREFIX)
    message(FATAL_ERROR "The client interface does not take a PREFIX")
  endif()
  foreach(symbol IN LISTS ARG_EXTRA_SYMBOLS)
    if(NOT symbol MATCHES "^[A-Za-z_][A-Za-z0-9_]*$")
      message(
        FATAL_ERROR "EXTRA_SYMBOLS must contain exact C identifiers: ${symbol}")
    endif()
  endforeach()

  get_target_property(imported ${ARG_TARGET} IMPORTED)
  get_target_property(aliased ${ARG_TARGET} ALIASED_TARGET)
  get_target_property(type ${ARG_TARGET} TYPE)
  if(imported OR aliased)
    message(
      FATAL_ERROR
        "Configure exports on the implementation target, not an imported target or alias"
    )
  elseif(type STREQUAL "STATIC_LIBRARY")
    # A static archive has no dynamic export table. Configure its final library.
    return()
  elseif(NOT type MATCHES "^(SHARED|MODULE)_LIBRARY$")
    message(FATAL_ERROR "configure_qdmi_exports requires a library target")
  endif()

  if(WIN32)
    # Windows implementations retain their explicit dllexport declarations.
    return()
  elseif(NOT APPLE AND NOT CMAKE_EXECUTABLE_FORMAT STREQUAL "ELF")
    message(
      FATAL_ERROR
        "QDMI export restrictions are supported on ELF and Apple linkers")
  endif()

  set(include_dir "${QDMI_INCLUDE_BUILD_DIR}")
  if(NOT include_dir)
    set(include_dir "${qdmi_INCLUDE_DIR}")
  endif()
  set(header "${include_dir}/qdmi/${ARG_INTERFACE}.h")
  set_property(
    DIRECTORY
    APPEND
    PROPERTY CMAKE_CONFIGURE_DEPENDS "${header}")
  file(READ "${header}" declarations)
  # QDMI functions return int or void; allow line breaks in their declarations.
  string(
    REGEX
      MATCHALL
      "[\r\n](QDMI_EXPORT[ \t\r\n]+)?(int|void)[ \t\r\n]+QDMI_[A-Za-z0-9_]+[ \t\r\n]*\\("
      functions
      "${declarations}")
  if(NOT functions)
    message(FATAL_ERROR "No QDMI functions found in ${header}")
  endif()
  set(symbols ${ARG_EXTRA_SYMBOLS})
  foreach(declaration IN LISTS functions)
    string(REGEX MATCH "QDMI_[A-Za-z0-9_]+[ \t\r\n]*\\(" symbol
                 "${declaration}")
    string(REGEX REPLACE "[ \t\r\n]*\\($" "" symbol "${symbol}")
    if(ARG_INTERFACE STREQUAL "device")
      set(symbol "${ARG_PREFIX}_${symbol}")
    endif()
    list(APPEND symbols "${symbol}")
  endforeach()
  list(REMOVE_DUPLICATES symbols)
  list(SORT symbols)

  get_target_property(binary_dir ${ARG_TARGET} BINARY_DIR)
  if(APPLE)
    set(export_file "${binary_dir}/${ARG_TARGET}.exports")
    list(TRANSFORM symbols PREPEND "_")
    list(JOIN symbols "\n" content)
    string(APPEND content "\n")
    set(link_option "LINKER:-exported_symbols_list,${export_file}")
  else()
    set(export_file "${binary_dir}/${ARG_TARGET}.map")
    list(JOIN symbols ";\n    " content)
    set(content "{\n  global:\n    ${content};\n  local: *;\n};\n")
    set(link_option "LINKER:--version-script=${export_file}")
  endif()
  file(CONFIGURE OUTPUT "${export_file}" CONTENT "@content@" @ONLY)
  target_link_options(${ARG_TARGET} PRIVATE "${link_option}")
  set_property(
    TARGET ${ARG_TARGET}
    APPEND
    PROPERTY LINK_DEPENDS "${export_file}")
endfunction()
