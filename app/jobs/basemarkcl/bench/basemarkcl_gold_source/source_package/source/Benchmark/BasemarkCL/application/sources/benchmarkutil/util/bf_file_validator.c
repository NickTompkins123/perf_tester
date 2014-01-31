/*
* OpenCL benchmark file hash validator
*
* Copyright 2011 by Rightware. All rights reserved.
*/

#include <benchmarkutil/bf_benchmark_framework.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/util/string/kzc_string.h>
#include <core/debug/kzc_log.h>

#include <system/wrappers/kzs_string.h>
#include <system/kzs_types.h>

#include "md5.h"
#include "bf_file_validator.h"


/** Converts a single hexadecimal digit into 4 byte number. Return values are undefined for anything else than a valid hexadecimal number */
kzU8 hex2Bits_internal(kzChar digit);
/** Converts a 32 characters long hexadecimal string into 16 byte digest value */
kzsError string2Digest_internal(kzString hash, md5_byte_t digest[16]);


kzU8 hex2Bits_internal(kzChar digit)
{
    if(digit - '0' < 10 && digit - '0' >= 0)
    {
        return (kzU8)((digit - '0') & 0xF);
    }
    else
    {
        return (kzU8)(((digit - 'a') + 10) & 0xF);
    }
}

kzsError string2Digest_internal(kzString hash, md5_byte_t digest[16])
{
    kzInt i;
    for(i = 0; i < 16; i++)
    {
        digest[i] = ((md5_byte_t)hex2Bits_internal(hash[i*2]) << 4) | (md5_byte_t)hex2Bits_internal(hash[i*2 + 1]);
    }
    kzsSuccess();
}

#define BUFSIZE 128
kzsError digestFromFile_internal(struct BenchmarkFramework* framework,kzString path, md5_byte_t digest[16])
{
    struct KzcInputStream* stream;
    kzsError result;
    md5_state_t md5state;
    kzUint bytesRead = 0;
    md5_byte_t bytes[BUFSIZE];
    struct KzcMemoryManager* manager = bfGetMemoryManager(framework);
    md5_init(&md5state);

    result = kzcInputStreamCreateFromFile(manager, path, KZC_IO_STREAM_ENDIANNESS_PLATFORM, &stream);
    kzsErrorForward(result);
    do
    {
        result = kzcInputStreamRead(stream, BUFSIZE, &bytesRead, (kzByte*)bytes);
        kzsErrorForward(result);
        md5_append(&md5state, bytes, bytesRead);
    } while (bytesRead != 0);

    result = kzcInputStreamDelete(stream);
    kzsErrorForward(result);

    md5_finish(&md5state, digest);

    kzsSuccess();
}

kzsError bfValidateFile(struct BenchmarkFramework* framework, kzString path, kzString hash, kzBool *is_valid)
{
    kzsError result;
    md5_byte_t digest[16];
    md5_byte_t hash_digest[16];
    kzInt i;
    kzBool valid = KZ_TRUE;
    result = string2Digest_internal(hash, hash_digest);
    kzsErrorForward(result);
    result = digestFromFile_internal(framework, path, digest);
    kzsErrorForward(result);

    for(i = 0; i < 16; i++)
    {
        if(digest[i] != hash_digest[i])
        {
            valid = KZ_FALSE;
        }
    }

    *is_valid = valid;
    kzsSuccess();
}
