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

#pragma once
#include "config.h"

#include "port_utils.hpp"
#include "time_utils.hpp"

#include <shm_common.h>

#include <boost/algorithm/string.hpp>

#include <cctype>
#include <regex>
#include <string>
#include <unordered_map>

using namespace std;

namespace nv
{
namespace sensor_aggregation
{
namespace metricUtils
{

// Property data structures for Shared memory updates
using MetricProp = string;
using MetricVal = string;
using SHMKey = string;
using SHMValue = tuple<MetricVal, MetricProp>;
using MetricName = std::string;
using PDIName = std::string;
using MetricNameMap = unordered_map<MetricName, string>;
using PDINameMap = unordered_map<PDIName, MetricNameMap>;

/* Map for last reset type pdi to redfish string */
static unordered_map<string, string> lastResetTypeMap = {
    {"com.nvidia.ResetCounters.ResetCounterMetrics.ResetTypes.PFFLR", "PFFLR"},
    {"com.nvidia.ResetCounters.ResetCounterMetrics.ResetTypes.Conventional",
     "Conventional"},
    {"com.nvidia.ResetCounters.ResetCounterMetrics.ResetTypes.Fundamental",
     "Fundamental"},
    {"com.nvidia.ResetCounters.ResetCounterMetrics.ResetTypes.IRoTReset",
     "IRoTReset"}};

/* Map for reason type PDI to redfish string */
static unordered_map<string, string> reasonTypeMap = {
    {"xyz.openbmc_project.State.ProcessorPerformance."
     "ThrottleReasons.SWPowerCap",
     "SWPowerCap"},
    {"xyz.openbmc_project.State.ProcessorPerformance."
     "ThrottleReasons.HWSlowdown",
     "HWSlowdown"},
    {"xyz.openbmc_project.State.ProcessorPerformance."
     "ThrottleReasons.HWThermalSlowdown",
     "HWThermalSlowdown"},
    {"xyz.openbmc_project.State.ProcessorPerformance."
     "ThrottleReasons.HWPowerBrakeSlowdown",
     "HWPowerBrakeSlowdown"},
    {"xyz.openbmc_project.State.ProcessorPerformance."
     "ThrottleReasons.SyncBoost",
     "SyncBoost"},
    {"xyz.openbmc_project.State.ProcessorPerformance."
     "ThrottleReasons.GPUThermalOvertTreshold",
     "Current GPU temperature above the GPU Max Operating Temperature or "
     "Current memory temperature above the Memory Max Operating "
     "Temperature"},
    {"xyz.openbmc_project.State.ProcessorPerformance.ThrottleReasons.None",
     "NA"},
    {"xyz.openbmc_project.State.ProcessorPerformance.PerformanceStates.Normal",
     "Normal"},
    {"xyz.openbmc_project.State.ProcessorPerformance.PerformanceStates.Throttled",
     "Throttled"},
    {"xyz.openbmc_project.State.ProcessorPerformance.PerformanceStates.Degraded",
     "Degraded"},
    {"xyz.openbmc_project.State.ProcessorPerformance.PerformanceStates.Unknown",
     "Unknown"}};

/* Map to convert pcie type pdi to redfish string */
static unordered_map<string, string> pcieTypeMap = {
    {"xyz.openbmc_project.Inventory.Item.PCIeDevice.PCIeTypes.Gen1", "Gen1"},
    {"xyz.openbmc_project.Inventory.Item.PCIeDevice.PCIeTypes.Gen2", "Gen2"},
    {"xyz.openbmc_project.Inventory.Item.PCIeDevice.PCIeTypes.Gen3", "Gen3"},
    {"xyz.openbmc_project.Inventory.Item.PCIeDevice.PCIeTypes.Gen4", "Gen4"},
    {"xyz.openbmc_project.Inventory.Item.PCIeDevice.PCIeTypes.Gen5", "Gen5"},
    {"xyz.openbmc_project.Inventory.Item.PCIeDevice.PCIeTypes.Gen6", "Gen6"},
    {"xyz.openbmc_project.PCIe.PCIeECC.PCIeTypes.Gen1", "Gen1"},
    {"xyz.openbmc_project.PCIe.PCIeECC.PCIeTypes.Gen2", "Gen2"},
    {"xyz.openbmc_project.PCIe.PCIeECC.PCIeTypes.Gen3", "Gen3"},
    {"xyz.openbmc_project.PCIe.PCIeECC.PCIeTypes.Gen4", "Gen4"},
    {"xyz.openbmc_project.PCIe.PCIeECC.PCIeTypes.Gen5", "Gen5"},
    {"xyz.openbmc_project.PCIe.PCIeECC.PCIeTypes.Gen6", "Gen6"}};

/* Map to convert power state type pdi to redfish string */
static unordered_map<string, string> powerStateTypeMap = {
    {"xyz.openbmc_project.State.Decorator.OperationalStatus."
     "StateType.Absent",
     "Absent"},
    {"xyz.openbmc_project.State.Decorator.OperationalStatus."
     "StateType.Deferring",
     "Deferring"},
    {"xyz.openbmc_project.State.Decorator.OperationalStatus."
     "StateType.Disabled",
     "Disabled"},
    {"xyz.openbmc_project.State.Decorator.OperationalStatus."
     "StateType.Enabled",
     "Enabled"},
    {"xyz.openbmc_project.State.Decorator.OperationalStatus."
     "StateType.StandbyOffline",
     "StandbyOffline"},
    {"xyz.openbmc_project.State.Decorator.OperationalStatus."
     "StateType.Starting",
     "Starting"},
    {"xyz.openbmc_project.State.Decorator.OperationalStatus."
     "StateType.UnavailableOffline",
     "UnavailableOffline"},
    {"xyz.openbmc_project.State.Decorator.OperationalStatus."
     "StateType.Updating",
     "Updating"},
};

/* Map for LinkDownReasonCode pdi to redfish string */
static unordered_map<string, string> linkDownReasonCodeMap = {
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.NoLinkDown",
     "NoLinkDown"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.Unknown",
     "Unknown"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.HighBitErrorRate",
     "HighBitErrorRate"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.BlockLockLost",
     "BlockLockLost"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.AlignmentLost",
     "AlignmentLost"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.FECSyncLost",
     "FECSyncLost"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.PllLockLost",
     "PllLockLost"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.FIFOOverflow",
     "FIFOOverflow"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.FalseSkipDetected",
     "FalseSkipDetected"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.MinorErrorThresholdExceeded",
     "MinorErrorThresholdExceeded"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.PhyRetransmitTimeout",
     "PhyRetransmitTimeout"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.HeartbeatErrors",
     "HeartbeatErrors"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.CreditMonitorWatchdogTimeout",
     "CreditMonitorWatchdogTimeout"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.LinkLayerIntegrityThresholdExceeded",
     "LinkLayerIntegrityThresholdExceeded"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.LinkLayerBufferOverrun",
     "LinkLayerBufferOverrun"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.OOBCommandLinkHealthy",
     "OOBCommandLinkHealthy"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.OOBCommandLinkHighBER",
     "OOBCommandLinkHighBER"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.InbandCommandLinkHealthy",
     "InbandCommandLinkHealthy"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.InbandCommandLinkHighBER",
     "InbandCommandLinkHighBER"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.VerificationGatewayDown",
     "VerificationGatewayDown"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.RemoteFaultReceived",
     "RemoteFaultReceived"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.TrainingSequenceReceived",
     "TrainingSequenceReceived"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.ManagementCommandDown",
     "ManagementCommandDown"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.CableDisconnected",
     "CableDisconnected"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.CableAccessFault",
     "CableAccessFault"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.ThermalShutdown",
     "ThermalShutdown"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.CurrentIssue",
     "CurrentIssue"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.PowerBudgetExceeded",
     "PowerBudgetExceeded"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.FastRawBERRecovery",
     "FastRawBERRecovery"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.FastEffectiveBERRecovery",
     "FastEffectiveBERRecovery"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.FastSymbolBERRecovery",
     "FastSymbolBERRecovery"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.FastCreditWatchdogRecovery",
     "FastCreditWatchdogRecovery"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.PeerSleep",
     "PeerSleep"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.PeerDisabled",
     "PeerDisabled"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.PeerDisableLocked",
     "PeerDisableLocked"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.PeerThermalEvent",
     "PeerThermalEvent"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.PeerForcedEvent",
     "PeerForcedEvent"},
    {"xyz.openbmc_project.Metrics.IBPort.LinkDownReasonCodes.PeerResetEvent",
     "PeerResetEvent"}};

/* Map for MemorySpareChannelPresence pdi to redfish string */
static unordered_map<string, string> presenceStateMap = {
    {"com.nvidia.MemorySpareChannel.Presence.Present", "true"},
    {"com.nvidia.MemorySpareChannel.Presence.NotPresent", "false"},
    {"com.nvidia.MemorySpareChannel.Presence.Unavailable", "null"}};

/* Map for portInfo interface pdi to redfish string based on metric name */
static MetricNameMap portInfoInterfaceMap = {
    {"CurrentSpeed", "#/CurrentSpeedGbps"}, {"MaxSpeed", "#/MaxSpeedGbps"}};

/* Map for portState interface pdi to redfish string based on metric name */
static MetricNameMap portStateInterfaceMap = {{"LinkStatus", "#/LinkStatus"},
                                              {"LinkState", "#/Status/State"}};

/* Map for IBPort interface pdi to redfish string based on metric name */
static MetricNameMap ibPortInterfaceMap = {
    {"RXErrors", "/Metrics#/RXErrors"},
    {"TXPkts", "/Metrics#/Networking/TXFrames"},
    {"RXPkts", "/Metrics#/Networking/RXFrames"},
    {"TXDiscardPkts", "/Metrics#/Networking/TXDiscards"},
    {"MalformedPkts", "/Metrics#/Oem/Nvidia/MalformedPackets"},
    {"VL15DroppedPkts", "/Metrics#/Oem/Nvidia/VL15Dropped"},
    {"VL15TXPkts", "/Metrics#/Oem/Nvidia/VL15TXPackets"},
    {"VL15TXData", "/Metrics#/Oem/Nvidia/VL15TXBytes"},
    {"MTUDiscard", "/Metrics#/Oem/Nvidia/NeighborMTUDiscards"},
    {"SymbolErrors", "/Metrics#/Oem/Nvidia/SymbolErrors"},
    {"LinkErrorRecoveryCounter", "/Metrics#/Oem/Nvidia/LinkErrorRecoveryCount"},
    {"LinkDownCount", "/Metrics#/Oem/Nvidia/LinkDownedCount"},
    {"RXRemotePhysicalErrorPkts",
     "/Metrics#/Oem/Nvidia/RXRemotePhysicalErrors"},
    {"RXSwitchRelayErrorPkts", "/Metrics#/Oem/Nvidia/RXSwitchRelayErrors"},
    {"QP1DroppedPkts", "/Metrics#/Oem/Nvidia/QP1Dropped"},
    {"BitErrorRate", "/Metrics#/Oem/Nvidia/BitErrorRate"},
    {"TotalRawBER", "/Metrics#/Oem/Nvidia/TotalRawBER"},
    {"TotalRawError", "/Metrics#/Oem/Nvidia/TotalRawError"},
    {"LinkDownReasonCode", "/Metrics#/Oem/Nvidia/LinkDownReasonCode"},
    {"IntentionalLinkDownCount",
     "/Metrics#/Oem/Nvidia/IntentionalLinkDownCount"},
    {"UnintentionalLinkDownCount",
     "/Metrics#/Oem/Nvidia/UnintentionalLinkDownCount"},
    {"EffectiveBER", "/Metrics#/Oem/Nvidia/EffectiveBER"},
    {"TXWait", "/Metrics#/Oem/Nvidia/TXWait"},
    {"EffectiveError", "/Metrics#/Oem/Nvidia/EffectiveError"},
    {"IBG2RXPkts", "Metrics#/Oem/Nvidia/RXIbg2Packets"},
    {"IBG2TXPkts", "Metrics#/Oem/Nvidia/TXIbg2Packets"}};

/* Map for EthPort interface pdi to redfish string based on metric name */
static MetricNameMap ethPortInterfaceMap = {
    {"RXFCSErrors", "/Metrics#/Networking/RXFCSErrors"},
    {"RXAlignmentErrors", "/Metrics#/Networking/RXFrameAlignmentErrors"},
    {"RXFalseCarrierDetections", "/Metrics#/Networking/RXFalseCarrierErrors"},
    {"RXRuntPkts", "/Metrics#/Networking/RXUndersizeFrames"},
    {"RXJabberPkts", "/Metrics#/Networking/RXOversizeFrames"},
    {"RXXONFrames", "/Metrics#/Networking/RXPauseXONFrames"},
    {"RXXOFFFrames", "/Metrics#/Networking/RXPauseXOFFFrames"},
    {"TXXONFrames", "/Metrics#/Networking/TXPauseXONFrames"},
    {"TXXOFFFrames", "/Metrics#/Networking/TXPauseXOFFFrames"},
    {"TXSingleCollisionFrames", "/Metrics#/Networking/TXSingleCollisions"},
    {"TXMultipleCollisionFrames", "/Metrics#/Networking/TXMultipleCollisions"},
    {"TXLateCollisionFrames", "/Metrics#/Networking/TXLateCollisions"},
    {"TXExcessCollisionFrames", "/Metrics#/Networking/TXExcessiveCollisions"}};
/* Map for portMetricsOem1 interface pdi to redfish string based on metric name
 */
static MetricNameMap portMetricsOem1InterfaceMap = {
    {"DataCRCCount", "/Metrics#/Oem/Nvidia/NVLinkErrors/DataCRCCount"},
    {"FlitCRCCount", "/Metrics#/Oem/Nvidia/NVLinkErrors/FlitCRCCount"},
    {"RecoveryCount", "/Metrics#/Oem/Nvidia/NVLinkErrors/RecoveryCount"},
    {"ReplayErrorsCount", "/Metrics#/Oem/Nvidia/NVLinkErrors/ReplayCount"}};

/* Map for portMetricsOem2 interface pdi to redfish string based on metric name
 */
static MetricNameMap portMetricsOem2InterfaceMap = {
    {"RXBytes", "/Metrics#/RXBytes"}, {"TXBytes", "/Metrics#/TXBytes"}};

/* Map for portMetricsOem3 interface pdi to redfish string based on metric name
 */
static MetricNameMap portMetricsOem3InterfaceMap = {
    {"RXNoProtocolBytes", "/Metrics#/Oem/Nvidia/RXNoProtocolBytes"},
    {"TXNoProtocolBytes", "/Metrics#/Oem/Nvidia/TXNoProtocolBytes"},
    {"RuntimeError", "/Metrics#/Oem/Nvidia/NVLinkErrors/RuntimeError"},
    {"TrainingError", "/Metrics#/Oem/Nvidia/NVLinkErrors/TrainingError"},
    {"TXWidth", "#/Oem/Nvidia/TXWidth"},
    {"RXWidth", "#/Oem/Nvidia/RXWidth"}};

/* Map for PortPacketCounters interface pdi to redfish string based on metric
 * name
 */
static MetricNameMap portPacketCountersInterfaceMap = {
    {"RXUnicastPkts", "/Metrics#/Networking/RXUnicastFrames"},
    {"RXMulticastPkts", "/Metrics#/Networking/RXMulticastFrames"},
    {"RXBroadcastPkts", "/Metrics#/Networking/RXBroadcastFrames"},
    {"TXUnicastPkts", "/Metrics#/Networking/TXUnicastFrames"},
    {"TXMulticastPkts", "/Metrics#/Networking/TXMulticastFrames"},
    {"TXBroadcastPkts", "/Metrics#/Networking/TXBroadcastFrames"}};

/* Map for processor performance pdi to redfish string based on metric name*/
static MetricNameMap processorPerfMap = {
    {"ThrottleReason", "/Oem/Nvidia/ThrottleReasons"},
    {"PowerLimitThrottleDuration", "/PowerLimitThrottleDuration"},
    {"ThermalLimitThrottleDuration", "/ThermalLimitThrottleDuration"},
    {"AccumulatedSMUtilizationDuration",
     "/Oem/Nvidia/AccumulatedSMUtilizationDuration"},
    {"AccumulatedGPUContextUtilizationDuration",
     "/Oem/Nvidia/AccumulatedGPUContextUtilizationDuration"},
    {"GlobalSoftwareViolationThrottleDuration",
     "/Oem/Nvidia/GlobalSoftwareViolationThrottleDuration"},
    {"HardwareViolationThrottleDuration",
     "/Oem/Nvidia/HardwareViolationThrottleDuration"},
    {"PCIeTXBytes", "/Oem/Nvidia/PCIeTXBytes"},
    {"PCIeRXBytes", "/Oem/Nvidia/PCIeRXBytes"},
    {"Value", "#/Oem/Nvidia/PowerBreakPerformanceState"}};

/* Map for NvLinkMetricsMap pdi to redfish string based on metric name*/
static MetricNameMap nvLinkMetricsMap = {
    {"NVLinkRawTxBandwidthGbps", "/Oem/Nvidia/NVLinkRawTxBandwidthGbps"},
    {"NVLinkRawRxBandwidthGbps", "/Oem/Nvidia/NVLinkRawRxBandwidthGbps"},
    {"NVLinkDataTxBandwidthGbps", "/Oem/Nvidia/NVLinkDataTxBandwidthGbps"},
    {"NVLinkDataRxBandwidthGbps", "/Oem/Nvidia/NVLinkDataRxBandwidthGbps"}};

/* Map for GPMMetrics pdi to redfish string based on metric name*/
static MetricNameMap gpmMetricsMap = {
    {"NVDecInstanceUtilizationPercent",
     "/Oem/Nvidia/NVDecInstanceUtilizationPercent"},
    {"NVJpgInstanceUtilizationPercent",
     "/Oem/Nvidia/NVJpgInstanceUtilizationPercent"},
    {"GraphicsEngineActivityPercent",
     "/Oem/Nvidia/GraphicsEngineActivityPercent"},
    {"SMActivityPercent", "/Oem/Nvidia/SMActivityPercent"},
    {"SMOccupancyPercent", "/Oem/Nvidia/SMOccupancyPercent"},
    {"TensorCoreActivityPercent", "/Oem/Nvidia/TensorCoreActivityPercent"},
    {"FP64ActivityPercent", "/Oem/Nvidia/FP64ActivityPercent"},
    {"FP32ActivityPercent", "/Oem/Nvidia/FP32ActivityPercent"},
    {"FP16ActivityPercent", "/Oem/Nvidia/FP16ActivityPercent"},
    {"NVDecUtilizationPercent", "/Oem/Nvidia/NVDecUtilizationPercent"},
    {"NVJpgUtilizationPercent", "/Oem/Nvidia/NVJpgUtilizationPercent"},
    {"NVOfaUtilizationPercent", "/Oem/Nvidia/NVOfaUtilizationPercent"},
    {"PCIeRawTxBandwidthGbps", "/Oem/Nvidia/PCIeRawTxBandwidthGbps"},
    {"PCIeRawRxBandwidthGbps", "/Oem/Nvidia/PCIeRawRxBandwidthGbps"},
    {"IntegerActivityUtilizationPercent",
     "/Oem/Nvidia/IntegerActivityUtilizationPercent"},
    {"DMMAUtilizationPercent", "/Oem/Nvidia/DMMAUtilizationPercent"},
    {"HMMAUtilizationPercent", "/Oem/Nvidia/HMMAUtilizationPercent"},
    {"IMMAUtilizationPercent", "/Oem/Nvidia/IMMAUtilizationPercent"},
    {"NVEncUtilizationPercent", "/Oem/Nvidia/NVEncUtilizationPercent"},
    {"NVEncInstanceUtilizationPercent",
     "/Oem/Nvidia/NVEncInstanceUtilizationPercent"},
    {"HostMemoryCacheHitPercent", "/Oem/Nvidia/HostMemoryCacheHitPercent"},
    {"HostMemoryCacheMissPercent", "/Oem/Nvidia/HostMemoryCacheMissPercent"},
    {"PeerMemoryCacheHitPercent", "/Oem/Nvidia/PeerMemoryCacheHitPercent"},
    {"PeerMemoryCacheMissPercent", "/Oem/Nvidia/PeerMemoryCacheMissPercent"},
    {"DRAMMemoryCacheHitPercent", "/Oem/Nvidia/DRAMMemoryCacheHitPercent"},
    {"DRAMMemoryCacheMissPercent", "/Oem/Nvidia/DRAMMemoryCacheMissPercent"},
    {"C2CRawTxBandwidthGbps", "/Oem/Nvidia/C2CRawTxBandwidthGbps"},
    {"C2CRawRxBandwidthGbps", "/Oem/Nvidia/C2CRawRxBandwidthGbps"},
    {"C2CDataTxBandwidthGbps", "/Oem/Nvidia/C2CDataTxBandwidthGbps"},
    {"C2CDataRxBandwidthGbps", "/Oem/Nvidia/C2CDataRxBandwidthGbps"},
};

/* Map for ResetCounter MEtrics pdi to redfish string based on metric name*/
static MetricNameMap resetCountersMetricsMap = {
    {"PF_FLR_ResetEntryCount", "/PF_FLR_ResetEntryCount"},
    {"PF_FLR_ResetExitCount", "/PF_FLR_ResetExitCount"},
    {"ConventionalResetEntryCount", "/ConventionalResetEntryCount"},
    {"ConventionalResetExitCount", "/ConventionalResetExitCount"},
    {"FundamentalResetEntryCount", "/FundamentalResetEntryCount"},
    {"FundamentalResetExitCount", "/FundamentalResetExitCount"},
    {"IRoTResetExitCount", "/IRoTResetExitCount"},
    {"LastResetType", "/LastResetType"}};

/* Map for PCIeECC pdi to redfish string based on metric name*/
static MetricNameMap pcieECCMap = {
    {"nonfeCount", "/PCIeErrors/NonFatalErrorCount"},
    {"feCount", "/PCIeErrors/FatalErrorCount"},
    {"ceCount", "/PCIeErrors/CorrectableErrorCount"},
    {"PCIeECC.ceCount", "/PCIeErrors/CorrectableErrorCount"},
    {"L0ToRecoveryCount", "/PCIeErrors/L0ToRecoveryCount"},
    {"NAKReceivedCount", "/PCIeErrors/NAKReceivedCount"},
    {"ReplayCount", "/PCIeErrors/ReplayCount"},
    {"NAKSentCount", "/PCIeErrors/NAKSentCount"},
    {"ReplayRolloverCount", "/PCIeErrors/ReplayRolloverCount"},
    {"UnsupportedRequestCount", "/PCIeErrors/UnsupportedRequestCount"},
    {"PCIeType", "#/PCIeInterface/PCIeType"},
    {"MaxLanes", "#/PCIeInterface/MaxLanes"},
    {"LanesInUse", "#/PCIeInterface/LanesInUse"},
    {"FramingErrorCount", "/Oem/Nvidia/FramingErrorCount"},
    {"LinkDownedCount", "/Oem/Nvidia/LinkDownedCount"},
    {"DLLPCRCErrorCount", "/Oem/Nvidia/DLLPCRCErrorCount"}};

/* Map for PCIeLaneError pdi to redfish string based on metric name*/
static MetricNameMap pcIeLaneErrorMap = {
    {"CDRErrorCount", "/Oem/Nvidia/CDRErrorCount"}};

/* Map for PCIeTransactionCounter pdi to redfish string based on metric name*/
static MetricNameMap pcieTransactionCounterMap = {
    {"OutboundWritePktCount", "/PCIeMetrics/OutboundWriteTLPCount"},
    {"OutboundWriteTransfer", "/PCIeMetrics/OutboundWriteBytes"},
    {"ReqDroppedTag", "/PCIeMetrics/RequestsDroppedTagUnavailable"},
    {"ReqDroppedCreditCompletion",
     "/PCIeMetrics/RequestsDroppedCreditExhaustion"},
    {"ReqDroppedNonPostCredit", "/PCIeMetrics/RequestsDroppedNonPostedCredit"},
    {"OutboundReadPktCount", "/PCIeMetrics/OutboundReadTLPCount"},
    {"OutboundReadTransfer", "/PCIeMetrics/OutboundReadBytes"},
    {"OutboundTLPCount", "/PCIeMetrics/OutboundCompletionTLPCount"},
    {"OutboundTLPsTransfer", "/PCIeMetrics/OutboundCompletionBytes"},
    {"InboundTLPCount", "/Oem/Nvidia/PCIeMetrics/InboundCompletionTLPCount"},
    {"InboundTLPsTransfer", "/Oem/Nvidia/PCIeMetrics/InboundCompletionBytes"}};

/* Map for MemoryECC pdi to redfish string based on metric name*/
static MetricNameMap memoryECCMap = {
    {"ueCount", "/UncorrectableECCErrorCount"},
    {"ceCount", "/CorrectableECCErrorCount"},
    {"isThresholdExceeded", "/SRAMECCErrorThresholdExceeded"}};

/* Map for OperatingConfig pdi to redfish string based on metric name*/
static MetricNameMap operatingConfigMap = {
    {"Utilization", "/BandwidthPercent"},
    {"OperatingSpeed", "/OperatingSpeedMHz"},
    {"MaxSpeed", "/MaxSpeedMHz"},
    {"MinSpeed", "/MinSpeedMHz"},
    {"SpeedLimit", "/SpeedLimitMHz"},
    {"SpeedLocked", "/SpeedLocked"}};

/* Map for DIMM pdi to redfish string based on metric name*/
static MetricNameMap dimmMap = {
    {"MemoryConfiguredSpeedInMhz", "/OperatingSpeedMHz"},
    {"Utilization", "/BandwidthPercent"}};

/* Map for PCIe device pdi to redfish string based on metric name*/
static MetricNameMap pcieDeviceMap = {
    {"PCIeType", "#/PCIeInterface/PCIeType"},
    {"MaxPCIeType", "#/PCIeInterface/MaxPCIeType"},
    {"LanesInUse", "#/PCIeInterface/LanesInUse"},
    {"MaxLanes", "#/PCIeInterface/MaxLanes"}};

/* Map for MemoryRowRemapping pdi to redfish string based on metric name*/
static MetricNameMap memoryRowRemappingMap = {
    {"ueRowRemappingCount",
     "/Oem/Nvidia/RowRemapping/UncorrectableRowRemappingCount"},
    {"ceRowRemappingCount",
     "/Oem/Nvidia/RowRemapping/CorrectableRowRemappingCount"},
    {"RowRemappingFailureState", "/Oem/Nvidia/RowRemappingFailed"},
    {"MaxRemappingAvailablityBankCount",
     "/Oem/Nvidia/RowRemapping/MaxAvailabilityBankCount"},
    {"HighRemappingAvailablityBankCount",
     "/Oem/Nvidia/RowRemapping/HighAvailabilityBankCount"},
    {"PartialRemappingAvailablityBankCount",
     "/Oem/Nvidia/RowRemapping/PartialAvailabilityBankCount"},
    {"LowRemappingAvailablityBankCount",
     "/Oem/Nvidia/RowRemapping/LowAvailabilityBankCount"},
    {"NoRemappingAvailablityBankCount",
     "/Oem/Nvidia/RowRemapping/NoAvailabilityBankCount"},
    {"RowRemappingPendingState", "/Oem/Nvidia/RowRemappingPending"}};

static MetricNameMap capacityUtilizationPercentMap{
    {"CapacityUtilizationPercent", "/CapacityUtilizationPercent"}};

/* Map for SMUtilization pdi to redfish string based on metric name*/
static MetricNameMap SMUtilizationPercentMap{
    {"SMUtilization", "/Oem/Nvidia/SMUtilizationPercent"}};

/* Map for OperationalStatus pdi to redfish string based on metric name*/
static MetricNameMap operationalStatusMap = {{"State", "/Status/State"}};

/* Map for MemorySpareChannel pdi to redfish string based on metric name*/
static MetricNameMap memorySpareChannelMap = {
    {"MemorySpareChannelPresence", "#/Oem/Nvidia/MemorySpareChannelPresence"}};

/* Map for EDPViolationState pdi to redfish string based on metric name*/
static MetricNameMap edpViolationStateMap = {
    {"Status", "#/Oem/Nvidia/EDPViolationState"}};

/* Map for Switch pdi to redfish string based on metric name*/
static MetricNameMap switchInterfaceMap = {
    {"CurrentBandwidth", "/CurrentBandwidthGbps"},
    {"MaxBandwidth", "/MaxBandwidthGbps"}};

/* Map for Power Limit pdi to redfish string based on metric name*/
static MetricNameMap PowerLimitMap = {{"MaxPowerWatts", "/MaxPowerWatts"},
                                      {"MinPowerWatts", "/MinPowerWatts"}};

/* Map for PCIe Reference Clock pdi to redfish string based on metric name*/
static MetricNameMap pcieRefClockMap = {
    {"PCIeReferenceClockEnabled", "#/Oem/Nvidia/PCIeReferenceClockEnabled"}};

/* Map for Port width pdi to redfish string based on metric name*/
static MetricNameMap portWidthInterfaceMap = {{"ActiveWidth", "#/ActiveWidth"}};

/* Map for Health to redfish string based on metric name*/
static MetricNameMap healthInterfaceMap = {{"Health", "#/Status/Health"}};

/* Map for HealthRollup to redfish string based on metric name*/
static MetricNameMap healthRollupInterfaceMap = {
    {"HealthRollup", "#/Status/HealthRollup"}};

/* Map for PowerSmoothing to redfish string based on metric name*/
static MetricNameMap powerSmoothingCurrentPwrProfileInterfaceMap = {
    {"TMPFloorPercent", "/TMPFloorPercent"},
    {"RampDownHysteresis", "/RampDownHysteresisSeconds"},
    {"RampDownRate", "/RampDownWattsPerSecond"},
    {"RampUpRate", "/RampUpWattsPerSecond"},
    {"PrimaryFloorActivationOffset", "/PFAOffsetWatts"},
    {"SecondaryPowerFloorSetting", "/SecondaryPowerFloorWatts"},
    {"PrimaryFloorActivationWindowMultiplier", "/PFAWindowMultiplier"},
    {"PrimaryFloorTargetWindowMultiplier", "/PFTWindowMultiplier"}};

/* Map for PowerSmoothing.CurrentPowerProfile to redfish string based on metric
 * name*/
static MetricNameMap powerSmoothingInterfaceMap = {
    {"PowerSmoothingEnabled", "/Enabled"},
    {"ImmediateRampDownEnabled", "/ImmediateRampDown"},
    {"LifeTimeRemaining", "/RemainingLifetimeCircuitryPercent"},
    {"DelayedPowerSmoothingSupported", "/DelayedPowerSmoothingSupported"},
    {"FloorWindowMultiplier", "/FloorWindowMultiplierPeriod"},
    {"MinPrimaryFloorActivationOffset", "/MinAllowedPFAOffsetWatts"},
    {"MinPrimaryFloorActivationPoint", "/MinAllowedPFAPointWatts"}};

/* Map for DOEReconfigPermissions/InbandReconfigPermissions to redfish string
 * based on metric name*/
static MetricNameMap reConfigPermission = {
    {"AllowFLRPersistentConfig", "/AllowFLRPersistentConfig"},
    {"AllowOneShotConfig", "/AllowOneShotConfig"},
    {"AllowPersistentConfig", "/AllowPersistentConfig"}};

/* This map is for PDI name to metric name. Key is pdi name and value is
 * corresponding metric name map */
static PDINameMap pdiNameMap = {
    {"xyz.openbmc_project.Inventory.Decorator.PortInfo", portInfoInterfaceMap},
    {"xyz.openbmc_project.Inventory.Decorator.PortState",
     portStateInterfaceMap},
    {"xyz.openbmc_project.Metrics.IBPort", ibPortInterfaceMap},
    {"xyz.openbmc_project.Metrics.EthPort", ethPortInterfaceMap},
    {"xyz.openbmc_project.Metrics.PortMetricsOem1",
     portMetricsOem1InterfaceMap},
    {"xyz.openbmc_project.Metrics.PortMetricsOem2",
     portMetricsOem2InterfaceMap},
    {"xyz.openbmc_project.Metrics.PortMetricsOem3",
     portMetricsOem3InterfaceMap},
    {"xyz.openbmc_project.Metrics.PortPacketCounters",
     portPacketCountersInterfaceMap},
    {"xyz.openbmc_project.State.ProcessorPerformance", processorPerfMap},
    {"com.nvidia.NVLink.NVLinkMetrics", nvLinkMetricsMap},
    {"com.nvidia.GPMMetrics", gpmMetricsMap},
    {"com.nvidia.ResetCounters.ResetCounterMetrics", resetCountersMetricsMap},
    {"xyz.openbmc_project.PCIe.PCIeECC", pcieECCMap},
    {"xyz.openbmc_project.PCIe.PCIeLaneError", pcIeLaneErrorMap},
    {"xyz.openbmc_project.PCIe.PCIeTransactionCounter",
     pcieTransactionCounterMap},
    {"xyz.openbmc_project.Inventory.Item.Dimm.MemoryMetrics",
     capacityUtilizationPercentMap},
    {"xyz.openbmc_project.Memory.MemoryECC", memoryECCMap},
    {"xyz.openbmc_project.Inventory.Item.Cpu.OperatingConfig",
     operatingConfigMap},
    {"com.nvidia.SMUtilization", SMUtilizationPercentMap},
    {"xyz.openbmc_project.Inventory.Decorator.PowerLimit", PowerLimitMap},
    {"xyz.openbmc_project.Inventory.Item.Dimm", dimmMap},
    {"xyz.openbmc_project.Inventory.Item.PCIeDevice", pcieDeviceMap},
    {"xyz.openbmc_project.Inventory.Item.Switch", switchInterfaceMap},
    {"xyz.openbmc_project.State.Decorator.OperationalStatus",
     operationalStatusMap},
    {"com.nvidia.MemoryRowRemapping", memoryRowRemappingMap},
    {"com.nvidia.MemorySpareChannel", memorySpareChannelMap},
    {"xyz.openbmc_project.State.Decorator.PowerSystemInputs",
     edpViolationStateMap},
    {"xyz.openbmc_project.Inventory.Decorator.PortWidth",
     portWidthInterfaceMap},
    {"xyz.openbmc_project.Inventory.Decorator.PCIeRefClock", pcieRefClockMap},
    {"xyz.openbmc_project.State.Decorator.Health", healthInterfaceMap},
    {"xyz.openbmc_project.State.Decorator.HealthRollup",
     healthRollupInterfaceMap},
    {"com.nvidia.PowerSmoothing.PowerSmoothing", powerSmoothingInterfaceMap},
    {"com.nvidia.PowerSmoothing.CurrentPowerProfile",
     powerSmoothingCurrentPwrProfileInterfaceMap},
    {"com.nvidia.InbandReconfigSettings", reConfigPermission}};

/**
 * @brief This method will form suffix for redfish URI for device/sub device
 * property.
 *
 * @param[in] ifaceName - pdi name
 * @param[in] metricName - metric name
 * @return string
 */
inline string getPropertySuffix(const string& ifaceName,
                                const string& metricName)
{
    string suffix;
    if (pdiNameMap.find(ifaceName) != pdiNameMap.end())
    {
        if (pdiNameMap[ifaceName].find(metricName) !=
            pdiNameMap[ifaceName].end())
        {
            return pdiNameMap[ifaceName][metricName];
        }
    }
    return suffix;
}

/**
 * @brief Method to get reason type of the metric.
 *
 * @param[in] reason
 * @return string
 */
inline string toReasonType(const string& reason)
{
    if (reasonTypeMap.find(reason) != reasonTypeMap.end())
    {
        return reasonTypeMap[reason];
    }
    return "";
}

/**
 * @brief Method to Get the Last Reset Type redfish string from PDI name.
 *
 * @param[in] lastresetType
 * @return string
 */
inline string getLastResetType(const string& lastResetType)
{
    if (lastResetTypeMap.find(lastResetType) != lastResetTypeMap.end())
    {
        return lastResetTypeMap[lastResetType];
    }
    return "Unknown";
}

/**
 * @brief Method to get pcie type of metric from PDI.
 *
 * @param[in] pcieType
 * @return string
 */
inline string toPCIeType(const string& pcieType)
{
    if (pcieTypeMap.find(pcieType) != pcieTypeMap.end())
    {
        return pcieTypeMap[pcieType];
    }
    // Unknown or others
    return "Unknown";
}

/**
 * @brief Method to get the Link Down Reason Code of metric from PDI .
 *
 * @param[in] linkDownReasonCode
 * @return string
 */
inline string toLinkDownReasonCode(const string& linkDownReasonCode)
{
    if (linkDownReasonCodeMap.find(linkDownReasonCode) !=
        linkDownReasonCodeMap.end())
    {
        return linkDownReasonCodeMap[linkDownReasonCode];
    }
    return "Unknown";
}
/**
 * @brief Method to get the Power State Type of metric from PDI .
 *
 * @param[in] stateType
 * @return string
 */
inline string getPowerStateType(const string& stateType)
{
    if (powerStateTypeMap.find(stateType) != powerStateTypeMap.end())
    {
        return powerStateTypeMap[stateType];
    }
    // Unknown or others
    return "";
}

/**
 * @brief Method to get the Memory Spare Channel presence metric from PDI .
 *
 * @param[in] presence
 * @return string
 */
inline string toPresenceType(const string& presence)
{
    if (presenceStateMap.find(presence) != presenceStateMap.end())
    {
        return presenceStateMap[presence];
    }
    // Unknown or others
    return "";
}

/**
 * @brief Method to get the Processor and cpu number for the device name.
 *
 * @param[in] deviceName
 * @return pair<string, string>
 * e.g For the input "ProcessorModule_1_CPU_0_CoreUtil_64", it will
 * output (1, 0).
 * For the input "ProcessorModule_0_Vreg_0_SocVoltage_0", it will
 * output (0, 0).
 */

inline pair<string, string> getProcessorAndCpuNum(string deviceName)
{
    vector<string> deviceNameKeys;
    boost::trim_if(deviceName, boost::is_any_of("_"));
    boost::split(deviceNameKeys, deviceName, boost::is_any_of("_"));
    if ((deviceNameKeys[1].size() == 1 && isdigit(deviceNameKeys[1][0])) &&
        deviceNameKeys[3].size() == 1 && isdigit(deviceNameKeys[3][0]))
    {
        return make_pair(deviceNameKeys[1], deviceNameKeys[3]);
    }
    return {"", ""};
}

/**
 * @brief This method returns translated string for metric reading values based
 * on PDI and metric name.
 *
 * @param[in] ifaceName - pdi
 * @param metricName - metric name
 * @param[in] reading - metric reading value
 * @return string
 */
inline string translateReading(const string& ifaceName,
                               const string& metricName, const string& reading)
{
    string metricValue;
    if (ifaceName == "xyz.openbmc_project.State.ProcessorPerformance")
    {
        if (metricName == "ThrottleReason")
        {
            metricValue = toReasonType(reading);
        }
        else if (metricName == "Value")
        {
            metricValue = toReasonType(reading);
        }
    }
    else if (ifaceName == "xyz.openbmc_project.PCIe.PCIeECC")
    {
        if (metricName == "PCIeType")
        {
            metricValue = toPCIeType(reading);
        }
    }
    else if (ifaceName == "xyz.openbmc_project.Inventory.Decorator.PortState")
    {
        if (metricName == "LinkStatus")
        {
            metricValue = getLinkStatusType(reading);
            // Nvlink Status.Health update
            if (metricValue == "LinkDown" || metricValue == "LinkUp")
            {
                metricValue = "OK";
            }
            else if (metricValue == "NoLink")
            {
                metricValue = "Critical";
            }
            else
            {
                metricValue = "";
            }
        }
        if (metricName == "LinkState")
        {
            metricValue = getLinkStateType(reading);
        }
    }
    else if (ifaceName ==
             "xyz.openbmc_project.State.Decorator.PowerSystemInputs")
    {
        if (metricName == "Status")
        {
            metricValue = getPowerSystemInputType(reading);
        }
    }
    else if (ifaceName ==
             "xyz.openbmc_project.State.Decorator.OperationalStatus")
    {
        if (metricName == "State")
        {
            metricValue = getPowerStateType(reading);
        }
    }
    else if (ifaceName == "com.nvidia.ResetCounters.ResetCounterMetrics")
    {
        if (metricName == "LastResetType")
        {
            metricValue = getLastResetType(reading);
        }
    }
    else if (ifaceName == "xyz.openbmc_project.Inventory.Item.PCIeDevice")
    {
        if (metricName == "MaxPCIeType" || metricName == "PCIeType")
        {
            metricValue = toPCIeType(reading);
        }
    }
    else if (ifaceName == "xyz.openbmc_project.Metrics.IBPort")
    {
        if (metricName == "LinkDownReasonCode")
        {
            metricValue = toLinkDownReasonCode(reading);
        }
    }
    else if (ifaceName == "com.nvidia.MemorySpareChannel")
    {
        if (metricName == "MemorySpareChannelPresence")
        {
            metricValue = toPresenceType(reading);
        }
    }
    else
    {
        metricValue = reading;
    }
    return metricValue;
}

/**
 * @brief This method will form suffix for redfish URI for device/sub device
 * property from device path.
 *
 * @param[in] ifaceName - pdi name
 * @param[in] devicePath - device path
 * @return string
 */
inline string getPropertySuffixFromPath(const string& ifaceName,
                                        const string& devicePath)
{
    string suffix;
    if (ifaceName == "xyz.openbmc_project.Metric.Value")
    {
        if (devicePath.find("PowerBRKAssertionTime") != std::string::npos)
        {
            suffix = "#/Oem/Nvidia/PowerBrakeAssertionDuration";
        }
        else if (devicePath.find("PageRetirementCount") != std::string::npos)
        {
            suffix = "#/Oem/Nvidia/MemoryPageRetirementCount";
        }
        else if (devicePath.find("TjMaxDramIndex") != std::string::npos)
        {
            suffix = "#/Oem/Nvidia/TjMaxDramIndex";
        }
        else if (devicePath.find("CpuUptime") != std::string::npos)
        {
            suffix = "#/Oem/Nvidia/CPUptime";
        }
    }
    return suffix;
}
/**
 * @brief Method to generate metric property uri from namespace, devicename and
 * other properties.
 *
 * @param[in] deviceType
 * @param[in] deviceName
 * @param[in] subDeviceName
 * @param[in] devicePath
 * @param[in] metricName
 * @param[in] ifaceName
 * @return string
 */
inline string generateURI(const string& deviceType, const string& deviceName,
                          const string& subDeviceName, const string& devicePath,
                          const string& metricName, const string& ifaceName)
{
    string metricURI;
    string propSuffix;
    // form redfish URI for sub device
    if (deviceType == "PlatformEnvironmentMetrics")
    {
        metricURI = "/redfish/v1/Chassis/";
        metricURI += deviceName;
        metricURI += "/Sensors/";
        metricURI += subDeviceName;
    }
    else if (deviceType == "CpuProcessorMetrics")
    {
        if (ifaceName == "xyz.openbmc_project.Sensor.Value")
        {
            metricURI = "/redfish/v1/Chassis/" PLATFORMDEVICEPREFIX;
            metricURI += deviceName;
            metricURI += "/Sensors/";
            metricURI += subDeviceName;
        }
        else if (ifaceName == "xyz.openbmc_project.Metric.Value")
        {
            metricURI = "/redfish/v1/Systems/" PLATFORMSYSTEMID;
            metricURI += "/Processors/";
            metricURI += deviceName;
            metricURI += "/ProcessorMetrics";
            propSuffix = getPropertySuffixFromPath(ifaceName, devicePath);
        }
        else if (ifaceName == "com.nvidia.MemorySpareChannel" ||
                 ifaceName ==
                     "xyz.openbmc_project.State.Decorator.PowerSystemInputs" ||
                 ifaceName == "xyz.openbmc_project.State.ProcessorPerformance")
        {
            metricURI = "/redfish/v1/Systems/" PLATFORMSYSTEMID;
            metricURI += "/Processors/";
            metricURI += deviceName;
            metricURI += "/ProcessorMetrics";
            propSuffix = getPropertySuffix(ifaceName, metricName);
        }
        else if (ifaceName ==
                 "xyz.openbmc_project.Inventory.Decorator.PortState")
        {
            if (metricName == "LinkStatus")
            {
                metricURI = "/redfish/v1/Systems/" PLATFORMSYSTEMID;
                metricURI += "/Processors/";
                metricURI += deviceName;
                metricURI += "/Ports/";
                metricURI += subDeviceName;
                metricURI += "#/Status/Health";
            }
            else if (metricName == "LinkState")
            {
                metricURI = "/redfish/v1/Systems/" PLATFORMSYSTEMID;
                metricURI += "/Processors/";
                metricURI += deviceName;
                metricURI += "/Ports/";
                metricURI += subDeviceName;
                metricURI += "#/Status/State";
            }
        }
        else
        {
            metricURI = "/redfish/v1/Systems/" PLATFORMSYSTEMID;
            metricURI += "/Processors/";
            metricURI += deviceName;
            metricURI += "/Ports/";
            metricURI += subDeviceName;
            propSuffix = getPropertySuffix(ifaceName, metricName);
        }
    }
    else if (deviceType == "ProcessorPortMetrics")
    {
        metricURI = "/redfish/v1/Systems/" PLATFORMSYSTEMID;
        metricURI += "/Processors/";
        metricURI += deviceName;
        metricURI += "/Ports/";
        metricURI += subDeviceName;
        propSuffix = getPropertySuffix(ifaceName, metricName);
    }
    else if (deviceType == "ProcessorPortGPMMetrics")
    {
        metricURI = "/redfish/v1/Systems/" PLATFORMSYSTEMID;
        metricURI += "/Processors/";
        metricURI += deviceName;
        metricURI += "/Ports/";
        metricURI += subDeviceName;
        metricURI += "/Metrics#";
        propSuffix = getPropertySuffix(ifaceName, metricName);
    }
    else if (deviceType == "NVSwitchPortMetrics")
    {
        metricURI = "/redfish/v1/Fabrics/" PLATFORMDEVICEPREFIX;
        metricURI += "NVLinkFabric_0/Switches/";
        metricURI += deviceName;
        metricURI += "/Ports/";
        metricURI += subDeviceName;
        propSuffix = getPropertySuffix(ifaceName, metricName);
    }
    else if (deviceType == "NetworkAdapterPortMetrics")
    {
        std::regex chassisRegex("chassis/(.*)/NetworkAdapters");
        std::smatch smatch;
        metricURI = "/redfish/v1/Chassis/";
        if (std::regex_search(devicePath, smatch, chassisRegex))
        {
            metricURI += smatch[1];
        }
        else
        {
            metricURI += PLATFORMDEVICEPREFIX + deviceName;
        }
        metricURI += "/NetworkAdapters/";
        metricURI += deviceName;
        metricURI += "/Ports/";
        metricURI += subDeviceName;
        propSuffix = getPropertySuffix(ifaceName, metricName);
    }
    else if (deviceType == "ProcessorMetrics")
    {
        metricURI = "/redfish/v1/Systems/" PLATFORMSYSTEMID;
        metricURI += "/Processors/";
        metricURI += deviceName;
        metricURI += "/ProcessorMetrics#";
        if (ifaceName == "xyz.openbmc_project.Memory.MemoryECC")
        {
            if (metricName == "isThresholdExceeded")
            {
                metricURI += "/Oem/Nvidia";
            }
            else
            {
                metricURI += "/CacheMetricsTotal/LifeTime";
            }
        }
        else if (ifaceName == "xyz.openbmc_project.PCIe.PCIeECC")
        {
            if (metricName == "PCIeType" || metricName == "MaxLanes" ||
                metricName == "LanesInUse")
            {
                sdbusplus::message::object_path deviceObjectPath(devicePath);
                const string childDeviceName = deviceObjectPath.filename();
                string parentDeviceName = PLATFORMDEVICEPREFIX;
                parentDeviceName += childDeviceName;
                metricURI = "/redfish/v1/Chassis/";
                metricURI += parentDeviceName;
                metricURI += "/PCIeDevices/";
                metricURI += childDeviceName;
            }
        }
        else if (ifaceName ==
                 "xyz.openbmc_project.State.Decorator.OperationalStatus")
        {
            metricURI = "/redfish/v1/Systems/" PLATFORMSYSTEMID;
            metricURI += "/Processors/";
            metricURI += deviceName;
            metricURI += "#";
        }
        else if (ifaceName ==
                 "xyz.openbmc_project.Inventory.Decorator.PowerLimit")
        {
            metricURI = "/redfish/v1/Chassis/" PLATFORMDEVICEPREFIX;
            metricURI += deviceName;
            metricURI += "#";
        }
        else if (ifaceName ==
                 "xyz.openbmc_project.Inventory.Item.Cpu.OperatingConfig")
        {
            if (metricName == "MaxSpeed" || metricName == "MinSpeed" ||
                metricName == "SpeedLimit" || metricName == "SpeedLocked")
            {
                metricURI = "/redfish/v1/Systems/" PLATFORMSYSTEMID;
                metricURI += "/Processors/";
                metricURI += deviceName;
                metricURI += "#";
            }
        }
        else if (ifaceName == "com.nvidia.PowerSmoothing.CurrentPowerProfile" ||
                 ifaceName == "com.nvidia.PowerSmoothing.PowerSmoothing")
        {
            metricURI = "/redfish/v1/Systems/" PLATFORMSYSTEMID;
            metricURI += "/Processors/";
            metricURI += deviceName;
            metricURI += "/Oem/Nvidia/PowerSmoothing";
            metricURI += "#";
        }
        else if (ifaceName == "com.nvidia.InbandReconfigSettings")
        {
            if (metricName == "AllowFLRPersistentConfig" ||
                metricName == "AllowOneShotConfig" ||
                metricName == "AllowPersistentConfig")
            {
                if (devicePath.find("InbandReconfigPermissions") !=
                    std::string::npos)
                {
                    sdbusplus::message::object_path deviceObjectPath(
                        devicePath);
                    const string childDeviceName = deviceObjectPath.filename();
                    metricURI = "/redfish/v1/Systems/" PLATFORMSYSTEMID;
                    metricURI += "/Processors/";
                    metricURI += string(deviceObjectPath.parent_path()
                                            .parent_path()
                                            .filename());
                    metricURI += "#";
                    metricURI += "/Oem/Nvidia/InbandReconfigPermissions/";
                    metricURI += childDeviceName;
                }
                else if (devicePath.find("DOEReconfigPermissions") !=
                         std::string::npos)
                {
                    sdbusplus::message::object_path deviceObjectPath(
                        devicePath);
                    const string childDeviceName = deviceObjectPath.filename();
                    metricURI = "/redfish/v1/Systems/" PLATFORMSYSTEMID;
                    metricURI += "/Processors/";
                    metricURI += string(deviceObjectPath.parent_path()
                                            .parent_path()
                                            .filename());
                    metricURI += "#";
                    metricURI += "/Oem/Nvidia/DOEReconfigPermissions/";
                    metricURI += childDeviceName;
                }
            }
        }

        propSuffix = getPropertySuffix(ifaceName, metricName);
    }
    else if (deviceType == "ProcessorGPMMetrics")
    {
        metricURI = "/redfish/v1/Systems/" PLATFORMSYSTEMID;
        metricURI += "/Processors/";
        metricURI += deviceName;
        metricURI += "/ProcessorMetrics#";
        propSuffix = getPropertySuffix(ifaceName, metricName);
    }
    else if (deviceType == "ProcessorResetMetrics")
    {
        metricURI = "/redfish/v1/Systems/" PLATFORMSYSTEMID;
        metricURI += "/Processors/";
        metricURI += deviceName;
        metricURI += "/Oem/Nvidia";
        metricURI += "/ProcessorResetMetrics#";
        propSuffix = getPropertySuffix(ifaceName, metricName);
    }
    else if (deviceType == "NVSwitchMetrics")
    {
        metricURI = "/redfish/v1/Fabrics/" PLATFORMDEVICEPREFIX;
        metricURI += "NVLinkFabric_0/Switches/";
        metricURI += deviceName;
        if (!(metricName == "CurrentBandwidth" || metricName == "MaxBandwidth"))
        {
            metricURI += "/SwitchMetrics#";
        }
        else
        {
            metricURI += "#";
        }
        if (ifaceName == "xyz.openbmc_project.Memory.MemoryECC")
        {
            metricURI += "/InternalMemoryMetrics/LifeTime";
        }
        propSuffix = getPropertySuffix(ifaceName, metricName);
    }
    else if (deviceType == "PCIeRetimerMetrics")
    {
        metricURI = "/redfish/v1/Chassis/";
        metricURI += deviceName;
        metricURI += "/PCIeDevices/";
        metricURI += subDeviceName;
        propSuffix = getPropertySuffix(ifaceName, metricName);
    }
    else if (deviceType == "PCIeRetimerPortMetrics")
    {
        size_t pos = deviceName.rfind('_');
        std::string retimerID = "0";
        if (pos != std::string::npos)
        {
            retimerID = deviceName.substr(pos + 1);
        }

        metricURI = "/redfish/v1/Fabrics/" PLATFORMDEVICEPREFIX;
        metricURI += "PCIeRetimerTopology_" + retimerID;
        metricURI += "/Switches/";
        metricURI += deviceName;
        metricURI += "/Ports/";
        metricURI += subDeviceName;
        if (ifaceName == "xyz.openbmc_project.PCIe.PCIeECC" ||
            ifaceName == "xyz.openbmc_project.PCIe.PCIeTransactionCounter" ||
            ifaceName == "xyz.openbmc_project.PCIe.PCIeLaneError")
        {
            metricURI += "/Metrics#";
        }

        propSuffix = getPropertySuffix(ifaceName, metricName);
    }
    else if (deviceType == "MemoryMetrics")
    {
        metricURI = "/redfish/v1/Systems/" PLATFORMSYSTEMID;
        metricURI += "/Memory/";
        metricURI += deviceName;
        if (ifaceName == "com.nvidia.MemoryRowRemapping")
        {
            if (metricName == "RowRemappingFailureState" ||
                metricName == "RowRemappingPendingState")
            {
                metricURI += "#";
            }
            else
            {
                metricURI += "/MemoryMetrics#";
            }
        }
        else if (ifaceName ==
                 "xyz.openbmc_project.Inventory.Item.Dimm.MemoryMetrics")
        {
            if (metricName == "CapacityUtilizationPercent")
            {
                metricURI += "/MemoryMetrics#";
            }
        }
        else if (ifaceName == "xyz.openbmc_project.Memory.MemoryECC")
        {
            metricURI += "/MemoryMetrics#/LifeTime";
        }
        else
        {
            metricURI += "/MemoryMetrics#";
        }
        propSuffix = getPropertySuffix(ifaceName, metricName);
    }
    else if (deviceType == "HealthMetrics")
    {
        if (devicePath.find("inventory_software") != std::string::npos &&
            ifaceName ==
                "xyz.openbmc_project.State.Decorator.OperationalStatus")
        {
            if (metricName == "Functional")
            {
                metricURI = "/redfish/v1/UpdateService/SoftwareInventory/";
                metricURI += deviceName;
                metricURI += "#/Status/State";
            }
        }
        else
        {
            metricURI = "/redfish/v1/Chassis/" PLATFORMDEVICEPREFIX;
            std::string systemdId = PLATFORMDEVICEPREFIX + deviceName;
            if (systemdId == PLATFORMSYSTEMID)
            {
                metricURI = "/redfish/v1/Systems/" PLATFORMDEVICEPREFIX;
            }
            metricURI += deviceName;
            propSuffix = getPropertySuffix(ifaceName, metricName);
        }
    }
    else
    {
        metricURI.clear();
    }

    if (!propSuffix.empty())
    {
        metricURI += propSuffix;
    }
    else
    {
        if (!((deviceType != "PlatformEnvironmentMetrics") ||
              (deviceType != "CpuProcessorMetrics")))
        {
            metricURI.clear();
        }
    }
    return metricURI;
}

/**
 * @brief Method to translate D-Bus OperationalStatus to redfish
 * OperationalStatus
 *
 * @param[in] metricName
 * @param[in] reading
 * @return string
 */
inline string translateOperationalStatus(const string& metricName,
                                         const bool& reading)
{
    string metricValue;
    if (metricName == "Functional")
    {
        metricValue = "Disabled";
        if (reading == true)
        {
            metricValue = "Enabled";
        }
    }
    return metricValue;
}

/**
 * @brief Method to translate D-Bus throttle reason to redfish ThrottleDuration
 *
 * @param[in] metricName
 * @param[in] reading
 * @return string
 */
inline string translateThrottleDuration(const string& metricName,
                                        const uint64_t& reading)
{
    string metricValue;
    if ((metricName == "PowerLimitThrottleDuration") ||
        (metricName == "ThermalLimitThrottleDuration") ||
        (metricName == "HardwareViolationThrottleDuration") ||
        (metricName == "GlobalSoftwareViolationThrottleDuration"))
    {
        optional<string> duration =
            nv::sensor_aggregation::metricUtils::toDurationStringFromNano(
                reading);

        if (duration)
        {
            metricValue = *duration;
        }
    }
    else
    {
        metricValue = to_string(reading);
    }
    return metricValue;
}
/**
 * @brief Method to translate D-Bus AccumlatedDuration redfish AccumlatedDuratio
 *
 * @param reading
 * @return string
 */
inline string translateAccumlatedDuration(const uint64_t& reading)
{
    std::string metricValue;
    std::optional<std::string> duration =
        nv::sensor_aggregation::metricUtils::toDurationStringFromUint(reading);
    if (duration)
    {
        metricValue = *duration;
    }

    return metricValue;
}

/**
 * @brief This method returns metric values for each of namespaces and device
 * name for simple and array data types. Ouput will map of be a shared memory
 * key and value. Value contains metric property, translated value and
 * timestamp. This method should be used during discovery or for array value
 * updates.
 *
 * @param[in] deviceType
 * @param[in] deviceName
 * @param[in] subDeviceName
 * @param[in] devicePath
 * @param[in] metricName
 * @param[in] ifaceName
 * @param[in] value
 * @return pair<unordered_map<SHMKey, SHMValue>, bool>
 */
inline pair<unordered_map<SHMKey, SHMValue>, bool>
    getMetricValues(const string& deviceType, const string& deviceName,
                    const string& subDeviceName, const string& devicePath,
                    const string& metricName, const string& ifaceName,
                    DbusVariantType& value)
{
    unordered_map<SHMKey, SHMValue> shmValues;
    bool isList = false;
    if (const vector<string>* readingArray = get_if<vector<string>>(&value))
    {
        // This is for the property whose value is of type list and each element
        // in the list on the redfish is represented with
        // "PropertyName/<index_of_list_element>". and it always starts with 0
        // Eg:- ThrottleReasosns: [Idle, AppClock]-> "Idle" maps to
        // ThrottleReasons/0
        isList = true;
        int i = 0;
        for (const string& reading : *readingArray)
        {
            string val = translateReading(ifaceName, metricName, reading);
            string metricProp = generateURI(deviceType, deviceName,
                                            subDeviceName, devicePath,
                                            metricName, ifaceName);
            metricProp += "/";
            metricProp += to_string(i);
            string sensorKey = devicePath + "/" + ifaceName + "." + metricName +
                               "/" + to_string(i);
            SHMValue shmValue = {metricProp, val};
            shmValues.emplace(sensorKey, shmValue);
            i++;
        }
    }
    else if (const vector<double>* readingArray =
                 get_if<vector<double>>(&value))
    {
        // This is for the property whose value is of type list and each element
        // in the list on the redfish is represented with
        // "PropertyName/<index_of_list_element>". and it always starts with 0
        isList = true;
        int i = 0;
        for (const double& reading : *readingArray)
        {
            string val = to_string(reading);
            string metricProp = generateURI(deviceType, deviceName,
                                            subDeviceName, devicePath,
                                            metricName, ifaceName);
            metricProp += "/";
            metricProp += to_string(i);
            string sensorKey = devicePath + "/" + ifaceName + "." + metricName +
                               "/" + to_string(i);
            SHMValue shmValue = {metricProp, val};
            shmValues.emplace(sensorKey, shmValue);
            i++;
        }
    }
    else
    {
        const string metricProp = generateURI(deviceType, deviceName,
                                              subDeviceName, devicePath,
                                              metricName, ifaceName);
        if (metricProp.empty())
        {
            string errorMessage =
                "SHMEMDEBUG: Metric Property Empty for deviceType " +
                deviceType + " deviceName " + deviceName + " subDeviceName " +
                subDeviceName + " devicePath " + devicePath + " metricName " +
                metricName + " ifaceName " + ifaceName;
            LOG_ERROR(errorMessage);
            return {shmValues, isList};
        }
        string val;
        if (const string* reading = get_if<string>(&value))
        {
            val = translateReading(ifaceName, metricName, *reading);
        }
        else if (const int* reading = get_if<int>(&value))
        {
            val = to_string(*reading);
        }
        else if (const int16_t* reading = get_if<int16_t>(&value))
        {
            val = to_string(*reading);
        }
        else if (const int64_t* reading = get_if<int64_t>(&value))
        {
            val = to_string(*reading);
        }
        else if (const uint16_t* reading = get_if<uint16_t>(&value))
        {
            val = to_string(*reading);
        }
        else if (const uint32_t* reading = get_if<uint32_t>(&value))
        {
            val = to_string(*reading);
        }
        else if (const uint64_t* reading = get_if<uint64_t>(&value))
        {
            if ((ifaceName ==
                 "xyz.openbmc_project.State.ProcessorPerformance") &&
                ((metricName == "AccumulatedSMUtilizationDuration") ||
                 (metricName == "AccumulatedGPUContextUtilizationDuration")))
            {
                val = translateAccumlatedDuration(*reading);
            }
            else
            {
                val = translateThrottleDuration(metricName, *reading);
            }
        }
        else if (const double* reading = get_if<double>(&value))
        {
            val = to_string(*reading);
        }
        else if (const bool* reading = get_if<bool>(&value))
        {
            if (devicePath.find("inventory_software") != std::string::npos &&
                ifaceName ==
                    "xyz.openbmc_project.State.Decorator.OperationalStatus")
            {
                val = translateOperationalStatus(metricName, *reading);
            }
            else
            {
                val = "false";
                if (*reading == true)
                {
                    val = "true";
                }
            }
        }
        string sensorKey = devicePath + "/" + ifaceName + "." + metricName;
        SHMValue shmValue = {metricProp, val};
        shmValues.emplace(sensorKey, shmValue);
    }
    return {shmValues, isList};
}

/**
 * @brief This method returns metric value for namespaces and device
 * name for simple and array data types. Ouput will be a shared memory key and
 * value. Value contains metric property, translated value and timestamp. This
 * method should be used during value and timestamp and nan updates.
 *
 * @param[in] metricName
 * @param[in] ifaceName
 * @param[in] value
 * @return SHMValue
 */
inline SHMValue getMetricValue(const string& metricName,
                               const string& ifaceName, DbusVariantType& value)
{
    string val;
    if (const string* reading = get_if<string>(&value))
    {
        val = translateReading(ifaceName, metricName, *reading);
    }
    else if (const int* reading = get_if<int>(&value))
    {
        val = to_string(*reading);
    }
    else if (const int16_t* reading = get_if<int16_t>(&value))
    {
        val = to_string(*reading);
    }
    else if (const int64_t* reading = get_if<int64_t>(&value))
    {
        val = to_string(*reading);
    }
    else if (const uint16_t* reading = get_if<uint16_t>(&value))
    {
        val = to_string(*reading);
    }
    else if (const uint32_t* reading = get_if<uint32_t>(&value))
    {
        val = to_string(*reading);
    }
    else if (const uint64_t* reading = get_if<uint64_t>(&value))
    {
        if ((ifaceName == "xyz.openbmc_project.State.ProcessorPerformance") &&
            ((metricName == "AccumulatedSMUtilizationDuration") ||
             (metricName == "AccumulatedGPUContextUtilizationDuration")))
        {
            val = translateAccumlatedDuration(*reading);
        }
        else
        {
            val = translateThrottleDuration(metricName, *reading);
        }
    }
    else if (const double* reading = get_if<double>(&value))
    {
        val = to_string(*reading);
    }
    else if (const bool* reading = get_if<bool>(&value))
    {
        if (ifaceName ==
            "xyz.openbmc_project.State.Decorator.OperationalStatus")
        {
            val = translateOperationalStatus(metricName, *reading);
        }
        else
        {
            val = "false";
            if (*reading == true)
            {
                val = "true";
            }
        }
    }
    SHMValue shmValue = {"", val};
    return shmValue;
}

} // namespace metricUtils
} // namespace sensor_aggregation
} // namespace nv
