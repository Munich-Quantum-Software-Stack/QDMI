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

#include "example_tool.hpp"

#include <cstddef>
#include <iterator>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

std::string Tool::replace_all_occurrences(std::string str,
                                          const std::string &from,
                                          const std::string &to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // Move past the last replaced substring
  }
  return str;
}

std::string Tool::compile(const std::string &qasm_string) {
  // Check whether the circuit only uses up to two qubits
  const std::regex re(R"(qreg \w+\[(\d)\];)");
  const auto begin =
      std::sregex_iterator(qasm_string.begin(), qasm_string.end(), re);
  const auto end = std::sregex_iterator();
  if (std::distance(begin, end) == 0) {
    throw std::invalid_argument("The circuit does not declare any qubits.");
  }
  if (std::distance(begin, end) > 1) {
    throw std::invalid_argument(
        "The circuit declares more than one qubit register.");
  }
  const std::smatch &match = *begin;
  const auto num_qubits_str = match.str(1);
  const auto num_qubits = std::stoi(num_qubits_str);
  if (num_qubits > 2) {
    throw std::invalid_argument("The circuit uses more than two qubits but "
                                "this tool only supports up to two qubits.");
  }
  // Check whether the device provides enough qubits
  if (num_qubits > fomac.get_qubits_num()) {
    throw std::invalid_argument(
        "The device does not provide enough qubits for the circuit.");
  }
  // Choose an arbitrary edge for the two qubits
  const auto edge = fomac.get_coupling_map().front();
  std::stringstream from;
  from << "qreg q[" << num_qubits << "];";
  std::stringstream to;
  to << "qreg q[" << fomac.get_qubits_num() << "];";
  auto result = replace_all_occurrences(qasm_string, from.str(), to.str());
  from.clear();
  from << "q[0]";
  to.clear();
  to << "q[" << edge.first << "]";
  result = replace_all_occurrences(result, from.str(), to.str());
  from.clear();
  from << "q[1]";
  to.clear();
  to << "q[" << edge.second << "]";
  result = replace_all_occurrences(result, from.str(), to.str());
  return result;
}
