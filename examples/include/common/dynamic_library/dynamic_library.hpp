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
/// Implementation of a @ref qdmi::Dylib "simple RAII wrapper around a dynamic
/// library handle".

#pragma once

#include <functional>
#include <string>

namespace qdmi {
/// A simple RAII wrapper around a dynamic library handle.
///
/// Provides safe copying and moving semantics, as well as a convenient method
/// for symbol lookup.
class Dylib final {
public:
  /// Constructs an empty Dylib instance with no loaded library.
  ///
  /// The handle will be null.
  Dylib() = default;
  /// Constructs a Dylib instance by loading the dynamic library at the
  /// specified path.
  ///
  /// @param path is the filesystem path to the dynamic library to load.
  ///
  /// @throws std::runtime_error if the library cannot be loaded.
  explicit Dylib(std::string path);

  Dylib(const Dylib &other);
  Dylib &operator=(const Dylib &other);
  Dylib(Dylib &&other) noexcept;
  Dylib &operator=(Dylib &&other) noexcept;
  ~Dylib();

  /// Retrieves a symbol from the loaded dynamic library and casts it to
  /// the specified type.
  ///
  /// @tparam T the expected type of the symbol (e.g., a function pointer type).
  /// @param name is the name of the symbol to retrieve from the library.
  ///
  /// @returns a pointer to the symbol cast to the specified type. If the symbol
  /// cannot be found, @c NULL is returned.
  template <typename T>
  [[nodiscard]] auto get_symbol(const std::string &name) const -> T * {
    // note when instantiated, a static_cast is not enough and a
    // reinterpret_cast is required.

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    return reinterpret_cast<T *>(lookup_symbol(name));
  }

  /// Overload that deduces the function signature from a @c std::function
  /// target, avoiding the need to repeat the type at the call site.
  ///
  /// @tparam R return type (deduced)
  /// @tparam Args argument types (deduced)
  /// @param target the @c std::function variable to assign the symbol to.
  /// @param name is the name of the symbol to retrieve from the library.
  template <typename R, typename... Args>
  auto get_symbol(std::function<R(Args...)> &target,
                  const std::string &name) const -> void {
    target = get_symbol<R(Args...)>(name);
  }

private:
  /// Swaps the contents of this Dylib instance with another.
  ///
  /// This is a noexcept operation that simply exchanges the internal state
  /// (path and handle) of the two instances. It is used to implement the
  /// copy-and-swap idiom for the copy constructor and copy assignment operator,
  /// as well as the move constructor and move assignment operator.
  ///
  /// @param other is the Dylib instance to swap with. After this operation,
  /// this instance will contain the state of the other instance, and the other
  /// instance will contain the state of this instance.
  ///
  /// @noexcept This function is guaranteed not to throw exceptions.
  auto swap(Dylib &other) noexcept -> void;

  /// Looks up a symbol by name in the loaded dynamic library and returns a
  /// pointer to it.
  ///
  /// @param name is the name of the symbol to look up in the library.
  ///
  /// @returns a pointer to the symbol if found. If the symbol cannot be found,
  /// it returns @c NULL.
  [[nodiscard]] auto lookup_symbol(const std::string &name) const -> void *;

  /// The filesystem path to the loaded dynamic library.
  std::string path_{};
  /// The handle to the loaded dynamic library, obtained from @ref DL_OPEN.
  void *handle_{};
};
} // namespace qdmi
