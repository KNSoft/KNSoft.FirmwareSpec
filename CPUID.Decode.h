/*
 * KNSoft.FirmwareSpec (https://github.com/KNSoft/KNSoft.FirmwareSpec)
 *
 * Minimal CPUID execution and feature decoding.
 *
 * Licensed under the MIT license.
 * Copyright (c) KNSoft.org (https://github.com/KNSoft). All rights reserved.
 */

#pragma once

#include "CPUID.h"
#include "CPUID.TypeInfo.h"

#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
#include <cpuid.h>
#endif

typedef bool (*CPUID_FEATURE_VISITOR)(const CPUID_BIT_INFO* Feature, void* Context);

typedef struct _CPUID_VERSION_INFO
{
    uint8_t Stepping;
    uint8_t Model;
    uint8_t Family;
    uint8_t ProcessorType;
    uint8_t ExtendedModel;
    uint8_t ExtendedFamily;
    uint16_t DisplayModel;
    uint16_t DisplayFamily;
} CPUID_VERSION_INFO;

static inline
FIRMWARE_DECODE_STATUS
CpuidExecute(
    uint32_t Leaf,
    uint32_t SubLeaf,
    CPUID_INFO* Info)
{
    if (Info == NULL)
    {
        return FirmwareDecodeInvalidArgument;
    }
#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
    __cpuidex(Info->Registers, (int)Leaf, (int)SubLeaf);
    return FirmwareDecodeSuccess;
#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
    __cpuid_count(Leaf,
                  SubLeaf,
                  Info->Registers[0],
                  Info->Registers[1],
                  Info->Registers[2],
                  Info->Registers[3]);
    return FirmwareDecodeSuccess;
#else
    (void)Leaf;
    (void)SubLeaf;
    return FirmwareDecodeUnsupported;
#endif
}

static inline
FIRMWARE_DECODE_STATUS
CpuidGetVendor(
    char Vendor[13],
    uint32_t* MaximumLeaf)
{
    CPUID_INFO Info;
    FIRMWARE_DECODE_STATUS Status;

    if (Vendor == NULL || MaximumLeaf == NULL)
    {
        return FirmwareDecodeInvalidArgument;
    }
    Status = CpuidExecute(0, 0, &Info);
    if (Status != FirmwareDecodeSuccess)
    {
        return Status;
    }

    *MaximumLeaf = (uint32_t)Info.Registers[0];
    memcpy(Vendor, &Info.Registers[1], 4);
    memcpy(Vendor + 4, &Info.Registers[3], 4);
    memcpy(Vendor + 8, &Info.Registers[2], 4);
    Vendor[12] = 0;
    return FirmwareDecodeSuccess;
}

static inline
void
CpuidDecodeVersion(
    uint32_t Value,
    CPUID_VERSION_INFO* Version)
{
    Version->Stepping = (uint8_t)FirmwareExtractBits(Value, 0, 4);
    Version->Model = (uint8_t)FirmwareExtractBits(Value, 4, 4);
    Version->Family = (uint8_t)FirmwareExtractBits(Value, 8, 4);
    Version->ProcessorType = (uint8_t)FirmwareExtractBits(Value, 12, 2);
    Version->ExtendedModel = (uint8_t)FirmwareExtractBits(Value, 16, 4);
    Version->ExtendedFamily = (uint8_t)FirmwareExtractBits(Value, 20, 8);
    Version->DisplayFamily = Version->Family == 0xF ?
        (uint16_t)(Version->Family + Version->ExtendedFamily) : Version->Family;
    Version->DisplayModel = Version->Family == 0x6 || Version->Family == 0xF ?
        (uint16_t)(Version->Model | Version->ExtendedModel << 4) : Version->Model;
}

static inline
FIRMWARE_DECODE_STATUS
CpuidVisitFeatures(
    CPUID_FEATURE_VISITOR Visitor,
    void* Context)
{
    CPUID_INFO Info;
    uint32_t CurrentLeaf = UINT32_MAX;
    uint32_t CurrentSubLeaf = UINT32_MAX;
    uint32_t MaximumLeaf;
    uint32_t MaximumStructuredSubLeaf = UINT32_MAX;
    char Vendor[13];
    size_t Index;
    bool HasInfo = false;
    FIRMWARE_DECODE_STATUS Status;

    if (Visitor == NULL)
    {
        return FirmwareDecodeInvalidArgument;
    }
    Status = CpuidGetVendor(Vendor, &MaximumLeaf);
    if (Status != FirmwareDecodeSuccess)
    {
        return Status;
    }

    for (Index = 0; Index < sizeof(CpuidFeatureInfo) / sizeof(CpuidFeatureInfo[0]); Index++)
    {
        const CPUID_BIT_INFO* Feature = &CpuidFeatureInfo[Index];

        if (Feature->Leaf > MaximumLeaf)
        {
            continue;
        }
        if (Feature->Leaf == 7 && Feature->SubLeaf != 0)
        {
            if (MaximumStructuredSubLeaf == UINT32_MAX)
            {
                Status = CpuidExecute(7, 0, &Info);
                if (Status != FirmwareDecodeSuccess)
                {
                    return Status;
                }
                MaximumStructuredSubLeaf = (uint32_t)Info.Registers[0];
            }
            if (Feature->SubLeaf > MaximumStructuredSubLeaf)
            {
                continue;
            }
        }
        if (!HasInfo || Feature->Leaf != CurrentLeaf || Feature->SubLeaf != CurrentSubLeaf)
        {
            Status = CpuidExecute(Feature->Leaf, Feature->SubLeaf, &Info);
            if (Status != FirmwareDecodeSuccess)
            {
                return Status;
            }
            CurrentLeaf = Feature->Leaf;
            CurrentSubLeaf = Feature->SubLeaf;
            HasInfo = true;
        }
        if (((uint32_t)Info.Registers[Feature->Register] & (UINT32_C(1) << Feature->Bit)) != 0 &&
            !Visitor(Feature, Context))
        {
            break;
        }
    }
    return FirmwareDecodeSuccess;
}
