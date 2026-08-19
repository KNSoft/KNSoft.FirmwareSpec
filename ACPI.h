/*
 * KNSoft.FirmwareSpec (https://github.com/KNSoft/KNSoft.FirmwareSpec)
 *
 * Definitions for ACPI Specification 6.6.
 * Complex AML and OEM-defined table bodies are intentionally left as raw data.
 *
 * Licensed under the MIT license.
 * Copyright (c) KNSoft.org (https://github.com/KNSoft). All rights reserved.
 */

#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#define ACPI_VERSION 0x06060000
#define ACPI_SIGNATURE(A, B, C, D) \
    ((uint32_t)(A) | (uint32_t)(B) << 8 | (uint32_t)(C) << 16 | (uint32_t)(D) << 24)

#define ACPI_SIGNATURE_APIC ACPI_SIGNATURE('A', 'P', 'I', 'C')
#define ACPI_SIGNATURE_BGRT ACPI_SIGNATURE('B', 'G', 'R', 'T')
#define ACPI_SIGNATURE_DSDT ACPI_SIGNATURE('D', 'S', 'D', 'T')
#define ACPI_SIGNATURE_FACP ACPI_SIGNATURE('F', 'A', 'C', 'P')
#define ACPI_SIGNATURE_HPET ACPI_SIGNATURE('H', 'P', 'E', 'T')
#define ACPI_SIGNATURE_HMAT ACPI_SIGNATURE('H', 'M', 'A', 'T')
#define ACPI_SIGNATURE_IORT ACPI_SIGNATURE('I', 'O', 'R', 'T')
#define ACPI_SIGNATURE_MCFG ACPI_SIGNATURE('M', 'C', 'F', 'G')
#define ACPI_SIGNATURE_NFIT ACPI_SIGNATURE('N', 'F', 'I', 'T')
#define ACPI_SIGNATURE_PCCT ACPI_SIGNATURE('P', 'C', 'C', 'T')
#define ACPI_SIGNATURE_PPTT ACPI_SIGNATURE('P', 'P', 'T', 'T')
#define ACPI_SIGNATURE_RSDT ACPI_SIGNATURE('R', 'S', 'D', 'T')
#define ACPI_SIGNATURE_SLIT ACPI_SIGNATURE('S', 'L', 'I', 'T')
#define ACPI_SIGNATURE_SRAT ACPI_SIGNATURE('S', 'R', 'A', 'T')
#define ACPI_SIGNATURE_SSDT ACPI_SIGNATURE('S', 'S', 'D', 'T')
#define ACPI_SIGNATURE_TPM2 ACPI_SIGNATURE('T', 'P', 'M', '2')
#define ACPI_SIGNATURE_WAET ACPI_SIGNATURE('W', 'A', 'E', 'T')
#define ACPI_SIGNATURE_WSMT ACPI_SIGNATURE('W', 'S', 'M', 'T')
#define ACPI_SIGNATURE_XSDT ACPI_SIGNATURE('X', 'S', 'D', 'T')

#pragma pack(push, 1)

