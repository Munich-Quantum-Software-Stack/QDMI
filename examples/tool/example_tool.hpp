/*
 * Copyright (c) 2024 - 2025 Munich Quantum Software Stack Project
 * All rights reserved.
 *
 * Licensed under the Apache License v2.0 with LLVM Exceptions (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE.md
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

/** @file
 * @brief A simple example of an implementation of a tool using QDMI in C++.
 * @details This file can be used as a template for implementing a compilation
 * pass in C++.
 */

#pragma once

#include "example_fomac.hpp"
#include "qdmi/client.h"

#include <string>

class Tool {
private:
  QDMI_Device device;
  FoMaC fomac;

public:
  explicit Tool(QDMI_Device dev) : device(dev), fomac(dev) {}

private:
  static std::string replace_all_occurrences(std::string str,
                                             const std::string &from,
                                             const std::string &to);

public:
  /**
   * @brief Compile a QASM string for a device with maximum two qubits.
   * @details The function scans the QASM string for the number of qubits and
   * maps the two qubits on some pair of connected qubits on the device.
   * @param qasm_string The QASM string to compile.
   * @return The compiled QASM string.
   */
  std::string compile(const std::string &qasm_string);
};
