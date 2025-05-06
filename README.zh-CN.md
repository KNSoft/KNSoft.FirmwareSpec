| [English (en-US)](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/README.md) | **简体中文 (zh-CN)** |
| --- | --- |

&nbsp;

# KNSoft.FirmwareSpec

[![NuGet Downloads](https://img.shields.io/nuget/dt/KNSoft.FirmwareSpec)](https://www.nuget.org/packages/KNSoft.FirmwareSpec) [![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/KNSoft/KNSoft.FirmwareSpec/Build.yml)](https://github.com/KNSoft/KNSoft.FirmwareSpec/actions/workflows/Build.yml) ![PR Welcome](https://img.shields.io/badge/PR-welcome-0688CB.svg) [![GitHub License](https://img.shields.io/github/license/KNSoft/KNSoft.FirmwareSpec)](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/LICENSE)

[KNSoft.FirmwareSpec](https://github.com/KNSoft/KNSoft.FirmwareSpec)包含多种固件规范及对应类型信息的C/C++定义，兼容MSVC和GCC编译器。

| 规范 | 定义 | 类型信息 | 示例程序 |
| :- | :- | :- | :- |
| [SMBIOS 参考规范](https://www.dmtf.org/standards/smbios) | [SMBIOS.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/SMBIOS.h) | [SMBIOS.TypeInfo.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/SMBIOS.TypeInfo.h) | [SmbiosDecode](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/SmbiosDecode.c) |
| [ACPI规范](https://uefi.org/specifications) | (TODO) | - | - |
| CPUID | [CPUID.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/CPUID.h) | (WIP) | (WIP) |

[TypeInfoGenerator](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/TypeInfoGenerator)根据对应的规范定义（如[SMBIOS.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/SMBIOS.h)）生成类型信息（如[SMBIOS.TypeInfo.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/SMBIOS.TypeInfo.h)），类型信息对于像`dmidecode`这样的工具用以解析各个结构体成员十分有用。

[GitHub Action](https://github.com/KNSoft/KNSoft.FirmwareSpec/actions)编译并运行示例程序，制品包含它们及它们的文本输出。

## Usage

NuGet包[KNSoft.FirmwareSpec](https://www.nuget.org/packages/KNSoft.FirmwareSpec)是开箱即用的，安装到项目后按需包含头文件，例如：
```C
#include <KNSoft/FirmwareSpec/SMBIOS.h>             // SMBIOS参考标准
#include <KNSoft/FirmwareSpec/SMBIOS.TypeInfo.h>    // SMBIOS参考标准的类型信息

#include <KNSoft/FirmwareSpec/CPUID.h>              // CPUID
```

我们始终保持定义更新，支持使用宏（如`SMBIOS_VERSION`）控制版本，更多信息参考对应的头文件。

> [!CAUTION]
> 处于Beta阶段，可能存在一些Bug和各种问题，应谨慎使用。

您可以[报告问题](https://github.com/KNSoft/KNSoft.SMBIOS/issues/new)或[提交PR](https://github.com/KNSoft/KNSoft.SMBIOS/pulls)让此项目更好。

## 协议

[KNSoft.FirmwareSpec](https://github.com/KNSoft/KNSoft.FirmwareSpec)根据[MIT](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/LICENSE)协议进行许可。
