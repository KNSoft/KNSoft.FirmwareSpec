#include "ACPI.Decode.h"
#include "CPUID.Decode.h"
#include "SMBIOS.Decode.h"

#include <assert.h>
#include <stdio.h>

static
void
SetChecksum(
    void* Data,
    size_t Size,
    size_t ChecksumOffset)
{
    uint8_t* Bytes = (uint8_t*)Data;
    uint8_t Sum = 0;
    size_t Index;

    Bytes[ChecksumOffset] = 0;
    for (Index = 0; Index < Size; Index++)
    {
        Sum = (uint8_t)(Sum + Bytes[Index]);
    }
    Bytes[ChecksumOffset] = (uint8_t)(0 - Sum);
}

static
void
TestFirmwareBuffer()
{
    uint8_t Data[4] = { 1, 2, 3, 4 };
    FIRMWARE_BUFFER Buffer = { Data, sizeof(Data) };
    uint64_t Value;

    assert(FirmwareBuffer_Get(&Buffer, 1, 3) == Data + 1);
    assert(FirmwareBuffer_Get(&Buffer, 2, 3) == NULL);
    assert(FirmwareBuffer_Get(&Buffer, SIZE_MAX, 1) == NULL);
    assert(FirmwareBuffer_ReadUInt(&Buffer, 0, 4, &Value));
    assert(Value == UINT32_C(0x04030201));
    assert(FirmwareExtractBits(UINT64_C(0xF0), 4, 4) == 0xF);
}

static
void
TestSmbios()
{
    const uint8_t Data[] = {
        1, 4, 0x34, 0x12, 'A', 0, 0,
        127, 4, 0xFF, 0xFF, 0, 0,
    };
    const uint8_t InvalidLength[] = { 1, 3, 0, 0, 0, 0 };
    const uint8_t MissingTerminator[] = { 1, 4, 0, 0, 'A', 0 };
    FIRMWARE_BUFFER Buffer = { Data, sizeof(Data) };
    FIRMWARE_BUFFER InvalidBuffer = { InvalidLength, sizeof(InvalidLength) };
    FIRMWARE_BUFFER MissingTerminatorBuffer = { MissingTerminator, sizeof(MissingTerminator) };
    SMBIOS_STRUCTURE_VIEW View;
    FIRMWARE_DECODE_STATUS Status;
    const char* String;
    size_t Length;
    size_t Offset = 0;

    assert(SmbiosNextStructure(&Buffer, &Offset, &View) == FirmwareDecodeSuccess);
    assert(View.Header->Type == 1 && View.Header->Handle == 0x1234);
    assert(SmbiosGetString(&View, 1, &String, &Length) == FirmwareDecodeSuccess);
    assert(Length == 1 && String[0] == 'A');
    assert(SmbiosNextStructure(&Buffer, &Offset, &View) == FirmwareDecodeSuccess);
    assert(View.Header->Type == 127);
    assert(SmbiosNextStructure(&Buffer, &Offset, &View) == FirmwareDecodeEnd);

    Offset = 0;
    Status = SmbiosNextStructure(&InvalidBuffer, &Offset, &View);
    assert(Status == FirmwareDecodeInvalidLength);

    Offset = 0;
    Status = SmbiosNextStructure(&MissingTerminatorBuffer, &Offset, &View);
    assert(Status == FirmwareDecodeMissingTerminator);
    assert(FirmwareFindEnum(SmbiosMemoryDeviceTechnologyEnum,
                            sizeof(SmbiosMemoryDeviceTechnologyEnum) / sizeof(SmbiosMemoryDeviceTechnologyEnum[0]),
                            SMBIOS_MEMORY_DEVICE_TECHNOLOGY_MRDIMM) != NULL);
}

static
void
TestAcpi()
{
    ACPI_DESCRIPTION_HEADER Header = { 0 };
    ACPI_TABLE_VIEW View;
    FIRMWARE_BUFFER Buffer = { (const uint8_t*)&Header, sizeof(Header) };
    ACPI_ROOT_SYSTEM_DESCRIPTION_POINTER Rsdp = { 0 };
    FIRMWARE_BUFFER RsdpBuffer = { (const uint8_t*)&Rsdp, sizeof(Rsdp) };
    uint8_t MadtData[sizeof(ACPI_MADT) + sizeof(ACPI_SUBTABLE_HEADER)] = { 0 };
    ACPI_MADT* Madt = (ACPI_MADT*)MadtData;
    ACPI_SUBTABLE_HEADER* Subtable = (ACPI_SUBTABLE_HEADER*)(Madt + 1);
    FIRMWARE_BUFFER MadtBuffer = { MadtData, sizeof(MadtData) };
    ACPI_SUBTABLE_VIEW SubtableView;
    size_t Offset = 0;

    Header.Signature = ACPI_SIGNATURE_DSDT;
    Header.Length = sizeof(Header);
    Header.Revision = 2;
    SetChecksum(&Header, sizeof(Header), offsetof(ACPI_DESCRIPTION_HEADER, Checksum));
    assert(AcpiValidateTable(&Buffer, true, &View) == FirmwareDecodeSuccess);
    assert(View.Info != NULL && View.Info->RawBody);

    Header.Length++;
    assert(AcpiValidateTable(&Buffer, false, &View) == FirmwareDecodeTruncatedData);

    memcpy(Rsdp.Signature, "RSD PTR ", 8);
    memcpy(Rsdp.OemId, "KNSOFT", 6);
    Rsdp.Revision = 2;
    Rsdp.Length = sizeof(Rsdp);
    SetChecksum(&Rsdp, 20, offsetof(ACPI_ROOT_SYSTEM_DESCRIPTION_POINTER, Checksum));
    SetChecksum(&Rsdp, sizeof(Rsdp), offsetof(ACPI_ROOT_SYSTEM_DESCRIPTION_POINTER, ExtendedChecksum));
    {
        const ACPI_ROOT_SYSTEM_DESCRIPTION_POINTER* Value;
        assert(AcpiValidateRsdp(&RsdpBuffer, &Value) == FirmwareDecodeSuccess);
        assert(Value == &Rsdp);
    }

    Madt->Header.Signature = ACPI_SIGNATURE_APIC;
    Madt->Header.Length = sizeof(MadtData);
    Subtable->Length = sizeof(*Subtable);
    assert(AcpiValidateTable(&MadtBuffer, false, &View) == FirmwareDecodeSuccess);
    assert(AcpiNextSubtable(&View, sizeof(*Madt), &Offset, &SubtableView) == FirmwareDecodeSuccess);
    assert(SubtableView.Offset == sizeof(*Madt));
    assert(AcpiNextSubtable(&View, sizeof(*Madt), &Offset, &SubtableView) == FirmwareDecodeEnd);
    Subtable->Length = 0;
    Offset = 0;
    assert(AcpiNextSubtable(&View, sizeof(*Madt), &Offset, &SubtableView) == FirmwareDecodeInvalidLength);
}

static
void
TestCpuid()
{
    char Vendor[13];
    uint32_t MaximumLeaf;
    FIRMWARE_DECODE_STATUS Status = CpuidGetVendor(Vendor, &MaximumLeaf);

    assert(Status == FirmwareDecodeSuccess || Status == FirmwareDecodeUnsupported);
    if (Status == FirmwareDecodeSuccess)
    {
        assert(Vendor[0] != 0);
    }
}

int
main()
{
    TestFirmwareBuffer();
    TestSmbios();
    TestAcpi();
    TestCpuid();
    puts("FirmwareSpec tests passed");
    return 0;
}
