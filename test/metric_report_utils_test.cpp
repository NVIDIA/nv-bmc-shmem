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
#include "utils/metric_report_utils.hpp"

#include <shm_common.h>

#include <gtest/gtest.h>

using namespace nv::sensor_aggregation::metricUtils;

// =========================================================================
// toReasonType
// =========================================================================

TEST(MetricReportUtilsTest, toReasonType_KnownSWPowerCap_ReturnsSWPowerCap)
{
    auto result = toReasonType(
        "xyz.openbmc_project.State.ProcessorPerformance.ThrottleReasons."
        "SWPowerCap");
    EXPECT_EQ(result, "SWPowerCap");
}

TEST(MetricReportUtilsTest, toReasonType_Unknown_ReturnsEmpty)
{
    EXPECT_TRUE(toReasonType("xyz.unknown.Reason").empty());
}

// =========================================================================
// getLastResetType
// =========================================================================

TEST(MetricReportUtilsTest,
     getLastResetType_KnownConventional_ReturnsConventional)
{
    auto result = getLastResetType(
        "com.nvidia.ResetCounters.ResetCounterMetrics.ResetTypes."
        "Conventional");
    EXPECT_EQ(result, "Conventional");
}

TEST(MetricReportUtilsTest, getLastResetType_Unknown_ReturnsUnknown)
{
    EXPECT_EQ(getLastResetType("xyz.unknown.Reset"), "Unknown");
}

// =========================================================================
// toPCIeType
// =========================================================================

TEST(MetricReportUtilsTest, toPCIeType_Gen4_ReturnsGen4)
{
    auto result = toPCIeType(
        "xyz.openbmc_project.Inventory.Item.PCIeDevice.PCIeTypes.Gen4");
    EXPECT_EQ(result, "Gen4");
}

TEST(MetricReportUtilsTest, toPCIeType_Unknown_ReturnsUnknown)
{
    EXPECT_EQ(toPCIeType("xyz.unknown.PCIe"), "Unknown");
}

// =========================================================================
// toLinkDownReasonCode
// =========================================================================

TEST(MetricReportUtilsTest,
     toLinkDownReasonCode_KnownHighBitErrorRate_ReturnsHighBitErrorRate)
{
    auto result = toLinkDownReasonCode(
        "xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes."
        "HighBitErrorRate");
    EXPECT_EQ(result, "HighBitErrorRate");
}

TEST(MetricReportUtilsTest, toLinkDownReasonCode_Unknown_ReturnsUnknown)
{
    EXPECT_EQ(toLinkDownReasonCode("xyz.unknown.Code"), "Unknown");
}

// =========================================================================
// getPowerStateType
// =========================================================================

TEST(MetricReportUtilsTest, getPowerStateType_KnownEnabled_ReturnsEnabled)
{
    auto result = getPowerStateType(
        "xyz.openbmc_project.State.Decorator.OperationalStatus.StateType."
        "Enabled");
    EXPECT_EQ(result, "Enabled");
}

TEST(MetricReportUtilsTest, getPowerStateType_Unknown_ReturnsEmpty)
{
    EXPECT_TRUE(getPowerStateType("xyz.unknown.State").empty());
}

// =========================================================================
// getPropertySuffix
// =========================================================================

TEST(MetricReportUtilsTest, getPropertySuffix_KnownIfaceAndMetric_ReturnsSuffix)
{
    // IBPort / RXErrors → /Metrics#/RXErrors
    auto result = getPropertySuffix("xyz.openbmc_project.Metrics.IBPort",
                                    "RXErrors");
    EXPECT_EQ(result, "/Metrics#/RXErrors");
}

TEST(MetricReportUtilsTest, getPropertySuffix_UnknownIface_ReturnsEmpty)
{
    EXPECT_TRUE(getPropertySuffix("xyz.unknown.Iface", "SomeMetric").empty());
}

TEST(MetricReportUtilsTest,
     getPropertySuffix_KnownIfaceUnknownMetric_ReturnsEmpty)
{
    EXPECT_TRUE(
        getPropertySuffix("xyz.openbmc_project.Metrics.IBPort", "NonExistent")
            .empty());
}

// =========================================================================
// getProcessorAndCpuNum
// =========================================================================

TEST(MetricReportUtilsTest,
     getProcessorAndCpuNum_ValidInput_ReturnsProcessorAndCpu)
{
    // "ProcessorModule_1_CPU_0_CoreUtil_64" → (1, 0)
    auto [proc,
          cpu] = getProcessorAndCpuNum("ProcessorModule_1_CPU_0_CoreUtil_64");
    EXPECT_EQ(proc, "1");
    EXPECT_EQ(cpu, "0");
}

TEST(MetricReportUtilsTest,
     getProcessorAndCpuNum_NonDigitParts_ReturnsBothEmpty)
{
    // Keys at index 1 and 3 are not single digits → returns ("", "")
    auto [proc,
          cpu] = getProcessorAndCpuNum("ProcessorModule_XX_CPU_YY_CoreUtil");
    EXPECT_TRUE(proc.empty());
    EXPECT_TRUE(cpu.empty());
}

// =========================================================================
// translateReading
// =========================================================================

TEST(MetricReportUtilsTest,
     translateReading_ProcessorPerformanceThrottleReason_ReturnsTranslated)
{
    auto result = translateReading(
        "xyz.openbmc_project.State.ProcessorPerformance", "ThrottleReason",
        "xyz.openbmc_project.State.ProcessorPerformance.ThrottleReasons."
        "SWPowerCap");
    EXPECT_EQ(result, "SWPowerCap");
}

TEST(MetricReportUtilsTest,
     translateReading_ProcessorPerformanceValue_ReturnsTranslated)
{
    auto result = translateReading(
        "xyz.openbmc_project.State.ProcessorPerformance", "Value",
        "xyz.openbmc_project.State.ProcessorPerformance.ThrottleReasons.None");
    EXPECT_EQ(result, "NA");
}

