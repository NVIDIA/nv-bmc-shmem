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

#include "utils/port_utils.hpp"

#include <gtest/gtest.h>

using namespace nv::sensor_aggregation::metricUtils;

// =========================================================================
// getLinkStatusType
// =========================================================================

TEST(PortUtilsTest, getLinkStatusType_KnownLinkDown_ReturnsLinkDown)
{
    auto result = getLinkStatusType(
        "xyz.openbmc_project.Inventory.Decorator.PortState.LinkStatusType."
        "LinkDown");
    EXPECT_EQ(result, "LinkDown");
}

TEST(PortUtilsTest, getLinkStatusType_KnownLinkUp_ReturnsLinkUp)
{
    auto result = getLinkStatusType(
        "xyz.openbmc_project.Inventory.Decorator.PortState.LinkStatusType."
        "LinkUp");
    EXPECT_EQ(result, "LinkUp");
}

TEST(PortUtilsTest, getLinkStatusType_Unknown_ReturnsEmpty)
{
    auto result = getLinkStatusType("xyz.unknown.Status");
    EXPECT_TRUE(result.empty());
}

// =========================================================================
// getLinkStateType
// =========================================================================

TEST(PortUtilsTest, getLinkStateType_KnownEnabled_ReturnsEnabled)
{
    auto result = getLinkStateType(
        "xyz.openbmc_project.Inventory.Decorator.PortState.LinkStates."
        "Enabled");
    EXPECT_EQ(result, "Enabled");
}

TEST(PortUtilsTest, getLinkStateType_KnownDisabled_ReturnsDisabled)
{
    auto result = getLinkStateType(
        "xyz.openbmc_project.Inventory.Decorator.PortState.LinkStates."
        "Disabled");
    EXPECT_EQ(result, "Disabled");
}

TEST(PortUtilsTest, getLinkStateType_Unknown_ReturnsEmpty)
{
    auto result = getLinkStateType("xyz.unknown.State");
    EXPECT_TRUE(result.empty());
}

// =========================================================================
// getPowerSystemInputType
// =========================================================================

TEST(PortUtilsTest, getPowerSystemInputType_Good_ReturnsNormal)
{
    auto result = getPowerSystemInputType(
        "xyz.openbmc_project.State.Decorator.PowerSystemInputs.Status.Good");
    EXPECT_EQ(result, "Normal");
}

TEST(PortUtilsTest, getPowerSystemInputType_Fault_ReturnsFault)
{
    auto result = getPowerSystemInputType(
        "xyz.openbmc_project.State.Decorator.PowerSystemInputs.Status.Fault");
    EXPECT_EQ(result, "Fault");
}

TEST(PortUtilsTest, getPowerSystemInputType_Unknown_ReturnsEmpty)
{
    auto result = getPowerSystemInputType("xyz.unknown.Input");
    EXPECT_TRUE(result.empty());
}
