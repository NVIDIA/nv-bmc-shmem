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
// ConfigReader no-Producers-key tests
//
// SHM_MAPPING_JSON → mapping_no_producers.json which has a "Namespaces" entry
// ("TestNamespace") that deliberately omits the "Producers" key.
//
// Covers:
//   getSHMSize lines 198-203 (namespace found, but no "Producers" key → throws)
// =========================================================================

TEST(ConfigJsonReaderNoProducersTest, LoadSHMMappingConfig_Succeeds)
{
    EXPECT_NO_THROW(ConfigReader::loadSHMMappingConfig());
}

TEST(ConfigJsonReaderNoProducersTest,
     GetSHMSize_NamespaceHasNoProducersKey_Throws)
{
    // "TestNamespace" exists in "Namespaces" but has no "Producers" key
    // → getSHMSize throws runtime_error (lines 198-203)
    EXPECT_THROW(ConfigReader::getSHMSize("TestNamespace", "any_producer"),
                 std::runtime_error);
}
