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
#include "utils/time_utils.hpp"

#include <chrono>
#include <optional>

#include <gtest/gtest.h>

using namespace std;
using namespace nv::sensor_aggregation::metricUtils;

// =========================================================================
// fromDurationString
// =========================================================================

TEST(TimeUtilsTest, fromDurationString_EmptyString_ReturnsZeroMs)
{
    auto result = fromDurationString("");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->count(), 0);
}

TEST(TimeUtilsTest, fromDurationString_NoPPrefix_ReturnsNullopt)
{
    auto result = fromDurationString("T5M");
    EXPECT_FALSE(result.has_value());
}

TEST(TimeUtilsTest, fromDurationString_ValidSeconds_Returns30000Ms)
{
    auto result = fromDurationString("PT30S");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->count(), 30000);
}

TEST(TimeUtilsTest, fromDurationString_ValidMinutes_Returns300000Ms)
{
    auto result = fromDurationString("PT5M");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->count(), 300000);
}

TEST(TimeUtilsTest, fromDurationString_ValidHoursMinutesSeconds)
{
    auto result = fromDurationString("PT1H1M1S");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->count(), (3600 + 60 + 1) * 1000LL);
}

TEST(TimeUtilsTest, fromDurationString_ValidDayOnly_Returns86400000Ms)
{
    auto result = fromDurationString("P1D");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->count(), 86400000LL);
}

TEST(TimeUtilsTest, fromDurationString_ValidZeroDays_ReturnsZero)
{
    auto result = fromDurationString("P0D");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->count(), 0);
}

TEST(TimeUtilsTest, fromDurationString_ValidDaysAndHours)
{
    auto result = fromDurationString("P1DT2H");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->count(), (86400 + 7200) * 1000LL);
}

TEST(TimeUtilsTest, fromDurationString_ValidFractionalSeconds_Returns1500Ms)
{
    // PT1.500S = 1 second + 500ms = 1500ms
    auto result = fromDurationString("PT1.500S");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->count(), 1500);
}

TEST(TimeUtilsTest, fromDurationString_InvalidMissingTBeforeTime_ReturnsNullopt)
{
    // Has days but time component lacks 'T' separator
    auto result = fromDurationString("P1D5H");
    EXPECT_FALSE(result.has_value());
}

// =========================================================================
// toDurationString
// =========================================================================

TEST(TimeUtilsTest, toDurationString_ZeroMs_ReturnsPT0S)
{
    auto result = toDurationString(chrono::milliseconds(0));
    EXPECT_EQ(result, "PT0S");
}

TEST(TimeUtilsTest, toDurationString_NegativeMs_ReturnsEmpty)
{
    auto result = toDurationString(chrono::milliseconds(-1));
    EXPECT_TRUE(result.empty());
}

TEST(TimeUtilsTest, toDurationString_OneSecond_ReturnsPT1Dot000S)
{
    auto result = toDurationString(chrono::milliseconds(1000));
    EXPECT_EQ(result, "PT1.000S");
}

TEST(TimeUtilsTest, toDurationString_FiveMinutes_ReturnsPT5M)
{
    auto result = toDurationString(chrono::milliseconds(300000));
    EXPECT_EQ(result, "PT5M");
}

TEST(TimeUtilsTest, toDurationString_1H1M1S)
{
    auto result =
        toDurationString(chrono::milliseconds((3600 + 60 + 1) * 1000LL));
    EXPECT_EQ(result, "PT1H1M1.000S");
}

TEST(TimeUtilsTest, toDurationString_OneDay_ReturnsP1DT)
{
    auto result = toDurationString(chrono::milliseconds(86400000LL));
    EXPECT_EQ(result, "P1DT");
}

TEST(TimeUtilsTest, toDurationString_OneDayAndOneHour)
{
    auto result =
        toDurationString(chrono::milliseconds(86400000LL + 3600000LL));
    EXPECT_EQ(result, "P1DT1H");
}

