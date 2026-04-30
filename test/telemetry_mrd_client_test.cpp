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
#include "include/telemetry_mrd_client.hpp"

#include <gtest/gtest.h>

using namespace nv::shmem::sensor_aggregation;

// =========================================================================
// telemetry_mrd_client tests
//
// JSON config files are present in the test environment so
// getMRDNamespaceLookup returns a populated map.
// =========================================================================

TEST(TelemetryMrdClientTest,
     GetAllMRDValues_UnknownNamespace_ThrowsNameSpaceNotFoundException)
{
    // Namespace not in the MRD lookup → NameSpaceNotFoundException
    EXPECT_THROW(getAllMRDValues("NonExistentNamespace_XYZ_Test"),
                 NameSpaceNotFoundException);
}

TEST(TelemetryMrdClientTest,
     GetMrdNamespacesValues_ConfigPresent_ReturnsNamespaces)
{
    // Config present → lookup is populated → returns namespace list
    auto result = getMrdNamespacesValues();
    EXPECT_FALSE(result.empty());
}

TEST(TelemetryMrdClientTest,
     GetAllKeyValuePair_NonExistentNamespace_ThrowsNameSpaceNotFoundException)
{
    // Shared memory for this namespace was never created → exception →
    // NameSpaceNotFoundException re-thrown
    EXPECT_THROW(getAllKeyValuePair("nonexistent_shmem_namespace_xyz"),
                 NameSpaceNotFoundException);
}

TEST(TelemetryMrdClientTest,
     GetAllMRDValues_ValidNamespaceNoProducer_ThrowsNoElementsException)
{
    // Namespace is in the MRD lookup (config present) but no SHM producer is
    // running → inner exception caught per producer → values stays empty →
    // NoElementsException thrown
    auto namespaces = getMrdNamespacesValues();
    ASSERT_FALSE(namespaces.empty());
    EXPECT_THROW(getAllMRDValues(namespaces.front()), NoElementsException);
}

TEST(TelemetryMrdClientTest,
     GetAllMRDValues_EmptyExistingShmNamespace_ThrowsNoElementsException)
{
    // Create an empty SHM namespace that matches a known MRD entry.
    // getAllMRDValues opens it, calls getAllValues() → empty → LOG_ERROR branch
    // → NoElementsException at the end.
    nv::shmem::ShmSensorMapIntf intf;
    intf.createNamespace("gpumgrd_NVSwitchMetrics_0", 131072);
    EXPECT_THROW(getAllMRDValues("NVSwitchMetrics_0"), NoElementsException);
}

TEST(TelemetryMrdClientTest, GetAllMRDValues_ShmNamespaceWithData_ReturnsValues)
{
    // Create a SHM namespace with one entry — getAllMRDValues succeeds and
    // returns a non-empty vector (covers the getAllValues > 0 path).
    nv::shmem::ShmSensorMapIntf intf;
    intf.createNamespace("gpumgrd_ProcessorMetrics_0", 262144);
    nv::shmem::SensorValue val("42", "/redfish/v1/test", 1000, "2024-01-01");
    intf.insert("gpumgrd_ProcessorMetrics_0", "test_sensor_client", val);
    auto values = getAllMRDValues("ProcessorMetrics_0");
    EXPECT_FALSE(values.empty());
}

TEST(TelemetryMrdClientTest, GetAllKeyValuePair_ExistingShmNamespace_ReturnsMap)
{
    // Create a SHM namespace with data → covers lines 47 and 51
    // (insert into static sensor_map and return getAllKeyValuePair)
    nv::shmem::ShmSensorMapIntf intf;
    intf.createNamespace("gpumgrd_ProcessorMetrics_0", 262144);
    nv::shmem::SensorValue val("99", "/redfish/v1/kv", 2000, "2024-06-01");
    intf.insert("gpumgrd_ProcessorMetrics_0", "kv_sensor", val);
    auto result = getAllKeyValuePair("gpumgrd_ProcessorMetrics_0");
    EXPECT_FALSE(result.empty());
}
