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
// ConfigReader tests
//
// The JSON config files installed by the package are present in the test
// environment (SHM_MAPPING_JSON, SHM_NAMESPACE_CFG_JSON).
// Tests cover load, parse, and key-not-found error paths.
// ConfigReader uses static members — state is shared across all tests in
// this binary. Tests are ordered so that load happens before get calls.
// =========================================================================

TEST(ConfigJsonReaderTest, LoadSHMMappingConfig_DoesNotThrow)
{
    EXPECT_NO_THROW(ConfigReader::loadSHMMappingConfig());
}

TEST(ConfigJsonReaderTest, LoadSHMMappingConfig_SecondCall_DoesNotThrow)
{
    // Early-return path: shmMappingJson already loaded → returns immediately
    EXPECT_NO_THROW(ConfigReader::loadSHMMappingConfig());
}

TEST(ConfigJsonReaderTest, LoadNamespaceConfig_DoesNotThrow)
{
    EXPECT_NO_THROW(ConfigReader::loadNamespaceConfig());
}

TEST(ConfigJsonReaderTest, LoadNamespaceConfig_SecondCall_DoesNotThrow)
{
    // Early-return path: namespaceCfgJson already loaded → returns immediately
    EXPECT_NO_THROW(ConfigReader::loadNamespaceConfig());
}

TEST(ConfigJsonReaderTest, GetProducers_AfterLoad_DoesNotThrow)
{
    EXPECT_NO_THROW(ConfigReader::getProducers());
}

TEST(ConfigJsonReaderTest, GetNameSpaceConfiguration_AfterLoad_DoesNotThrow)
{
    EXPECT_NO_THROW(ConfigReader::getNameSpaceConfiguration());
}

TEST(ConfigJsonReaderTest, GetSHMSize_UnknownNamespace_Throws)
{
    // Namespace not in config → runtime_error("Namespace not found")
    EXPECT_THROW(
        ConfigReader::getSHMSize("nonexistent_namespace_xyz", "any_producer"),
        std::runtime_error);
}

TEST(ConfigJsonReaderTest, GetSHMSize_ValidNamespaceAndProducer_ReturnsSize)
{
    // Use a real namespace and its first producer from config → returns size >
    // 0
    auto producers = ConfigReader::getProducers();
    ASSERT_FALSE(producers.empty());
    const string& ns = producers.begin()->first;
    ASSERT_FALSE(producers.begin()->second.empty());
    const string& prod = producers.begin()->second.front();
    size_t sz = 0;
    EXPECT_NO_THROW(sz = ConfigReader::getSHMSize(ns, prod));
    EXPECT_GT(sz, 0u);
}

TEST(ConfigJsonReaderTest, GetSHMSize_ValidNamespaceInvalidProducer_Throws)
{
    // Namespace exists but producer not in its list → runtime_error
    auto producers = ConfigReader::getProducers();
    ASSERT_FALSE(producers.empty());
    const string& ns = producers.begin()->first;
    EXPECT_THROW(ConfigReader::getSHMSize(ns, "nonexistent_producer_xyz_abc"),
                 std::runtime_error);
}

TEST(ConfigJsonReaderTest, GetMRDNamespaceLookup_ReturnsMap)
{
    // Config is present → returns populated MRD namespace lookup
    auto result = ConfigReader::getMRDNamespaceLookup();
    EXPECT_FALSE(result.empty());
}
