/*
 * KNSoft.FirmwareSpec (https://github.com/KNSoft/KNSoft.FirmwareSpec)
 *
 * Type information for commonly used ACPI 6.6 tables.
 *
 * Licensed under the MIT license.
 * Copyright (c) KNSoft.org (https://github.com/KNSoft). All rights reserved.
 */

#pragma once

#include "ACPI.h"
#include "FirmwareDecode.h"

typedef struct _ACPI_TABLE_INFO
{
    uint32_t Signature;
    FIRMWARE_UTF8_STRING Name;
    uint16_t MinimumSize;
    uint16_t FieldCount;
    const FIRMWARE_FIELD_INFO* Fields;
    uint8_t RawBody;
} ACPI_TABLE_INFO;

#define ACPI_FIELD(Type, Field, Name, DataType) \
    { u8##Name, false, (uint16_t)offsetof(Type, Field), \
      (uint8_t)sizeof(((Type*)0)->Field), DataType, { 0, NULL } }
#define ACPI_FIELD_UINT(Type, Field, Name) ACPI_FIELD(Type, Field, Name, FirmwareDataTypeUInt)
#define ACPI_FIELD_RAW(Type, Field, Name) ACPI_FIELD(Type, Field, Name, FirmwareDataTypeRaw)
#define ACPI_FIELDS(Fields) (uint16_t)(sizeof(Fields) / sizeof((Fields)[0])), Fields
#define ACPI_TABLE(Signature, Name, Type, Fields) \
    { Signature, u8##Name, (uint16_t)sizeof(Type), ACPI_FIELDS(Fields), false }
#define ACPI_RAW_TABLE(Signature, Name) \
    { Signature, u8##Name, (uint16_t)sizeof(ACPI_DESCRIPTION_HEADER), 0, NULL, true }

static const FIRMWARE_FIELD_INFO AcpiHeaderFields[] = {
    ACPI_FIELD_UINT(ACPI_DESCRIPTION_HEADER, Length, "Length"),
    ACPI_FIELD_UINT(ACPI_DESCRIPTION_HEADER, Revision, "Revision"),
    ACPI_FIELD_UINT(ACPI_DESCRIPTION_HEADER, Checksum, "Checksum"),
    ACPI_FIELD_RAW(ACPI_DESCRIPTION_HEADER, OemId, "OEM ID"),
    ACPI_FIELD_RAW(ACPI_DESCRIPTION_HEADER, OemTableId, "OEM Table ID"),
    ACPI_FIELD_UINT(ACPI_DESCRIPTION_HEADER, OemRevision, "OEM Revision"),
    ACPI_FIELD_RAW(ACPI_DESCRIPTION_HEADER, CreatorId, "Creator ID"),
    ACPI_FIELD_UINT(ACPI_DESCRIPTION_HEADER, CreatorRevision, "Creator Revision"),
};

static const FIRMWARE_FIELD_INFO AcpiFadtFields[] = {
    ACPI_FIELD_UINT(ACPI_FADT, FirmwareControl, "Firmware Control"),
    ACPI_FIELD_UINT(ACPI_FADT, Dsdt, "DSDT"),
    ACPI_FIELD_UINT(ACPI_FADT, PreferredPmProfile, "Preferred PM Profile"),
    ACPI_FIELD_UINT(ACPI_FADT, SciInterrupt, "SCI Interrupt"),
    ACPI_FIELD_UINT(ACPI_FADT, SmiCommand, "SMI Command"),
    ACPI_FIELD_UINT(ACPI_FADT, Flags, "Flags"),
    ACPI_FIELD_UINT(ACPI_FADT, ResetValue, "Reset Value"),
    ACPI_FIELD_UINT(ACPI_FADT, XFirmwareControl, "Extended Firmware Control"),
    ACPI_FIELD_UINT(ACPI_FADT, XDsdt, "Extended DSDT"),
    ACPI_FIELD_UINT(ACPI_FADT, HypervisorVendorIdentity, "Hypervisor Vendor Identity"),
};

static const FIRMWARE_FIELD_INFO AcpiMadtFields[] = {
    ACPI_FIELD_UINT(ACPI_MADT, LocalApicAddress, "Local APIC Address"),
    ACPI_FIELD_UINT(ACPI_MADT, Flags, "Flags"),
};

static const FIRMWARE_FIELD_INFO AcpiHpetFields[] = {
    ACPI_FIELD_UINT(ACPI_HPET, EventTimerBlockId, "Event Timer Block ID"),
    ACPI_FIELD_UINT(ACPI_HPET, HpetNumber, "HPET Number"),
    ACPI_FIELD_UINT(ACPI_HPET, MinimumClockTick, "Minimum Clock Tick"),
    ACPI_FIELD_UINT(ACPI_HPET, PageProtection, "Page Protection"),
};

static const FIRMWARE_FIELD_INFO AcpiBgrtFields[] = {
    ACPI_FIELD_UINT(ACPI_BGRT, Version, "Version"),
    ACPI_FIELD_UINT(ACPI_BGRT, Status, "Status"),
    ACPI_FIELD_UINT(ACPI_BGRT, ImageType, "Image Type"),
    ACPI_FIELD_UINT(ACPI_BGRT, ImageAddress, "Image Address"),
    ACPI_FIELD_UINT(ACPI_BGRT, ImageOffsetX, "Image Offset X"),
    ACPI_FIELD_UINT(ACPI_BGRT, ImageOffsetY, "Image Offset Y"),
};

static const FIRMWARE_FIELD_INFO AcpiTpm2Fields[] = {
    ACPI_FIELD_UINT(ACPI_TPM2, PlatformClass, "Platform Class"),
    ACPI_FIELD_UINT(ACPI_TPM2, ControlArea, "Control Area"),
    ACPI_FIELD_UINT(ACPI_TPM2, StartMethod, "Start Method"),
};

static const FIRMWARE_FIELD_INFO AcpiWaetFields[] = {
    ACPI_FIELD_UINT(ACPI_WAET, Flags, "Flags"),
};

static const FIRMWARE_FIELD_INFO AcpiWsmtFields[] = {
    ACPI_FIELD_UINT(ACPI_WSMT, ProtectionFlags, "Protection Flags"),
};

static const FIRMWARE_FIELD_INFO AcpiSlitFields[] = {
    ACPI_FIELD_UINT(ACPI_SLIT, LocalityCount, "Locality Count"),
};

static const ACPI_TABLE_INFO AcpiTableInfo[] = {
    ACPI_TABLE(ACPI_SIGNATURE_APIC, "Multiple APIC Description Table", ACPI_MADT, AcpiMadtFields),
    ACPI_TABLE(ACPI_SIGNATURE_BGRT, "Boot Graphics Resource Table", ACPI_BGRT, AcpiBgrtFields),
    ACPI_RAW_TABLE(ACPI_SIGNATURE_DSDT, "Differentiated System Description Table (AML)"),
    { ACPI_SIGNATURE_FACP, u8"Fixed ACPI Description Table",
      (uint16_t)offsetof(ACPI_FADT, ResetRegister),
      ACPI_FIELDS(AcpiFadtFields), false },
    ACPI_RAW_TABLE(ACPI_SIGNATURE_HMAT, "Heterogeneous Memory Attribute Table"),
    ACPI_TABLE(ACPI_SIGNATURE_HPET, "High Precision Event Timer Table", ACPI_HPET, AcpiHpetFields),
    ACPI_RAW_TABLE(ACPI_SIGNATURE_IORT, "IO Remapping Table"),
    { ACPI_SIGNATURE_MCFG, u8"PCI Express Memory Mapped Configuration Table",
      (uint16_t)sizeof(ACPI_MCFG), 0, NULL, false },
    ACPI_RAW_TABLE(ACPI_SIGNATURE_NFIT, "NVDIMM Firmware Interface Table"),
    ACPI_RAW_TABLE(ACPI_SIGNATURE_PCCT, "Platform Communications Channel Table"),
    ACPI_RAW_TABLE(ACPI_SIGNATURE_PPTT, "Processor Properties Topology Table"),
    { ACPI_SIGNATURE_RSDT, u8"Root System Description Table",
      (uint16_t)sizeof(ACPI_DESCRIPTION_HEADER), 0, NULL, false },
    ACPI_TABLE(ACPI_SIGNATURE_SLIT, "System Locality Information Table", ACPI_SLIT, AcpiSlitFields),
    { ACPI_SIGNATURE_SRAT, u8"System Resource Affinity Table",
      (uint16_t)sizeof(ACPI_SRAT), 0, NULL, false },
    ACPI_RAW_TABLE(ACPI_SIGNATURE_SSDT, "Secondary System Description Table (AML)"),
    ACPI_TABLE(ACPI_SIGNATURE_TPM2, "Trusted Computing Platform 2 Table", ACPI_TPM2, AcpiTpm2Fields),
    ACPI_TABLE(ACPI_SIGNATURE_WAET, "Windows ACPI Emulated Devices Table", ACPI_WAET, AcpiWaetFields),
    ACPI_TABLE(ACPI_SIGNATURE_WSMT, "Windows SMM Security Mitigations Table", ACPI_WSMT, AcpiWsmtFields),
    { ACPI_SIGNATURE_XSDT, u8"Extended System Description Table",
      (uint16_t)sizeof(ACPI_DESCRIPTION_HEADER), 0, NULL, false },
};

static inline
const ACPI_TABLE_INFO*
AcpiFindTableInfo(
    uint32_t Signature)
{
    size_t Index;

    for (Index = 0; Index < sizeof(AcpiTableInfo) / sizeof(AcpiTableInfo[0]); Index++)
    {
        if (AcpiTableInfo[Index].Signature == Signature)
        {
            return &AcpiTableInfo[Index];
        }
    }
    return NULL;
}

#undef ACPI_RAW_TABLE
#undef ACPI_TABLE
#undef ACPI_FIELDS
#undef ACPI_FIELD_RAW
#undef ACPI_FIELD_UINT
#undef ACPI_FIELD