TEST(TimeUtilsTest, toDurationString_SubSecondOnly)
{
    // 500ms has no whole seconds but ms != 0
    auto result = toDurationString(chrono::milliseconds(500));
    EXPECT_EQ(result, "PT0.500S");
}

// =========================================================================
// toDurationStringFromUint
// =========================================================================

TEST(TimeUtilsTest, toDurationStringFromUint_Zero_ReturnsPT0S)
{
    auto result = toDurationStringFromUint(0);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, "PT0S");
}

TEST(TimeUtilsTest, toDurationStringFromUint_OneSecond_ReturnsPT1S)
{
    auto result = toDurationStringFromUint(1000);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, "PT1.000S");
}

TEST(TimeUtilsTest, toDurationStringFromUint_Overflow_ReturnsNullopt)
{
    uint64_t overflow =
        static_cast<uint64_t>(chrono::milliseconds::max().count()) + 1ULL;
    auto result = toDurationStringFromUint(overflow);
    EXPECT_FALSE(result.has_value());
}

// =========================================================================
// getDateTimeUint
// =========================================================================

TEST(TimeUtilsTest, getDateTimeUint_EpochZero_Returns1970Date)
{
    auto result = getDateTimeUint(0);
    EXPECT_EQ(result, "1970-01-01T00:00:00+00:00");
}

TEST(TimeUtilsTest, getDateTimeUint_KnownTimestamp)
{
    // 1699255438 seconds = 2023-11-06T07:23:58+00:00
    auto result = getDateTimeUint(1699255438ULL);
    EXPECT_EQ(result, "2023-11-06T07:23:58+00:00");
}

// =========================================================================
// getDateTimeUintMs
// =========================================================================

TEST(TimeUtilsTest, getDateTimeUintMs_KnownTimestampWithMs)
{
    // 1699255438000ms = 2023-11-06T07:23:58.000+00:00
    auto result = getDateTimeUintMs(1699255438000ULL);
    EXPECT_EQ(result, "2023-11-06T07:23:58.000+00:00");
}

TEST(TimeUtilsTest, getDateTimeUintMs_WithSubSecondMs)
{
    auto result = getDateTimeUintMs(1699255438500ULL);
    EXPECT_EQ(result, "2023-11-06T07:23:58.500+00:00");
}

// =========================================================================
// getDateTimeUintUs
// =========================================================================

TEST(TimeUtilsTest, getDateTimeUintUs_KnownTimestampWithUs)
{
    // 1699255438000000us = 2023-11-06T07:23:58.000000+00:00
    auto result = getDateTimeUintUs(1699255438000000ULL);
    EXPECT_EQ(result, "2023-11-06T07:23:58.000000+00:00");
}

// =========================================================================
// getTimestamp
// =========================================================================

TEST(TimeUtilsTest, getTimestamp_5000Ms_Returns5)
{
    auto result = getTimestamp(5000);
    EXPECT_EQ(result, 5);
}

TEST(TimeUtilsTest, getTimestamp_ZeroMs_ReturnsZero)
{
    auto result = getTimestamp(0);
    EXPECT_EQ(result, 0);
}

// =========================================================================
// nanoSecToDurationString
// =========================================================================

TEST(TimeUtilsTest, nanoSecToDurationString_Zero_ReturnsPT0S)
{
    auto result = nanoSecToDurationString(chrono::nanoseconds(0));
    EXPECT_EQ(result, "PT0S");
}

TEST(TimeUtilsTest, nanoSecToDurationString_Negative_ReturnsEmpty)
{
    auto result = nanoSecToDurationString(chrono::nanoseconds(-1));
    EXPECT_TRUE(result.empty());
}

TEST(TimeUtilsTest, nanoSecToDurationString_OneSecondExact)
{
    // 1 second = 1,000,000,000 ns
    auto result = nanoSecToDurationString(chrono::nanoseconds(1000000000LL));
    EXPECT_EQ(result, "PT1.000000000S");
}

