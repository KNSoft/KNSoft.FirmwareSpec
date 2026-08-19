using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace KNSoft.FirmwareSpec;

/// <summary>The common header of an ACPI description table.</summary>
public readonly record struct AcpiHeader(
    string Signature,
    uint Length,
    byte Revision,
    byte Checksum,
    string OemId,
    string OemTableId,
    uint OemRevision,
    uint CreatorId,
    uint CreatorRevision);

/// <summary>A validated ACPI table backed by the caller-supplied memory.</summary>
public sealed class AcpiTable
{
    internal AcpiTable(AcpiHeader header, ReadOnlyMemory<byte> data)
    {
        Header = header;
        Data = data;
    }

    /// <summary>Gets the parsed common table header.</summary>
    public AcpiHeader Header { get; }

    /// <summary>Gets the exact table bytes, excluding trailing transport data.</summary>
    public ReadOnlyMemory<byte> Data { get; }

    /// <summary>Gets the payload after the common ACPI table header.</summary>
    public ReadOnlyMemory<byte> Body => Data[AcpiParser.HeaderSize..];

    /// <summary>Gets a stable English description for a known table signature.</summary>
    public string Description => AcpiParser.GetDescription(Header.Signature);

    /// <summary>Gets the common header and known table-specific field metadata.</summary>
    public IReadOnlyList<FirmwareFieldInfo> Fields => AcpiParser.GetFields(Header.Signature);

    /// <summary>Reads a little-endian unsigned field without throwing for an invalid range.</summary>
    public bool TryReadUnsigned(int offset, int size, out ulong value)
    {
        value = 0;
        if (!FirmwareBinary.Contains(Data, offset, size))
        {
            return false;
        }

        ReadOnlySpan<byte> data = Data.Span;
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
}

/// <summary>Parses ACPI tables returned by a controlled endpoint.</summary>
public static class AcpiParser
{
    /// <summary>The byte size of the common ACPI description header.</summary>
    public const int HeaderSize = 36;

    private static readonly ReadOnlyCollection<FirmwareFieldInfo> HeaderFields = Array.AsReadOnly<FirmwareFieldInfo>(
    [
        new("Length", 4, 4),
        new("Revision", 8, 1),
        new("Checksum", 9, 1),
        new("OEM Revision", 24, 4),
        new("Creator Revision", 32, 4),
    ]);

    private static readonly Dictionary<string, FirmwareFieldInfo[]> TableFields = new(StringComparer.Ordinal)
    {
        ["APIC"] = [new("Local APIC Address", 36, 4), new("Flags", 40, 4)],
        ["BGRT"] =
        [
            new("Version", 36, 2), new("Status", 38, 1), new("Image Type", 39, 1),
            new("Image Address", 40, 8), new("Image Offset X", 48, 4), new("Image Offset Y", 52, 4),
        ],
        ["FACP"] =
        [
            new("Firmware Control", 36, 4), new("DSDT", 40, 4), new("Preferred PM Profile", 45, 1),
            new("SCI Interrupt", 46, 2), new("SMI Command", 48, 4), new("Flags", 112, 4),
            new("Reset Value", 128, 1), new("Extended Firmware Control", 132, 8),
            new("Extended DSDT", 140, 8), new("Hypervisor Vendor Identity", 268, 8),
        ],
        ["HPET"] =
        [
            new("Event Timer Block ID", 36, 4), new("HPET Number", 52, 1),
            new("Minimum Clock Tick", 53, 2), new("Page Protection", 55, 1),
        ],
        ["SLIT"] = [new("Locality Count", 36, 8)],
        ["TPM2"] = [new("Platform Class", 36, 2), new("Control Area", 40, 8), new("Start Method", 48, 4)],
        ["WAET"] = [new("Flags", 36, 4)],
        ["WSMT"] = [new("Protection Flags", 36, 4)],
    };

    private static readonly Dictionary<string, IReadOnlyList<FirmwareFieldInfo>> AllFields = CreateFields();

