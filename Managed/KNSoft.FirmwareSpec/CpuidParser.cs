using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;

namespace KNSoft.FirmwareSpec;

/// <summary>A CPUID output register.</summary>
public enum CpuidRegister
{
    /// <summary>EAX.</summary>
    Eax,
    /// <summary>EBX.</summary>
    Ebx,
    /// <summary>ECX.</summary>
    Ecx,
    /// <summary>EDX.</summary>
    Edx,
}

/// <summary>One CPUID invocation and its four output registers.</summary>
public readonly record struct CpuidRecord(uint Leaf, uint SubLeaf, uint Eax, uint Ebx, uint Ecx, uint Edx)
{
    /// <summary>Gets one output register.</summary>
    public uint GetRegister(CpuidRegister register) => register switch
    {
        CpuidRegister.Eax => Eax,
        CpuidRegister.Ebx => Ebx,
        CpuidRegister.Ecx => Ecx,
        CpuidRegister.Edx => Edx,
        _ => throw new ArgumentOutOfRangeException(nameof(register)),
    };
}

/// <summary>Describes one named CPUID feature bit.</summary>
public readonly record struct CpuidFeature(uint Leaf, uint SubLeaf, CpuidRegister Register, byte Bit, string Name);

/// <summary>A decoded set of CPUID records.</summary>
public sealed class CpuidSnapshot
{
    internal CpuidSnapshot(List<CpuidRecord> records)
    {
        Records = new ReadOnlyCollection<CpuidRecord>(records);
        VendorId = GetVendorId(records);
    }

    /// <summary>Gets records in transport order.</summary>
    public IReadOnlyList<CpuidRecord> Records { get; }

    /// <summary>Gets the vendor ID from basic leaf zero, or an empty string when absent.</summary>
    public string VendorId { get; }

    /// <summary>Finds one leaf and subleaf.</summary>
    public bool TryGetRecord(uint leaf, uint subLeaf, out CpuidRecord record)
    {
        foreach (CpuidRecord candidate in Records)
        {
            if (candidate.Leaf == leaf && candidate.SubLeaf == subLeaf)
            {
                record = candidate;
                return true;
            }
        }
        record = default;
        return false;
    }

    /// <summary>Tests a feature against the captured register records.</summary>
    public bool IsSupported(CpuidFeature feature) =>
        feature.Bit < 32 && feature.Register is >= CpuidRegister.Eax and <= CpuidRegister.Edx &&
        TryGetRecord(feature.Leaf, feature.SubLeaf, out CpuidRecord record) &&
        (record.GetRegister(feature.Register) & (1U << feature.Bit)) != 0;

    private static string GetVendorId(List<CpuidRecord> records)
    {
        foreach (CpuidRecord record in records)
        {
            if (record.Leaf != 0 || record.SubLeaf != 0)
            {
                continue;
            }

            Span<byte> value = stackalloc byte[12];
            WriteUInt32(value, 0, record.Ebx);
            WriteUInt32(value, 4, record.Edx);
            WriteUInt32(value, 8, record.Ecx);
            return Encoding.ASCII.GetString(value);
        }
        return string.Empty;
    }

    private static void WriteUInt32(Span<byte> destination, int offset, uint value)
    {
        destination[offset] = (byte)value;
        destination[offset + 1] = (byte)(value >> 8);
        destination[offset + 2] = (byte)(value >> 16);
        destination[offset + 3] = (byte)(value >> 24);
    }
}

/// <summary>Parses the compact CPUID transport format used by remote collectors.</summary>
public static class CpuidParser
{
    /// <summary>The size of one little-endian Leaf/SubLeaf/EAX/EBX/ECX/EDX record.</summary>
    public const int RecordSize = 24;

    /// <summary>Parses records without executing CPUID on the parsing machine.</summary>
    public static FirmwareDecodeStatus TryParse(ReadOnlyMemory<byte> data, out CpuidSnapshot? snapshot)
    {
        snapshot = null;
        if (data.Length == 0 || data.Length % RecordSize != 0)
        {
            return FirmwareDecodeStatus.InvalidLength;
        }

        ReadOnlySpan<byte> bytes = data.Span;
        List<CpuidRecord> records = new(data.Length / RecordSize);
        HashSet<ulong> keys = new(data.Length / RecordSize);
        for (int offset = 0; offset < data.Length; offset += RecordSize)
        {
            uint leaf = FirmwareBinary.UInt32(bytes, offset);
            uint subLeaf = FirmwareBinary.UInt32(bytes, offset + 4);
            if (!keys.Add(((ulong)leaf << 32) | subLeaf))
            {
                return FirmwareDecodeStatus.InvalidArgument;
            }

            records.Add(new CpuidRecord(
                leaf,
                subLeaf,
                FirmwareBinary.UInt32(bytes, offset + 8),
                FirmwareBinary.UInt32(bytes, offset + 12),
                FirmwareBinary.UInt32(bytes, offset + 16),
                FirmwareBinary.UInt32(bytes, offset + 20)));
        }

        snapshot = new CpuidSnapshot(records);
        return FirmwareDecodeStatus.Success;
    }
}