TEST(TimeUtilsTest, nanoSecToDurationString_OneSecondAndHalfNs)
{
    // 1.5s = 1,500,000,000 ns
    auto result = nanoSecToDurationString(chrono::nanoseconds(1500000000LL));
    EXPECT_EQ(result, "PT1.500000000S");
}

TEST(TimeUtilsTest, nanoSecToDurationString_Minutes)
{
    // 5 minutes = 300,000,000,000 ns
    auto result = nanoSecToDurationString(chrono::nanoseconds(300000000000LL));
    EXPECT_EQ(result, "PT5M");
}

TEST(TimeUtilsTest, nanoSecToDurationString_OneHour)
{
    // 1 hour = 3,600,000,000,000 ns
    auto result = nanoSecToDurationString(chrono::nanoseconds(3600000000000LL));
    EXPECT_EQ(result, "PT1H");
}

TEST(TimeUtilsTest, nanoSecToDurationString_OneDayAndOneHour)
{
    // 25 hours = 90,000,000,000,000 ns
    auto result =
        nanoSecToDurationString(chrono::nanoseconds(90000000000000LL));
    EXPECT_EQ(result, "P1DT1H");
}

// =========================================================================
// toDurationStringFromNano
// =========================================================================

TEST(TimeUtilsTest, toDurationStringFromNano_Zero_ReturnsPT0S)
{
    auto result = toDurationStringFromNano(0);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, "PT0S");
}

TEST(TimeUtilsTest, toDurationStringFromNano_OneSecond_ReturnsPT1S)
{
    // 1,000,000,000 ns = 1 second
    auto result = toDurationStringFromNano(1000000000ULL);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, "PT1.000000000S");
}

TEST(TimeUtilsTest, toDurationStringFromNano_Overflow_ReturnsNullopt)
{
    // maxTimeNs + 1 overflows → nullopt
    static const uint64_t maxNs =
        static_cast<uint64_t>(chrono::nanoseconds::max().count());
    auto result = toDurationStringFromNano(maxNs + 1ULL);
    EXPECT_FALSE(result.has_value());
}

// =========================================================================
// fromDurationString — additional error paths
// =========================================================================

TEST(TimeUtilsTest, fromDurationString_MalformedDays_ReturnsNullopt)
{
    // 'x' is non-numeric → fromDurationItem<Days> fails → nullopt
    auto result = fromDurationString("PxD");
    EXPECT_FALSE(result.has_value());
}

TEST(TimeUtilsTest, fromDurationString_MalformedHours_ReturnsNullopt)
{
    // 'x' is non-numeric → fromDurationItem<hours> fails → nullopt
    auto result = fromDurationString("PTxH");
    EXPECT_FALSE(result.has_value());
}

TEST(TimeUtilsTest, fromDurationString_MalformedSeconds_ReturnsNullopt)
{
    // 'x' is non-numeric → fromDurationItem<seconds> fails → nullopt
    auto result = fromDurationString("PTxS");
    EXPECT_FALSE(result.has_value());
}

TEST(TimeUtilsTest, fromDurationString_MalformedFractionalMs_ReturnsNullopt)
{
    // seconds parse OK but milliseconds part "xS" is non-numeric → nullopt
    auto result = fromDurationString("PT1.xS");
    EXPECT_FALSE(result.has_value());
}

TEST(TimeUtilsTest, fromDurationString_TrailingGarbage_ReturnsNullopt)
{
    // After parsing all components, trailing 'X' leaves v non-empty → nullopt
    auto result = fromDurationString("PT0SX");
    EXPECT_FALSE(result.has_value());
}

// =========================================================================
// getDateTimeStdtime
// =========================================================================

TEST(TimeUtilsTest, getDateTimeStdtime_KnownTimestamp)
{
    // 1699255438 = 2023-11-06T07:23:58+00:00
    auto result = getDateTimeStdtime(static_cast<time_t>(1699255438));
    EXPECT_EQ(result, "2023-11-06T07:23:58+00:00");
}