    private static readonly Dictionary<string, string> Descriptions = new(StringComparer.Ordinal)
    {
        ["APIC"] = "Multiple APIC Description Table",
        ["BGRT"] = "Boot Graphics Resource Table",
        ["DSDT"] = "Differentiated System Description Table (AML)",
        ["FACP"] = "Fixed ACPI Description Table",
        ["HMAT"] = "Heterogeneous Memory Attribute Table",
        ["HPET"] = "High Precision Event Timer Table",
        ["IORT"] = "IO Remapping Table",
        ["MCFG"] = "PCI Express Memory Mapped Configuration Table",
        ["NFIT"] = "NVDIMM Firmware Interface Table",
        ["PCCT"] = "Platform Communications Channel Table",
        ["PPTT"] = "Processor Properties Topology Table",
        ["RSDT"] = "Root System Description Table",
        ["SLIT"] = "System Locality Information Table",
        ["SRAT"] = "System Resource Affinity Table",
        ["SSDT"] = "Secondary System Description Table (AML)",
        ["TPM2"] = "Trusted Computing Platform 2 Table",
        ["WAET"] = "Windows ACPI Emulated Devices Table",
        ["WSMT"] = "Windows SMM Security Mitigations Table",
        ["XSDT"] = "Extended System Description Table",
    };

    /// <summary>Validates and parses one ACPI table.</summary>
    public static FirmwareDecodeStatus TryParse(
        ReadOnlyMemory<byte> data,
        out AcpiTable? table,
        bool validateChecksum = true)
    {
        table = null;
        if (data.Length < HeaderSize)
        {
            return FirmwareDecodeStatus.TruncatedHeader;
        }

        ReadOnlySpan<byte> bytes = data.Span;
        uint length = FirmwareBinary.UInt32(bytes, 4);
        if (length < HeaderSize)
        {
            return FirmwareDecodeStatus.InvalidLength;
        }
        if (length > data.Length)
        {
            return FirmwareDecodeStatus.TruncatedData;
        }

        int tableLength = checked((int)length);
        ReadOnlyMemory<byte> tableData = data[..tableLength];
        if (validateChecksum && !FirmwareBinary.Checksum8(tableData.Span))
        {
            return FirmwareDecodeStatus.BadChecksum;
        }

        AcpiHeader header = new(
            FirmwareBinary.Ascii(bytes[..4]),
            length,
            bytes[8],
            bytes[9],
            FirmwareBinary.Ascii(bytes.Slice(10, 6)),
            FirmwareBinary.Ascii(bytes.Slice(16, 8)),
            FirmwareBinary.UInt32(bytes, 24),
            FirmwareBinary.UInt32(bytes, 28),
            FirmwareBinary.UInt32(bytes, 32));
        table = new AcpiTable(header, tableData);
        return FirmwareDecodeStatus.Success;
    }

    /// <summary>Gets a stable English description for a table signature.</summary>
    public static string GetDescription(string signature) =>
        Descriptions.TryGetValue(signature, out string? description) ? description : "Unknown ACPI Table";

    /// <summary>Gets common and table-specific unsigned field metadata.</summary>
    public static IReadOnlyList<FirmwareFieldInfo> GetFields(string signature)
    {
        return AllFields.TryGetValue(signature, out IReadOnlyList<FirmwareFieldInfo>? fields) ? fields : HeaderFields;
    }

    private static Dictionary<string, IReadOnlyList<FirmwareFieldInfo>> CreateFields()
    {
        Dictionary<string, IReadOnlyList<FirmwareFieldInfo>> result = new(StringComparer.Ordinal);
        foreach ((string signature, FirmwareFieldInfo[] fields) in TableFields)
        {
            FirmwareFieldInfo[] combined = new FirmwareFieldInfo[HeaderFields.Count + fields.Length];
            for (int index = 0; index < HeaderFields.Count; index++)
            {
                combined[index] = HeaderFields[index];
            }
            fields.CopyTo(combined, HeaderFields.Count);
            result.Add(signature, new ReadOnlyCollection<FirmwareFieldInfo>(combined));
        }
        return result;
    }
}
