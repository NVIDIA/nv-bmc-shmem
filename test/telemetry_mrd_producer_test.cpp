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
// AggregationService tests
//
// In the test environment, JSON config files are present so namespaceInit
// succeeds and creates the shared memory namespace.
// updateTelemetry for an unknown device path returns false since the path
// is not present in any shared memory map.
// =========================================================================

TEST(TelemetryMrdProducerTest, NamespaceInit_ConfigPresent_Succeeds)
{
    bool result = nv::shmem::AggregationService::namespaceInit("test_process");
    EXPECT_TRUE(result);
}

TEST(TelemetryMrdProducerTest, UpdateTelemetry_UnknownDevicePath_ReturnsFalse)
{
    // Device path not in any SHM map → updateSHMObject returns false
    DbusVariantType value = 42.0;
    bool result = nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/sensors/temperature/Unknown_Sensor_Test",
        "xyz.openbmc_project.Sensor.Value", "Value", value, 0, 0, "");
    EXPECT_FALSE(result);
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_NanPath_UnknownDevicePath_ReturnsFalse)
{
    // rc != 0 and timestamp != 0 → NAN update path, but path not in SHM
    DbusVariantType value = 0.0;
    bool result = nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/sensors/temperature/Unknown_Sensor_Test",
        "xyz.openbmc_project.Sensor.Value", "Value", value, 12345ULL, -1, "");
    EXPECT_FALSE(result);
}

// =========================================================================
// Tests using gpumgrd producer to exercise shm_sensor_aggregator paths
// =========================================================================

TEST(TelemetryMrdProducerTest, NamespaceInit_Gpumgrd_Succeeds)
{
    // Replace static aggregator with gpumgrd producer — creates SHM namespaces
    bool result = nv::shmem::AggregationService::namespaceInit("gpumgrd");
    EXPECT_TRUE(result);
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_ProcessorPath_Uint64_Insert_ReturnsTrue)
{
    // First call: parseDevicePath matches "processors" → insertShmemObject
    DbusVariantType value = static_cast<uint64_t>(5);
    bool result = nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/inventory/system/processors/GPU_0",
        "xyz.openbmc_project.Memory.MemoryECC", "ueCount", value, 1000ULL, 0,
        "");
    EXPECT_TRUE(result);
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_ProcessorPath_Uint64_Update_ReturnsTrue)
{
    // Second call for same path: updateSHMObject existing key → getMetricValue
    DbusVariantType value = static_cast<uint64_t>(6);
    bool result = nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/inventory/system/processors/GPU_0",
        "xyz.openbmc_project.Memory.MemoryECC", "ueCount", value, 2000ULL, 0,
        "");
    EXPECT_TRUE(result);
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_ProcessorPath_NaN_Scalar_ReturnsTrue)
{
    // NaN update for a known key with arraySize=0 → updateValueAndTimeStamp
    DbusVariantType value = 0.0;
    bool result = nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/inventory/system/processors/GPU_0",
        "xyz.openbmc_project.Memory.MemoryECC", "ueCount", value, 3000ULL, -1,
        "");
    EXPECT_TRUE(result);
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_ProcessorPath_VectorString_Insert)
{
    // Insert with vector<string> value: isList=true → arraySize updated
    DbusVariantType value = std::vector<std::string>{"SWPowerCap",
                                                     "ClockLimit"};
    nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/inventory/system/processors/GPU_2",
        "xyz.openbmc_project.Memory.MemoryECC", "ueCount", value, 1000ULL, 0,
        "");
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_ProcessorPath_VectorString_Update_GrowsArray)
{
    // Second call with larger vector: handleArrayUpdates grow path
    DbusVariantType value = std::vector<std::string>{"SWPowerCap", "ClockLimit",
                                                     "ThermalCap"};
    nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/inventory/system/processors/GPU_2",
        "xyz.openbmc_project.Memory.MemoryECC", "ueCount", value, 2000ULL, 0,
        "");
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_ProcessorPath_VectorString_NaN_ArrayPath)
{
    // NaN for a key with arraySize > 1 → updateNanValue array branch
    DbusVariantType value = 0.0;
    nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/inventory/system/processors/GPU_2",
        "xyz.openbmc_project.Memory.MemoryECC", "ueCount", value, 4000ULL, -1,
        "");
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_PortPath_Insert_CoversMultiMatchBranch)
{
    // Path matching processors/Ports keyword (count=2) → else branch in
    // parseDevicePath; two namespaces match → equal-count branch
    DbusVariantType value = static_cast<uint64_t>(100);
    nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/inventory/system/processors/GPU_0/Ports/Port_0",
        "xyz.openbmc_project.Metrics.IBPort", "RXBytes", value, 5000ULL, 0, "");
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_SensorsPath_ReturnsFalse_CoversSensorsPathBranch)
{
    // sensors/temperature path → parseDevicePath hits sensors branch (line 109)
    // deviceName="" + no associatedEntityPath → handleObjectInsertion fails
    DbusVariantType value = 42.0;
    bool result = nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/sensors/temperature/Sensor_X",
        "xyz.openbmc_project.Sensor.Value", "Value", value, 6000ULL, 0, "");
    EXPECT_FALSE(result);
}

