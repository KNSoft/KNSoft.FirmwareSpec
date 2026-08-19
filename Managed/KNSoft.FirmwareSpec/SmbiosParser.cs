using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;

namespace KNSoft.FirmwareSpec;

/// <summary>An SMBIOS specification version.</summary>
public readonly record struct SmbiosVersion(byte Major, byte Minor, byte DmiRevision = 0);

/// <summary>One SMBIOS structure backed by the caller-supplied table memory.</summary>
public sealed class SmbiosStructure
{
    private readonly ReadOnlyMemory<byte> _strings;

    internal SmbiosStructure(byte type, byte length, ushort handle, int offset, ReadOnlyMemory<byte> data,
        ReadOnlyMemory<byte> formatted, ReadOnlyMemory<byte> strings)
    {
        Type = type;
        Length = length;
        Handle = handle;
        Offset = offset;
        Data = data;
        Formatted = formatted;
        _strings = strings;
    }

    /// <summary>Gets the SMBIOS structure type.</summary>
    public byte Type { get; }

    /// <summary>Gets the formatted structure length, including its four-byte header.</summary>
    public byte Length { get; }

    /// <summary>Gets the structure handle.</summary>
    public ushort Handle { get; }

    /// <summary>Gets the byte offset in the containing SMBIOS structure table.</summary>
    public int Offset { get; }

    /// <summary>Gets the formatted structure and terminating string-set.</summary>
    public ReadOnlyMemory<byte> Data { get; }

    /// <summary>Gets the formatted structure, including its four-byte header.</summary>
    public ReadOnlyMemory<byte> Formatted { get; }

    /// <summary>Gets a stable English name for the structure type.</summary>
    public string TypeName => SmbiosParser.GetTypeName(Type);

    /// <summary>Reads a little-endian unsigned formatted field without throwing for an invalid range.</summary>
    public bool TryReadUnsigned(int offset, int size, out ulong value)
    {
        value = 0;
        if (!FirmwareBinary.Contains(Formatted, offset, size))
        {
            return false;
        }

        ReadOnlySpan<byte> data = Formatted.Span;
        value = size switch
        {
            1 => data[offset],
            2 => FirmwareBinary.UInt16(data, offset),
            4 => FirmwareBinary.UInt32(data, offset),
            8 => FirmwareBinary.UInt64(data, offset),
            _ => 0,
        };
        return size is 1 or 2 or 4 or 8;
    }

    /// <summary>Resolves a one-based SMBIOS string index. Index zero means no string.</summary>
    public bool TryGetString(byte index, out string? value)
    {
        value = null;
        if (index == 0)
        {
            return true;
        }

        ReadOnlySpan<byte> strings = _strings.Span;
        int start = 0;
        for (int current = 1; start < strings.Length; current++)
        {
            if (strings[start] == 0)
            {
                return false;
            }
            int length = strings[start..].IndexOf((byte)0);
            if (length < 0)
            {
                return false;
            }
            if (current == index)
            {
                value = Encoding.UTF8.GetString(strings.Slice(start, length));
                return true;
            }
            start += length + 1;
        }
        return false;
    }
}

/// <summary>A validated SMBIOS structure table.</summary>
public sealed class SmbiosTable
{
    internal SmbiosTable(SmbiosVersion version, ReadOnlyMemory<byte> data, List<SmbiosStructure> structures)
    {
        Version = version;
        Data = data;
        Structures = new ReadOnlyCollection<SmbiosStructure>(structures);
    }

    /// <summary>Gets the SMBIOS version supplied by the transport envelope or caller.</summary>
    public SmbiosVersion Version { get; }

    /// <summary>Gets the bare SMBIOS structure-table bytes.</summary>
    public ReadOnlyMemory<byte> Data { get; }

    /// <summary>Gets the decoded structures in source order.</summary>
    public IReadOnlyList<SmbiosStructure> Structures { get; }
}

/// <summary>Parses SMBIOS data returned by Windows or a controlled endpoint.</summary>
public static class SmbiosParser
{
    private const int RawHeaderSize = 8;
    private const int StructureHeaderSize = 4;

