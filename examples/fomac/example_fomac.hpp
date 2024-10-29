/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

/** @file
 * @brief A simple example of an implementation of a FoMaC library in C++.
 * @details This file can be used as a template for implementing a figures of
 * merit and constraints library in C++.
 */

#pragma once

#include "qdmi/client.h"

#include <cassert>
#include <map>
#include <string>
#include <utility>
#include <vector>

class FoMaC {
private:
  QDMI_Device device;

  static auto throw_if_error(int status, const std::string &message) -> void;

public:
  explicit FoMaC(QDMI_Device dev) : device(dev) {}

  [[nodiscard]] auto get_qubits_num() const -> int;

  [[nodiscard]] auto get_operation_map() const
      -> std::map<std::string, QDMI_Operation>;

  [[nodiscard]] auto get_sites() const -> std::vector<QDMI_Site>;

  [[nodiscard]] auto get_coupling_map() const
      -> std::vector<std::pair<QDMI_Site, QDMI_Site>>;

  [[nodiscard]] auto get_operands_num(const QDMI_Operation &op) const -> int;
};