typedef struct _ACPI_ROOT_SYSTEM_DESCRIPTION_POINTER
{
    uint8_t Signature[8];
    uint8_t Checksum;
    uint8_t OemId[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
    uint32_t Length;
    uint64_t XsdtAddress;
    uint8_t ExtendedChecksum;
    uint8_t Reserved[3];
} ACPI_ROOT_SYSTEM_DESCRIPTION_POINTER;

typedef struct _ACPI_DESCRIPTION_HEADER
{
    uint32_t Signature;
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    uint8_t OemId[6];
    uint8_t OemTableId[8];
    uint32_t OemRevision;
    uint8_t CreatorId[4];
    uint32_t CreatorRevision;
} ACPI_DESCRIPTION_HEADER;

typedef struct _ACPI_GENERIC_ADDRESS
{
    uint8_t AddressSpaceId;
    uint8_t RegisterBitWidth;
    uint8_t RegisterBitOffset;
    uint8_t AccessSize;
    uint64_t Address;
} ACPI_GENERIC_ADDRESS;

typedef struct _ACPI_RSDT
{
    ACPI_DESCRIPTION_HEADER Header;
    uint32_t TableAddresses[];
} ACPI_RSDT;

typedef struct _ACPI_XSDT
{
    ACPI_DESCRIPTION_HEADER Header;
    uint64_t TableAddresses[];
} ACPI_XSDT;

typedef struct _ACPI_FACS
{
    uint32_t Signature;
    uint32_t Length;
    uint32_t HardwareSignature;
    uint32_t FirmwareWakingVector;
    uint32_t GlobalLock;
    uint32_t Flags;
    uint64_t XFirmwareWakingVector;
    uint8_t Version;
    uint8_t Reserved[3];
    uint32_t OspmFlags;
    uint8_t Reserved1[24];
} ACPI_FACS;

typedef struct _ACPI_FADT
{
    ACPI_DESCRIPTION_HEADER Header;
    uint32_t FirmwareControl;
    uint32_t Dsdt;
    uint8_t Reserved0;
    uint8_t PreferredPmProfile;
    uint16_t SciInterrupt;
    uint32_t SmiCommand;
    uint8_t AcpiEnable;
    uint8_t AcpiDisable;
    uint8_t S4BiosRequest;
    uint8_t PstateControl;
    uint32_t Pm1aEventBlock;
    uint32_t Pm1bEventBlock;
    uint32_t Pm1aControlBlock;
    uint32_t Pm1bControlBlock;
    uint32_t Pm2ControlBlock;
    uint32_t PmTimerBlock;
    uint32_t Gpe0Block;
    uint32_t Gpe1Block;
    uint8_t Pm1EventLength;
    uint8_t Pm1ControlLength;
    uint8_t Pm2ControlLength;
    uint8_t PmTimerLength;
    uint8_t Gpe0BlockLength;
    uint8_t Gpe1BlockLength;
    uint8_t Gpe1Base;
    uint8_t CstateControl;
    uint16_t C2Latency;
    uint16_t C3Latency;
    uint16_t FlushSize;
    uint16_t FlushStride;
    uint8_t DutyOffset;
    uint8_t DutyWidth;
    uint8_t DayAlarm;
    uint8_t MonthAlarm;
    uint8_t Century;
    uint16_t IaPcBootArchitecture;
    uint8_t Reserved1;
    uint32_t Flags;
    ACPI_GENERIC_ADDRESS ResetRegister;
    uint8_t ResetValue;
    uint16_t ArmBootArchitecture;
    uint8_t MinorVersion;
    uint64_t XFirmwareControl;
    uint64_t XDsdt;
    ACPI_GENERIC_ADDRESS XPm1aEventBlock;
    ACPI_GENERIC_ADDRESS XPm1bEventBlock;
    ACPI_GENERIC_ADDRESS XPm1aControlBlock;
    ACPI_GENERIC_ADDRESS XPm1bControlBlock;
    ACPI_GENERIC_ADDRESS XPm2ControlBlock;
    ACPI_GENERIC_ADDRESS XPmTimerBlock;
    ACPI_GENERIC_ADDRESS XGpe0Block;
    ACPI_GENERIC_ADDRESS XGpe1Block;
    ACPI_GENERIC_ADDRESS SleepControlRegister;
    ACPI_GENERIC_ADDRESS SleepStatusRegister;
    uint64_t HypervisorVendorIdentity;
} ACPI_FADT;

typedef struct _ACPI_SUBTABLE_HEADER
{
    uint8_t Type;
    uint8_t Length;
} ACPI_SUBTABLE_HEADER;

typedef struct _ACPI_MADT
{
    ACPI_DESCRIPTION_HEADER Header;
    uint32_t LocalApicAddress;
    uint32_t Flags;
    uint8_t Entries[];
} ACPI_MADT;

typedef struct _ACPI_MADT_LOCAL_APIC
{
    ACPI_SUBTABLE_HEADER Header;
    uint8_t ProcessorId;
    uint8_t ApicId;
    uint32_t Flags;
} ACPI_MADT_LOCAL_APIC;

typedef struct _ACPI_MADT_IO_APIC
{
    ACPI_SUBTABLE_HEADER Header;
    uint8_t IoApicId;
    uint8_t Reserved;
    uint32_t IoApicAddress;
    uint32_t GlobalSystemInterruptBase;
} ACPI_MADT_IO_APIC;

typedef struct _ACPI_MADT_INTERRUPT_OVERRIDE
{
    ACPI_SUBTABLE_HEADER Header;
    uint8_t Bus;
    uint8_t Source;
    uint32_t GlobalSystemInterrupt;
    uint16_t Flags;
} ACPI_MADT_INTERRUPT_OVERRIDE;

typedef struct _ACPI_MADT_LOCAL_X2APIC
{
    ACPI_SUBTABLE_HEADER Header;
    uint16_t Reserved;
    uint32_t X2ApicId;
    uint32_t Flags;
    uint32_t AcpiProcessorUid;
} ACPI_MADT_LOCAL_X2APIC;

typedef struct _ACPI_HPET
{
    ACPI_DESCRIPTION_HEADER Header;
    uint32_t EventTimerBlockId;
    ACPI_GENERIC_ADDRESS BaseAddress;
    uint8_t HpetNumber;
    uint16_t MinimumClockTick;
    uint8_t PageProtection;
} ACPI_HPET;

typedef struct _ACPI_MCFG
{
    ACPI_DESCRIPTION_HEADER Header;
    uint64_t Reserved;
    uint8_t Allocations[];
} ACPI_MCFG;

typedef struct _ACPI_MCFG_ALLOCATION
{
    uint64_t BaseAddress;
    uint16_t PciSegmentGroup;
    uint8_t StartBusNumber;
    uint8_t EndBusNumber;
    uint32_t Reserved;
} ACPI_MCFG_ALLOCATION;

typedef struct _ACPI_BGRT
{
    ACPI_DESCRIPTION_HEADER Header;
    uint16_t Version;
    uint8_t Status;
    uint8_t ImageType;
    uint64_t ImageAddress;
    uint32_t ImageOffsetX;
    uint32_t ImageOffsetY;
} ACPI_BGRT;

typedef struct _ACPI_TPM2
{
    ACPI_DESCRIPTION_HEADER Header;
    uint16_t PlatformClass;
    uint16_t Reserved;
    uint64_t ControlArea;
    uint32_t StartMethod;
    uint8_t StartMethodParameters[];
} ACPI_TPM2;

typedef struct _ACPI_WAET
{
    ACPI_DESCRIPTION_HEADER Header;
    uint32_t Flags;
} ACPI_WAET;

typedef struct _ACPI_WSMT
{
    ACPI_DESCRIPTION_HEADER Header;
    uint32_t ProtectionFlags;
} ACPI_WSMT;

typedef struct _ACPI_SRAT
{
    ACPI_DESCRIPTION_HEADER Header;
    uint32_t Reserved0;
    uint64_t Reserved1;
    uint8_t Entries[];
} ACPI_SRAT;

typedef struct _ACPI_SLIT
{
    ACPI_DESCRIPTION_HEADER Header;
    uint64_t LocalityCount;
    uint8_t Entry[];
} ACPI_SLIT;

#pragma pack(pop)

#if defined(__cplusplus)
static_assert(sizeof(ACPI_ROOT_SYSTEM_DESCRIPTION_POINTER) == 36);
static_assert(sizeof(ACPI_DESCRIPTION_HEADER) == 36);
static_assert(sizeof(ACPI_GENERIC_ADDRESS) == 12);
static_assert(sizeof(ACPI_FACS) == 64);
static_assert(sizeof(ACPI_FADT) == 276);
#else
_Static_assert(sizeof(ACPI_ROOT_SYSTEM_DESCRIPTION_POINTER) == 36, "Invalid RSDP layout");
_Static_assert(sizeof(ACPI_DESCRIPTION_HEADER) == 36, "Invalid ACPI header layout");
_Static_assert(sizeof(ACPI_GENERIC_ADDRESS) == 12, "Invalid GAS layout");
_Static_assert(sizeof(ACPI_FACS) == 64, "Invalid FACS layout");
_Static_assert(sizeof(ACPI_FADT) == 276, "Invalid FADT layout");
#endif
