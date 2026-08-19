/*
 * KNSoft.FirmwareSpec (https://github.com/KNSoft/KNSoft.FirmwareSpec)
 *
 * Zero-copy ACPI table validation and subtable walking.
 *
 * Licensed under the MIT license.
 * Copyright (c) KNSoft.org (https://github.com/KNSoft). All rights reserved.
 */

#pragma once

#include "ACPI.TypeInfo.h"

typedef struct _ACPI_TABLE_VIEW
{
    const ACPI_DESCRIPTION_HEADER* Header;
    FIRMWARE_BUFFER Data;
    const ACPI_TABLE_INFO* Info;
} ACPI_TABLE_VIEW;

typedef struct _ACPI_SUBTABLE_VIEW
{
    const ACPI_SUBTABLE_HEADER* Header;
    FIRMWARE_BUFFER Data;
    size_t Offset;
} ACPI_SUBTABLE_VIEW;

static inline
FIRMWARE_DECODE_STATUS
AcpiValidateRsdp(
    const FIRMWARE_BUFFER* Buffer,
    const ACPI_ROOT_SYSTEM_DESCRIPTION_POINTER** Rsdp)
{
    const ACPI_ROOT_SYSTEM_DESCRIPTION_POINTER* Value;
    static const uint8_t Signature[] = { 'R', 'S', 'D', ' ', 'P', 'T', 'R', ' ' };

    if (Buffer == NULL || Rsdp == NULL)
    {
        return FirmwareDecodeInvalidArgument;
    }
    Value = (const ACPI_ROOT_SYSTEM_DESCRIPTION_POINTER*)FirmwareBuffer_Get(Buffer, 0, 20);
    if (Value == NULL)
    {
        return FirmwareDecodeTruncatedHeader;
    }
    if (memcmp(Value->Signature, Signature, sizeof(Signature)) != 0)
    {
        return FirmwareDecodeInvalidArgument;
    }
    if (!FirmwareChecksum8(Value, 20))
    {
        return FirmwareDecodeBadChecksum;
    }
    if (Value->Revision >= 2)
    {
        if (FirmwareBuffer_Get(Buffer, 0, offsetof(ACPI_ROOT_SYSTEM_DESCRIPTION_POINTER, XsdtAddress)) == NULL)
        {
            return FirmwareDecodeTruncatedHeader;
        }
        if (Value->Length < sizeof(*Value))
        {
            return FirmwareDecodeInvalidLength;
        }
        if (FirmwareBuffer_Get(Buffer, 0, Value->Length) == NULL)
        {
            return FirmwareDecodeTruncatedData;
        }
        if (!FirmwareChecksum8(Value, Value->Length))
        {
            return FirmwareDecodeBadChecksum;
        }
    }

    *Rsdp = Value;
    return FirmwareDecodeSuccess;
}

static inline
FIRMWARE_DECODE_STATUS
AcpiValidateTable(
    const FIRMWARE_BUFFER* Buffer,
    bool ValidateChecksum,
    ACPI_TABLE_VIEW* View)
{
    const ACPI_DESCRIPTION_HEADER* Header;
    const ACPI_TABLE_INFO* Info;

    if (Buffer == NULL || View == NULL)
    {
        return FirmwareDecodeInvalidArgument;
    }
    Header = (const ACPI_DESCRIPTION_HEADER*)FirmwareBuffer_Get(Buffer, 0, sizeof(*Header));
    if (Header == NULL)
    {
        return FirmwareDecodeTruncatedHeader;
    }
    if (Header->Length < sizeof(*Header))
    {
        return FirmwareDecodeInvalidLength;
    }
    if (FirmwareBuffer_Get(Buffer, 0, Header->Length) == NULL)
    {
        return FirmwareDecodeTruncatedData;
    }
    Info = AcpiFindTableInfo(Header->Signature);
    if (Info != NULL && Header->Length < Info->MinimumSize)
    {
        return FirmwareDecodeInvalidLength;
    }
    if (ValidateChecksum && !FirmwareChecksum8(Header, Header->Length))
    {
        return FirmwareDecodeBadChecksum;
    }

    View->Header = Header;
    View->Data.Data = Buffer->Data;
    View->Data.Size = Header->Length;
    View->Info = Info;
    return FirmwareDecodeSuccess;
}

static inline
FIRMWARE_DECODE_STATUS
AcpiNextSubtable(
    const ACPI_TABLE_VIEW* Table,
    size_t Start,
    size_t* Offset,
    ACPI_SUBTABLE_VIEW* View)
{
    const ACPI_SUBTABLE_HEADER* Header;
    size_t Position;

    if (Table == NULL || Offset == NULL || View == NULL || Start > Table->Data.Size)
    {
        return FirmwareDecodeInvalidArgument;
    }
    if (*Offset == 0)
    {
        *Offset = Start;
    }
    Position = *Offset;
    if (Position == Table->Data.Size)
    {
        return FirmwareDecodeEnd;
    }
    Header = (const ACPI_SUBTABLE_HEADER*)FirmwareBuffer_Get(&Table->Data, Position, sizeof(*Header));
    if (Header == NULL)
    {
        return FirmwareDecodeTruncatedHeader;
    }
    if (Header->Length < sizeof(*Header))
    {
        return FirmwareDecodeInvalidLength;
    }
    if (FirmwareBuffer_Get(&Table->Data, Position, Header->Length) == NULL)
    {
        return FirmwareDecodeTruncatedData;
    }

    View->Header = Header;
    View->Data.Data = Table->Data.Data + Position;
    View->Data.Size = Header->Length;
    View->Offset = Position;
    *Offset = Position + Header->Length;
    return FirmwareDecodeSuccess;
}

static inline
FIRMWARE_DECODE_STATUS
AcpiNextFixedEntry(
    const ACPI_TABLE_VIEW* Table,
    size_t Start,
    size_t EntrySize,
    size_t* Offset,
    FIRMWARE_BUFFER* Entry)
{
    size_t Position;
    const uint8_t* Data;

    if (Table == NULL || Offset == NULL || Entry == NULL || EntrySize == 0 || Start > Table->Data.Size)
    {
        return FirmwareDecodeInvalidArgument;
    }
    Position = *Offset == 0 ? Start : *Offset;
    if (Position == Table->Data.Size)
    {
        return FirmwareDecodeEnd;
    }
    Data = (const uint8_t*)FirmwareBuffer_Get(&Table->Data, Position, EntrySize);
    if (Data == NULL)
    {
        return FirmwareDecodeTruncatedData;
    }

    Entry->Data = Data;
    Entry->Size = EntrySize;
    *Offset = Position + EntrySize;
    return FirmwareDecodeSuccess;
}
