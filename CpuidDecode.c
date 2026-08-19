#include "CPUID.Decode.h"

#include <stdio.h>

static
bool
PrintFeature(
    const CPUID_BIT_INFO* Feature,
    void* Context)
{
    (void)Context;
    printf("  %s\n", (const char*)Feature->Name);
    return true;
}

int
main()
{
    CPUID_INFO Info;
    CPUID_VERSION_INFO Version;
    FIRMWARE_DECODE_STATUS Status;
    char Vendor[13];
    uint32_t MaximumLeaf;

    Status = CpuidGetVendor(Vendor, &MaximumLeaf);
    if (Status == FirmwareDecodeUnsupported)
    {
        puts("CPUID is not available on this architecture");
        return 0;
    }
    if (Status != FirmwareDecodeSuccess)
    {
        return 1;
    }
    printf("Vendor: %s\nMaximum basic leaf: 0x%08X\n", Vendor, MaximumLeaf);

    if (MaximumLeaf >= 1 && CpuidExecute(1, 0, &Info) == FirmwareDecodeSuccess)
    {
        CpuidDecodeVersion((uint32_t)Info.Registers[0], &Version);
        printf("Family: 0x%X, Model: 0x%X, Stepping: 0x%X\n",
               Version.DisplayFamily,
               Version.DisplayModel,
               Version.Stepping);
    }
    puts("Features:");
    return CpuidVisitFeatures(PrintFeature, NULL) == FirmwareDecodeSuccess ? 0 : 1;
}
