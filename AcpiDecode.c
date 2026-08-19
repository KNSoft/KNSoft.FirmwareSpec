#include "ACPI.Decode.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

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
DecodeFile(
    const char* Path)
{
    ACPI_TABLE_VIEW View;
    FIRMWARE_BUFFER Buffer;
    FIRMWARE_DECODE_STATUS Status;
    FILE* File;
    uint8_t* Data;
    long FileSize;

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
    if (fseek(File, 0, SEEK_END) != 0 || (FileSize = ftell(File)) < 0 || fseek(File, 0, SEEK_SET) != 0)
    {
        fclose(File);
        return 1;
    }
    Data = (uint8_t*)malloc((size_t)FileSize);
    if (Data == NULL || fread(Data, 1, (size_t)FileSize, File) != (size_t)FileSize)
    {
        free(Data);
        fclose(File);
        return 1;
    }
    fclose(File);

    Buffer.Data = Data;
    Buffer.Size = (size_t)FileSize;
    Status = AcpiValidateTable(&Buffer, true, &View);
    if (Status != FirmwareDecodeSuccess)
    {
        printf("Invalid ACPI table, status %u\n", Status);
        free(Data);
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
    free(Data);
    return 0;
}

int
main(
    int ArgumentCount,
    char** Arguments)
{
    if (ArgumentCount != 2)
    {
        printf("Usage: %s <ACPI table binary>\n", Arguments[0]);
        return 0;
    }
    return DecodeFile(Arguments[1]);
}
