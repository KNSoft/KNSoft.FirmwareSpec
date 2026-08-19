#pragma once

#include "SMBIOS.h"

#include "FirmwareDecode.h"

#if defined(_MSC_VER)
#define SMBIOS_TYPEINFO_SELECTANY __declspec(selectany)
#elif defined(__GNUC__)
#define SMBIOS_TYPEINFO_SELECTANY __attribute__((weak))
#else
#error Unsupported compiler, currently supports GNUC (__GNUC__) and MSVC (_MSC_VER)
#endif

typedef FIRMWARE_DATA_TYPE SMBIOS_DATA_TYPE, *PSMBIOS_DATA_TYPE;
typedef FIRMWARE_ENUM_INFO SMBIOS_FIELD_ENUM, *PSMBIOS_FIELD_ENUM;
typedef FIRMWARE_FIELD_INFO SMBIOS_FIELD_TYPE_INFO, *PSMBIOS_FIELD_TYPE_INFO;

#define SmbiosDataTypeOther     FirmwareDataTypeOther
#define SmbiosDataTypeString    FirmwareDataTypeString
#define SmbiosDataTypeUInt      FirmwareDataTypeUInt
#define SmbiosDataTypeBit       FirmwareDataTypeBit
#define SmbiosDataTypeEnum      FirmwareDataTypeEnum
#define SmbiosDataTypeUuid      FirmwareDataTypeUuid
#define SmbiosDataTypeRaw       FirmwareDataTypeRaw

typedef struct _SMBIOS_TYPE_INFO
{
    BYTE Type;          // Type number in SMBIOS spec.
    const char* Name;   // Type name in SMBIOS spec.
    WORD FieldCount;    // Number of Fields
    const SMBIOS_FIELD_TYPE_INFO* Fields; // Field information in type structure // _Field_size_(FieldCount)
} SMBIOS_TYPE_INFO, *PSMBIOS_TYPE_INFO;

#define SMBIOS_DEFINE_FIELD(Type, Name, Field, FieldType, ...) { Name, false, (WORD)offsetof(SMBIOS_TYPE_##Type, Field), (BYTE)sizeof(((SMBIOS_TYPE_##Type*)0)->Field), FieldType, ##__VA_ARGS__ }
#define SMBIOS_DEFINE_BIT_FIELD(Name, BitOffset, BitSize, FieldType, ...) { Name, true, BitOffset, BitSize, FieldType, ##__VA_ARGS__ }
#define SMBIOS_FIELD_ENUM_VALUES(EnumName) { (WORD)(sizeof(EnumName) / sizeof(EnumName[0])), EnumName }

#define SMBIOS_DEFINE_FIELD_STRING(Type, Name, Field) SMBIOS_DEFINE_FIELD(Type, Name, Field, SmbiosDataTypeString)
#define SMBIOS_DEFINE_FIELD_UINT(Type, Name, Field) SMBIOS_DEFINE_FIELD(Type, Name, Field, SmbiosDataTypeUInt)
#define SMBIOS_DEFINE_FIELD_RAW(Type, Name, Field) SMBIOS_DEFINE_FIELD(Type, Name, Field, SmbiosDataTypeRaw)
#define SMBIOS_DEFINE_FIELD_ENUM(Type, Name, Field, EnumName) SMBIOS_DEFINE_FIELD(Type, Name, Field, SmbiosDataTypeEnum, SMBIOS_FIELD_ENUM_VALUES(EnumName))
#define SMBIOS_DEFINE_FIELD_BIT(Name, BitOffset) SMBIOS_DEFINE_BIT_FIELD(Name, BitOffset, 1, SmbiosDataTypeBit)

#define SMBIOS_DEFINE_TYPE(Type, Name) { Type, Name, sizeof(SmbiosType##Type##FieldInfo) / sizeof(SmbiosType##Type##FieldInfo[0]), SmbiosType##Type##FieldInfo }
