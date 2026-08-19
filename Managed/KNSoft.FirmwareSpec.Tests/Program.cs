using System;
using System.Buffers.Binary;
using System.Text;
using KNSoft.FirmwareSpec;

static class Program
{
    private static int Main()
    {
        TestAcpi();
        TestSmbios();
        TestCpuid();
        Console.WriteLine("Managed firmware parser tests passed.");
        return 0;
    }

    private static void TestAcpi()
    {
        byte[] bytes = new byte[AcpiParser.HeaderSize];
        Encoding.ASCII.GetBytes("TEST").CopyTo(bytes, 0);
        BinaryPrimitives.WriteUInt32LittleEndian(bytes.AsSpan(4), (uint)bytes.Length);
        bytes[8] = 1;
        Encoding.ASCII.GetBytes("KNSOFT").CopyTo(bytes, 10);
        Encoding.ASCII.GetBytes("MANAGED ").CopyTo(bytes, 16);
        bytes[9] = unchecked((byte)-Checksum(bytes));

        Equal(FirmwareDecodeStatus.Success, AcpiParser.TryParse(bytes, out AcpiTable? table));
        AcpiTable parsed = NotNull(table);
        Equal("TEST", parsed.Header.Signature);
        Equal(bytes.Length, parsed.Data.Length);
        Equal(5, parsed.Fields.Count);
        Equal(FirmwareDecodeStatus.TruncatedHeader, AcpiParser.TryParse(bytes.AsMemory(0, 10), out _));

        bytes[9]++;
        Equal(FirmwareDecodeStatus.BadChecksum, AcpiParser.TryParse(bytes, out _));
    }

    private static void TestSmbios()
    {
        byte[] structures =
        [
            1, 8, 0x34, 0x12, 1, 2, 0, 0,
            (byte)'K', (byte)'N', (byte)'S', (byte)'o', (byte)'f', (byte)'t', 0,
            (byte)'Z', (byte)'P', (byte)'i', (byte)'g', (byte)'e', (byte)'o', (byte)'n', 0, 0,
            127, 4, 0xFF, 0xFF, 0, 0,
        ];
        byte[] raw = new byte[8 + structures.Length];
        raw[1] = 3;
        raw[2] = 9;
        BinaryPrimitives.WriteUInt32LittleEndian(raw.AsSpan(4), (uint)structures.Length);
        structures.CopyTo(raw, 8);

        Equal(FirmwareDecodeStatus.Success, SmbiosParser.TryParseWindowsRaw(raw, out SmbiosTable? table));
        SmbiosTable parsed = NotNull(table);
        Equal(new SmbiosVersion(3, 9), parsed.Version);
        Equal(2, parsed.Structures.Count);
        Equal(49, SmbiosParser.Types.Count);
        Equal((ushort)0x1234, parsed.Structures[0].Handle);
        Equal("System Information", parsed.Structures[0].TypeName);
        SmbiosFieldInfo manufacturerField = parsed.Structures[0].Fields[0];
        Equal("Manufacturer", manufacturerField.Name);
        True(parsed.Structures[0].TryReadUnsigned(4, 1, out ulong manufacturerIndex));
        Equal(1UL, manufacturerIndex);
        True(parsed.Structures[0].TryGetString(manufacturerField, out string? manufacturer));
        Equal("KNSoft", manufacturer);
        True(parsed.Structures[0].TryGetString(2, out string? product));
        Equal("ZPigeon", product);

        SmbiosTypeInfo memoryDevice = NotNull(SmbiosParser.GetTypeInfo(17));
        SmbiosFieldInfo? technology = null;
        foreach (SmbiosFieldInfo field in memoryDevice.Fields)
        {
            if (field.Name == "Memory Technology")
            {
                technology = field;
                break;
            }
        }
        Equal("MRDIMM (Deprecated in SMBIOS 3.9)", NotNull(NotNull(technology).GetEnumName(8)));

        raw[^1] = 1;
        Equal(FirmwareDecodeStatus.MissingTerminator, SmbiosParser.TryParseWindowsRaw(raw, out _));
    }

    private static void TestCpuid()
    {
        byte[] bytes = new byte[CpuidParser.RecordSize * 2];
        WriteCpuid(bytes, 0, 0, 0, 7, 0x756E6547, 0x6C65746E, 0x49656E69);
        WriteCpuid(bytes, CpuidParser.RecordSize, 1, 0, 0, 0, 1U << 28, 1U << 25);

        Equal(FirmwareDecodeStatus.Success, CpuidParser.TryParse(bytes, out CpuidSnapshot? snapshot));
        CpuidSnapshot parsed = NotNull(snapshot);
        Equal("GenuineIntel", parsed.VendorId);
        Equal(188, CpuidFeatures.All.Count);
        True(parsed.TryGetRecord(1, 0, out CpuidRecord leaf1));
        Equal(1U << 28, leaf1.Ecx);
        True(parsed.IsSupported(new CpuidFeature(1, 0, CpuidRegister.Ecx, 28, "AVX")));
        Equal(FirmwareDecodeStatus.InvalidLength, CpuidParser.TryParse(bytes.AsMemory(0, bytes.Length - 1), out _));

        bytes.AsSpan(CpuidParser.RecordSize, CpuidParser.RecordSize).CopyTo(bytes);
        Equal(FirmwareDecodeStatus.InvalidArgument, CpuidParser.TryParse(bytes, out _));
    }

    private static void WriteCpuid(
        Span<byte> data,
        int offset,
        uint leaf,
        uint subLeaf,
        uint eax,
        uint ebx,
        uint ecx,
        uint edx)
    {
        BinaryPrimitives.WriteUInt32LittleEndian(data[offset..], leaf);
        BinaryPrimitives.WriteUInt32LittleEndian(data[(offset + 4)..], subLeaf);
        BinaryPrimitives.WriteUInt32LittleEndian(data[(offset + 8)..], eax);
        BinaryPrimitives.WriteUInt32LittleEndian(data[(offset + 12)..], ebx);
        BinaryPrimitives.WriteUInt32LittleEndian(data[(offset + 16)..], ecx);
        BinaryPrimitives.WriteUInt32LittleEndian(data[(offset + 20)..], edx);
    }

    private static byte Checksum(ReadOnlySpan<byte> data)
    {
        byte sum = 0;
        foreach (byte value in data)
        {
            sum += value;
        }
        return sum;
    }

    private static T NotNull<T>(T? value) where T : class => value ?? throw new InvalidOperationException();

    private static void Equal<T>(T expected, T actual)
    {
        if (!System.Collections.Generic.EqualityComparer<T>.Default.Equals(expected, actual))
        {
            throw new InvalidOperationException($"Expected {expected}, got {actual}.");
        }
    }

    private static void True(bool value)
    {
        if (!value)
        {
            throw new InvalidOperationException("Expected true.");
        }
    }
}
