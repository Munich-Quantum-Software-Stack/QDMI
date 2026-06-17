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
/// Implementation of the @ref qdmi::LibraryWrapper RAII wrapper for @ref
/// QDMI_Library "QDMI device libraries".

#pragma once

#include "common/dynamic_library/dynamic_library.hpp"
#include "qdmi/core.h"

#include <string>

namespace qdmi {
/// An RAII wrapper that loads a QDMI device library and manages its lifecycle.
///
/// On construction, the wrapper resolves the device's @ref QDMI_initialize
/// "`<prefix>_QDMI_initialize`" entry point from the provided dynamic library,
/// calls it to obtain a @ref QDMI_Context and a @ref QDMI_Library handle, and
/// immediately retrieves the @ref QDMI_Core_Interface from that library handle.
/// On destruction, the context is finalized via @ref QDMI_context_finalize.
///
/// Copying re-initializes the library from scratch using the same dynamic
/// library and prefix, so each copy owns an independent context. Moving
/// transfers ownership without re-initialization.
class LibraryWrapper final {
public:
  /// Constructs an empty LibraryWrapper with no loaded library.
  /// The context and interface pointers will be null.
  LibraryWrapper() = default;

  /// Constructs a LibraryWrapper by initializing the device library.
  ///
  /// Looks up the symbol @ref QDMI_initialize "`<prefix>_QDMI_initialize`" in
  /// the given dynamic library, invokes it to obtain a @ref QDMI_Context and a
  /// @ref QDMI_Library handle, then calls
  /// @ref QDMI_get_interface to retrieve the @ref QDMI_Core_Interface.
  ///
  /// @param dylib is the already-loaded dynamic library containing the device
  /// implementation.
  /// @param callback is an optional logging callback to use for logging
  /// messages from the library. If not provided, no logging will occur.
  /// @param user_data is an optional pointer to user data to pass to the
  /// logging callback when logging messages. If not provided, @c NULL will be
  /// passed to the callback.
  ///
  /// @throws std::runtime_error if the device cannot be initialized or the
  /// interface cannot be retrieved.
  explicit LibraryWrapper(Dylib dylib, QDMI_Log_Callback callback = nullptr,
                          void *user_data = nullptr);

  LibraryWrapper(const LibraryWrapper &other);
  LibraryWrapper &operator=(const LibraryWrapper &other);
  LibraryWrapper(LibraryWrapper &&other) noexcept;
  LibraryWrapper &operator=(LibraryWrapper &&other) noexcept;
  ~LibraryWrapper();

  /// @returns the @ref QDMI_Library handle obtained during initialization.
  /// If the library was not successfully initialized or moved from, it returns
  /// @c NULL.
  [[nodiscard]] auto get() const -> const QDMI_Library *;

  /// @returns the @ref QDMI_Context obtained during initialization.
  /// If the library was not successfully initialized or moved from, it returns
  /// @c NULL.
  [[nodiscard]] auto context() const -> QDMI_Context;

  /// Updates the logging callback and user data for this library wrapper.
  ///
  /// This will update the logging callback used for any future logging messages
  /// from the library. If the library was not successfully initialized or moved
  /// from, this function will have no effect.
  auto set_logging_callback(QDMI_Log_Callback callback, void *user_data)
      -> void;

private:
  LibraryWrapper(Dylib dylib, std::string prefix,
                 QDMI_Log_Callback callback = nullptr,
                 void *user_data = nullptr);
  auto init_prefix() -> void;
  auto init_context_and_library() -> void;
  /// Swaps the contents of this instance with @p other.
  ///
  /// Exchanges all internal states (dynamic library handle, prefix, context,
  /// library, and interface pointers) with @p other. Used to implement
  /// copy-and-swap and move-and-swap idioms.
  ///
  /// @param other is the LibraryWrapper to swap with.
  auto swap(LibraryWrapper &other) noexcept -> void;

  /// The logging callback to use for logging messages from the library.
  QDMI_Log_Callback callback_{};
  /// The user data to pass to the logging callback when logging messages.
  void *user_data_{};
  /// The loaded dynamic library containing the device implementation.
  Dylib dylib_{};
  /// The device-specific symbol prefix used to locate @ref QDMI_initialize.
  std::string prefix_{};
  /// The QDMI context handle obtained from @ref QDMI_initialize.
  QDMI_Context context_{};
  /// The QDMI library handle obtained from @ref QDMI_initialize.
  const QDMI_Library *library_{};
};
} // namespace qdmi
