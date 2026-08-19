#include "ACPI.Decode.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include <dirent.h>
#include <sys/stat.h>
#endif

static
void
PrintSignature(
    uint32_t Signature)
{
    putchar((char)FirmwareExtractBits(Signature, 0, 8));
    putchar((char)FirmwareExtractBits(Signature, 8, 8));
    putchar((char)FirmwareExtractBits(Signature, 16, 8));
    putchar((char)FirmwareExtractBits(Signature, 24, 8));
}

static
void
PrintFields(
    const FIRMWARE_BUFFER* Buffer,
    const FIRMWARE_FIELD_INFO* Fields,
    size_t Count)
{
    size_t Index;

    for (Index = 0; Index < Count; Index++)
    {
        const FIRMWARE_FIELD_INFO* Field = &Fields[Index];

        if (Field->Type == FirmwareDataTypeUInt)
        {
            uint64_t Value;

            if (FirmwareBuffer_ReadUInt(Buffer, Field->Offset, Field->Size, &Value))
            {
                printf("  %s: 0x%0*llX\n", Field->Name, Field->Size * 2, (unsigned long long)Value);
            }
        } else if (Field->Type == FirmwareDataTypeRaw)
        {
            const uint8_t* Bytes = (const uint8_t*)FirmwareBuffer_Get(Buffer, Field->Offset, Field->Size);
            size_t ByteIndex;

            if (Bytes == NULL)
            {
                continue;
            }
            printf("  %s: \"", Field->Name);
            for (ByteIndex = 0; ByteIndex < Field->Size; ByteIndex++)
            {
                putchar(Bytes[ByteIndex] >= 0x20 && Bytes[ByteIndex] <= 0x7E ? Bytes[ByteIndex] : '.');
            }
            puts("\"");
        }
    }
}

static
int
DecodeTable(
    const uint8_t* Data,
    size_t Size)
{
    ACPI_TABLE_VIEW View;
    FIRMWARE_BUFFER Buffer = { Data, Size };
    FIRMWARE_DECODE_STATUS Status;

    Status = AcpiValidateTable(&Buffer, true, &View);
    if (Status != FirmwareDecodeSuccess)
    {
        printf("Invalid ACPI table, status %u\n", Status);
        return 1;
    }

    printf("Signature: ");
    PrintSignature(View.Header->Signature);
    printf("\nName: %s\nLength: %u\nRevision: %u\n",
           View.Info == NULL ? "Unknown or OEM-defined table" : View.Info->Name,
           View.Header->Length,
           View.Header->Revision);
    PrintFields(&View.Data, AcpiHeaderFields, sizeof(AcpiHeaderFields) / sizeof(AcpiHeaderFields[0]));
    if (View.Info != NULL)
    {
        PrintFields(&View.Data, View.Info->Fields, View.Info->FieldCount);
        if (View.Info->RawBody)
        {
            puts("  Body: raw (AML, OEM-defined, or intentionally unsupported)");
        }
    }
    return 0;
}

static
int
DecodeFile(
    const char* Path)
{
    ACPI_DESCRIPTION_HEADER Header;
    FILE* File;
    uint8_t* Data;
    int Result;

#if defined(_MSC_VER)
    if (fopen_s(&File, Path, "rb") != 0)
#else
    File = fopen(Path, "rb");
    if (File == NULL)
#endif
    {
        printf("Cannot open %s: %d\n", Path, errno);
        return 1;
    }
    if (fread(&Header, 1, sizeof(Header), File) != sizeof(Header) || Header.Length < sizeof(Header))
    {
        printf("Cannot read ACPI header from %s\n", Path);
        fclose(File);
        return 1;
    }
    Data = (uint8_t*)malloc(Header.Length);
    if (Data == NULL)
    {
        fclose(File);
        return 1;
    }
    memcpy(Data, &Header, sizeof(Header));
    if (fread(Data + sizeof(Header), 1, Header.Length - sizeof(Header), File) != Header.Length - sizeof(Header))
    {
        printf("Cannot read ACPI table from %s\n", Path);
        free(Data);
        fclose(File);
        return 1;
    }
    fclose(File);
    Result = DecodeTable(Data, Header.Length);
    free(Data);
    return Result;
}

#if defined(_WIN32)

static
int
DecodeSystemTables()
{
    DWORD* TableIds;
    UINT IdBytes = EnumSystemFirmwareTables('ACPI', NULL, 0);
    UINT Index;
    int Result = 0;

    if (IdBytes == 0)
    {
        printf("EnumSystemFirmwareTables failed: 0x%08lX\n", GetLastError());
        return 1;
    }
    if (IdBytes % sizeof(*TableIds) != 0)
    {
        puts("EnumSystemFirmwareTables returned an invalid table list");
        return 1;
    }
    TableIds = (DWORD*)malloc(IdBytes);
    if (TableIds == NULL)
    {
        return 1;
    }
    if (EnumSystemFirmwareTables('ACPI', TableIds, IdBytes) != IdBytes)
    {
        printf("EnumSystemFirmwareTables failed: 0x%08lX\n", GetLastError());
        free(TableIds);
        return 1;
    }

    for (Index = 0; Index < IdBytes / sizeof(*TableIds); Index++)
    {
        UINT TableSize = GetSystemFirmwareTable('ACPI', TableIds[Index], NULL, 0);
        void* Table;

        if (TableSize == 0 || (Table = malloc(TableSize)) == NULL)
        {
            Result = 1;
            continue;
        }
        if (GetSystemFirmwareTable('ACPI', TableIds[Index], Table, TableSize) != TableSize)
        {
            printf("GetSystemFirmwareTable failed: 0x%08lX\n", GetLastError());
            Result = 1;
        } else
        {
            Result |= DecodeTable((const uint8_t*)Table, TableSize);
        }
        free(Table);
    }
    free(TableIds);
    return Result;
}

#elif defined(__linux__)

static
int
DecodeSystemTables()
{
    static const char DirectoryPath[] = "/sys/firmware/acpi/tables";
    DIR* Directory = opendir(DirectoryPath);
    struct dirent* Entry;
    int Result = 0;

    if (Directory == NULL)
    {
        printf("ACPI tables are unavailable: %d\n", errno);
        return 0;
    }
    while ((Entry = readdir(Directory)) != NULL)
    {
        char Path[sizeof(DirectoryPath) + 1 + 256];
        int Length;
        struct stat FileStatus;

        if (Entry->d_name[0] == '.')
        {
            continue;
        }
        Length = snprintf(Path, sizeof(Path), "%s/%s", DirectoryPath, Entry->d_name);
        if (Length < 0 || (size_t)Length >= sizeof(Path))
        {
            Result = 1;
            continue;
        }
        if (stat(Path, &FileStatus) != 0)
        {
            Result = 1;
            continue;
        }
        if (!S_ISREG(FileStatus.st_mode))
        {
            continue;
        }
        Result |= DecodeFile(Path);
    }
    closedir(Directory);
    return Result;
}

#endif

int
main(
    int ArgumentCount,
    char** Arguments)
{
    if (ArgumentCount == 2)
    {
        return DecodeFile(Arguments[1]);
    }
#if defined(_WIN32) || defined(__linux__)
    if (ArgumentCount == 1)
    {
        return DecodeSystemTables();
    }
#endif
    printf("Usage: %s [ACPI table binary]\n", Arguments[0]);
    return 1;
}
