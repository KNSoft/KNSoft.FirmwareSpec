/*
 * KNSoft.FirmwareSpec (https://github.com/KNSoft/KNSoft.FirmwareSpec)
 *
 * Minimal shared primitives for decoding firmware data.
 *
 * Licensed under the MIT license.
 * Copyright (c) KNSoft.org (https://github.com/KNSoft). All rights reserved.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef enum _FIRMWARE_DECODE_STATUS
{
    FirmwareDecodeSuccess,
    FirmwareDecodeEnd,
    FirmwareDecodeInvalidArgument,
    FirmwareDecodeTruncatedHeader,
    FirmwareDecodeInvalidLength,
    FirmwareDecodeTruncatedData,
    FirmwareDecodeMissingTerminator,
    FirmwareDecodeBadChecksum,
    FirmwareDecodeUnsupported,
} FIRMWARE_DECODE_STATUS;

typedef struct _FIRMWARE_BUFFER
{
    const uint8_t* Data;
    size_t Size;
} FIRMWARE_BUFFER;

typedef struct _FIRMWARE_ENUM_INFO
{
    const char* Name;
    uint64_t Value;
} FIRMWARE_ENUM_INFO;

typedef enum _FIRMWARE_DATA_TYPE
{
    FirmwareDataTypeOther,
    FirmwareDataTypeString,
    FirmwareDataTypeUInt,
    FirmwareDataTypeBit,
    FirmwareDataTypeEnum,
    FirmwareDataTypeUuid,
    FirmwareDataTypeRaw,
} FIRMWARE_DATA_TYPE;

typedef struct _FIRMWARE_FIELD_INFO
{
    const char* Name;
    uint8_t IsBitField;
    uint16_t Offset;
    uint8_t Size;
    FIRMWARE_DATA_TYPE Type;
    struct
    {
        uint16_t Count;
        const FIRMWARE_ENUM_INFO* Values;
    } Enum;
} FIRMWARE_FIELD_INFO;

static inline
const void*
FirmwareBuffer_Get(
    const FIRMWARE_BUFFER* Buffer,
    size_t Offset,
    size_t Length)
{
    if (Buffer == NULL || Buffer->Data == NULL ||
        Offset > Buffer->Size || Length > Buffer->Size - Offset)
    {
        return NULL;
    }

    return Buffer->Data + Offset;
}

static inline
bool
FirmwareBuffer_ReadUInt(
    const FIRMWARE_BUFFER* Buffer,
    size_t Offset,
    uint8_t Size,
    uint64_t* Value)
{
    const void* Data;
    uint64_t Result = 0;

    if (Value == NULL || Size == 0 || Size > sizeof(*Value))
    {
        return false;
    }
    Data = FirmwareBuffer_Get(Buffer, Offset, Size);
    if (Data == NULL)
    {
        return false;
    }

    memcpy(&Result, Data, Size);
    *Value = Result;
    return true;
}

static inline
uint64_t
FirmwareExtractBits(
    uint64_t Value,
    uint8_t Offset,
    uint8_t Count)
{
    if (Offset >= 64 || Count == 0 || Count > 64 - Offset)
    {
        return 0;
    }

    Value >>= Offset;
    return Count == 64 ? Value : Value & (UINT64_MAX >> (64 - Count));
}

static inline
bool
FirmwareChecksum8(
    const void* Data,
    size_t Size)
{
    const uint8_t* Bytes = (const uint8_t*)Data;
    uint8_t Sum = 0;

    if (Bytes == NULL && Size != 0)
    {
        return false;
    }
    while (Size-- != 0)
    {
        Sum = (uint8_t)(Sum + *Bytes++);
    }
    return Sum == 0;
}

static inline
const char*
FirmwareFindEnum(
    const FIRMWARE_ENUM_INFO* Values,
    size_t Count,
    uint64_t Value)
{
    size_t Index;

    if (Values == NULL)
    {
        return NULL;
    }

    for (Index = 0; Index < Count; Index++)
    {
        if (Values[Index].Value == Value)
        {
            return Values[Index].Name;
        }
    }
    return NULL;
}
