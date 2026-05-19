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
// ConfigReader malformed-JSON tests
//
// Both SHM_MAPPING_JSON and SHM_NAMESPACE_CFG_JSON point to the same
// malformed fixture file (valid path, invalid JSON content). The JSON::parse
// call returns a discarded value, triggering the parse-error branches.
//
// Covers:
//   loadNamespaceConfig  lines 46,48-51  (malformed JSON)
//   loadSHMMappingConfig lines 74,76-79  (malformed JSON)
// =========================================================================

TEST(ConfigJsonReaderMalformedTest, LoadNamespaceConfig_MalformedJSON_Throws)
{
    // File exists but JSON is invalid → parse fails → throws runtime_error
    EXPECT_THROW(ConfigReader::loadNamespaceConfig(), std::runtime_error);
}

TEST(ConfigJsonReaderMalformedTest, LoadSHMMappingConfig_MalformedJSON_Throws)
{
    // File exists but JSON is invalid → parse fails → throws runtime_error
    EXPECT_THROW(ConfigReader::loadSHMMappingConfig(), std::runtime_error);
}
