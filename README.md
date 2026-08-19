| **English (en-US)** | [简体中文 (zh-CN)](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/README.zh-CN.md) |
| --- | --- |

&nbsp;

# KNSoft.FirmwareSpec

[![NuGet Downloads](https://img.shields.io/nuget/dt/KNSoft.FirmwareSpec)](https://www.nuget.org/packages/KNSoft.FirmwareSpec) [![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/KNSoft/KNSoft.FirmwareSpec/Build.yml)](https://github.com/KNSoft/KNSoft.FirmwareSpec/actions/workflows/Build.yml) ![PR Welcome](https://img.shields.io/badge/PR-welcome-0688CB.svg) [![GitHub License](https://img.shields.io/github/license/KNSoft/KNSoft.FirmwareSpec)](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/LICENSE)

[KNSoft.FirmwareSpec](https://github.com/KNSoft/KNSoft.FirmwareSpec) provides C/C++ definitions, explicit type metadata, and zero-copy decoders for firmware and processor interfaces. It is compatible with MSVC and GCC.

| Specification | Definition | Type information | Sample program |
| :- | :- | :- | :- |
| [SMBIOS 3.9.0](https://www.dmtf.org/standards/smbios) | [SMBIOS.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/SMBIOS.h) | [SMBIOS.TypeInfo.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/SMBIOS.TypeInfo.h) | [SmbiosDecode.c](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/SmbiosDecode.c) |
| [ACPI 6.6](https://uefi.org/specifications) | [ACPI.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/ACPI.h) | [ACPI.TypeInfo.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/ACPI.TypeInfo.h) | [AcpiDecode.c](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/AcpiDecode.c) |
| CPUID | [CPUID.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/CPUID.h) | [CPUID.TypeInfo.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/CPUID.TypeInfo.h) | [CpuidDecode.c](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/CpuidDecode.c) |

The shared decoder primitives in `FirmwareDecode.h` perform bounds, overflow, length, and checksum validation without allocation. Complex AML, OEM-defined, and architecture-specific payloads remain available as bounded raw data when a compact generic representation would lose semantics.

[GitHub Action](https://github.com/KNSoft/KNSoft.FirmwareSpec/actions) compiles and runs sample programs, artifact contains them and theirs text outputs.

## Usage

NuGet package [KNSoft.FirmwareSpec](https://www.nuget.org/packages/KNSoft.FirmwareSpec) is out-of-the-box, install to project and include wanted header, for example:
```C
#include <KNSoft/FirmwareSpec/SMBIOS.Decode.h>
#include <KNSoft/FirmwareSpec/ACPI.Decode.h>
#include <KNSoft/FirmwareSpec/CPUID.Decode.h>
```

SMBIOS definitions support compile-time version selection through `SMBIOS_VERSION`. See each header for its supported specification level.

> [!CAUTION]
> In beta stage, may contains some bugs and various issues, should be used with caution.

You could [report issue](https://github.com/KNSoft/KNSoft.FirmwareSpec/issues/new) or [send PR](https://github.com/KNSoft/KNSoft.FirmwareSpec/pulls) to make this project better.

## License

[KNSoft.FirmwareSpec](https://github.com/KNSoft/KNSoft.FirmwareSpec) is licensed under the [MIT](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/LICENSE) license.
