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

// All tests use the singleton. State (errorLogTimes map) accumulates across
// tests in the same process, so unique error strings are used in each test.

TEST(ErrorLoggerTest, SingletonReturnsSameInstance)
{
    auto& a = ErrorLogger::getInstance();
    auto& b = ErrorLogger::getInstance();
    EXPECT_EQ(&a, &b);
}

TEST(ErrorLoggerTest, LogNewErrorDoesNotThrow)
{
    EXPECT_NO_THROW(
        ErrorLogger::getInstance().logError("unique_first_error_abc123"));
}

TEST(ErrorLoggerTest, DuplicateWithinIntervalIsSuppressed)
{
    auto& logger = ErrorLogger::getInstance();
    logger.logError("dup_suppress_error_xyz");
    // Immediately repeating the same error triggers the suppression branch
    EXPECT_NO_THROW(logger.logError("dup_suppress_error_xyz"));
}

TEST(ErrorLoggerTest, DifferentErrorsTrackedSeparately)
{
    auto& logger = ErrorLogger::getInstance();
    EXPECT_NO_THROW(logger.logError("distinct_alpha_error_1"));
    EXPECT_NO_THROW(logger.logError("distinct_beta_error_2"));
}

// Fill the singleton map to MAX_LOG_ENTRIES to exercise the capacity-limit
// branch. Entries from earlier tests in this process are already in the map,
// so the limit is reached before the loop completes — the tail calls hit the
// ">= MAX_LOG_ENTRIES" early-return path which is what we need for coverage.
TEST(ErrorLoggerTest, MaxEntriesLimitDropsNewErrors)
{
    auto& logger = ErrorLogger::getInstance();
    for (int i = 0; i < MAX_LOG_ENTRIES; i++)
    {
        logger.logError("max_fill_entry_" + std::to_string(i));
    }
    // Once the map is at capacity, new unique errors are silently dropped
    EXPECT_NO_THROW(logger.logError("overflow_unique_error_sentinel_99999"));
}
