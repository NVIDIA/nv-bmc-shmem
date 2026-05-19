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

#include "include/telemetry_mrd_producer.hpp"

#include <gtest/gtest.h>

// =========================================================================
// AggregationService null-aggregator tests
//
// This binary NEVER calls namespaceInit(), so the static sensorAggregator
// remains nullptr throughout. This exercises the early-return guard in
// updateTelemetry (telemetry_mrd_producer.cpp line 90) that is unreachable
// in the main telemetry_mrd_producer_test binary (where namespaceInit is
// called first).
// =========================================================================

TEST(TelemetryMrdProducerNullTest,
     UpdateTelemetry_AggregatorNull_ReturnsFalse)
{
    // sensorAggregator == nullptr (namespaceInit never called) → return false
    DbusVariantType value = 0.0;
    bool result = nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/inventory/system/processors/GPU_0",
        "xyz.openbmc_project.Memory.MemoryECC", "ueCount", value, 0ULL, 0, "");
    EXPECT_FALSE(result);
}
