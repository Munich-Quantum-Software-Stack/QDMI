/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

/** @file
 * @brief A simple example of an implementation of a tool using QDMI in C++.
 * @details This file can be used as a template for implementing a compilation
 * pass in C++.
 */

#pragma once

#include "qdmi/interface.h"

#include <cstddef>
#include <string>
#include <vector>

class Tool {
private:
  QDMI_Device device;

public:
  explicit Tool(QDMI_Device device) : device(device) {}

private:
  int getDeviceNumQubits();

  std::vector<std::pair<int, int>> getDeviceCouplingMap();

  std::string replaceAllOccurrences(std::string str, const std::string &from,
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