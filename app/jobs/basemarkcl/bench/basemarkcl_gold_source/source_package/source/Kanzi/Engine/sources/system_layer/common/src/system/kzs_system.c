/**
 * \file
 * System module.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzs_system.h"

#include <system/debug/kzs_log.h>
#include <system/debug/kzs_counter.h>
#include <system/wrappers/kzs_memory.h>
#include <system/debug/kzs_log.h>
#include <system/kzs_globals.h>


typedef kzsError (*ModuleInitializer)(void);
typedef kzsError (*ModuleUninitializer)(void);


struct Module
{
    ModuleInitializer initializer;
    ModuleUninitializer uninitializer;
};


static const struct Module modules[] =
{
    {kzsLogInitialize, kzsLogUninitialize},
    {kzsCountersInitialize, kzsCountersUninitialize},
    {kzsGlobalsInitialize, kzsGlobalsUninitialize},
    {kzsErrorInitialize, kzsErrorUninitialize}
};
static kzUint moduleCount = sizeof(modules) / sizeof(struct Module);


static enum KzsEndianness
{
    KZS_ENDIANNESS_UNKNOWN,
    KZS_ENDIANNESS_BIG_ENDIAN,
    KZS_ENDIANNESS_LITTLE_ENDIAN
} g_kzsEndianness = KZS_ENDIANNESS_UNKNOWN;


static kzBool g_memoryLeakReportEnabled = KZ_TRUE;


kzsError kzsSystemInitialize()
{
    kzsError result;
    kzUint i;

    for (i = 0; i < moduleCount; ++i)
    {
        const struct Module* module = &modules[i];

        result = (*module->initializer)();
        kzsErrorForward(result);
    }

    kzsSuccess();
}

void kzsSystemSetMemoryLeakReportEnabled(kzBool enabled)
{
    g_memoryLeakReportEnabled = enabled;
}

kzBool kzsSystemIsMemoryLeakReportEnabled(void)
{
    return g_memoryLeakReportEnabled;
}

kzsError kzsSystemUninitialize()
{
    kzsError result;

    kzUint i;
    kzInt allocationCount;

    /* Uninitialize in reverse order */
    for (i = 0; i < moduleCount; ++i)
    {
        const struct Module* module = &modules[moduleCount - 1 - i];

        result = (*module->uninitializer)();
        kzsErrorForward(result);
    }

    if (g_memoryLeakReportEnabled)
    {
        allocationCount = kzsMemoryGetAllocationCount();
        if (allocationCount != 0)
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "Some system memory was not freed properly");
        }
    }

    kzsSuccess();
}

kzBool kzsIsBigEndian(void)
{
    if (g_kzsEndianness == KZS_ENDIANNESS_UNKNOWN)
    {
        /** Endianness is determined by checking how negative zero floating point value is encoded in bytes. */
        const kzFloat NEGATIVE_ZERO = -0.0f; /*lint !e835 */
        const kzByte* NEGATIVE_ZERO_BYTES = (kzByte*)&NEGATIVE_ZERO;
        if (NEGATIVE_ZERO_BYTES[0] == 0x80 &&
            NEGATIVE_ZERO_BYTES[1] == 0x00 &&
            NEGATIVE_ZERO_BYTES[2] == 0x00 &&
            NEGATIVE_ZERO_BYTES[3] == 0x00)
        {
            g_kzsEndianness = KZS_ENDIANNESS_BIG_ENDIAN;
        }
        else if (NEGATIVE_ZERO_BYTES[0] == 0x00 &&
                 NEGATIVE_ZERO_BYTES[1] == 0x00 &&
                 NEGATIVE_ZERO_BYTES[2] == 0x00 &&
                 NEGATIVE_ZERO_BYTES[3] == 0x80)
        {
            g_kzsEndianness = KZS_ENDIANNESS_LITTLE_ENDIAN;
        }
        else
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "Unable to detect endianness of the platform. Floating point binary conversion will behave incorrectly.");
        }
    }

    return g_kzsEndianness == KZS_ENDIANNESS_BIG_ENDIAN;
}

static void kzsCopyBytes_internal(const kzByte* input, kzByte* out_output)
{
    out_output[0] = input[0];
    out_output[1] = input[1];
    out_output[2] = input[2];
    out_output[3] = input[3];
}

static void kzsCopyBytesReversed_internal(const kzByte* input, kzByte* out_output)
{
    out_output[0] = input[3];
    out_output[1] = input[2];
    out_output[2] = input[1];
    out_output[3] = input[0];
}

void kzsFloatToBytesLittleEndian(kzFloat floatValue, kzByte* out_floatBytes)
{
    const kzByte* platformFloatBytes = (const kzByte*)&floatValue;
    if (!kzsIsBigEndian())
    {
        kzsCopyBytes_internal(platformFloatBytes, out_floatBytes);
    }
    else
    {
        kzsCopyBytesReversed_internal(platformFloatBytes, out_floatBytes);
    }
}

void kzsFloatToBytesBigEndian(kzFloat floatValue, kzByte* out_floatBytes)
{
    const kzByte* platformFloatBytes = (const kzByte*)&floatValue;
    if (kzsIsBigEndian())
    {
        kzsCopyBytes_internal(platformFloatBytes, out_floatBytes);
    }
    else
    {
        kzsCopyBytesReversed_internal(platformFloatBytes, out_floatBytes);
    }
}

kzFloat kzsBytesToFloatLittleEndian(const kzByte* floatBytes)
{
    kzByte platformFloatBytes[4];
    if (!kzsIsBigEndian())
    {
        kzsCopyBytes_internal(floatBytes, platformFloatBytes);
    }
    else
    {
        kzsCopyBytesReversed_internal(floatBytes, platformFloatBytes);
    }

    return *(kzFloat*)platformFloatBytes;
}

kzFloat kzsBytesToFloatBigEndian(const kzByte* floatBytes)
{
    kzByte platformFloatBytes[4];
    if (kzsIsBigEndian())
    {
        kzsCopyBytes_internal(floatBytes, platformFloatBytes);
    }
    else
    {
        kzsCopyBytesReversed_internal(floatBytes, platformFloatBytes);
    }

    return *(kzFloat*)platformFloatBytes;
}
