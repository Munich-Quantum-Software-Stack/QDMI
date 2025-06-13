/*------------------------------------------------------------------------------
Copyright 2024 Munich Quantum Software Stack Project

Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
"License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at

https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations under
the License.

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
#include <cstddef>
#include <cstdint>
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

  [[nodiscard]] auto get_qubits_num() const -> size_t;

  [[nodiscard]] auto get_operation_map() const
      -> std::map<std::string, QDMI_Operation>;

  [[nodiscard]] auto get_sites() const -> std::vector<QDMI_Site>;

  [[nodiscard]] auto get_coupling_map() const
      -> std::vector<std::pair<QDMI_Site, QDMI_Site>>;

  [[nodiscard]] auto get_site_id(QDMI_Site site) const -> uint64_t;

  [[nodiscard]] auto get_site_t1(QDMI_Site site) const -> double;

  [[nodiscard]] auto get_site_t2(QDMI_Site site) const -> double;

  [[nodiscard]] auto get_operands_num(const QDMI_Operation &op) const -> size_t;

  [[nodiscard]] auto get_parameters_num(const QDMI_Operation &op) const
      -> size_t;

  [[nodiscard]] auto get_environment_variables() const
      -> std::vector<QDMI_EnvironmentSensor>;

  [[nodiscard]] auto
  get_environment_id(QDMI_EnvironmentSensor environment) const -> std::string;

  [[nodiscard]] auto
  get_environment_unit(QDMI_EnvironmentSensor environment) const -> std::string;

  [[nodiscard]] auto
  get_environment_sampling_rate(QDMI_EnvironmentSensor environment) const
      -> int;
};
