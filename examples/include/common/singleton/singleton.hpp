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

#pragma once

namespace qdmi {
/// A CRTP base class that implements the Singleton design pattern.
///
/// Provides a single, lazily initialized instance of the derived class @p
/// Concrete. The instance is allocated on the heap and intentionally leaked to
/// avoid static deinitialization order issues (the "static deinitialization
/// order fiasco"). Access is provided through the static @ref Singleton::get
/// method.
///
/// @remark To create a singleton, inherit from this class by inheriting from
/// `public Singleton<Derived>` and declare `Singleton` as a friend so that
/// `Singleton` can invoke the protected default constructor of the derived
/// class.
///
/// @tparam Concrete is the concrete class that should be a singleton. It must
/// be default-constructible.
///
/// @note Copy and move operations are deleted to enforce the singleton
/// invariant and because users always access the instance through `get()`.
template <class Concrete> class Singleton {
protected:
  /// Protected default constructor to prevent direct instantiation. Only the
  /// `get()` factory method may create an instance.
  Singleton() = default;

public:
  // Delete move constructor and move assignment operator because of the
  // following reason:
  //
  // - Users access the singleton via get() which returns a reference
  // - Moving would invalidate the singleton's state
  // - Users never own the singleton instance
  Singleton(Singleton &&) = delete;
  Singleton &operator=(Singleton &&) = delete;
  // Delete copy constructor and assignment operator to enforce singleton.
  Singleton(const Singleton &) = delete;
  Singleton &operator=(const Singleton &) = delete;

  virtual ~Singleton() = default;

  /// Returns a reference to the single instance of @p Concrete.
  ///
  /// On the first call, allocates a new instance of @p Concrete on the heap.
  /// Subsequent calls return a reference to the same instance. The instance is
  /// intentionally never deleted to avoid the static deinitialization order
  /// fiasco.
  ///
  /// @returns a reference to the singleton instance of @p Concrete.
  [[nodiscard]] static auto get() -> Concrete & {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    static auto *instance = new Concrete();
    // The instance is intentionally leaked to avoid static deinitialization
    // issues (cf. static (de)initialization order fiasco)
    return *instance;
  }
};
} // namespace qdmi