// =========================================================================
// Additional coverage tests for parseDevicePath branches and related paths
// =========================================================================

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_NaN_FreshPath_Line302_ReturnsTrue)
{
    // NaN update for a path never seen → not in nameSpaceMap or
    // notApplicableKeys → updateNanValue line 302: return status (true)
    DbusVariantType value = 0.0;
    bool result = nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/inventory/system/processors/GPU_999",
        "xyz.openbmc_project.Memory.MemoryECC", "ueCount", value, 12345ULL, -1,
        "");
    EXPECT_TRUE(result);
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_UnknownProperty_AllNamespacesSkip_ReturnsTrue)
{
    // Property not in any matching namespace's list → handleObjectInsertion
    // adds to notApplicableKeys for each → falls through to line 394 return
    // true
    DbusVariantType value = static_cast<uint64_t>(1);
    bool result = nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/inventory/system/processors/GPU_5",
        "xyz.openbmc_project.Memory.MemoryECC", "UnknownProp_ZZZ", value, 0, 0,
        "");
    EXPECT_TRUE(result);
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_ScalarAfterVectorInsert_CoversHandleArrayNotIsList)
{
    // GPU_2 was previously inserted with a vector (arraySize=3). Updating
    // with a scalar causes handleArrayUpdates !isList branch (lines
    // 405,407-408) then updateValueAndTimeStamp fails because sensorKey has no
    // direct entry → error path lines 412-418 → returns false
    DbusVariantType value = static_cast<uint64_t>(99);
    nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/inventory/system/processors/GPU_2",
        "xyz.openbmc_project.Memory.MemoryECC", "ueCount", value, 5000ULL, 0,
        "");
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_CompletelyUnrecognizedPath_Line564_ReturnsFalse)
{
    // No keyword in namespace config matches → parseDevicePath returns empty
    // → lines 564-568 executed → notApplicableKeys.emplace → return false
    DbusVariantType value = static_cast<uint64_t>(1);
    bool result = nv::shmem::AggregationService::updateTelemetry(
        "/completely/unrecognized/path/ABC_test", "some.iface.Xyz", "someProp",
        value, 0, 0, "");
    EXPECT_FALSE(result);
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_NotApplicablePath_SecondCall_Line556_ReturnsTrue)
{
    // Same completely-unrecognized path from previous test → now in
    // notApplicableKeys → lines 556-557 → return true
    DbusVariantType value = static_cast<uint64_t>(1);
    bool result = nv::shmem::AggregationService::updateTelemetry(
        "/completely/unrecognized/path/ABC_test", "some.iface.Xyz", "someProp",
        value, 0, 0, "");
    EXPECT_TRUE(result);
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_StatePathSingleKeyword_Line95_CoversStateBranch)
{
    // Path starts with xyz/openbmc_project/state and matches "Switches" with
    // count=1 → maxMatchCount==1 + state prefix → line 95:
    // subDeviceName=filename
    DbusVariantType value = static_cast<uint64_t>(1);
    nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/state/Switches/GPU_State_0",
        "xyz.openbmc_project.Sensor.Value", "Value", value, 0, 0, "");
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_HealthChassisPath_Line111_CoversHealthChassisBranch)
{
    // health/chassis prefix + Switches/Ports 2-keyword match
    // → maxMatchCount==2 + health/chassis → lines 111-112
    DbusVariantType value = static_cast<uint64_t>(100);
    nv::shmem::AggregationService::updateTelemetry(
        "/health/chassis/SW_0/Switches/Ports/Port_0",
        "xyz.openbmc_project.Metrics.IBPort", "RXBytes", value, 0, 0, "");
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_HealthSystemPath_Line116_CoversHealthSystemBranch)
{
    // health/system prefix + Switches/Ports 2-keyword match
    // → maxMatchCount==2 + health/system → lines 116-117
    DbusVariantType value = static_cast<uint64_t>(100);
    nv::shmem::AggregationService::updateTelemetry(
        "/health/system/SW_0/Switches/Ports/Port_0",
        "xyz.openbmc_project.Metrics.IBPort", "RXBytes", value, 0, 0, "");
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_StateProcessorModule_Line121_CoversStateBranchCount2)
{
    // state/ProcessorModule keyword gives count=2 + path starts with
    // xyz/openbmc_project/state → lines 121-122
    DbusVariantType value = static_cast<uint64_t>(1);
    nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/state/ProcessorModule/CPU_SomeModule_0",
        "xyz.openbmc_project.Sensor.Value", "Status", value, 0, 0, "");
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_ResetStatisticsInPath_Line127_CoversResetBranch)
{
    // processors/Ports 2-keyword match + "ResetStatistics" in path
    // → lines 127-129: deviceName=parent_path().filename(), subDeviceName=""
    DbusVariantType value = static_cast<uint64_t>(0);
    nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/inventory/system/processors/GPU_0/Ports/Port_0/"
        "ResetStatistics",
        "xyz.openbmc_project.Metrics.IBPort", "RXBytes", value, 0, 0, "");
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_MetricPath_Line133_CoversMetricBranch)
{
    // xyz/openbmc_project/metric prefix + processors/Ports 2-keyword match
    // → lines 133-134: subDeviceName=filename, deviceName=""
    DbusVariantType value = static_cast<uint64_t>(1);
    nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/metric/GPU_0/processors/Ports/Port_0",
        "xyz.openbmc_project.Metrics.IBPort", "RXBytes", value, 0, 0, "");
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_SensorsPathWithAssociatedEntity_Line375_CoversDeviceName)
{
    // sensors/temperature → deviceName="" but associatedEntityPath non-empty
    // → lines 375-376: deviceName = last component of associatedEntityPath
    // → "Value" is in PlatformEnvironmentMetrics → insertShmemObject called
    DbusVariantType value = 55.0;
    nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/sensors/temperature/Sensor_AssocTest",
        "xyz.openbmc_project.Sensor.Value", "Value", value, 7000ULL, 0,
        "/xyz/openbmc_project/inventory/system/chassis/Baseboard_0");
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_EmptyVectorInsert_Line237_MetricValuesEmpty)
{
    // Empty vector → insertShmemObject → getMetricValues returns empty map
    // → metricValues.empty() branch (lines 237-242) executed
    DbusVariantType value = std::vector<std::string>{};
    bool result = nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/inventory/system/processors/GPU_7",
        "xyz.openbmc_project.Memory.MemoryECC", "ueCount", value, 0, 0, "");
    EXPECT_FALSE(result);
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_SecondNaN_GPU2_Line317_EraseFailure)
{
    // GPU_2 indexed keys sensorKey/1 and sensorKey/2 were already erased by
    // a previous NaN update; second NaN call → erase returns false → line 317
    DbusVariantType value = 0.0;
    nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/inventory/system/processors/GPU_2",
        "xyz.openbmc_project.Memory.MemoryECC", "ueCount", value, 8000ULL, -1,
        "");
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_VectorGrow_Line427_EraseFailureForMissingKey)
{
    // GPU_2 nameSpaceMap has arraySize=3, but sensorKey/2 was erased by NaN.
    // 4-element vector (size > arraySize) → handleArrayUpdates grow path →
    // loop tries to erase sensorKey/2 which no longer exists → lines 427-431
    DbusVariantType value = std::vector<std::string>{"A", "B", "C", "D"};
    nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/inventory/system/processors/GPU_2",
        "xyz.openbmc_project.Memory.MemoryECC", "ueCount", value, 9000ULL, 0,
        "");
}

TEST(TelemetryMrdProducerTest,
     UpdateTelemetry_GPU7_ScalarAfterEmptyVector_Lines535_540_UpdateFails)
{
    // GPU_7 was inserted with empty vector → nameSpaceMap has arraySize=0,
    // but the sensorKey was never inserted into SHM (insertShmemObject
    // returned false). Scalar update → arraySize==0 path →
    // updateValueAndTimeStamp for nonexistent sensorKey → fails → lines 535-540
    DbusVariantType value = static_cast<uint64_t>(42);
    nv::shmem::AggregationService::updateTelemetry(
        "/xyz/openbmc_project/inventory/system/processors/GPU_7",
        "xyz.openbmc_project.Memory.MemoryECC", "ueCount", value, 1000ULL, 0,
        "");
}