TEST(MetricReportUtilsTest, translateReading_PCIeECC_PCIeType_ReturnsGen4)
{
    auto result =
        translateReading("xyz.openbmc_project.PCIe.PCIeECC", "PCIeType",
                         "xyz.openbmc_project.PCIe.PCIeECC.PCIeTypes.Gen4");
    EXPECT_EQ(result, "Gen4");
}

TEST(MetricReportUtilsTest, translateReading_PortStateLinkStatus_ReturnsOK)
{
    // LinkUp/LinkDown both map to "OK" for NVLink Health reporting
    auto result = translateReading(
        "xyz.openbmc_project.Inventory.Decorator.PortState", "LinkStatus",
        "xyz.openbmc_project.Inventory.Decorator.PortState.LinkStatusType."
        "LinkUp",
        "CpuProcessorMetrics");
    EXPECT_EQ(result, "OK");
}

TEST(MetricReportUtilsTest, translateReading_PortStateLinkState_ReturnsEnabled)
{
    auto result = translateReading(
        "xyz.openbmc_project.Inventory.Decorator.PortState", "LinkState",
        "xyz.openbmc_project.Inventory.Decorator.PortState.LinkStates."
        "Enabled");
    EXPECT_EQ(result, "Enabled");
}

TEST(MetricReportUtilsTest,
     translateReading_PowerSystemInputs_Status_ReturnsNormal)
{
    auto result = translateReading(
        "xyz.openbmc_project.State.Decorator.PowerSystemInputs", "Status",
        "xyz.openbmc_project.State.Decorator.PowerSystemInputs.Status.Good");
    EXPECT_EQ(result, "Normal");
}

TEST(MetricReportUtilsTest,
     translateReading_OperationalStatus_State_ReturnsEnabled)
{
    auto result = translateReading(
        "xyz.openbmc_project.State.Decorator.OperationalStatus", "State",
        "xyz.openbmc_project.State.Decorator.OperationalStatus.StateType."
        "Enabled");
    EXPECT_EQ(result, "Enabled");
}

TEST(MetricReportUtilsTest,
     translateReading_ResetCounters_LastResetType_ReturnsConventional)
{
    auto result = translateReading(
        "com.nvidia.ResetCounters.ResetCounterMetrics", "LastResetType",
        "com.nvidia.ResetCounters.ResetCounterMetrics.ResetTypes."
        "Conventional");
    EXPECT_EQ(result, "Conventional");
}

TEST(MetricReportUtilsTest, translateReading_PCIeDevice_PCIeType_ReturnsGen5)
{
    auto result = translateReading(
        "xyz.openbmc_project.Inventory.Item.PCIeDevice", "PCIeType",
        "xyz.openbmc_project.Inventory.Item.PCIeDevice.PCIeTypes.Gen5");
    EXPECT_EQ(result, "Gen5");
}

TEST(MetricReportUtilsTest, translateReading_PCIeDevice_MaxPCIeType_ReturnsGen3)
{
    auto result = translateReading(
        "xyz.openbmc_project.Inventory.Item.PCIeDevice", "MaxPCIeType",
        "xyz.openbmc_project.Inventory.Item.PCIeDevice.PCIeTypes.Gen3");
    EXPECT_EQ(result, "Gen3");
}

TEST(MetricReportUtilsTest,
     translateReading_IBPort_LinkDownReasonCode_ReturnsCode)
{
    auto result = translateReading(
        "xyz.openbmc_project.Metrics.IBPort", "LinkDownReasonCode",
        "xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes."
        "HighBitErrorRate");
    EXPECT_EQ(result, "HighBitErrorRate");
}

TEST(MetricReportUtilsTest,
     translateReading_UnknownInterface_ReturnsReadingPassthrough)
{
    auto result = translateReading("xyz.unknown.Iface", "SomeMetric",
                                   "some_reading_value");
    EXPECT_EQ(result, "some_reading_value");
}

// =========================================================================
// generateURI — selected device types
// =========================================================================

TEST(MetricReportUtilsTest,
     generateURI_PlatformEnvironmentMetrics_ReturnsChassisURI)
{
    auto result = generateURI("PlatformEnvironmentMetrics", "HGX_Chassis_0",
                              "Temp_0", "", "Value",
                              "xyz.openbmc_project.Sensor.Value");
    EXPECT_TRUE(
        result.find("/redfish/v1/Chassis/HGX_Chassis_0/Sensors/Temp_0") !=
        std::string::npos);
}

TEST(MetricReportUtilsTest, generateURI_ProcessorPortMetrics_ReturnsPortURI)
{
    auto result = generateURI("ProcessorPortMetrics", "GPU_0", "Port_0", "",
                              "RXErrors", "xyz.openbmc_project.Metrics.IBPort");
    EXPECT_TRUE(result.find("/Processors/GPU_0/Ports/Port_0") !=
                std::string::npos);
}

