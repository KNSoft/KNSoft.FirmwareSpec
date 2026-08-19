| [English (en-US)](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/README.md) | **简体中文 (zh-CN)** |
| --- | --- |

&nbsp;

# KNSoft.FirmwareSpec

[![NuGet Downloads](https://img.shields.io/nuget/dt/KNSoft.FirmwareSpec)](https://www.nuget.org/packages/KNSoft.FirmwareSpec) [![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/KNSoft/KNSoft.FirmwareSpec/Build.yml)](https://github.com/KNSoft/KNSoft.FirmwareSpec/actions/workflows/Build.yml) ![PR Welcome](https://img.shields.io/badge/PR-welcome-0688CB.svg) [![GitHub License](https://img.shields.io/github/license/KNSoft/KNSoft.FirmwareSpec)](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/LICENSE)

[KNSoft.FirmwareSpec](https://github.com/KNSoft/KNSoft.FirmwareSpec)提供固件和处理器接口的 C/C++ 定义、显式类型元数据与零拷贝解码器，兼容 MSVC 和 GCC。

| 规范 | 定义 | 类型信息 | 示例程序 |
| :- | :- | :- | :- |
| [SMBIOS 3.9.0](https://www.dmtf.org/standards/smbios) | [SMBIOS.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/SMBIOS.h) | [SMBIOS.TypeInfo.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/SMBIOS.TypeInfo.h) | [SmbiosDecode.c](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/SmbiosDecode.c) |
| [ACPI 6.6](https://uefi.org/specifications) | [ACPI.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/ACPI.h) | [ACPI.TypeInfo.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/ACPI.TypeInfo.h) | [AcpiDecode.c](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/AcpiDecode.c) |
| CPUID | [CPUID.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/CPUID.h) | [CPUID.TypeInfo.h](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/CPUID.TypeInfo.h) | [CpuidDecode.c](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/CpuidDecode.c) |

`FirmwareDecode.h` 提供无内存分配的公共解码基础，并统一完成边界、溢出、长度和校验和检查。对于 AML、OEM 自定义内容及架构专用数据等复杂载荷，库会保留有边界的原始数据，避免通用抽象损失语义。

[GitHub Action](https://github.com/KNSoft/KNSoft.FirmwareSpec/actions)编译并运行示例程序，制品包含它们及它们的文本输出。

## Usage

NuGet包[KNSoft.FirmwareSpec](https://www.nuget.org/packages/KNSoft.FirmwareSpec)是开箱即用的，安装到项目后按需包含头文件，例如：
```C
#include <KNSoft/FirmwareSpec/SMBIOS.Decode.h>
#include <KNSoft/FirmwareSpec/ACPI.Decode.h>
#include <KNSoft/FirmwareSpec/CPUID.Decode.h>
```

SMBIOS 定义支持通过 `SMBIOS_VERSION` 选择编译期版本；各头文件注明了对应的规范版本。

> [!CAUTION]
> 处于Beta阶段，可能存在一些Bug和各种问题，应谨慎使用。

您可以[报告问题](https://github.com/KNSoft/KNSoft.FirmwareSpec/issues/new)或[提交 PR](https://github.com/KNSoft/KNSoft.FirmwareSpec/pulls)让此项目更好。

## 协议

[KNSoft.FirmwareSpec](https://github.com/KNSoft/KNSoft.FirmwareSpec)根据[MIT](https://github.com/KNSoft/KNSoft.FirmwareSpec/blob/main/LICENSE)协议进行许可。
