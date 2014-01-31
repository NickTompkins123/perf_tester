/**
 * \file
 * String buffer.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_STRING_BUFFER_H
#define KZC_STRING_BUFFER_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;


/**
 * \struct KzcStringBuffer
 * String buffer. String buffer allows appending arbitrary strings while taking care of memory management and
 * increasing capacity as necessary.
 */
struct KzcStringBuffer;


/** Creates a new string buffer with capacity to hold string of given length. */
kzsError kzcStringBufferCreate(const struct KzcMemoryManager* memoryManager, kzUint initialCapacity,
                               struct KzcStringBuffer** out_stringBuffer);

/** Creates a new string buffer with initial capacity of 16 characters. */
kzsError kzcStringBufferCreateDefault(const struct KzcMemoryManager* memoryManager,
                                      struct KzcStringBuffer** out_stringBuffer);

/** Deletes a string buffer. */
kzsError kzcStringBufferDelete(struct KzcStringBuffer* stringBuffer);


/** Appends the given string to the end of a string buffer. */
kzsError kzcStringBufferAppend(struct KzcStringBuffer* stringBuffer, kzString string);

/** Appends the given character to the end of a string buffer. */
kzsError kzcStringBufferAppendCharacter(struct KzcStringBuffer* stringBuffer, kzChar character);

/** Appends the given unicode character to the end of a string buffer. */
kzsError kzcStringBufferAppendUnicodeCharacter(struct KzcStringBuffer* stringBuffer, kzUnicodeChar character);

/** Appends the given format string to the end of a string buffer. */
kzsError kzcStringBufferAppendFormat(struct KzcStringBuffer* stringBuffer, kzString format, ...);

/** Clears the content of the string buffer. As a result the string buffer will contain an empty string. Capacity is not decreased with this call. */
void kzcStringBufferClear(struct KzcStringBuffer* stringBuffer);

/** Returns the length of a string buffer. */
kzUint kzcStringBufferGetLength(const struct KzcStringBuffer* stringBuffer);

/** Returns the underlying string of the string buffer. This can be used to quickly access the content of the string buffer. */
kzString kzcStringBufferGetString(const struct KzcStringBuffer* stringBuffer);

/**
 * Creates an ordinary mutable string from a string buffer.
 * This can be used to retrieve the content of the string buffer for storing it after the string buffer itself is no longer needed.
 * This function allocates a new string, which must be manually freed when no longer needed.
 */
kzsError kzcStringBufferToString(const struct KzcMemoryManager* memoryManager, const struct KzcStringBuffer* stringBuffer, kzMutableString* out_string);


#endif
