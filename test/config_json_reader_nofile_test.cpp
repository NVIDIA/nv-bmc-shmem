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
// ConfigReader file-not-found tests
//
// This binary is compiled with SHM_MAPPING_JSON and SHM_NAMESPACE_CFG_JSON
// pointing to nonexistent paths. The static pointers therefore remain null
// throughout and every load attempt throws invalid_argument.
//
// Covers:
//   loadNamespaceConfig  lines 37-41  (file not found)
//   loadSHMMappingConfig lines 65-69  (file not found)
//   getMRDNamespaceLookup lines 246-251,253 (outer catch when load fails)
// =========================================================================

TEST(ConfigJsonReaderNofileTest, LoadNamespaceConfig_FileNotFound_Throws)
{
    // SHM_NAMESPACE_CFG_JSON does not exist → throws invalid_argument
    EXPECT_THROW(ConfigReader::loadNamespaceConfig(), std::invalid_argument);
}

TEST(ConfigJsonReaderNofileTest, LoadSHMMappingConfig_FileNotFound_Throws)
{
    // SHM_MAPPING_JSON does not exist → throws invalid_argument
    EXPECT_THROW(ConfigReader::loadSHMMappingConfig(), std::invalid_argument);
}

TEST(ConfigJsonReaderNofileTest,
     GetMRDNamespaceLookup_FileNotFound_ReturnsEmpty)
{
    // loadSHMMappingConfig throws internally → outer catch logs and returns
    // empty
    auto result = ConfigReader::getMRDNamespaceLookup();
    EXPECT_TRUE(result.empty());
}
