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

# Doxygen 1.18 emits letter links without targets on short member indexes.
# Remove this workaround when Doxygen generates those targets consistently.
file(GLOB index_pages "${QDMI_DOCS_HTML_DIR}/globals*.html")
foreach(page IN LISTS index_pages)
  file(READ "${page}" html)
  set(original "${html}")
  string(REGEX MATCHALL "href=\"#index_[a-z]\"" letter_links "${html}")
  foreach(link IN LISTS letter_links)
    string(REGEX MATCH "index_([a-z])" anchor "${link}")
    set(letter "${CMAKE_MATCH_1}")
    if(html MATCHES "id=\"${anchor}\"")
      continue()
    endif()

    # Match the first API entry after the IGNORE_PREFIX in Doxyfile.in.
    string(TOUPPER "${letter}" upper)
    string(REGEX MATCH "<li>QDMI_[${letter}${upper}]" member "${html}")
    if(NOT member)
      message(FATAL_ERROR "No API entry for ${page}#${anchor}")
    endif()
    string(FIND "${html}" "${member}" position)
    string(SUBSTRING "${html}" 0 ${position} before)
    string(SUBSTRING "${html}" ${position} -1 after)
    string(REGEX REPLACE "^<li>" "<li id=\"${anchor}\">" after "${after}")
    set(html "${before}${after}")
  endforeach()
  if(NOT html STREQUAL original)
    file(WRITE "${page}" "${html}")
  endif()
endforeach()