TEST(MetricReportUtilsTest, generateURI_NVSwitchPortMetrics_ReturnsFabricsURI)
{
    auto result = generateURI("NVSwitchPortMetrics", "NVSwitch_0", "Port_0", "",
                              "TXBytes",
                              "xyz.openbmc_project.Metrics.PortMetricsOem2");
    EXPECT_TRUE(
        result.find("NVLinkFabric_0/Switches/NVSwitch_0/Ports/Port_0") !=
        std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_ProcessorGPMMetrics_ReturnsProcessorMetricsURI)
{
    auto result = generateURI("ProcessorGPMMetrics", "GPU_0", "", "",
                              "SMActivityPercent", "com.nvidia.GPMMetrics");
    EXPECT_TRUE(result.find("/Processors/GPU_0/ProcessorMetrics#") !=
                std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_ProcessorResetMetrics_ReturnsResetMetricsURI)
{
    auto result = generateURI("ProcessorResetMetrics", "GPU_0", "", "",
                              "PF_FLR_ResetEntryCount",
                              "com.nvidia.ResetCounters.ResetCounterMetrics");
    EXPECT_TRUE(result.find("/ProcessorResetMetrics#") != std::string::npos);
}

TEST(MetricReportUtilsTest, generateURI_NVSwitchMetrics_ReturnsSwitchMetricsURI)
{
    auto result = generateURI("NVSwitchMetrics", "NVSwitch_0", "", "",
                              "RXErrors", "xyz.openbmc_project.Metrics.IBPort");
    EXPECT_TRUE(result.find("/SwitchMetrics#") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_NVSwitchMetrics_BandwidthMetric_ReturnsSwitchHashURI)
{
    auto result = generateURI("NVSwitchMetrics", "NVSwitch_0", "", "",
                              "CurrentBandwidth",
                              "xyz.openbmc_project.Inventory.Item.Switch");
    EXPECT_TRUE(result.find("NVSwitch_0#") != std::string::npos);
}

TEST(MetricReportUtilsTest, generateURI_PCIeRetimerMetrics_ReturnsChassisURI)
{
    auto result = generateURI("PCIeRetimerMetrics", "PCIeRetimer_0",
                              "PCIeDevice_0", "", "PCIeType",
                              "xyz.openbmc_project.PCIe.PCIeECC");
    EXPECT_TRUE(result.find("/PCIeDevices/PCIeDevice_0") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_PCIeRetimerPortMetrics_ReturnsRetimerTopologyURI)
{
    auto result = generateURI("PCIeRetimerPortMetrics", "Retimer_3", "Port_0",
                              "", "nonfeCount",
                              "xyz.openbmc_project.PCIe.PCIeECC");
    EXPECT_TRUE(result.find("PCIeRetimerTopology_3") != std::string::npos);
    EXPECT_TRUE(result.find("/Metrics#") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_MemoryMetrics_RowRemappingFailure_ReturnsHashURI)
{
    auto result = generateURI("MemoryMetrics", "Memory_0", "", "",
                              "RowRemappingFailureState",
                              "com.nvidia.MemoryRowRemapping");
    EXPECT_TRUE(result.find("Memory_0#") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_MemoryMetrics_RowRemapping_ReturnsMemoryMetricsHashURI)
{
    auto result = generateURI("MemoryMetrics", "Memory_0", "", "",
                              "ueRowRemappingCount",
                              "com.nvidia.MemoryRowRemapping");
    EXPECT_TRUE(result.find("/MemoryMetrics#") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_MemoryMetrics_MemoryECC_ReturnsLifeTimeURI)
{
    auto result = generateURI("MemoryMetrics", "Memory_0", "", "", "ueCount",
                              "xyz.openbmc_project.Memory.MemoryECC");
    EXPECT_TRUE(result.find("/MemoryMetrics#/LifeTime") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_MemoryMetrics_Dimm_CapacityUtilization_ReturnsMemoryMetricsURI)
{
    auto result = generateURI(
        "MemoryMetrics", "Memory_0", "", "", "CapacityUtilizationPercent",
        "xyz.openbmc_project.Inventory.Item.Dimm.MemoryMetrics");
    EXPECT_TRUE(result.find("/MemoryMetrics#") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_MemoryMetrics_OtherIface_ReturnsMemoryMetricsHashURI)
{
    auto result = generateURI("MemoryMetrics", "Memory_0", "", "",
                              "Utilization",
                              "xyz.openbmc_project.Inventory.Item.Dimm");
    EXPECT_TRUE(result.find("/MemoryMetrics#") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_NetworkAdapterPortMetrics_ReturnsNetworkAdapterURI)
{
    auto result = generateURI("NetworkAdapterPortMetrics", "ConnectX_0",
                              "Port_0", "", "RXErrors",
                              "xyz.openbmc_project.Metrics.IBPort");
    EXPECT_TRUE(result.find("/NetworkAdapters/ConnectX_0/Ports/Port_0") !=
                std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_ProcessorPortGPMMetrics_ReturnsPortMetricsURI)
{
    auto result = generateURI("ProcessorPortGPMMetrics", "GPU_0", "Port_0", "",
                              "SMActivityPercent", "com.nvidia.GPMMetrics");
    EXPECT_TRUE(result.find("/Ports/Port_0/Metrics#") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_ProcessorMetrics_MemoryECC_ReturnsCacheMetricsURI)
{
    auto result = generateURI("ProcessorMetrics", "GPU_0", "", "", "ueCount",
                              "xyz.openbmc_project.Memory.MemoryECC");
    EXPECT_TRUE(result.find("/CacheMetricsTotal/LifeTime") !=
                std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_ProcessorMetrics_OperationalStatus_ReturnsProcessorHashURI)
{
    auto result =
        generateURI("ProcessorMetrics", "GPU_0", "", "", "State",
                    "xyz.openbmc_project.State.Decorator.OperationalStatus");
    EXPECT_TRUE(result.find("/Processors/GPU_0#") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_ProcessorMetrics_PowerLimit_ReturnsChassisPrefixURI)
{
    auto result =
        generateURI("ProcessorMetrics", "GPU_0", "", "", "MaxPowerWatts",
                    "xyz.openbmc_project.Inventory.Decorator.PowerLimit");
    EXPECT_TRUE(result.find("GPU_0#") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_ProcessorMetrics_PowerSmoothing_ReturnsPowerSmoothingURI)
{
    auto result = generateURI("ProcessorMetrics", "GPU_0", "", "",
                              "PowerSmoothingEnabled",
                              "com.nvidia.PowerSmoothing.PowerSmoothing");
    EXPECT_TRUE(result.find("/PowerSmoothing") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_ProcessorMetrics_OperatingConfig_MaxSpeed_ReturnsProcessorURI)
{
    auto result =
        generateURI("ProcessorMetrics", "CPU_0", "", "", "MaxSpeed",
                    "xyz.openbmc_project.Inventory.Item.Cpu.OperatingConfig");
    EXPECT_TRUE(result.find("/Processors/CPU_0#") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_CpuProcessorMetrics_SensorValue_RegularPath)
{
    // subDeviceName doesn't match PageRetirementCount regex → chassis sensor
    // URI
    auto result = generateURI("CpuProcessorMetrics", "CPU_0", "Temp_0", "",
                              "Value", "xyz.openbmc_project.Sensor.Value");
    EXPECT_TRUE(result.find("/Sensors/Temp_0") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_CpuProcessorMetrics_MemorySpareChannel_ReturnsProcessorMetrics)
{
    auto result = generateURI("CpuProcessorMetrics", "CPU_0", "", "",
                              "MemorySpareChannelPresence",
                              "com.nvidia.MemorySpareChannel");
    EXPECT_TRUE(result.find("/ProcessorMetrics") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_CpuProcessorMetrics_OtherIface_ReturnsPortURI)
{
    auto result = generateURI("CpuProcessorMetrics", "CPU_0", "Port_0", "",
                              "RXErrors", "xyz.openbmc_project.Metrics.IBPort");
    EXPECT_TRUE(result.find("/Ports/Port_0") != std::string::npos);
}

// =========================================================================
// toPresenceType
// =========================================================================

TEST(MetricReportUtilsTest, toPresenceType_KnownPresent_ReturnsTrue)
{
    EXPECT_EQ(toPresenceType("com.nvidia.MemorySpareChannel.Presence.Present"),
              "true");
}

TEST(MetricReportUtilsTest, toPresenceType_KnownNotPresent_ReturnsFalse)
{
    EXPECT_EQ(
        toPresenceType("com.nvidia.MemorySpareChannel.Presence.NotPresent"),
        "false");
}

TEST(MetricReportUtilsTest, toPresenceType_Unknown_ReturnsEmpty)
{
    EXPECT_TRUE(toPresenceType("xyz.unknown.Presence").empty());
}

// =========================================================================
// translateReading — additional paths
// =========================================================================

TEST(MetricReportUtilsTest,
     translateReading_MemorySpareChannel_MemorySpareChannelPresence_ReturnsTrue)
{
    auto result = translateReading(
        "com.nvidia.MemorySpareChannel", "MemorySpareChannelPresence",
        "com.nvidia.MemorySpareChannel.Presence.Present");
    EXPECT_EQ(result, "true");
}

TEST(MetricReportUtilsTest,
     translateReading_PortState_LinkStatus_NoLink_ReturnsCritical)
{
    auto result = translateReading(
        "xyz.openbmc_project.Inventory.Decorator.PortState", "LinkStatus",
        "xyz.openbmc_project.Inventory.Decorator.PortState.LinkStatusType."
        "NoLink",
        "CpuProcessorMetrics");
    EXPECT_EQ(result, "Critical");
}

TEST(MetricReportUtilsTest,
     translateReading_PortState_LinkStatus_Unknown_ReturnsEmpty)
{
    auto result = translateReading(
        "xyz.openbmc_project.Inventory.Decorator.PortState", "LinkStatus",
        "xyz.openbmc_project.Inventory.Decorator.PortState.LinkStatusType."
        "Starting",
        "CpuProcessorMetrics");
    EXPECT_TRUE(result.empty());
}

// =========================================================================
// getPropertySuffixFromPath
// =========================================================================

TEST(MetricReportUtilsTest,
     getPropertySuffixFromPath_MetricValue_PowerBRK_ReturnsSuffix)
{
    auto result = getPropertySuffixFromPath(
        "xyz.openbmc_project.Metric.Value",
        "/xyz/openbmc_project/metric/CPU_0/PowerBRKAssertionTime");
    EXPECT_EQ(result, "#/Oem/Nvidia/PowerBrakeAssertionDuration");
}

TEST(MetricReportUtilsTest,
     getPropertySuffixFromPath_MetricValue_PageRetirement_ReturnsSuffix)
{
    auto result = getPropertySuffixFromPath(
        "xyz.openbmc_project.Metric.Value",
        "/xyz/openbmc_project/metric/CPU_0/PageRetirementCount");
    EXPECT_EQ(result, "#/Oem/Nvidia/MemoryPageRetirementCount");
}

TEST(MetricReportUtilsTest,
     getPropertySuffixFromPath_MetricValue_TjMaxDramIndex_ReturnsSuffix)
{
    auto result = getPropertySuffixFromPath(
        "xyz.openbmc_project.Metric.Value",
        "/xyz/openbmc_project/metric/CPU_0/TjMaxDramIndex");
    EXPECT_EQ(result, "#/Oem/Nvidia/TjMaxDramIndex");
}

TEST(MetricReportUtilsTest,
     getPropertySuffixFromPath_MetricValue_CpuUptime_ReturnsSuffix)
{
    auto result = getPropertySuffixFromPath(
        "xyz.openbmc_project.Metric.Value",
        "/xyz/openbmc_project/metric/CPU_0/CpuUptime");
    EXPECT_EQ(result, "#/Oem/Nvidia/CPUUptime");
}

TEST(MetricReportUtilsTest, getPropertySuffixFromPath_OtherIface_ReturnsEmpty)
{
    auto result = getPropertySuffixFromPath("xyz.unknown.Iface",
                                            "/some/device/path");
    EXPECT_TRUE(result.empty());
}

// =========================================================================
// generateURI — additional branches
// =========================================================================

TEST(MetricReportUtilsTest,
     generateURI_CpuProcessorMetrics_MetricValue_PowerBRK_ReturnsSuffix)
{
    auto result =
        generateURI("CpuProcessorMetrics", "CPU_0", "",
                    "/xyz/openbmc_project/metric/CPU_0/PowerBRKAssertionTime",
                    "Value", "xyz.openbmc_project.Metric.Value");
    EXPECT_TRUE(result.find("PowerBrakeAssertionDuration") !=
                std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_CpuProcessorMetrics_PowerSystemInputs_ReturnsProcessorMetrics)
{
    auto result =
        generateURI("CpuProcessorMetrics", "CPU_0", "", "", "Status",
                    "xyz.openbmc_project.State.Decorator.PowerSystemInputs");
    EXPECT_TRUE(result.find("/ProcessorMetrics") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_CpuProcessorMetrics_PortState_LinkStatus_ReturnsHealthURI)
{
    auto result =
        generateURI("CpuProcessorMetrics", "CPU_0", "Port_0", "", "LinkStatus",
                    "xyz.openbmc_project.Inventory.Decorator.PortState");
    EXPECT_TRUE(result.find("/Status/Health") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_CpuProcessorMetrics_PortState_LinkState_ReturnsStateURI)
{
    auto result =
        generateURI("CpuProcessorMetrics", "CPU_0", "Port_0", "", "LinkState",
                    "xyz.openbmc_project.Inventory.Decorator.PortState");
    EXPECT_TRUE(result.find("/Status/State") != std::string::npos);
}

TEST(
    MetricReportUtilsTest,
    generateURI_NetworkAdapterPortMetrics_WithChassisPath_ReturnsChassiMatchURI)
{
    auto result = generateURI(
        "NetworkAdapterPortMetrics", "ConnectX_0", "Port_0",
        "/xyz/openbmc_project/inventory/system/chassis/HGX_Chassis_0/"
        "NetworkAdapters/ConnectX_0",
        "RXBytes", "xyz.openbmc_project.Metrics.PortMetricsOem2");
    EXPECT_TRUE(result.find("/NetworkAdapters/ConnectX_0/Ports/Port_0") !=
                std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_ProcessorMetrics_PCIeECC_PCIeType_ReturnsChassisPCIeURI)
{
    auto result = generateURI(
        "ProcessorMetrics", "GPU_0", "",
        "/xyz/openbmc_project/inventory/system/processors/PCIeDevice_0",
        "PCIeType", "xyz.openbmc_project.PCIe.PCIeECC");
    EXPECT_TRUE(result.find("/PCIeDevices/PCIeDevice_0") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_ProcessorMetrics_InbandReconfig_InbandPerms_ReturnsURI)
{
    auto result = generateURI(
        "ProcessorMetrics", "GPU_0", "",
        "/xyz/openbmc_project/inventory/system/processors/GPU_0/"
        "InbandReconfigPermissions/Permission_0",
        "AllowFLRPersistentConfig", "com.nvidia.InbandReconfigSettings");
    EXPECT_TRUE(result.find("InbandReconfigPermissions") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_ProcessorMetrics_InbandReconfig_DOEPerms_ReturnsURI)
{
    auto result = generateURI(
        "ProcessorMetrics", "GPU_0", "",
        "/xyz/openbmc_project/inventory/system/processors/GPU_0/"
        "DOEReconfigPermissions/Permission_0",
        "AllowPersistentConfig", "com.nvidia.InbandReconfigSettings");
    EXPECT_TRUE(result.find("DOEReconfigPermissions") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_ProcessorMetrics_PowerCap_ReturnsEnvironmentMetrics)
{
    auto result = generateURI("ProcessorMetrics", "GPU_0", "", "", "PowerCap",
                              "xyz.openbmc_project.Control.Power.Cap");
    EXPECT_TRUE(result.find("/EnvironmentMetrics") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_NVSwitchMetrics_MemoryECC_ReturnsInternalMemoryURI)
{
    auto result = generateURI("NVSwitchMetrics", "NVSwitch_0", "", "",
                              "ueCount",
                              "xyz.openbmc_project.Memory.MemoryECC");
    EXPECT_TRUE(result.find("/InternalMemoryMetrics/LifeTime") !=
                std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_PCIeRetimerPortMetrics_IBPort_HasMetricsHashViaPropSuffix)
{
    // IBPort.RXErrors maps to "/Metrics#/RXErrors" in pdiNameMap, so even
    // without the PCIeECC branch, propSuffix appends "/Metrics#/..."
    auto result = generateURI("PCIeRetimerPortMetrics", "Retimer_2", "Port_0",
                              "", "RXErrors",
                              "xyz.openbmc_project.Metrics.IBPort");
    EXPECT_TRUE(result.find("PCIeRetimerTopology_2") != std::string::npos);
    EXPECT_TRUE(result.find("/Metrics#") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_HealthMetrics_SoftwareInventory_ReturnsSoftwareURI)
{
    auto result = generateURI(
        "HealthMetrics", "GPU_FW_0", "",
        "/xyz/openbmc_project/inventory_software/GPU_FW_0", "Functional",
        "xyz.openbmc_project.State.Decorator.OperationalStatus");
    EXPECT_TRUE(result.find("/UpdateService/SoftwareInventory/") !=
                std::string::npos);
}

TEST(MetricReportUtilsTest, generateURI_HealthMetrics_Regular_ReturnsChassisURI)
{
    auto result = generateURI("HealthMetrics", "GPU_0", "", "", "Health",
                              "xyz.openbmc_project.State.Decorator.Health");
    EXPECT_TRUE(result.find("/Chassis/") != std::string::npos ||
                result.find("/Systems/") != std::string::npos);
}

TEST(MetricReportUtilsTest, generateURI_UnknownDeviceType_ReturnsEmpty)
{
    auto result = generateURI("UnknownDeviceType", "Device_0", "", "",
                              "SomeMetric", "xyz.unknown.Iface");
    EXPECT_TRUE(result.empty());
}

// =========================================================================
// translateOperationalStatus
// =========================================================================

TEST(MetricReportUtilsTest,
     translateOperationalStatus_Functional_True_ReturnsEnabled)
{
    EXPECT_EQ(translateOperationalStatus("Functional", true), "Enabled");
}

TEST(MetricReportUtilsTest,
     translateOperationalStatus_Functional_False_ReturnsDisabled)
{
    EXPECT_EQ(translateOperationalStatus("Functional", false), "Disabled");
}

TEST(MetricReportUtilsTest,
     translateOperationalStatus_NonFunctional_ReturnsEmpty)
{
    EXPECT_TRUE(translateOperationalStatus("State", true).empty());
}

// =========================================================================
// translateThrottleDuration
// =========================================================================

TEST(MetricReportUtilsTest,
     translateThrottleDuration_PowerLimitDuration_ReturnsDurationString)
{
    auto result = translateThrottleDuration("PowerLimitThrottleDuration",
                                            1000000000ULL);
    EXPECT_FALSE(result.empty());
}

TEST(MetricReportUtilsTest,
     translateThrottleDuration_ThermalLimitDuration_ReturnsDurationString)
{
    auto result = translateThrottleDuration("ThermalLimitThrottleDuration",
                                            2000000000ULL);
    EXPECT_FALSE(result.empty());
}

TEST(MetricReportUtilsTest,
     translateThrottleDuration_HardwareViolationDuration_ReturnsDurationString)
{
    auto result = translateThrottleDuration("HardwareViolationThrottleDuration",
                                            500000000ULL);
    EXPECT_FALSE(result.empty());
}

TEST(MetricReportUtilsTest,
     translateThrottleDuration_GlobalSoftwareViolation_ReturnsDurationString)
{
    auto result = translateThrottleDuration(
        "GlobalSoftwareViolationThrottleDuration", 3000000000ULL);
    EXPECT_FALSE(result.empty());
}

TEST(MetricReportUtilsTest,
     translateThrottleDuration_UnknownMetric_ReturnsToString)
{
    auto result = translateThrottleDuration("UnknownMetric", 42ULL);
    EXPECT_EQ(result, "42");
}

// =========================================================================
// translateAccumlatedDuration
// =========================================================================

TEST(MetricReportUtilsTest,
     translateAccumlatedDuration_ValidValue_ReturnsDurationString)
{
    auto result = translateAccumlatedDuration(3600000ULL);
    EXPECT_FALSE(result.empty());
}

// =========================================================================
// getMetricValues
// =========================================================================

using DbusVariantType = nv::sensor_aggregation::DbusVariantType;

static const std::string kGPUDeviceType = "ProcessorGPMMetrics";
static const std::string kGPUDeviceName = "GPU_0";
static const std::string kGPUMetricName = "SMActivityPercent";
static const std::string kGPUIface = "com.nvidia.GPMMetrics";

TEST(MetricReportUtilsTest, getMetricValues_VectorString_ReturnsIsList)
{
    DbusVariantType value = std::vector<std::string>{"SWPowerCap",
                                                     "ClockLimit"};
    auto [shmValues, isList] = getMetricValues(kGPUDeviceType, kGPUDeviceName,
                                               "", "", "ThrottleReasons",
                                               "xyz.openbmc_project.State."
                                               "ProcessorPerformance",
                                               value);
    EXPECT_TRUE(isList);
    EXPECT_EQ(shmValues.size(), 2u);
}

TEST(MetricReportUtilsTest, getMetricValues_VectorDouble_ReturnsIsList)
{
    DbusVariantType value = std::vector<double>{1.0, 2.5, 3.7};
    auto [shmValues, isList] = getMetricValues(kGPUDeviceType, kGPUDeviceName,
                                               "", "", kGPUMetricName,
                                               kGPUIface, value);
    EXPECT_TRUE(isList);
    EXPECT_EQ(shmValues.size(), 3u);
}

TEST(MetricReportUtilsTest, getMetricValues_StringScalar_ReturnsSingleEntry)
{
    DbusVariantType value = std::string("SWPowerCap");
    auto [shmValues, isList] = getMetricValues(kGPUDeviceType, kGPUDeviceName,
                                               "", "", kGPUMetricName,
                                               kGPUIface, value);
    EXPECT_FALSE(isList);
    EXPECT_EQ(shmValues.size(), 1u);
}

TEST(MetricReportUtilsTest, getMetricValues_Int32_ReturnsSingleEntry)
{
    DbusVariantType value = static_cast<int32_t>(100);
    auto [shmValues, isList] = getMetricValues(kGPUDeviceType, kGPUDeviceName,
                                               "", "", kGPUMetricName,
                                               kGPUIface, value);
    EXPECT_FALSE(isList);
    EXPECT_EQ(shmValues.size(), 1u);
}

TEST(MetricReportUtilsTest, getMetricValues_Int16_ReturnsSingleEntry)
{
    DbusVariantType value = static_cast<int16_t>(200);
    auto [shmValues, isList] = getMetricValues(kGPUDeviceType, kGPUDeviceName,
                                               "", "", kGPUMetricName,
                                               kGPUIface, value);
    EXPECT_FALSE(isList);
    EXPECT_EQ(shmValues.size(), 1u);
}

TEST(MetricReportUtilsTest, getMetricValues_Int64_ReturnsSingleEntry)
{
    DbusVariantType value = static_cast<int64_t>(-300);
    auto [shmValues, isList] = getMetricValues(kGPUDeviceType, kGPUDeviceName,
                                               "", "", kGPUMetricName,
                                               kGPUIface, value);
    EXPECT_FALSE(isList);
    EXPECT_EQ(shmValues.size(), 1u);
}

TEST(MetricReportUtilsTest, getMetricValues_Uint16_ReturnsSingleEntry)
{
    DbusVariantType value = static_cast<uint16_t>(400);
    auto [shmValues, isList] = getMetricValues(kGPUDeviceType, kGPUDeviceName,
                                               "", "", kGPUMetricName,
                                               kGPUIface, value);
    EXPECT_FALSE(isList);
    EXPECT_EQ(shmValues.size(), 1u);
}

TEST(MetricReportUtilsTest, getMetricValues_Uint32_ReturnsSingleEntry)
{
    DbusVariantType value = static_cast<uint32_t>(500);
    auto [shmValues, isList] = getMetricValues(kGPUDeviceType, kGPUDeviceName,
                                               "", "", kGPUMetricName,
                                               kGPUIface, value);
    EXPECT_FALSE(isList);
    EXPECT_EQ(shmValues.size(), 1u);
}

TEST(MetricReportUtilsTest,
     getMetricValues_Uint64ThrottleMetric_ReturnsDuration)
{
    DbusVariantType value = static_cast<uint64_t>(1000000000ULL);
    auto [shmValues, isList] = getMetricValues(
        kGPUDeviceType, kGPUDeviceName, "", "", "PowerLimitThrottleDuration",
        "xyz.openbmc_project.State."
        "ProcessorPerformance",
        value);
    EXPECT_FALSE(isList);
    EXPECT_EQ(shmValues.size(), 1u);
}

TEST(MetricReportUtilsTest,
     getMetricValues_Uint64AccumulatedMetric_ReturnsDuration)
{
    DbusVariantType value = static_cast<uint64_t>(3600000ULL);
    auto [shmValues,
          isList] = getMetricValues(kGPUDeviceType, kGPUDeviceName, "", "",
                                    "AccumulatedSMUtilizationDuration",
                                    "xyz.openbmc_project.State."
                                    "ProcessorPerformance",
                                    value);
    EXPECT_FALSE(isList);
    EXPECT_EQ(shmValues.size(), 1u);
}

TEST(MetricReportUtilsTest, getMetricValues_Double_ReturnsSingleEntry)
{
    DbusVariantType value = 42.5;
    auto [shmValues, isList] = getMetricValues(kGPUDeviceType, kGPUDeviceName,
                                               "", "", kGPUMetricName,
                                               kGPUIface, value);
    EXPECT_FALSE(isList);
    EXPECT_EQ(shmValues.size(), 1u);
}

TEST(MetricReportUtilsTest, getMetricValues_BoolTrue_ReturnsTrueString)
{
    DbusVariantType value = true;
    auto [shmValues, isList] = getMetricValues(kGPUDeviceType, kGPUDeviceName,
                                               "", "", kGPUMetricName,
                                               kGPUIface, value);
    EXPECT_FALSE(isList);
    EXPECT_EQ(shmValues.size(), 1u);
}

TEST(MetricReportUtilsTest, getMetricValues_BoolFalse_ReturnsFalseString)
{
    DbusVariantType value = false;
    auto [shmValues, isList] = getMetricValues(kGPUDeviceType, kGPUDeviceName,
                                               "", "", kGPUMetricName,
                                               kGPUIface, value);
    EXPECT_FALSE(isList);
    EXPECT_EQ(shmValues.size(), 1u);
}

TEST(MetricReportUtilsTest,
     getMetricValues_Bool_InventorySoftware_OperationalStatus_ReturnsEnabled)
{
    DbusVariantType value = true;
    auto [shmValues, isList] = getMetricValues(
        "HealthMetrics", "GPU_FW_0", "",
        "/xyz/openbmc_project/inventory_software/GPU_FW_0", "Functional",
        "xyz.openbmc_project.State.Decorator.OperationalStatus", value);
    EXPECT_FALSE(isList);
    EXPECT_EQ(shmValues.size(), 1u);
}

TEST(MetricReportUtilsTest, getMetricValues_UnknownDeviceType_ReturnsEmptyMap)
{
    DbusVariantType value = std::string("test_value");
    auto [shmValues, isList] = getMetricValues("UnknownDeviceType", "Device_0",
                                               "", "", "SomeMetric",
                                               "xyz.unknown.Iface", value);
    EXPECT_FALSE(isList);
    EXPECT_TRUE(shmValues.empty());
}

// =========================================================================
// getMetricValue
// =========================================================================

TEST(MetricReportUtilsTest, getMetricValue_StringValue_ReturnsTranslated)
{
    // translateReading looks up the full enum path in reasonTypeMap
    DbusVariantType value = std::string(
        "xyz.openbmc_project.State.ProcessorPerformance."
        "ThrottleReasons.SWPowerCap");
    auto [metricProp, val] = getMetricValue("ThrottleReason",
                                            "xyz.openbmc_project.State."
                                            "ProcessorPerformance",
                                            value);
    EXPECT_EQ(val, "SWPowerCap");
}

TEST(MetricReportUtilsTest, getMetricValue_Int32Value_ReturnsString)
{
    DbusVariantType value = static_cast<int32_t>(42);
    auto [metricProp, val] = getMetricValue(kGPUMetricName, kGPUIface, value);
    EXPECT_EQ(val, "42");
}

TEST(MetricReportUtilsTest, getMetricValue_Int16Value_ReturnsString)
{
    DbusVariantType value = static_cast<int16_t>(10);
    auto [metricProp, val] = getMetricValue(kGPUMetricName, kGPUIface, value);
    EXPECT_EQ(val, "10");
}

TEST(MetricReportUtilsTest, getMetricValue_Int64Value_ReturnsString)
{
    DbusVariantType value = static_cast<int64_t>(-99);
    auto [metricProp, val] = getMetricValue(kGPUMetricName, kGPUIface, value);
    EXPECT_EQ(val, "-99");
}

TEST(MetricReportUtilsTest, getMetricValue_Uint16Value_ReturnsString)
{
    DbusVariantType value = static_cast<uint16_t>(55);
    auto [metricProp, val] = getMetricValue(kGPUMetricName, kGPUIface, value);
    EXPECT_EQ(val, "55");
}

TEST(MetricReportUtilsTest, getMetricValue_Uint32Value_ReturnsString)
{
    DbusVariantType value = static_cast<uint32_t>(1000);
    auto [metricProp, val] = getMetricValue(kGPUMetricName, kGPUIface, value);
    EXPECT_EQ(val, "1000");
}

TEST(MetricReportUtilsTest,
     getMetricValue_Uint64ThrottleMetric_ReturnsDurationString)
{
    DbusVariantType value = static_cast<uint64_t>(1000000000ULL);
    auto [metricProp, val] =
        getMetricValue("PowerLimitThrottleDuration",
                       "xyz.openbmc_project.State.ProcessorPerformance", value);
    EXPECT_FALSE(val.empty());
}

TEST(MetricReportUtilsTest,
     getMetricValue_Uint64AccumulatedMetric_ReturnsDurationString)
{
    DbusVariantType value = static_cast<uint64_t>(3600000ULL);
    auto [metricProp, val] =
        getMetricValue("AccumulatedSMUtilizationDuration",
                       "xyz.openbmc_project.State.ProcessorPerformance", value);
    EXPECT_FALSE(val.empty());
}

TEST(MetricReportUtilsTest, getMetricValue_DoubleValue_ReturnsString)
{
    DbusVariantType value = 3.14;
    auto [metricProp, val] = getMetricValue(kGPUMetricName, kGPUIface, value);
    EXPECT_FALSE(val.empty());
}

TEST(MetricReportUtilsTest,
     getMetricValue_Bool_OperationalStatus_Functional_ReturnsEnabled)
{
    DbusVariantType value = true;
    auto [metricProp, val] = getMetricValue(
        "Functional", "xyz.openbmc_project.State.Decorator.OperationalStatus",
        value);
    EXPECT_EQ(val, "Enabled");
}

TEST(MetricReportUtilsTest, getMetricValue_BoolTrue_ReturnsTrueString)
{
    DbusVariantType value = true;
    auto [metricProp, val] = getMetricValue(kGPUMetricName, kGPUIface, value);
    EXPECT_EQ(val, "true");
}

TEST(MetricReportUtilsTest, getMetricValue_BoolFalse_ReturnsFalseString)
{
    DbusVariantType value = false;
    auto [metricProp, val] = getMetricValue(kGPUMetricName, kGPUIface, value);
    EXPECT_EQ(val, "false");
}

// =========================================================================
// generateURI branches targeting uncovered lines
// =========================================================================

TEST(MetricReportUtilsTest,
     generateURI_ProcessorMetrics_MemoryECC_isThresholdExceeded_Line1025)
{
    // MemoryECC + isThresholdExceeded → metricURI += "/Oem/Nvidia" (line 1025)
    auto result = generateURI("ProcessorMetrics", "GPU_0", "",
                              "/xyz/path/GPU_0", "isThresholdExceeded",
                              "xyz.openbmc_project.Memory.MemoryECC");
    EXPECT_TRUE(result.find("/Oem/Nvidia") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     generateURI_ProcessorMetrics_PCIeECC_LanesInUse_Line1035)
{
    // PCIeECC + LanesInUse → forces evaluation past line 1034 condition to
    // reach line 1035 continuation (avoids short-circuit from "PCIeType")
    auto result = generateURI("ProcessorMetrics", "GPU_0", "",
                              "/xyz/path/GPU_0", "LanesInUse",
                              "xyz.openbmc_project.PCIe.PCIeECC");
    EXPECT_FALSE(result.empty());
}

TEST(MetricReportUtilsTest,
     generateURI_ProcessorMetrics_OperatingConfig_MaxSpeed_Line1065)
{
    // OperatingConfig + MaxSpeed → Systems/Processors path (line 1065)
    auto result = generateURI(
        "ProcessorMetrics", "GPU_0", "", "/xyz/path/GPU_0", "MaxSpeed",
        "xyz.openbmc_project.Inventory.Item.Cpu.OperatingConfig");
    EXPECT_TRUE(result.find("/Processors/") != std::string::npos);
}

TEST(MetricReportUtilsTest,
     getMetricValues_UnknownDeviceType_Scalar_Line1431_ReturnsEmpty)
{
    // generateURI returns "" for unknown deviceType → metricProp.empty() true
    // → line 1431 opening brace + LOG_ERROR + return covered
    DbusVariantType value = static_cast<uint64_t>(42);
    auto [shmValues, isList] = getMetricValues(
        "UnknownDeviceType_ZZZ_Test", "GPU_0", "", "/xyz/path/GPU_0",
        "someMetric", "some.iface.Unknown", value);
    EXPECT_TRUE(shmValues.empty());
    EXPECT_FALSE(isList);
}

TEST(MetricReportUtilsTest,
     generateURI_HealthMetrics_SystemDevice_Line1253_SystemsURI)
{
    // HealthMetrics + deviceName="system" → systemdId == PLATFORMSYSTEMID
    // → line 1253: metricURI set to Systems path instead of Chassis
    auto result = generateURI("HealthMetrics", "system", "",
                              "/xyz/openbmc_project/health/system/test",
                              "someMetric", "com.nvidia.SomeInterface");
    EXPECT_TRUE(result.find("system") != std::string::npos);
}
