/**
* \file
* Binary reader utilities
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_binary_util.h"

#include <core/util/io/kzc_input_stream.h>
#include <core/util/string/kzc_string.h>


kzsException kzuBinaryReadString(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream, kzMutableString* out_string)
{
    kzsError result;
    kzUint size;
    kzMutableString string;

    result = kzcInputStreamReadU16Int(inputStream, &size);
    kzsErrorForward(result);

    result = kzcStringAllocate(memoryManager, (kzUint)size, &string);
    kzsErrorForward(result);

    result = kzcInputStreamReadBytes(inputStream, size, (kzByte*)string);
    kzsExceptionForward(result);

    string[size] = KZC_STRING_TERMINATION_SYMBOL;

    result = kzcInputStreamSkip(inputStream, 3 - (2 + size + 3) % 4); /* Skip padding. */
    kzsErrorForward(result);

    *out_string = string;
    kzsSuccess();
}
