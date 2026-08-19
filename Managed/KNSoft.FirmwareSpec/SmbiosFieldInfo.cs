using System;
using System.Collections.Generic;

namespace KNSoft.FirmwareSpec;

/// <summary>Identifies how an SMBIOS field is represented.</summary>
public enum SmbiosDataType
{
    /// <summary>The field has structure-specific semantics.</summary>
    Other,
    /// <summary>The field is a one-based SMBIOS string index.</summary>
    StringIndex,
    /// <summary>The field is an unsigned integer.</summary>
    UnsignedInteger,
    /// <summary>The field is a single bit.</summary>
    Bit,
    /// <summary>The field is an enumerated integer.</summary>
    Enum,
    /// <summary>The field is an SMBIOS UUID.</summary>
    Uuid,
    /// <summary>The field is opaque binary data.</summary>
    Raw,
}

/// <summary>Maps an SMBIOS enumerated value to its specification name.</summary>
public readonly record struct SmbiosEnumInfo(string Name, ulong Value);

/// <summary>Describes one SMBIOS formatted field.</summary>
public sealed class SmbiosFieldInfo
{
    internal SmbiosFieldInfo(string name, int offset, int size, SmbiosDataType dataType, int bitOffset,
        int bitCount, SmbiosEnumInfo[] enumValues)
    {
        Name = name;
        Offset = offset;
        Size = size;
        DataType = dataType;
        BitOffset = bitOffset;
        BitCount = bitCount;
        EnumValues = enumValues.Length == 0 ? enumValues : Array.AsReadOnly(enumValues);
    }

    /// <summary>Gets the specification name.</summary>
    public string Name { get; }

    /// <summary>Gets the byte offset of the field or its backing integer.</summary>
    public int Offset { get; }

    /// <summary>Gets the byte size of the field or its backing integer.</summary>
    public int Size { get; }

    /// <summary>Gets the field representation.</summary>
    public SmbiosDataType DataType { get; }

    /// <summary>Gets whether this field occupies part of a backing integer.</summary>
    public bool IsBitField => BitCount != 0;

    /// <summary>Gets the bit offset in the backing integer, or zero for an ordinary field.</summary>
    public int BitOffset { get; }

    /// <summary>Gets the bit count, or zero for an ordinary field.</summary>
    public int BitCount { get; }

    /// <summary>Gets the known names for an enumerated field.</summary>
    public IReadOnlyList<SmbiosEnumInfo> EnumValues { get; }

    /// <summary>Gets the specification name of an enumerated value, if known.</summary>
    public string? GetEnumName(ulong value)
    {
        foreach (SmbiosEnumInfo item in EnumValues)
        {
            if (item.Value == value)
            {
                return item.Name;
            }
        }
        return null;
    }
}

/// <summary>Describes one standard SMBIOS structure type.</summary>
public sealed class SmbiosTypeInfo
{
    internal SmbiosTypeInfo(byte type, string name, SmbiosFieldInfo[] fields)
    {
        Type = type;
        Name = name;
        Fields = fields.Length == 0 ? fields : Array.AsReadOnly(fields);
    }

    /// <summary>Gets the structure type number.</summary>
    public byte Type { get; }

    /// <summary>Gets the specification name.</summary>
    public string Name { get; }

    /// <summary>Gets the formatted fields in specification order.</summary>
    public IReadOnlyList<SmbiosFieldInfo> Fields { get; }
}
