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

#include "impl/shm_sensormap_intf.hpp"

#include <gtest/gtest.h>

using namespace nv::shmem;

// =========================================================================
// ShmSensorMapIntf tests
//
// Each TEST_F gets a fresh ShmSensorMapIntf whose internal sensor_map is
// empty. The shared-memory segment ("shmintftest_0") persists across tests
// (boost open_or_create semantics) but the internal std::unordered_map that
// tracks created namespaces is reset per fixture instance.
// =========================================================================

static constexpr const char* kNs = "shmintftest_0";
static constexpr size_t kSize = 1024 * 100; // 100 KB

class ShmSensorMapIntfTest : public ::testing::Test
{
  protected:
    ShmSensorMapIntf intf;
};

// -------------------------------------------------------------------------
// isNameSpacePresent
// -------------------------------------------------------------------------

TEST_F(ShmSensorMapIntfTest, IsNameSpacePresent_BeforeCreate_ReturnsFalse)
{
    // Fresh intf — nothing in internal map → false
    EXPECT_FALSE(intf.isNameSpacePresent(kNs));
}

TEST_F(ShmSensorMapIntfTest, IsNameSpacePresent_AfterCreate_ReturnsTrue)
{
    // After createNamespace adds to internal map → true
    EXPECT_TRUE(intf.createNamespace(kNs, kSize));
    EXPECT_TRUE(intf.isNameSpacePresent(kNs));
}

// -------------------------------------------------------------------------
// createNamespace
// -------------------------------------------------------------------------

TEST_F(ShmSensorMapIntfTest, CreateNamespace_ValidArgs_ReturnsTrue)
{
    // Happy path: creates (or opens existing) SHM segment → true
    EXPECT_TRUE(intf.createNamespace(kNs, kSize));
}

// -------------------------------------------------------------------------
// insert
// -------------------------------------------------------------------------

TEST_F(ShmSensorMapIntfTest, Insert_UnknownNamespace_ReturnsFalse)
{
    // Namespace not in internal map → error log + false
    SensorValue val("10", "/redfish/v1/test", 0, "1/1/2022");
    EXPECT_FALSE(intf.insert("unknown_ns_xyz", "key", val));
}

TEST_F(ShmSensorMapIntfTest, Insert_KnownNamespace_ReturnsTrue)
{
    // Namespace in internal map → insert into SHM → true
    intf.createNamespace(kNs, kSize);
    SensorValue val("42", "/redfish/v1/test", 1000, "1/1/2022");
    EXPECT_TRUE(intf.insert(kNs, "sensor_key_insert", val));
}

// -------------------------------------------------------------------------
// updateValueAndTimeStamp
// -------------------------------------------------------------------------

TEST_F(ShmSensorMapIntfTest,
       UpdateValueAndTimeStamp_UnknownNamespace_ReturnsFalse)
{
    // Namespace not in internal map → false
    EXPECT_FALSE(
        intf.updateValueAndTimeStamp("unknown_ns_xyz", "k", "v", 0, "t"));
}

TEST_F(ShmSensorMapIntfTest,
       UpdateValueAndTimeStamp_KnownNamespace_KeyNotFound_ReturnsFalse)
{
    // Namespace present but key not in SHM map → false
    intf.createNamespace(kNs, kSize);
    EXPECT_FALSE(
        intf.updateValueAndTimeStamp(kNs, "nonexistent_key", "v", 0, "t"));
}

TEST_F(ShmSensorMapIntfTest,
       UpdateValueAndTimeStamp_KnownNamespace_KeyFound_ReturnsTrue)
{
    // Insert key then update → true
    intf.createNamespace(kNs, kSize);
    SensorValue val("10", "/redfish/v1/test", 1000, "1/1/2022");
    intf.insert(kNs, "sensor_key_vts", val);
    EXPECT_TRUE(intf.updateValueAndTimeStamp(kNs, "sensor_key_vts", "99", 2000,
                                             "2/2/2023"));
}

// -------------------------------------------------------------------------
// erase
// -------------------------------------------------------------------------

TEST_F(ShmSensorMapIntfTest, Erase_UnknownNamespace_ReturnsFalse)
{
    // Namespace not in internal map → error log + false
    EXPECT_FALSE(intf.erase("unknown_ns_xyz", "key"));
}

TEST_F(ShmSensorMapIntfTest, Erase_KnownNamespace_ReturnsTrue)
{
    // Insert key then erase it → true
    intf.createNamespace(kNs, kSize);
    SensorValue val("42", "/redfish/v1/test", 1000, "1/1/2022");
    intf.insert(kNs, "sensor_key_erase", val);
    EXPECT_TRUE(intf.erase(kNs, "sensor_key_erase"));
}
