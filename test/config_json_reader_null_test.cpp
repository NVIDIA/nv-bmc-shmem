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

#include "config.h"

#include "impl/config_json_reader.hpp"

#include <gtest/gtest.h>

using namespace nv::shmem;

// =========================================================================
// ConfigReader null-state tests
//
// This binary NEVER calls loadSHMMappingConfig() or loadNamespaceConfig().
// Therefore shmMappingJson and namespaceCfgJson start (and stay) as nullptr,
// exercising the null-check error paths that are unreachable in the main
// config_json_reader_test binary (where load is called first).
// =========================================================================

TEST(ConfigJsonReaderNullTest, GetProducers_JsonNotLoaded_Throws)
{
    // shmMappingJson == nullptr → runtime_error
    EXPECT_THROW(ConfigReader::getProducers(), std::runtime_error);
}

TEST(ConfigJsonReaderNullTest, GetNameSpaceConfiguration_JsonNotLoaded_Throws)
{
    // namespaceCfgJson == nullptr → runtime_error
    EXPECT_THROW(ConfigReader::getNameSpaceConfiguration(), std::runtime_error);
}

TEST(ConfigJsonReaderNullTest, GetSHMSize_JsonNotLoaded_Throws)
{
    // shmMappingJson == nullptr → runtime_error
    EXPECT_THROW(ConfigReader::getSHMSize("any_ns", "any_producer"),
                 std::runtime_error);
}
