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

#include "impl/shmem_map.hpp"

#include <memory>

#include "gmock/gmock.h"
#include <gtest/gtest.h>

using namespace nv::shmem;
using namespace ::testing;

class SensorMapExtraTests : public testing::Test
{
  public:
    SensorMapExtraTests()
    {
        mShmem = std::make_unique<Map<SensorMap, SensorValue>>(
            "maptest_extra", O_CREAT, 1024 * 1000);
        mShmem->clear();
    }
    std::unique_ptr<Map<SensorMap, SensorValue>> mShmem;
};

TEST_F(SensorMapExtraTests, GetAllKeyValuePair)
{
    SensorValue val("42.0", "/redfish/prop", 1000, "2022-01-01T00:00:00");
    mShmem->insert("Device_0/sensor", val);

    auto pairs = mShmem->getAllKeyValuePair();
    ASSERT_EQ(pairs.size(), 1);
    EXPECT_EQ(pairs.at("Device_0/sensor"), "42.0");
}

TEST_F(SensorMapExtraTests, GetAllKeyValuePairEmpty)
{
    auto pairs = mShmem->getAllKeyValuePair();
    EXPECT_TRUE(pairs.empty());
}

TEST_F(SensorMapExtraTests, UpdateValueAndTimestampSuccess)
{
    SensorValue val("10", "/prop", 100, "ts1");
    mShmem->insert("Device_0/upd", val);

    EXPECT_TRUE(
        mShmem->updateValueAndTimeStamp("Device_0/upd", "20", 200, "ts2"));

    SensorValue result;
    ASSERT_TRUE(mShmem->getValue("Device_0/upd", result));
    EXPECT_EQ(result.sensorValue, "20");
    EXPECT_EQ(result.timestamp, 200u);
    EXPECT_EQ(result.timestampStr, "ts2");
}

TEST_F(SensorMapExtraTests, UpdateValueAndTimestampKeyNotFound)
{
    EXPECT_FALSE(mShmem->updateValueAndTimeStamp("no_such_key", "x", 0, "ts"));
}

TEST_F(SensorMapExtraTests, UpdateValueAndTimestampReadOnlyThrows)
{
    auto rdOnly = std::make_unique<Map<SensorMap, SensorValue>>("maptest_extra",
                                                                O_RDONLY);
    EXPECT_THROW(rdOnly->updateValueAndTimeStamp("any", "v", 0, "ts"),
                 std::runtime_error);
}

TEST_F(SensorMapExtraTests, UpdateValueKeyNotFound)
{
    EXPECT_FALSE(mShmem->updateValue("nonexistent_key", "val"));
}

TEST_F(SensorMapExtraTests, UpdateTimestampKeyNotFound)
{
    EXPECT_FALSE(mShmem->updateTimestamp("nonexistent_key", 1234, "ts"));
}

TEST_F(SensorMapExtraTests, UpdateValueReadOnlyThrows)
{
    auto rdOnly = std::make_unique<Map<SensorMap, SensorValue>>("maptest_extra",
                                                                O_RDONLY);
    EXPECT_THROW(rdOnly->updateValue("key", "val"), std::runtime_error);
}

TEST_F(SensorMapExtraTests, UpdateTimestampReadOnlyThrows)
{
    auto rdOnly = std::make_unique<Map<SensorMap, SensorValue>>("maptest_extra",
                                                                O_RDONLY);
    EXPECT_THROW(rdOnly->updateTimestamp("key", 0, "ts"), std::runtime_error);
}

TEST_F(SensorMapExtraTests, EraseReadOnlyThrows)
{
    auto rdOnly = std::make_unique<Map<SensorMap, SensorValue>>("maptest_extra",
                                                                O_RDONLY);
    EXPECT_THROW(rdOnly->erase("key"), std::runtime_error);
}

TEST_F(SensorMapExtraTests, ClearReadOnlyThrows)
{
    auto rdOnly = std::make_unique<Map<SensorMap, SensorValue>>("maptest_extra",
                                                                O_RDONLY);
    EXPECT_THROW(rdOnly->clear(), std::runtime_error);
}

TEST_F(SensorMapExtraTests, GetFreeSize)
{
    EXPECT_GT(mShmem->getFreeSize(), 0u);
}

TEST_F(SensorMapExtraTests, GetValueMissing)
{
    SensorValue result;
    EXPECT_FALSE(mShmem->getValue("totally_missing_key", result));
}

TEST(SensorMapOpenTest, OpenReadOnlyNonExistentThrows)
{
    EXPECT_THROW(
        (Map<SensorMap, SensorValue>("does_not_exist_ns_xyz", O_RDONLY)),
        std::exception);
}
