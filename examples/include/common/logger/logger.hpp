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
/// Logger class template.

#pragma once
#include <format>

namespace qdmi {
/// A logger class template that can be used to log messages from the device.
///
/// @remark The logger is templated in all QDMI specific type to avoid including
/// the QDMI header here, which would mean that this header would need to link
/// against QDMI. Especially, for QPU implementations we want to avoid linking
/// against the (non-name shifted) QDMI.
///
/// @tparam Callback The type of the log callback function. This should be
/// `QDMI_Log_Callback` from the respectively included QDMI header file.
/// @tparam Log_Level The type of the log level. This should be `QDMI_Log_Level`
/// from the respectively included QDMI header file.
template <typename Callback, typename Log_Level> class Logger final {
public:
  Logger() = default;
  Logger(Callback callback, void *user_data)
      : callback_(callback), user_data_(user_data) {}
  [[nodiscard]] auto callback() const -> Callback { return callback_; }
  [[nodiscard]] auto user_data() const -> void * { return user_data_; }
  template <typename... Args>
  auto log(Log_Level level, std::format_string<Args...> fmt,
           Args &&...args) const -> void {
    if (callback_ != nullptr) {
      callback_(level, std::format(fmt, std::forward<Args>(args)...).c_str(),
                user_data_);
    }
  }

private:
  Callback callback_{};
  void *user_data_{};
};
} // namespace qdmi
