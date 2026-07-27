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

#include "impl/error_logger.hpp"

#include "gmock/gmock.h"
#include <gtest/gtest.h>

using namespace ::testing;

// The ErrorLogger singleton accumulates suppression state across the whole test
// process. Each test uses unique message strings and only asserts outcomes that
// hold regardless of how full the map already is, so the tests stay independent
// of execution order (e.g. under --gtest_shuffle) without a reset hook.

TEST(ErrorLoggerTest, SingletonReturnsSameInstance)
{
    EXPECT_EQ(&ErrorLogger::getInstance(), &ErrorLogger::getInstance());
}

TEST(ErrorLoggerTest, LogNewErrorDoesNotThrow)
{
    EXPECT_NO_THROW(ErrorLogger::getInstance().logError("new_error_unique"));
}

TEST(ErrorLoggerTest, DuplicateWithinIntervalIsSuppressed)
{
    auto& logger = ErrorLogger::getInstance();
    // A second occurrence of the same message within the interval is never
    // re-emitted. This holds whether or not the first call was emitted, so it
    // does not depend on the singleton's prior fill state.
    logger.logError("dup_error_unique");
    EXPECT_FALSE(logger.logError("dup_error_unique"));
}

TEST(ErrorLoggerTest, DifferentErrorsTrackedSeparately)
{
    auto& logger = ErrorLogger::getInstance();
    EXPECT_NO_THROW(logger.logError("alpha_error_unique"));
    EXPECT_NO_THROW(logger.logError("beta_error_unique"));
}

TEST(ErrorLoggerTest, MaxEntriesLimitDropsNewErrors)
{
    auto& logger = ErrorLogger::getInstance();
    // Inserting MAX_LOG_ENTRIES unique messages fills the map to capacity
    // regardless of what earlier tests inserted.
    for (int i = 0; i < MAX_LOG_ENTRIES; i++)
    {
        logger.logError("max_fill_entry_" + std::to_string(i));
    }
    // At capacity a brand-new message is dropped, not emitted.
    EXPECT_FALSE(logger.logError("overflow_sentinel_unique"));
}
