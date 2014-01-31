/**
 * \file
 * String buffer.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_string_buffer.h"

#include "kzc_string.h"

#include <core/memory/kzc_memory_manager.h>

#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_string.h>
#include <system/wrappers/kzs_math.h>


struct KzcStringBuffer
{
    kzUint capacity; /**< Number of characters the buffer can currently hold at maximum. */
    kzUint length; /**< Length of the current string in the buffer. */
    kzMutableString string; /**< Allocated characters of the buffer. */
};


/** Ensures that the buffer capacity will be at least the given one. */
static kzsError kzcStringBufferEnsureCapacity_internal(struct KzcStringBuffer* stringBuffer, kzUint capacity);


kzsError kzcStringBufferCreate(const struct KzcMemoryManager* memoryManager, kzUint initialCapacity,
                               struct KzcStringBuffer** out_stringBuffer)
{
    kzsError result;
    struct KzcStringBuffer* stringBuffer;

    result = kzcMemoryAllocVariable(memoryManager, stringBuffer, "String buffer");
    kzsErrorForward(result);

    stringBuffer->string = KZ_NULL;
    stringBuffer->length = 0;
    stringBuffer->capacity = 0;

    result = kzcStringBufferEnsureCapacity_internal(stringBuffer, initialCapacity);
    kzsErrorForward(result);

    *out_stringBuffer = stringBuffer;
    kzsSuccess();
}

kzsError kzcStringBufferCreateDefault(const struct KzcMemoryManager* memoryManager,
                                      struct KzcStringBuffer** out_stringBuffer)
{
    kzsError result;
    struct KzcStringBuffer* stringBuffer;

    result = kzcStringBufferCreate(memoryManager, 16, &stringBuffer);
    kzsErrorForward(result);

    *out_stringBuffer = stringBuffer;
    kzsSuccess();
}

kzsError kzcStringBufferDelete(struct KzcStringBuffer* stringBuffer)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(stringBuffer));

    result = kzcMemoryFreePointer(stringBuffer->string);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(stringBuffer);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzcStringBufferEnsureCapacity_internal(struct KzcStringBuffer* stringBuffer, kzUint capacity)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    kzMutableString newString;
    kzMutableString oldString;
    kzUint newCapacity;

    kzsAssert(kzcIsValidPointer(stringBuffer));

    /* If the current capacity is smaller than requested, at least double the size. */
    if (capacity > stringBuffer->capacity || capacity == 0)
    {
        memoryManager = kzcMemoryGetManager(stringBuffer);

        oldString = stringBuffer->string;
        newCapacity = kzsMaxU(capacity, (stringBuffer->capacity + 1) * 2);

        result = kzcMemoryAllocPointer(memoryManager, &newString, (newCapacity + 1) * sizeof(kzChar), "String buffer string");
        kzsErrorForward(result);

        if (oldString != KZ_NULL)
        {
            kzsMemcpy(newString, oldString, stringBuffer->length);
            newString[stringBuffer->length] = KZC_STRING_TERMINATION_SYMBOL;
            result = kzcMemoryFreePointer(oldString);
            kzsErrorForward(result);
        }

        stringBuffer->string = newString;
        stringBuffer->capacity = newCapacity;
    }

    kzsSuccess();
}

kzsError kzcStringBufferAppend(struct KzcStringBuffer* stringBuffer, kzString string)
{
    kzsError result;
    kzUint stringLength;
    kzUint newLength;

    kzsAssert(kzcIsValidPointer(stringBuffer));
    kzsAssert(string != KZ_NULL);
    
    stringLength = kzsStrlen(string);
    newLength = stringBuffer->length + stringLength;

    result = kzcStringBufferEnsureCapacity_internal(stringBuffer, newLength);
    kzsErrorForward(result);

    kzsMemcpy(stringBuffer->string + stringBuffer->length, string, stringLength + 1);
    
    stringBuffer->length += stringLength;

    kzsSuccess();
}

kzsError kzcStringBufferAppendCharacter(struct KzcStringBuffer* stringBuffer, kzChar character)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(stringBuffer));

    result = kzcStringBufferEnsureCapacity_internal(stringBuffer, stringBuffer->length + 1);
    kzsErrorForward(result);

    stringBuffer->string[stringBuffer->length++] = character;
    stringBuffer->string[stringBuffer->length] = KZC_STRING_TERMINATION_SYMBOL;

    kzsSuccess();
}

kzsError kzcStringBufferAppendUnicodeCharacter(struct KzcStringBuffer* stringBuffer, kzUnicodeChar character)
{
    kzsError result;
    kzUint byteCount = kzcStringGetUTF8ByteCount(character);

    kzsAssert(kzcIsValidPointer(stringBuffer));

    result = kzcStringBufferEnsureCapacity_internal(stringBuffer, stringBuffer->length + byteCount);
    kzsErrorForward(result);

    stringBuffer->length += kzcStringWriteUnicodeCharacter(stringBuffer->string + stringBuffer->length, character);
    stringBuffer->string[stringBuffer->length] = KZC_STRING_TERMINATION_SYMBOL;

    kzsSuccess();
}

kzsError kzcStringBufferAppendFormat(struct KzcStringBuffer* stringBuffer, kzString format, ...)
{
    kzsError result;
    kzArgList arguments;
    struct KzcMemoryManager* memoryManager;
    kzMutableString string;

    kzsAssert(kzcIsValidPointer(stringBuffer));

    memoryManager = kzcMemoryGetManager(stringBuffer);

    va_start(arguments, format);

    result = kzcStringFormatList(memoryManager, format, &arguments, &string);
    kzsErrorForward(result);

    va_end(arguments);

    result = kzcStringBufferAppend(stringBuffer, string);
    kzsErrorForward(result);

    result = kzcStringDelete(string);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzcStringBufferClear(struct KzcStringBuffer* stringBuffer)
{
    kzsAssert(kzcIsValidPointer(stringBuffer));

    stringBuffer->length = 0;
    stringBuffer->string[0] = KZC_STRING_TERMINATION_SYMBOL;
}

kzUint kzcStringBufferGetLength(const struct KzcStringBuffer* stringBuffer)
{
    kzsAssert(kzcIsValidPointer(stringBuffer));

    return stringBuffer->length;
}

kzString kzcStringBufferGetString(const struct KzcStringBuffer* stringBuffer)
{
    kzsAssert(kzcIsValidPointer(stringBuffer));

    return stringBuffer->string;
}

kzsError kzcStringBufferToString(const struct KzcMemoryManager* memoryManager, const struct KzcStringBuffer* stringBuffer,
                                 kzMutableString* out_string)
{
    kzsError result;
    kzMutableString string;

    kzsAssert(kzcIsValidPointer(stringBuffer));

    result = kzcStringAllocate(memoryManager, stringBuffer->length, &string);
    kzsErrorForward(result);

    kzsMemcpy(string, stringBuffer->string, stringBuffer->length + 1);

    *out_string = string;
    kzsSuccess();
}
