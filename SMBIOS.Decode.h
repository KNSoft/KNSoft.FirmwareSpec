/*
 * KNSoft.FirmwareSpec (https://github.com/KNSoft/KNSoft.FirmwareSpec)
 *
 * Zero-copy SMBIOS structure walker.
 *
 * Licensed under the MIT license.
 * Copyright (c) KNSoft.org (https://github.com/KNSoft). All rights reserved.
 */

#pragma once

#include "FirmwareDecode.h"
#include "SMBIOS.TypeInfo.h"

typedef struct _SMBIOS_STRUCTURE_VIEW
{
    const SMBIOS_HEADER* Header;
    FIRMWARE_BUFFER Formatted;
    FIRMWARE_BUFFER Strings;
    size_t Offset;
    size_t TotalSize;
} SMBIOS_STRUCTURE_VIEW;

static inline
const SMBIOS_TYPE_INFO*
SmbiosFindTypeInfo(
    uint8_t Type)
{
    size_t Index;

    for (Index = 0; Index < sizeof(SmbiosTypeInfo) / sizeof(SmbiosTypeInfo[0]); Index++)
    {
        if (SmbiosTypeInfo[Index].Type == Type)
        {
            return &SmbiosTypeInfo[Index];
        }
    }
    return NULL;
}

static inline
FIRMWARE_DECODE_STATUS
SmbiosNextStructure(
    const FIRMWARE_BUFFER* Table,
    size_t* Offset,
    SMBIOS_STRUCTURE_VIEW* View)
{
    const SMBIOS_HEADER* Header;
    size_t End;

    if (Table == NULL || Offset == NULL || View == NULL)
    {
        return FirmwareDecodeInvalidArgument;
    }
    if (*Offset == Table->Size)
    {
        return FirmwareDecodeEnd;
    }

    Header = (const SMBIOS_HEADER*)FirmwareBuffer_Get(Table, *Offset, sizeof(*Header));
    if (Header == NULL)
    {
        return FirmwareDecodeTruncatedHeader;
    }
    if (Header->Length < sizeof(*Header))
    {
        return FirmwareDecodeInvalidLength;
    }
    if (FirmwareBuffer_Get(Table, *Offset, Header->Length) == NULL)
    {
        return FirmwareDecodeTruncatedData;
    }

    End = *Offset + Header->Length;
    while (End < Table->Size)
    {
        if (Table->Data[End] == 0 && End + 1 < Table->Size && Table->Data[End + 1] == 0)
        {
            View->Header = Header;
            View->Formatted.Data = Table->Data + *Offset;
            View->Formatted.Size = Header->Length;
            View->Strings.Data = Table->Data + *Offset + Header->Length;
            View->Strings.Size = End + 2 - (*Offset + Header->Length);
            View->Offset = *Offset;
            View->TotalSize = End + 2 - *Offset;
            *Offset = End + 2;
            return FirmwareDecodeSuccess;
        }
        End++;
    }
    return FirmwareDecodeMissingTerminator;
}

static inline
FIRMWARE_DECODE_STATUS
SmbiosGetString(
    const SMBIOS_STRUCTURE_VIEW* View,
    uint8_t Index,
    const char** String,
    size_t* Length)
{
    size_t Offset = 0;
    uint8_t Current = 1;

    if (View == NULL || String == NULL || Length == NULL)
    {
        return FirmwareDecodeInvalidArgument;
    }
    if (Index == 0)
    {
        *String = "";
        *Length = 0;
        return FirmwareDecodeSuccess;
    }

    while (Offset + 1 < View->Strings.Size && View->Strings.Data[Offset] != 0)
    {
        size_t End = Offset;

        while (End < View->Strings.Size && View->Strings.Data[End] != 0)
        {
            End++;
        }
        if (End == View->Strings.Size)
        {
            return FirmwareDecodeMissingTerminator;
        }
        if (Current == Index)
        {
            *String = (const char*)View->Strings.Data + Offset;
            *Length = End - Offset;
            return FirmwareDecodeSuccess;
        }
        Current++;
        Offset = End + 1;
    }
    return FirmwareDecodeInvalidLength;
}
