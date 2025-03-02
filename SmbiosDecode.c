#include "SMBIOS.h"
#include "SMBIOS.TypeInfo.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <locale.h>
#include <string.h>
#include <errno.h>

#define AddPtr(P, I) ((void*)((unsigned char*)(P) + (I))) // aka Add2Ptr
#define SubPtr(B, O) ((unsigned int)((size_t)(O) - (size_t)(B))) // aka PtrOffset
#ifndef NULL
#define NULL ((void *)0)
#endif

#if defined(_WIN32)

#include <Windows.h>

static bool GetSmbiosTableData(PSMBIOS_RAW_DATA* Data)
{
    UINT BufferSize, BytesWritten;
    PVOID Buffer;

    BufferSize = GetSystemFirmwareTable('RSMB', 0, NULL, 0);
    if (BufferSize == 0)
    {
        printf("GetSystemFirmwareTable failed with: 0x%08lX\n", GetLastError());
        return false;
    }
    Buffer = malloc(BufferSize);
    if (Buffer == NULL)
    {
        printf("malloc failed to allocate %lu bytes buffer\n", BufferSize);
        return false;
    }
    BytesWritten = GetSystemFirmwareTable('RSMB', 0, Buffer, BufferSize);
    if (BytesWritten == 0)
    {
        printf("GetSystemFirmwareTable failed with: 0x%08lX\n", GetLastError());
    } else if (BytesWritten != BufferSize)
    {
        printf("Read size (%lu) is not match expected size (%lu)", BytesWritten, BufferSize);
    } else
    {
        *Data = (PSMBIOS_RAW_DATA)Buffer;
        return true;
    }

    free(Buffer);
    return false;
}

#elif defined(__linux__)

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static char SmbiosEntryPointAnchorString21[] = SMBIOS_ENTRY_POINT_21_ANCHOR_STRING;
static char SmbiosEntryPointAnchorString30[] = SMBIOS_ENTRY_POINT_30_ANCHOR_STRING;

static bool ReadSmbiosEntryField(int fd, off_t off, void* p, size_t len)
{
    if (lseek(fd, off, SEEK_SET) == (off_t)-1)
    {
        return false;
    }
    return read(fd, p, len) == len;
}

static bool GetSmbiosTableData(PSMBIOS_RAW_DATA* Data)
{
    int fd, ret;
    struct stat sb;
    PSMBIOS_RAW_DATA p;
    ssize_t size, i;
    char anchor[5];

    fd = open("/sys/firmware/dmi/tables/DMI", O_RDONLY);
    if (fd < 0)
    {
        printf("open failed with: %d\n", errno);
        return false;
    }
    ret = fstat(fd, &sb);
    if (ret < 0)
    {
        printf("fstat failed with: %d\n", errno);
        goto _exit_0;
    }
    size = sizeof(SMBIOS_RAW_DATA) + sb.st_size;
    p = (PSMBIOS_RAW_DATA)malloc(size);
    if (p == NULL)
    {
        printf("malloc failed to allocate %zd bytes buffer\n", size);
        goto _exit_0;
    }

    size = 0;
    do
    {
        i = read(fd, AddPtr(p, sizeof(SMBIOS_RAW_DATA) + size), sb.st_size - size);
        if (i < 0)
        {
            printf("read failed with: %d\n", errno);
            goto _exit_1;
        }
        size += i;
    } while (i > 0);
    if (size != sb.st_size)
    {
        printf("Read size (%zd) is not match expected size (%ld)\n", size, sb.st_size);
        goto _exit_1;
    }
    close(fd);

    fd = open("/sys/firmware/dmi/tables/smbios_entry_point", O_RDONLY);
    if (fd < 0)
    {
        printf("open failed with: %d\n", errno);
        free(p);
        return false;
    }
    i = read(fd, anchor, sizeof(anchor));
    if (i != sizeof(anchor))
    {
        goto _exit_2;
    }
    if (memcmp(anchor, SmbiosEntryPointAnchorString30, sizeof(SmbiosEntryPointAnchorString30)) == 0)
    {
        if (!ReadSmbiosEntryField(fd, offsetof(SMBIOS_ENTRY_POINT_30, MajorVersion), &p->SMBIOSMajorVersion, sizeof(p->SMBIOSMajorVersion)) ||
            !ReadSmbiosEntryField(fd, offsetof(SMBIOS_ENTRY_POINT_30, MinorVersion), &p->SMBIOSMinorVersion, sizeof(p->SMBIOSMajorVersion)) ||
            !ReadSmbiosEntryField(fd, offsetof(SMBIOS_ENTRY_POINT_30, TableMaxSize), &p->Length, sizeof(p->Length)))
        {
            goto _exit_2;
        }
        p->DmiRevision = 3;
    } else if (memcmp(anchor, SmbiosEntryPointAnchorString21, sizeof(SmbiosEntryPointAnchorString21)) == 0)
    {
        WORD len;

        if (!ReadSmbiosEntryField(fd, offsetof(SMBIOS_ENTRY_POINT_21, MajorVersion), &p->SMBIOSMajorVersion, sizeof(p->SMBIOSMajorVersion)) ||
            !ReadSmbiosEntryField(fd, offsetof(SMBIOS_ENTRY_POINT_21, MinorVersion), &p->SMBIOSMinorVersion, sizeof(p->SMBIOSMajorVersion)) ||
            !ReadSmbiosEntryField(fd, offsetof(SMBIOS_ENTRY_POINT_21, TableLength), &len, sizeof(len)))
        {
            goto _exit_2;
        }
        p->Length = len;
        p->DmiRevision = 2;
    } else
    {
        goto _exit_2;
    }
    p->Used20CallingMethod = 0;

    *Data = p;
    return true;

_exit_2:
    puts("Read SMBIOS entry point failed\n");
_exit_1:
    free(p);
_exit_0:
    close(fd);
    return false;
}