TEST(TimeUtilsTest, getDateTimeStdtime_NegativeTime_ClampsTo1970)
{
    // -86400 seconds = 1 day before epoch → year 1969 < 1970 → clamp
    auto result = getDateTimeStdtime(static_cast<time_t>(-86400));
    EXPECT_EQ(result, "1970-01-01T00:00:00+00:00");
}

// =========================================================================
// getDateTimeUint — far future clamp
// =========================================================================

TEST(TimeUtilsTest, getDateTimeUint_FarFuture_ClampsToYear9999)
{
    // 253402300800 = one second past 9999-12-31T23:59:59 → year >= 10000
    // clamped to 9999-12-31T23:59:59+00:00
    auto result = getDateTimeUint(253402300800ULL);
    EXPECT_EQ(result, "9999-12-31T23:59:59+00:00");
}

TEST(TimeUtilsTest, getDateTimeUintMs_FarFuture_ClampsToYear9999)
{
    // 253402300800000 ms = one second past 9999-12-31 23:59:59 in ms precision
    // → year >= 10000 → clamped (covers another template instantiation of
    // the year-clamp branch)
    auto result = getDateTimeUintMs(253402300800000ULL);
    EXPECT_TRUE(result.find("9999") != std::string::npos);
}

TEST(TimeUtilsTest, getDateTimeUintUs_FarFuture_ClampsToYear9999)
{
    // 253402300800000000 us → year >= 10000 → clamped (covers us instantiation)
    auto result = getDateTimeUintUs(253402300800000000ULL);
    EXPECT_TRUE(result.find("9999") != std::string::npos);
}

// =========================================================================
// getDateTimeOffsetNow
// =========================================================================

TEST(TimeUtilsTest, getDateTimeOffsetNow_ReturnsNonEmpty)
{
    auto [dateTime, offset] = getDateTimeOffsetNow();
    EXPECT_FALSE(dateTime.empty());
    EXPECT_FALSE(offset.empty());
}

// =========================================================================
// getDateTimeStdtime — far future clamp (time_t instantiation, lines 361-364)
// =========================================================================

TEST(TimeUtilsTest, getDateTimeStdtime_FarFuture_ClampsToYear9999)
{
    // 253402300800 seconds past epoch → year >= 10000 → clamp in time_t
    // instantiation of toISO8061ExtendedStr (lines 361-364 for that instance)
    auto result = getDateTimeStdtime(static_cast<time_t>(253402300800LL));
    EXPECT_TRUE(result.find("9999") != std::string::npos);
}

// =========================================================================
// fromDurationString — overflow tests (line 136 in fromDurationItem for
// seconds, minutes, hours, days instantiations)
// =========================================================================

TEST(TimeUtilsTest, fromDurationString_OverflowSeconds_ReturnsNullopt)
{
    // 10^16 seconds >> chrono::milliseconds::max/1000 → overflow → line 136
    auto result = fromDurationString("PT9999999999999999S");
    EXPECT_FALSE(result.has_value());
}

TEST(TimeUtilsTest, fromDurationString_OverflowMinutes_ReturnsNullopt)
{
    // 10^15 minutes >> maxConversionRange for minutes → overflow → line 136
    auto result = fromDurationString("PT999999999999999M");
    EXPECT_FALSE(result.has_value());
}

TEST(TimeUtilsTest, fromDurationString_OverflowHours_ReturnsNullopt)
{
    // 10^13 hours >> maxConversionRange for hours → overflow → line 136
    auto result = fromDurationString("PT9999999999999H");
    EXPECT_FALSE(result.has_value());
}

TEST(TimeUtilsTest, fromDurationString_OverflowDays_ReturnsNullopt)
{
    // 10^12 days >> maxConversionRange for days → overflow → line 136
    auto result = fromDurationString("P999999999999D");
    EXPECT_FALSE(result.has_value());
}
