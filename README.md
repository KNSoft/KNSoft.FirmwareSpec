| **English (en-US)** | [简体中文 (zh-CN)](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/README.zh-CN.md) |
| --- | --- |

&nbsp;

# KNSoft.FirmwareSpec

[![NuGet Downloads](https://img.shields.io/nuget/dt/KNSoft.FirmwareSpec)](https://www.nuget.org/packages/KNSoft.FirmwareSpec) [![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/KNSoft/KNSoft.FirmwareSpec/Build.yml)](https://github.com/KNSoft/KNSoft.FirmwareSpec/actions/workflows/Build.yml) ![PR Welcome](https://img.shields.io/badge/PR-welcome-0688CB.svg) [![GitHub License](https://img.shields.io/github/license/KNSoft/KNSoft.FirmwareSpec)](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/LICENSE)

[KNSoft.FirmwareSpec](https://github.com/KNSoft/KNSoft.FirmwareSpec) contains C/C++ definitions of multiple firmware specifications and corresponding type information, compatible with MSVC and GCC compilers.

| Specification | Definition | Type information | Sample program |
| :- | :- | :- | :- |
| [SMBIOS reference specification](https://www.dmtf.org/standards/smbios) | [SMBIOS.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/SMBIOS.h) | [SMBIOS.TypeInfo.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/SMBIOS.TypeInfo.h) | [SmbiosDecode](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/SmbiosDecode.c) |
| [ACPI Specification](https://uefi.org/specifications) | (TODO) | - | - |
| CPUID | (WIP) | - | - |

[TypeInfoGenerator](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/TypeInfoGenerator) generates type information (e.g. [SMBIOS.TypeInfo.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/SMBIOS.TypeInfo.h)) according to the corresponding specification definitions (e.g. [SMBIOS.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/SMBIOS.h)), type information is very useful for tools like `dmidecode` to resolve each of structure fields.

[GitHub Action](https://github.com/KNSoft/KNSoft.FirmwareSpec/actions) compiles and runs sample programs, artifact contains them and theirs text outputs.

## Usage

NuGet package [KNSoft.FirmwareSpec](https://www.nuget.org/packages/KNSoft.FirmwareSpec) is out-of-the-box, install to project and include wanted header, for example:
```C
#include <KNSoft/FirmwareSpec/SMBIOS.h>             // SMBIOS reference specification
#include <KNSoft/FirmwareSpec/SMBIOS.TypeInfo.h>    // Type information of SMBIOS reference specification
```

We always keep our definitions up-to-date, support version control by using macro (e.g. `SMBIOS_VERSION`), see corresponding header for more information.

> [!CAUTION]
> In beta stage, may contains some bugs and various issues, should be used with caution.

You could [report issue](https://github.com/KNSoft/KNSoft.FirmwareSpec/issues/new) or [send PR](https://github.com/KNSoft/KNSoft.FirmwareSpec/pulls) to make this project better.

## License

[KNSoft.FirmwareSpec](https://github.com/KNSoft/KNSoft.FirmwareSpec) is licensed under the [MIT](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/LICENSE) license.
