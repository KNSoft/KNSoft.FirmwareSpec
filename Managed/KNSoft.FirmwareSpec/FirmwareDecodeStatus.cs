namespace KNSoft.FirmwareSpec;

/// <summary>Describes the result of parsing untrusted firmware data.</summary>
public enum FirmwareDecodeStatus
{
    /// <summary>The operation completed successfully.</summary>
    Success,
    /// <summary>No more records are available.</summary>
    End,
    /// <summary>An argument is invalid.</summary>
    InvalidArgument,
    /// <summary>The fixed header is incomplete.</summary>
    TruncatedHeader,
    /// <summary>A declared length is invalid.</summary>
    InvalidLength,
    /// <summary>The buffer ends before the declared data.</summary>
    TruncatedData,
    /// <summary>A required terminator is missing.</summary>
    MissingTerminator,
    /// <summary>A checksum is invalid.</summary>
    BadChecksum,
    /// <summary>The data format is unsupported.</summary>
    Unsupported,
}
