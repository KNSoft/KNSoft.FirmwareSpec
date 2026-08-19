/*
 * KNSoft.FirmwareSpec (https://github.com/KNSoft/KNSoft.FirmwareSpec)
 *
 * Type information for the CPUID leaves defined by CPUID.h.
 *
 * Licensed under the MIT license.
 * Copyright (c) KNSoft.org (https://github.com/KNSoft). All rights reserved.
 */

#pragma once

#include "FirmwareDecode.h"

typedef enum _CPUID_REGISTER
{
    CpuidRegisterEax,
    CpuidRegisterEbx,
    CpuidRegisterEcx,
    CpuidRegisterEdx,
} CPUID_REGISTER;

typedef struct _CPUID_BIT_INFO
{
    uint32_t Leaf;
    uint32_t SubLeaf;
    CPUID_REGISTER Register;
    uint8_t Bit;
    const char* Name;
} CPUID_BIT_INFO;

#define CPUID_FEATURE(Leaf, SubLeaf, Register, Bit, Name) \
    { Leaf, SubLeaf, CpuidRegister##Register, Bit, Name }

static const CPUID_BIT_INFO CpuidFeatureInfo[] = {
    CPUID_FEATURE(0x01, 0, Ecx, 0, "SSE3"),
    CPUID_FEATURE(0x01, 0, Ecx, 1, "PCLMULQDQ"),
    CPUID_FEATURE(0x01, 0, Ecx, 3, "MONITOR"),
    CPUID_FEATURE(0x01, 0, Ecx, 5, "VMX"),
    CPUID_FEATURE(0x01, 0, Ecx, 6, "SMX"),
    CPUID_FEATURE(0x01, 0, Ecx, 9, "SSSE3"),
    CPUID_FEATURE(0x01, 0, Ecx, 12, "FMA"),
    CPUID_FEATURE(0x01, 0, Ecx, 13, "CMPXCHG16B"),
    CPUID_FEATURE(0x01, 0, Ecx, 17, "PCID"),
    CPUID_FEATURE(0x01, 0, Ecx, 19, "SSE4.1"),
    CPUID_FEATURE(0x01, 0, Ecx, 20, "SSE4.2"),
    CPUID_FEATURE(0x01, 0, Ecx, 21, "x2APIC"),
    CPUID_FEATURE(0x01, 0, Ecx, 22, "MOVBE"),
    CPUID_FEATURE(0x01, 0, Ecx, 23, "POPCNT"),
    CPUID_FEATURE(0x01, 0, Ecx, 25, "AESNI"),
    CPUID_FEATURE(0x01, 0, Ecx, 26, "XSAVE"),
    CPUID_FEATURE(0x01, 0, Ecx, 27, "OSXSAVE"),
    CPUID_FEATURE(0x01, 0, Ecx, 28, "AVX"),
    CPUID_FEATURE(0x01, 0, Ecx, 29, "F16C"),
    CPUID_FEATURE(0x01, 0, Ecx, 30, "RDRAND"),
    CPUID_FEATURE(0x01, 0, Edx, 0, "FPU"),
    CPUID_FEATURE(0x01, 0, Edx, 4, "TSC"),
    CPUID_FEATURE(0x01, 0, Edx, 5, "MSR"),
    CPUID_FEATURE(0x01, 0, Edx, 6, "PAE"),
    CPUID_FEATURE(0x01, 0, Edx, 9, "APIC"),
    CPUID_FEATURE(0x01, 0, Edx, 12, "MTRR"),
    CPUID_FEATURE(0x01, 0, Edx, 15, "CMOV"),
    CPUID_FEATURE(0x01, 0, Edx, 16, "PAT"),
    CPUID_FEATURE(0x01, 0, Edx, 23, "MMX"),
    CPUID_FEATURE(0x01, 0, Edx, 24, "FXSR"),
    CPUID_FEATURE(0x01, 0, Edx, 25, "SSE"),
    CPUID_FEATURE(0x01, 0, Edx, 26, "SSE2"),
    CPUID_FEATURE(0x01, 0, Edx, 28, "HTT"),
    CPUID_FEATURE(0x07, 0, Ebx, 0, "FSGSBASE"),
    CPUID_FEATURE(0x07, 0, Ebx, 3, "BMI1"),
    CPUID_FEATURE(0x07, 0, Ebx, 5, "AVX2"),
    CPUID_FEATURE(0x07, 0, Ebx, 7, "SMEP"),
    CPUID_FEATURE(0x07, 0, Ebx, 8, "BMI2"),
    CPUID_FEATURE(0x07, 0, Ebx, 9, "ERMS"),
    CPUID_FEATURE(0x07, 0, Ebx, 10, "INVPCID"),
    CPUID_FEATURE(0x07, 0, Ebx, 18, "RDSEED"),
    CPUID_FEATURE(0x07, 0, Ebx, 19, "ADX"),
    CPUID_FEATURE(0x07, 0, Ebx, 20, "SMAP"),
    CPUID_FEATURE(0x07, 0, Ebx, 23, "CLFLUSHOPT"),
    CPUID_FEATURE(0x07, 0, Ebx, 24, "CLWB"),
    CPUID_FEATURE(0x07, 0, Ebx, 29, "SHA"),
    CPUID_FEATURE(0x07, 0, Ecx, 2, "UMIP"),
    CPUID_FEATURE(0x07, 0, Ecx, 3, "PKU"),
    CPUID_FEATURE(0x07, 0, Ecx, 5, "WAITPKG"),
    CPUID_FEATURE(0x07, 0, Ecx, 8, "GFNI"),
    CPUID_FEATURE(0x07, 0, Ecx, 9, "VAES"),
    CPUID_FEATURE(0x07, 0, Ecx, 10, "VPCLMULQDQ"),
    CPUID_FEATURE(0x07, 0, Ecx, 16, "LA57"),
    CPUID_FEATURE(0x07, 0, Ecx, 22, "RDPID"),
    CPUID_FEATURE(0x07, 0, Ecx, 27, "MOVDIRI"),
    CPUID_FEATURE(0x07, 0, Ecx, 28, "MOVDIR64B"),
    CPUID_FEATURE(0x07, 0, Ecx, 29, "ENQCMD"),
    CPUID_FEATURE(0x07, 0, Edx, 4, "FSRM"),
    CPUID_FEATURE(0x07, 0, Edx, 5, "UINTR"),
    CPUID_FEATURE(0x07, 0, Edx, 14, "SERIALIZE"),
    CPUID_FEATURE(0x07, 0, Edx, 15, "HYBRID"),
    CPUID_FEATURE(0x07, 0, Edx, 16, "TSXLDTRK"),
    CPUID_FEATURE(0x07, 0, Edx, 22, "AMX_BF16"),
    CPUID_FEATURE(0x07, 0, Edx, 23, "AVX512_FP16"),
    CPUID_FEATURE(0x07, 0, Edx, 24, "AMX_TILE"),
    CPUID_FEATURE(0x07, 0, Edx, 25, "AMX_INT8"),
    CPUID_FEATURE(0x07, 1, Eax, 4, "AVX_VNNI"),
    CPUID_FEATURE(0x07, 1, Eax, 5, "AVX512_BF16"),
    CPUID_FEATURE(0x07, 1, Eax, 10, "FZRM"),
    CPUID_FEATURE(0x07, 1, Eax, 11, "FSRS"),
    CPUID_FEATURE(0x07, 1, Eax, 12, "FSRCS"),
    CPUID_FEATURE(0x07, 1, Eax, 22, "HRESET"),
    CPUID_FEATURE(0x07, 1, Ebx, 0, "PPIN"),
    CPUID_FEATURE(0x07, 1, Edx, 18, "CET_SSS"),
};

#undef CPUID_FEATURE
