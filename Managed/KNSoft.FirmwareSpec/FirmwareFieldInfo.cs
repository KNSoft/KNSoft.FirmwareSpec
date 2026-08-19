namespace KNSoft.FirmwareSpec;

/// <summary>Describes one safely readable field in a binary firmware structure.</summary>
public readonly record struct FirmwareFieldInfo(string Name, int Offset, int Size);