    /// <summary>Parses the RAW_SMBIOS_DATA buffer returned by GetSystemFirmwareTable.</summary>
    public static FirmwareDecodeStatus TryParseWindowsRaw(ReadOnlyMemory<byte> data, out SmbiosTable? table)
    {
        table = null;
        if (data.Length < RawHeaderSize)
        {
            return FirmwareDecodeStatus.TruncatedHeader;
        }

        ReadOnlySpan<byte> bytes = data.Span;
        uint length = FirmwareBinary.UInt32(bytes, 4);
        if (length > data.Length - RawHeaderSize)
        {
            return FirmwareDecodeStatus.TruncatedData;
        }

        SmbiosVersion version = new(bytes[1], bytes[2], bytes[3]);
        return TryParse(data.Slice(RawHeaderSize, checked((int)length)), version, out table);
    }

    /// <summary>Parses a bare SMBIOS structure table.</summary>
    public static FirmwareDecodeStatus TryParse(
        ReadOnlyMemory<byte> data,
        SmbiosVersion version,
        out SmbiosTable? table)
    {
        table = null;
        List<SmbiosStructure> structures = [];
        int offset = 0;
        while (offset < data.Length)
        {
            if (data.Length - offset < StructureHeaderSize)
            {
                return FirmwareDecodeStatus.TruncatedHeader;
            }

            ReadOnlySpan<byte> bytes = data.Span;
            byte type = bytes[offset];
            byte length = bytes[offset + 1];
            if (length < StructureHeaderSize)
            {
                return FirmwareDecodeStatus.InvalidLength;
            }
            if (length > data.Length - offset)
            {
                return FirmwareDecodeStatus.TruncatedData;
            }

            int stringStart = offset + length;
            int end = FindStringSetEnd(bytes, stringStart);
            if (end < 0)
            {
                return FirmwareDecodeStatus.MissingTerminator;
            }

            ushort handle = FirmwareBinary.UInt16(bytes, offset + 2);
            int totalLength = end - offset;
            structures.Add(new SmbiosStructure(
                type,
                length,
                handle,
                offset,
                data.Slice(offset, totalLength),
                data.Slice(offset, length),
                data.Slice(stringStart, end - stringStart)));
            offset = end;
            if (type == 127)
            {
                break;
            }
        }

        table = new SmbiosTable(version, data[..offset], structures);
        return FirmwareDecodeStatus.Success;
    }

    /// <summary>Gets a stable English name for a standard SMBIOS structure type.</summary>
    public static string GetTypeName(byte type) => type switch
    {
        0 => "BIOS Information",
        1 => "System Information",
        2 => "Baseboard Information",
        3 => "System Enclosure",
        4 => "Processor Information",
        5 => "Memory Controller Information",
        6 => "Memory Module Information",
        7 => "Cache Information",
        8 => "Port Connector Information",
        9 => "System Slots",
        10 => "On Board Devices Information",
        11 => "OEM Strings",
        12 => "System Configuration Options",
        13 => "BIOS Language Information",
        14 => "Group Associations",
        15 => "System Event Log",
        16 => "Physical Memory Array",
        17 => "Memory Device",
        18 => "32-Bit Memory Error Information",
        19 => "Memory Array Mapped Address",
        20 => "Memory Device Mapped Address",
        21 => "Built-in Pointing Device",
        22 => "Portable Battery",
        23 => "System Reset",
        24 => "Hardware Security",
        25 => "System Power Controls",
        26 => "Voltage Probe",
        27 => "Cooling Device",
        28 => "Temperature Probe",
        29 => "Electrical Current Probe",
        30 => "Out-of-Band Remote Access",
        31 => "Boot Integrity Services Entry Point",
        32 => "System Boot Information",
        33 => "64-Bit Memory Error Information",
        34 => "Management Device",
        35 => "Management Device Component",
        36 => "Management Device Threshold Data",
        37 => "Memory Channel",
        38 => "IPMI Device Information",
        39 => "System Power Supply",
        40 => "Additional Information",
        41 => "Onboard Devices Extended Information",
        42 => "Management Controller Host Interface",
        43 => "TPM Device",
        44 => "Processor Additional Information",
        45 => "Firmware Inventory Information",
        46 => "String Property",
        126 => "Inactive",
        127 => "End-of-Table",
        _ => "OEM or Unknown",
    };

    private static int FindStringSetEnd(ReadOnlySpan<byte> data, int start)
    {
        for (int index = start; index + 1 < data.Length; index++)
        {
            if (data[index] == 0 && data[index + 1] == 0)
            {
                return index + 2;
            }
        }
        return -1;
    }
}
