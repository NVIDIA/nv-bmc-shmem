/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION &
 * AFFILIATES. All rights reserved. SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Test stub for phosphor-logging/lg2.hpp.
//
// The real lg2 defines log_convert<std::signed_integral V> and
// log_convert<has_to_string V> overloads. When metric_report_utils.hpp is
// included (which has "using namespace std"), has_to_string<int> becomes true,
// making both overloads candidates for int — an ambiguous call.
//
// This stub replaces lg2 with no-op types that accept any arguments.
// Placed in test/stubs/ which is first in the include path, it shadows
// /usr/local/include/phosphor-logging/lg2.hpp for this test binary only.

#pragma once

namespace lg2
{

// Each log level is a struct whose constructor accepts any arguments and
// discards them — suitable for tests that only care about return values.
#define LG2_STUB_LEVEL(nm)                                                     \
    struct nm                                                                  \
    {                                                                          \
        template <typename... Ts>                                              \
        explicit nm(const char* /*msg*/, Ts&&... /*args*/)                     \
        {}                                                                     \
    };

LG2_STUB_LEVEL(critical)
LG2_STUB_LEVEL(error)
LG2_STUB_LEVEL(warning)
LG2_STUB_LEVEL(notice)
LG2_STUB_LEVEL(info)
LG2_STUB_LEVEL(debug)

#undef LG2_STUB_LEVEL

} // namespace lg2

// The real lg2.hpp declares each level via this macro at file scope.
// Define it as a no-op since the levels are already declared above.
#define PHOSPHOR_LOG2_DECLARE_LEVEL(level) // no-op — already defined
