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
// ConfigReader no-Namespaces-key and invalid-namespace-entry tests
//
// SHM_MAPPING_JSON → mapping_no_namespaces.json (valid JSON, no "Namespaces")
// SHM_NAMESPACE_CFG_JSON → namespace_invalid_entry.json (has "SensorNamespaces"
//   with a valid entry and two entries missing required fields)
//
// Covers:
//   getProducers            lines 106-110   (no Namespaces key → throws)
//   getNameSpaceConfiguration lines 153-155,157 (invalid entry → LOG_ERROR)
//   getSHMSize              lines 215-219   (no Namespaces key → throws)
//   getMRDNamespaceLookup   lines 239-244   (inner catch when getProducers
//   throws)
// =========================================================================

TEST(ConfigJsonReaderNoNsTest, LoadSHMMappingConfig_NoNamespacesJson_Succeeds)
{
    // Valid JSON file but without "Namespaces" key — load succeeds
    EXPECT_NO_THROW(ConfigReader::loadSHMMappingConfig());
}

TEST(ConfigJsonReaderNoNsTest, LoadNamespaceConfig_InvalidEntries_Succeeds)
{
    // Valid JSON file with "SensorNamespaces" containing invalid entries — load
    // succeeds
    EXPECT_NO_THROW(ConfigReader::loadNamespaceConfig());
}

TEST(ConfigJsonReaderNoNsTest, GetProducers_NoNamespacesKey_Throws)
{
    // shmMappingJson has no "Namespaces" key → throws runtime_error (lines
    // 106-110)
    EXPECT_THROW(ConfigReader::getProducers(), std::runtime_error);
}

TEST(ConfigJsonReaderNoNsTest,
     GetNameSpaceConfiguration_InvalidEntries_ReturnsPartialResult)
{
    // Invalid entries are skipped with LOG_ERROR (lines 153-155, 157);
    // the one valid entry ("ValidNamespace") is returned.
    auto result = ConfigReader::getNameSpaceConfiguration();
    EXPECT_FALSE(result.empty());
}

TEST(ConfigJsonReaderNoNsTest, GetSHMSize_NoNamespacesKey_Throws)
{
    // shmMappingJson has no "Namespaces" key → throws runtime_error (lines
    // 215-219)
    EXPECT_THROW(ConfigReader::getSHMSize("TestNs", "TestProd"),
                 std::runtime_error);
}

TEST(ConfigJsonReaderNoNsTest,
     GetMRDNamespaceLookup_GetProducersThrows_ReturnsEmpty)
{
    // shmMappingJson loaded but getProducers throws → inner catch (lines
    // 239-244) → getMRDNamespaceLookup returns empty map
    auto result = ConfigReader::getMRDNamespaceLookup();
    EXPECT_TRUE(result.empty());
}
