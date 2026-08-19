using System;
using System.Buffers.Binary;
using System.Text;

namespace KNSoft.FirmwareSpec;

internal static class FirmwareBinary
{
    internal static bool Contains(ReadOnlyMemory<byte> data, int offset, int length) =>
        offset >= 0 && length >= 0 && offset <= data.Length && length <= data.Length - offset;

    internal static ushort UInt16(ReadOnlySpan<byte> data, int offset) =>
        BinaryPrimitives.ReadUInt16LittleEndian(data.Slice(offset, sizeof(ushort)));

    internal static uint UInt32(ReadOnlySpan<byte> data, int offset) =>
        BinaryPrimitives.ReadUInt32LittleEndian(data.Slice(offset, sizeof(uint)));

    internal static ulong UInt64(ReadOnlySpan<byte> data, int offset) =>
        BinaryPrimitives.ReadUInt64LittleEndian(data.Slice(offset, sizeof(ulong)));

    internal static string Ascii(ReadOnlySpan<byte> data)
    {
        int end = data.IndexOf((byte)0);
        return Encoding.ASCII.GetString(end < 0 ? data : data[..end]).TrimEnd();
    }

    internal static bool Checksum8(ReadOnlySpan<byte> data)
    {
        byte sum = 0;
        foreach (byte value in data)
        {
            sum += value;
        }
        return sum == 0;
    }
}