#else

#error No OS target specified, currently supports Windows (_WIN32) and Linux (__linux__)

#endif

static const char* g_Strings[UCHAR_MAX] = { 0 };

static
PSMBIOS_TABLE
PrintSmbiosTable(
    PSMBIOS_TABLE Table,
    void* StartOfData,
    void* EndOfData)
{
    PSMBIOS_TABLE NextTable;
    void* EndOfTable;
    WORD i;
    const char *psz;
    BYTE StringCount;
    size_t MaxSize, Length;
    PSMBIOS_TYPE_INFO TypeInfo;
    QWORD BitFieldValue, Value;
    WORD BitFieldSize = 0, ValueSize;

    /* Get type information */
    TypeInfo = NULL;
    for (i = 0; i < sizeof(SmbiosTypeInfo) / sizeof(SmbiosTypeInfo[0]); i++)
    {
        if (SmbiosTypeInfo[i].Type == Table->Header.Type)
        {
            TypeInfo = &SmbiosTypeInfo[i];
            break;
        }
    }

    /* Build string index array and locate next table address */
    psz = (const char *)AddPtr(Table, Table->Header.Length);
    StringCount = 0;
    NextTable = NULL;
    while (true)
    {
        if (psz + 2 >= (const char*)EndOfData)
        {
            break;
        }
        if (psz[0] == '\0' && psz[1] == '\0')
        {
            NextTable = (PSMBIOS_TABLE)(psz + 2);
            break;
        }

        MaxSize = SubPtr(psz, EndOfData);
        Length = strnlen(psz, MaxSize);
        if (Length == MaxSize)
        {
            break;
        }
        g_Strings[StringCount++] = psz;
        psz += Length + 1;
        if (psz[0] == '\0')
        {
            NextTable = (PSMBIOS_TABLE)(psz + 1);
            break;
        }
    }
    if ((void*)NextTable >= EndOfData)
    {
        NextTable = NULL;
    }
    EndOfTable = NextTable == NULL ? EndOfData : NextTable;

    /* Print table header */
    if (TypeInfo != NULL)
    {
        printf("[Type %hhu: %s]\n", Table->Header.Type, TypeInfo->Name);
    } else
    {
        printf("[Type %hhu (Unrecognized)]\n", Table->Header.Type);
    }
    printf("Handle: 0x%04hX, Offset: 0x%08X, Length: 0x%02hhX bytes, Total: 0x%X bytes\n",
           Table->Header.Handle,
           SubPtr(StartOfData, Table),
           Table->Header.Length,
           SubPtr(Table, EndOfTable));

    /* Print table fields */
    if (TypeInfo == NULL)
    {
        goto _Exit;
    }
    for (i = 0; i < TypeInfo->FieldCount; i++)
    {
        Value = 0;
        if (TypeInfo->Fields[i].IsBitField)
        {
            if (BitFieldSize == 0)
            {
                if (i == 0 ||
                    TypeInfo->Fields[i - 1].IsBitField ||
                    TypeInfo->Fields[i - 1].Type != SmbiosDataTypeUInt)
                {
                    break;
                }
                BitFieldSize = TypeInfo->Fields[i - 1].Size;
                BitFieldValue = 0;
                memcpy(&BitFieldValue, AddPtr(Table, TypeInfo->Fields[i - 1].Offset), TypeInfo->Fields[i - 1].Size);
            }
            putchar('\t');
            if (TypeInfo->Fields[i].Type == SmbiosDataTypeBit)
            {
                printf("%02hhu [%c] %s",
                       (BYTE)TypeInfo->Fields[i].Offset,
                       (BitFieldValue & ((QWORD)1 << TypeInfo->Fields[i].Offset)) ? 'x' : ' ',
                       TypeInfo->Fields[i].Name);
            } else if (TypeInfo->Fields[i].Type == SmbiosDataTypeUInt || TypeInfo->Fields[i].Type == SmbiosDataTypeEnum)
            {
                printf("%02hhu:%02hhu %s: ",
                       (BYTE)TypeInfo->Fields[i].Offset,
                       (BYTE)TypeInfo->Fields[i].Offset + TypeInfo->Fields[i].Size - 1,
                       TypeInfo->Fields[i].Name);
                Value = BitFieldValue;
                Value >>= TypeInfo->Fields[i].Offset;
                Value &= ((QWORD)1 << TypeInfo->Fields[i].Size) - 1;
            }
        } else
        {
            if (TypeInfo->Fields[i].Offset + TypeInfo->Fields[i].Size > Table->Header.Length)
            {
                break;
            }
            BitFieldSize = 0;
            printf("0x%04hX %s: ", TypeInfo->Fields[i].Offset, TypeInfo->Fields[i].Name);
        }

        if (TypeInfo->Fields[i].Type == SmbiosDataTypeString && TypeInfo->Fields[i].Size == sizeof(BYTE))
        {
            BYTE Index = *(BYTE*)AddPtr(Table, TypeInfo->Fields[i].Offset);
            if (Index != 0 && Index <= StringCount)
            {
                printf("0x%02hhX \"%s\"", Index, g_Strings[(BYTE)(Index - 1)]);
            }
        } else if (TypeInfo->Fields[i].Type == SmbiosDataTypeUInt || TypeInfo->Fields[i].Type == SmbiosDataTypeEnum)
        {
            if (TypeInfo->Fields[i].IsBitField)
            {
                ValueSize = BitFieldSize;
            } else
            {
                Value = 0;
                memcpy(&Value, AddPtr(Table, TypeInfo->Fields[i].Offset), TypeInfo->Fields[i].Size);
                ValueSize = TypeInfo->Fields[i].Size;
            }
            if (ValueSize == sizeof(BYTE))
            {
                printf("0x%02hhX", (BYTE)Value);
            } else if (ValueSize == sizeof(WORD))
            {
                printf("0x%04hX", (WORD)Value);
            } else if (ValueSize == sizeof(DWORD))
            {
                printf("0x%08lX", (DWORD)Value);
            } else if (ValueSize == sizeof(QWORD))
            {
                printf("0x%016llX", Value);
            }
            if (TypeInfo->Fields[i].Type == SmbiosDataTypeEnum)
            {
                WORD j;
                for (j = 0; j < TypeInfo->Fields[i].AdditionalInfo.Enum.Count; j++)
                {
                    if (TypeInfo->Fields[i].AdditionalInfo.Enum.Values[j].Value == Value)
                    {
                        printf(" (%s)", (const char*)TypeInfo->Fields[i].AdditionalInfo.Enum.Values[j].Name);
                        break;
                    }
                }
            }
        } else if (TypeInfo->Fields[i].Type == SmbiosDataTypeRaw || TypeInfo->Fields[i].Type == SmbiosDataTypeOther)
        {
            BYTE j, *p;
            p = (BYTE*)AddPtr(Table, TypeInfo->Fields[i].Offset);
            for (j = 0; j < TypeInfo->Fields[i].Size; j++)
            {
                printf("%02X", p[j]);
                if (j != TypeInfo->Fields[i].Size - 1)
                {
                    putchar(' ');
                }
            }
        } else if (TypeInfo->Fields[i].Type == SmbiosDataTypeUuid && TypeInfo->Fields[i].Size == 16)
        {
            BYTE* Uuid = (BYTE*)AddPtr(Table, TypeInfo->Fields[i].Offset);
            printf("%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                   Uuid[0], Uuid[1], Uuid[2], Uuid[3], Uuid[4], Uuid[5], Uuid[6], Uuid[7],
                   Uuid[8], Uuid[9], Uuid[10], Uuid[11], Uuid[12], Uuid[13], Uuid[14], Uuid[15]);
        }
        putchar('\n');
    }

_Exit:
    putchar('\n');
    return NextTable;
}

int
main()
{
    PSMBIOS_RAW_DATA Data;
    PSMBIOS_TABLE Table;

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    setlocale(LC_ALL, ".UTF-8");

    if (!GetSmbiosTableData(&Data))
    {
        puts("Get SMBIOS table failed");
        return ENODATA;
    }
    printf("SMBIOS Version: %hhu.%hhu\n", Data->SMBIOSMajorVersion, Data->SMBIOSMinorVersion);
    printf("DMI Revision: %hhu\n", Data->DmiRevision);
    printf("Data Size: %lu bytes\n\n", Data->Length);
    Table = (PSMBIOS_TABLE)Data->SMBIOSTableData;
    do
    {
        Table = PrintSmbiosTable(Table, Data->SMBIOSTableData, AddPtr(Data->SMBIOSTableData, Data->Length));
    } while (Table != NULL);
    free(Data);
    return 0;
}
