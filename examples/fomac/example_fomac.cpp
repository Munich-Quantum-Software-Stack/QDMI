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

#include "example_fomac.hpp"

#include "qdmi/client.h"

#include <cstddef>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

auto FoMaC::throw_if_error(int status, const std::string &message) -> void {
  if (status != QDMI_SUCCESS) {
    if (status == QDMI_WARN_GENERAL) {
      if (message.empty()) {
        std::cerr << "A general warning." << '\n';
        return;
      }
      std::cerr << message << '\n';
      return;
    }
    if (status == QDMI_ERROR_FATAL) {
      if (message.empty()) {
        throw std::runtime_error("A fatal error.");
      }
      throw std::runtime_error(message);
    }
    if (status == QDMI_ERROR_OUTOFMEM) {
      if (message.empty()) {
        throw std::bad_alloc();
      }
      throw std::runtime_error(message);
    }
    if (status == QDMI_ERROR_NOTIMPLEMENTED) {
      if (message.empty()) {
        throw std::runtime_error("Not implemented.");
      }
      throw std::runtime_error(message);
    }
    if (status == QDMI_ERROR_LIBNOTFOUND) {
      if (message.empty()) {
        throw std::runtime_error("Library not found.");
      }
      throw std::runtime_error(message);
    }
    if (status == QDMI_ERROR_NOTFOUND) {
      if (message.empty()) {
        throw std::runtime_error("Element not found.");
      }
      throw std::runtime_error(message);
    }
    if (status == QDMI_ERROR_OUTOFRANGE) {
      if (message.empty()) {
        throw std::out_of_range("Out of range.");
      }
      throw std::out_of_range(message);
    }
    if (status == QDMI_ERROR_INVALIDARGUMENT) {
      if (message.empty()) {
        throw std::invalid_argument("Invalid argument.");
      }
      throw std::invalid_argument(message);
    }
    if (status == QDMI_ERROR_PERMISSIONDENIED) {
      if (message.empty()) {
        throw std::runtime_error("Permission denied.");
      }
      throw std::runtime_error(message);
    }
    if (status == QDMI_ERROR_NOTSUPPORTED) {
      if (message.empty()) {
        throw std::runtime_error("Operation is not supported.");
      }
      throw std::runtime_error(message);
    }
  }
}

auto FoMaC::get_qubits_num() const -> int {
  int num_qubits = 0;
  const int ret =
      QDMI_query_device_property(device, QDMI_DEVICE_PROPERTY_QUBITSNUM,
                                 sizeof(int), &num_qubits, nullptr);
  throw_if_error(ret, "Failed to query the number of qubits.");
  return num_qubits;
}

auto FoMaC::get_operation_map() const -> std::map<std::string, QDMI_Operation> {
  int ops_num = 0;
  int ret = QDMI_query_get_operations(device, 0, nullptr, &ops_num);
  throw_if_error(ret, "Failed to retrieve operation number.");
  std::vector<QDMI_Operation> ops(ops_num);
  ret = QDMI_query_get_operations(
      device,
      static_cast<int>(sizeof(QDMI_Operation) * static_cast<size_t>(ops_num)),
      ops.data(), nullptr);
  throw_if_error(ret, "Failed to retrieve operations.");
  std::map<std::string, QDMI_Operation> ops_map;
  for (const auto &op : ops) {
    int name_length = 0;
    ret = QDMI_query_operation_property(device, op, 0, nullptr,
                                        QDMI_OPERATION_PROPERTY_NAME, 0,
                                        nullptr, &name_length);
    throw_if_error(ret, "Failed to retrieve operation name length.");
    std::string name(name_length, '\0');
    ret = QDMI_query_operation_property(device, op, 0, nullptr,
                                        QDMI_OPERATION_PROPERTY_NAME,
                                        name_length, name.data(), nullptr);
    throw_if_error(ret, "Failed to retrieve operation name.");
    ops_map.emplace(name, op);
  }
  return ops_map;
}

auto FoMaC::get_coupling_map() const
    -> std::vector<std::pair<QDMI_Site, QDMI_Site>> {
  int size = 0;
  int ret = QDMI_query_device_property(device, QDMI_DEVICE_PROPERTY_COUPLINGMAP,
                                       0, nullptr, &size);
  throw_if_error(ret, "Failed to query the coupling map size.");

  const auto coupling_map_size =
      static_cast<std::size_t>(size) / sizeof(QDMI_Site);
  if (coupling_map_size % 2 != 0) {
    throw std::runtime_error("The coupling map needs to have an even number of "
                             "elements.");
  }

  // `std::vector` guarantees that the elements are contiguous in memory.
  std::vector<std::pair<QDMI_Site, QDMI_Site>> coupling_pairs(
      coupling_map_size / 2);
  ret = QDMI_query_device_property(
      device, QDMI_DEVICE_PROPERTY_COUPLINGMAP, size,
      static_cast<void *>(coupling_pairs.data()), nullptr);
  throw_if_error(ret, "Failed to query the coupling map.");
  return coupling_pairs;
}

auto FoMaC::get_sites() const -> std::vector<QDMI_Site> {
  int sites_num = 0;
  int ret = QDMI_query_get_sites(device, 0, nullptr, &sites_num);
  throw_if_error(ret, "Failed to get the sites number.");
  std::vector<QDMI_Site> sites(static_cast<std::size_t>(sites_num));
  ret = QDMI_query_get_sites(device, sites_num, sites.data(), nullptr);
  throw_if_error(ret, "Failed to get the sites.");
  return sites;
}

int FoMaC::get_operands_num(const QDMI_Operation &op) const {
  int operands_num = 0;
  const int ret = QDMI_query_operation_property(
      device, op, 0, nullptr, QDMI_OPERATION_PROPERTY_QUBITSNUM, sizeof(int),
      &operands_num, nullptr);
  throw_if_error(ret, "Failed to query the operand number");
  return operands_num;
}
