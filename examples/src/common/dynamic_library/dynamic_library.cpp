/*
 * Copyright (c) 2024 - 2026 QDMI Maintainers
 * All rights reserved.
 *
 * Licensed under the Apache License v2.0 with LLVM Exceptions (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://llvm.org/LICENSE.txt
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

/// @file
/// Implementation of the @ref qdmi::Dylib RAII dynamic library wrapper.
///
/// Provides portable wrappers around platform-specific dynamic library
/// functions. On Windows, `LoadLibraryA`, `GetProcAddress`, and `FreeLibrary`
/// are used. On POSIX systems, `dlopen`, `dlsym`, and `dlclose` are used.

#include "common/dynamic_library/dynamic_library.hpp"

#include <stdexcept>
#include <string>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif // _WIN32

/// @defgroup DylibPlatformMacros Portable dynamic library macros
/// @{
/// Opens a dynamic library by path. Returns a handle on success, or null on
/// failure.
#ifdef _WIN32
#define DL_OPEN(lib) LoadLibraryA((lib))
/// Resolves a symbol by name from an open library handle. Returns a void
/// pointer to the symbol, or null if not found. The result is cast to
/// `void *` for portability.
#define DL_SYM(lib, sym)                                                       \
  reinterpret_cast<void *>(GetProcAddress(static_cast<HMODULE>((lib)), (sym)))
/// @brief Closes a previously opened library handle.
#define DL_CLOSE(lib) FreeLibrary(static_cast<HMODULE>((lib)))
#else
#define DL_OPEN(lib) dlopen((lib), RTLD_NOW | RTLD_LOCAL)
#define DL_SYM(lib, sym) dlsym((lib), (sym))
#define DL_CLOSE(lib) dlclose((lib))
#endif
/// @}

namespace qdmi {

Dylib::Dylib(std::string path)
    : path_{std::move(path)}, handle_{DL_OPEN(path_.c_str())} {
  if (handle_ == nullptr) {
    throw std::runtime_error("Couldn't open the dynamic library: " + path_);
  }
}
Dylib::Dylib(const Dylib &other)
    : path_{other.path_}, handle_{DL_OPEN(path_.c_str())} {
  if (handle_ == nullptr) {
    throw std::runtime_error("Couldn't open the dynamic library: " + path_);
  }
}
Dylib &Dylib::operator=(const Dylib &other) {
  if (this != &other) {
    Dylib tmp(other);
    swap(tmp);
  }
  return *this;
}
Dylib::Dylib(Dylib &&other) noexcept
    : path_{std::move(other.path_)}, handle_{other.handle_} {
  other.handle_ = nullptr;
}
Dylib &Dylib::operator=(Dylib &&other) noexcept {
  if (this != &other) {
    Dylib tmp{std::move(other)};
    swap(tmp);
  }
  return *this;
}
Dylib::~Dylib() {
  if (handle_ != nullptr) {
    DL_CLOSE(handle_);
  }
}
auto Dylib::swap(Dylib &other) noexcept -> void {
  std::swap(path_, other.path_);
  std::swap(handle_, other.handle_);
}
auto Dylib::lookup_symbol(const std::string &name) const -> void * {
  return DL_SYM(handle_, name.c_str());
}
} // namespace qdmi

#undef DL_CLOSE
#undef DL_SYM
#undef DL_OPEN
