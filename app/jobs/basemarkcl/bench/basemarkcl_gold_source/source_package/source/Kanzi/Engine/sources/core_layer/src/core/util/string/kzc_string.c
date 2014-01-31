/**
 * \file
 * String utilities.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_string.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_quick.h>
#include <core/kzc_error_codes.h>

#include <system/wrappers/kzs_math.h>
#include <system/wrappers/kzs_string.h>
#include <system/wrappers/kzs_memory.h>
#include <system/debug/kzs_counter.h>
#ifdef __GNUC__
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#endif

/* Workaround for the current crappiness of the variable argument handling */
void copy_va(kzArgList *target, kzArgList *source)
{
#ifdef __GNUC__
    va_copy(*target, *source);
#else
    *target = *source;
#endif
}


/** How many decimal digits are used by default when converting floats to strings. */
#define KZC_STRING_FORMAT_DEFAULT_FLOAT_PRECISION 6

/** Indicates that a formatting parameter is not specified. */
#define KZC_STRING_FORMATTER_UNSPECIFIED -1


/** Mode of string formatter. */
enum KzcStringFormatterMode
{
    KZC_STRING_FORMATTER_MODE_ESCAPED, /**< Escaped % sign (%%). */
    KZC_STRING_FORMATTER_MODE_NORMAL /**< Any other formatter. */
};

/** Data type of the string formatter. */
enum KzcStringFormatterType
{
    KZC_STRING_FORMATTER_TYPE_UNSPECIFIED, /**< No type specified. This is forbidden. */
    KZC_STRING_FORMATTER_TYPE_SIGNED_INTEGER, /**< Signed integer (i). */
    KZC_STRING_FORMATTER_TYPE_UNSIGNED_INTEGER, /**< Unsigned integer (u). */
    KZC_STRING_FORMATTER_TYPE_FLOAT, /**< Float (f). */
    KZC_STRING_FORMATTER_TYPE_LOWERCASE_HEXADECIMAL, /**< Lowercase hexadecimal unsingned integer (h). */
    KZC_STRING_FORMATTER_TYPE_UPPERCASE_HEXADECIMAL, /**< Uppercase hexadecimal unsingned integer (H). */
    KZC_STRING_FORMATTER_TYPE_STRING, /**< String (s). */
    KZC_STRING_FORMATTER_TYPE_CHARACTER, /**< Character (c). */
    KZC_STRING_FORMATTER_TYPE_UNICODE_CHARACTER, /**< Unicode character (C). */
    KZC_STRING_FORMATTER_TYPE_LOWERCASE_BOOLEAN, /**< Lowercase boolean (b). */
    KZC_STRING_FORMATTER_TYPE_UPPERCASE_BOOLEAN, /**< Uppercase boolean (B). */
    KZC_STRING_FORMATTER_TYPE_POINTER /**< Pointer (p). */
};

/** Structure describing one string formatter instance. */
struct KzcStringFormatter
{
    kzUint length; /**< Length of the formatter itself. For example %2.3f -> 5. */
    enum KzcStringFormatterMode mode; /**< Formatter mode. */
    kzBool leftAligned; /**< Should the value be left-aligned if outputWidth is specified. */
    kzBool signedPositives; /**< Should positive values be prefixed with '+' character. */
    kzBool zeroPadded; /**< Should values be padded with '0' prefix if outputWidth is specified. */
    kzInt outputWidth; /**< Minimum width of the output. */
    kzInt precision; /**< Floating point precision. */
    enum KzcStringFormatterType type; /**< Type of the value. */
};

kzsError kzcStringCreate(const struct KzcMemoryManager* memoryManager, kzUint length, kzMutableString* out_string)
{
    kzsError result;
    kzMutableString string;

    result = kzcMemoryAllocPointer(memoryManager, &string, (length) * sizeof(kzChar), "String");
    kzsErrorForward(result);
    string[0] = KZC_STRING_TERMINATION_SYMBOL;

    kzsCounterIncrease("StringCreate");

    *out_string = string;
    kzsSuccess();
}

kzsError kzcStringAllocate(const struct KzcMemoryManager* memoryManager, kzUint length, kzMutableString* out_string)
{
    kzsError result;
    kzMutableString string;

    result = kzcMemoryAllocPointer(memoryManager, &string, (length + 1) * sizeof(kzChar), "String");
    kzsErrorForward(result);

    kzsCounterIncrease("StringAllocate");

    *out_string = string;
    kzsSuccess();
}

kzsError kzcStringCreateEmpty(const struct KzcMemoryManager* memoryManager, kzMutableString* out_string)
{
    kzsError result;
    kzMutableString string;

    result = kzcStringAllocate(memoryManager, 0, &string);
    kzsErrorForward(result);

    string[0] = KZC_STRING_TERMINATION_SYMBOL;

    *out_string = string;
    kzsSuccess();
}

kzsError kzcStringDelete(kzMutableString string)
{
    kzsError result;

    result = kzcMemoryFreePointer(string);
    kzsErrorForward(result);

    kzsSuccess();
}

kzUint kzcStringLength(kzString string)
{
    return kzsStrlen(string);
}

kzsError kzcStringCopy(const struct KzcMemoryManager* memoryManager, kzString source, kzMutableString* out_target)
{
    kzsError result;
    kzUint length;
    kzMutableString target;

    kzsAssert(source != KZ_NULL);

    length = kzcStringLength(source);

    result = kzcStringAllocate(memoryManager, length, &target);
    kzsErrorForward(result);

    kzsStrcpy(target, source);

    *out_target = target;
    kzsSuccess();
}

kzsError kzcStringConcatenate(const struct KzcMemoryManager* memoryManager, kzString front, kzString back, kzMutableString* out_string)
{
    kzsError result;
    kzUint frontLength;
    kzUint backLength;
    kzUint outLength;
    kzMutableString string;

    /* Calculate memory requirements. */
    frontLength = kzcStringLength(front);
    backLength = kzcStringLength(back);

    /* Calculate total space for concatenated string. */
    outLength = frontLength + backLength;

    /* Allocate output string. */
    result = kzcStringAllocate(memoryManager, outLength, &string);
    kzsErrorForward(result);

    /* Build output string. */
    kzsMemcpy(string, front, frontLength);
    kzsMemcpy(string + frontLength, back, backLength);
    string[outLength] = KZC_STRING_TERMINATION_SYMBOL;

    kzsCounterIncrease("StringConcatenate");

    *out_string = string;
    kzsSuccess();
}

kzsError kzcStringConcatenateMultiple(const struct KzcMemoryManager* memoryManager, kzUint stringCount, kzMutableString* out_string, ...)
{
    kzsError result;
    kzUint length = 0;
    kzMutableString string;

    /* Calculate lengths */
    {
        kzArgList arguments;
        kzUint i;

        va_start(arguments, out_string);

        for (i = 0; i < stringCount; ++i)
        {
            kzString currentString = KZ_READ_STRING_FROM_ARGLIST(arguments);
            length += kzsStrlen(currentString);
        }

        va_end(arguments);
    }

    /* Allocate output string. */
    result = kzcStringAllocate(memoryManager, length, &string);
    kzsErrorForward(result);

    /* Build output string. */
    {
        kzArgList arguments;
        kzUint currentIndex = 0;
        kzUint i;

        va_start(arguments, out_string);

        for (i = 0; i < stringCount; ++i)
        {
            kzString currentString = KZ_READ_STRING_FROM_ARGLIST(arguments);
            kzUint currentLength = kzsStrlen(currentString);
            kzsMemcpy(string + currentIndex, currentString, currentLength);
            currentIndex += currentLength;
        }

        va_end(arguments);

        kzsAssert(currentIndex == length);

        string[length] = KZC_STRING_TERMINATION_SYMBOL;
    }

    *out_string = string;
    kzsSuccess();
}

kzsError kzcStringSubstring(const struct KzcMemoryManager* memoryManager, kzString string, kzUint start, kzUint length,
                            kzMutableString* out_string)
{
    kzsError result;
    kzUint stringLength = kzsStrlen(string);
    kzUint newLength = (start >= stringLength) ? 0 : kzsMinU(length, stringLength - start);
    kzMutableString newString;
    
    /* Allocate output string. */
    result = kzcStringAllocate(memoryManager, newLength, &newString);
    kzsErrorForward(result);

    kzsMemcpy(newString, string + start, newLength);
    newString[newLength] = KZC_STRING_TERMINATION_SYMBOL;

    *out_string = newString;
    kzsSuccess();
}

kzBool kzcStringFindSubstring(kzString string, kzString subString, kzUint* out_index)
{
    return kzcStringFindNextSubstring(string, subString, 0, out_index);
}

kzBool kzcStringFindNextSubstring(kzString string, kzString subString, kzUint startIndex, kzUint* out_index)
{
    kzBool result;
    kzUint index;
    kzString matchPosition = kzsStrstr(string + startIndex, subString);

    if (matchPosition != KZ_NULL)
    {
        index = (kzUint)(matchPosition - string); /*lint !e947 Suppress warning of pointer substraction. */
        result = KZ_TRUE;
    }
    else
    {
        index = 0;
        result = KZ_FALSE;
    }

    if(out_index != KZ_NULL)
    {
        *out_index = index;
    }
    return result;
}

kzBool kzcStringFindCharacter(kzString string, kzChar character, kzUint* out_index)
{
    return kzcStringFindNextCharacter(string, character, 0, out_index);
}

kzBool kzcStringFindNextCharacter(kzString string, kzChar character, kzUint startIndex, kzUint* out_index)
{
    kzBool result;
    kzUint index;
    kzString matchPointer = kzsStrchr(string + startIndex, character); 

    if (matchPointer != KZ_NULL)
    {
        index = (kzUint)(matchPointer - string); /*lint !e947 Suppress warning of pointer substraction. */
        result = KZ_TRUE;
    }
    else
    {
        index = 0;
        result = KZ_FALSE;
    }

    *out_index = index;
    return result;
}

kzUint kzcStringGetSubstringCount(kzString string, kzString pattern)
{
    kzUint patternLength = kzsStrlen(pattern);
    kzUint patternsFound = 0;
    kzUint currentIndex = 0;
    kzUint index;

    while (kzcStringFindNextSubstring(string, pattern, currentIndex, &index))
    {
        currentIndex = index + patternLength;
        ++patternsFound;
    }

    return patternsFound;
}

kzBool kzcStringStartsWith(kzString string, kzString pattern)
{
    kzBool match = KZ_TRUE;
    kzUint stringLength = kzcStringLength(string);
    kzUint patternLength = kzcStringLength(pattern);
    kzUint index = 0;

    if (stringLength < patternLength)
    {
        match = KZ_FALSE;
    }
    else
    {
        while (match && index < patternLength)
        {
            kzChar char1 = string[index];
            kzChar char2 = pattern[index];
            if (char1 != char2 && char2 != KZC_STRING_TERMINATION_SYMBOL)
            {
                match = KZ_FALSE;
            }
            else
            {
                ++index;
            }
        }
    }

    return match;
}

kzBool kzcStringEndsWith(kzString string, kzString pattern)
{
    kzBool match = KZ_TRUE;
    kzUint stringLength = kzcStringLength(string);
    kzUint patternLength = kzcStringLength(pattern);
    kzUint index = 0;

    if (stringLength < patternLength)
    {
        match = KZ_FALSE;
    }
    else
    {
        while (match && index < patternLength)
        {
            kzChar char1 = string[stringLength - 1 - index];
            kzChar char2 = pattern[patternLength - 1 - index];
            if (char1 != char2 && char2 != KZC_STRING_TERMINATION_SYMBOL)
            {
                match = KZ_FALSE;
            }
            else
            {
                ++index;
            }
        }
    }

    return match;
}

kzsError kzcStringReplace(const struct KzcMemoryManager* memoryManager, kzString originalString, kzString pattern,
                          kzString patternReplacement, kzMutableString* out_string)
{
    kzsError result;
    kzUint originalLength = kzsStrlen(originalString);
    kzUint patternLength = kzsStrlen(pattern);
    kzUint replacementLength = kzsStrlen(patternReplacement);
    kzUint patternsFound = kzcStringGetSubstringCount(originalString, pattern);
    kzMutableString newString;

    /* Create new string with patterns replaced. */
    {
        kzUint resultLength = originalLength - (patternLength * patternsFound) + (replacementLength * patternsFound);
        kzUint currentTargetIndex = 0;
        kzUint currentSourceIndex = 0;
        kzUint i;
        result = kzcStringAllocate(memoryManager, resultLength, &newString);
        kzsErrorForward(result);

        for (i = 0; i < patternsFound; ++i)
        {
            kzUint patternIndex;
            kzUint offset;
            kzBool found = kzcStringFindNextSubstring(originalString, pattern, currentSourceIndex, &patternIndex);
            kzsAssert(found);
            KZ_UNUSED_RETURN_VALUE(found); /* This information is not needed. */
            offset = patternIndex - currentSourceIndex;
            kzsMemcpy(newString + currentTargetIndex, originalString + currentSourceIndex, offset);
            currentTargetIndex += offset;
            currentSourceIndex += offset;
            kzsMemcpy(newString + currentTargetIndex, patternReplacement, replacementLength);
            currentTargetIndex += replacementLength;
            currentSourceIndex += patternLength;
        }
        kzsMemcpy(newString + currentTargetIndex, originalString + currentSourceIndex, originalLength - currentSourceIndex);

        newString[resultLength] = KZC_STRING_TERMINATION_SYMBOL;
    }

    *out_string = newString;
    kzsSuccess();
}

kzsError kzcStringSplit(const struct KzcMemoryManager* memoryManager, kzString string, kzString separator,
                        kzUint* out_stringCount, kzMutableString** out_strings)
{
    kzsError result;
    kzUint separatorLength = kzsStrlen(separator);
    kzUint separatorsFound = kzcStringGetSubstringCount(string, separator);
    kzUint splitStringCount = separatorsFound + 1;
    kzMutableString* splitStrings;

    result = kzcMemoryAllocArray(memoryManager, splitStrings, splitStringCount, "Split strings");
    kzsErrorForward(result);

    /* Create split strings. */
    {
        kzUint currentIndex = 0;
        kzUint i;

        for (i = 0; i < separatorsFound; ++i)
        {
            kzUint separatorIndex;
            kzUint newStringLength;
            kzMutableString newString;
            kzBool found = kzcStringFindNextSubstring(string, separator, currentIndex, &separatorIndex);
            kzsAssert(found);
            KZ_UNUSED_RETURN_VALUE(found); /* This information is not needed. */

            newStringLength = separatorIndex - currentIndex;
            result = kzcStringSubstring(memoryManager, string, currentIndex, newStringLength, &newString);
            kzsErrorForward(result);
            currentIndex += newStringLength + separatorLength;

            splitStrings[i] = newString;
        }

        /* Last item */
        {
            kzUint newStringLength = kzsStrlen(string + currentIndex);
            kzMutableString newString;

            result = kzcStringSubstring(memoryManager, string, currentIndex, newStringLength, &newString);
            kzsErrorForward(result);

            splitStrings[i] = newString;
        }
    }

    *out_stringCount = splitStringCount;
    *out_strings = splitStrings;
    kzsSuccess();
}

kzsError kzcStringJoin(const struct KzcMemoryManager* memoryManager, kzUint stringCount, const kzString* strings,
                       kzString delimiter, kzMutableString* out_string)
{
    kzsError result;
    kzUint length = 0;
    kzUint delimiterLength = kzsStrlen(delimiter);
    kzMutableString string;
    kzUint i;

    for (i = 0; i < stringCount; ++i)
    {
        if (i > 0) /* Append delimiter for all but first item. */
        {
            length += delimiterLength;
        }
        length += kzsStrlen(strings[i]);
    }

    result = kzcStringAllocate(memoryManager, length, &string);
    kzsErrorForward(result);

    /* Build the string. */
    {
        kzUint index = 0;
        for (i = 0; i < stringCount; ++i)
        {
            kzUint currentLength = kzsStrlen(strings[i]);
            if (i > 0) /* Append delimiter for all but first item. */
            {
                kzsMemcpy(string + index, delimiter, delimiterLength);
                index += delimiterLength;
            }
            kzsMemcpy(string + index, strings[i], currentLength);
            index += currentLength;
        }

        string[length] = KZC_STRING_TERMINATION_SYMBOL;
    }

    *out_string = string;
    kzsSuccess();
}

kzInt kzcStringToInt(kzString string)
{
    kzsAssert(string != KZ_NULL);

    return kzsAtoi(string);
}

kzFloat kzcStringToFloat(kzString string)
{
    kzsAssert(string != KZ_NULL);

    return kzsAtof(string);
}

kzsError kzcIntToString(const struct KzcMemoryManager* memoryManager, kzInt value, kzMutableString* out_string)
{
    kzsError result;
    kzMutableString string;

    result = kzcStringFormat(memoryManager, "%i", &string, value);
    kzsErrorForward(result);

    *out_string = string;
    kzsSuccess();
}

kzsError kzcFloatToString(const struct KzcMemoryManager* memoryManager, kzFloat value, kzMutableString* out_string)
{
    kzsError result;
    kzMutableString string;

    result = kzcStringFormat(memoryManager, "%f", &string, value);
    kzsErrorForward(result);

    *out_string = string;
    kzsSuccess();
}

kzsError kzcStringAsciiToLowerCase(const struct KzcMemoryManager* memoryManager, kzString sourceString, kzMutableString* out_string)
{
    kzsError result;
    kzMutableString string;
    kzUint sourceStringLength;
    kzUint i;
    const kzChar upperToLowerCaseDelta = 'a' - 'A';

    sourceStringLength = kzcStringLength(sourceString);

    result = kzcStringAllocate(memoryManager, sourceStringLength, &string);
    kzsErrorForward(result);

    for (i = 0; i < sourceStringLength; ++i)
    {
        kzChar currentCharacter = sourceString[i];
        if (currentCharacter >= 'A' && currentCharacter <= 'Z')
        {
            currentCharacter += upperToLowerCaseDelta;
        }
        string[i] = currentCharacter;
    }
    string[sourceStringLength] = KZC_STRING_TERMINATION_SYMBOL;

    *out_string = string;
    kzsSuccess();
}

/** Checks if given character is a number. */
static kzBool kzcStringFormatIsNumber_internal(kzChar character)
{
    return (character >= '0') && (character <= '9');
}

/**
 * Converts the given string format type character specifier to type enum.
 * Returns KZ_FALSE if the character is not any type character.
 */
static kzBool kzcStringFormatGetType_internal(kzChar character, enum KzcStringFormatterType* out_type)
{
    kzBool result;
    enum KzcStringFormatterType type;

    switch (character)
    {
        case 'i': case 'd': type = KZC_STRING_FORMATTER_TYPE_SIGNED_INTEGER; result = KZ_TRUE; break;
        case 'u': type = KZC_STRING_FORMATTER_TYPE_UNSIGNED_INTEGER; result = KZ_TRUE; break;
        case 'f': type = KZC_STRING_FORMATTER_TYPE_FLOAT; result = KZ_TRUE; break;
        case 'x': type = KZC_STRING_FORMATTER_TYPE_LOWERCASE_HEXADECIMAL; result = KZ_TRUE; break;
        case 'X': type = KZC_STRING_FORMATTER_TYPE_UPPERCASE_HEXADECIMAL; result = KZ_TRUE; break;
        case 's': type = KZC_STRING_FORMATTER_TYPE_STRING; result = KZ_TRUE; break;
        case 'c': type = KZC_STRING_FORMATTER_TYPE_CHARACTER; result = KZ_TRUE; break;
        case 'C': type = KZC_STRING_FORMATTER_TYPE_UNICODE_CHARACTER; result = KZ_TRUE; break;
        case 'b': type = KZC_STRING_FORMATTER_TYPE_LOWERCASE_BOOLEAN; result = KZ_TRUE; break;
        case 'B': type = KZC_STRING_FORMATTER_TYPE_UPPERCASE_BOOLEAN; result = KZ_TRUE; break;
        case 'p': type = KZC_STRING_FORMATTER_TYPE_POINTER; result = KZ_TRUE; break;
        default:
        {
            result = KZ_FALSE;
            type = KZC_STRING_FORMATTER_TYPE_SIGNED_INTEGER;
        }
    }

    *out_type = type;
    return result;
}

/** Parses a formatter from the beginning of the given string parameter. */
static kzsError kzcStringFormatParseFormatter_internal(kzString formatterString, struct KzcStringFormatter* out_formatter)
{
    struct KzcStringFormatter formatter;

    /* States for string format parsing. */
    enum
    {
        FORMAT_STATE_START, /* Initial state, can be anything. */
        FORMAT_STATE_AFTER_FLAG_MINUS, /* State for anything but minus flag. */
        FORMAT_STATE_AFTER_FLAG_PLUS, /* State for anything but minus or plus flag. */
        FORMAT_STATE_AFTER_FLAGS, /* State for anything but flags. */
        FORMAT_STATE_PRECISION, /* State for first digit of precision. */
        FORMAT_STATE_PRECISION_CONTINUE, /* State after first digits of precision. */
        FORMAT_STATE_DONE /* Finish state. */
    } state = FORMAT_STATE_START;

    kzsAssert(formatterString[0] == '%');

    formatter.type = KZC_STRING_FORMATTER_TYPE_UNSPECIFIED;
    formatter.leftAligned = KZ_FALSE;
    formatter.mode = KZC_STRING_FORMATTER_MODE_NORMAL;
    formatter.signedPositives = KZ_FALSE;
    formatter.zeroPadded = KZ_FALSE;
    formatter.outputWidth = KZC_STRING_FORMATTER_UNSPECIFIED;
    formatter.precision = KZC_STRING_FORMATTER_UNSPECIFIED;
    formatter.length = 1;

    /* Parsing is done using a state machine.
       Every state in the state machine is a superset of another state. Thus the parsing is processed with a switch
       covering all possible states such that each case falls through to the next case if the case specific
       symbol was not found. In the beginning of the parsing the next symbol can be nearly anything, but when the
       parsing proceeds, the available possibilities are limited. */
     
    while (state != FORMAT_STATE_DONE)
    {
        kzChar character = formatterString[formatter.length++];
        switch (state)
        {
            case FORMAT_STATE_START:
            {
                if (character == '%') /* Escaped % */
                {
                    formatter.mode = KZC_STRING_FORMATTER_MODE_ESCAPED;
                    state = FORMAT_STATE_DONE; /* Nothing more */
                    break;
                }
                if (character == '-') /* Flag: - */
                {
                    formatter.leftAligned = KZ_TRUE;
                    state = FORMAT_STATE_AFTER_FLAG_MINUS;
                    break;
                }
            }
            /*lint -fallthrough */
            case FORMAT_STATE_AFTER_FLAG_MINUS:
            {
                if (character == '+') /* Flag: + */
                {
                    formatter.signedPositives = KZ_TRUE;
                    state = FORMAT_STATE_AFTER_FLAG_PLUS;
                    break;
                }
            }
            /*lint -fallthrough */
            case FORMAT_STATE_AFTER_FLAG_PLUS:
            {
                if (character == '0') /* Flag: 0 */
                {
                    formatter.zeroPadded = KZ_TRUE;
                    state = FORMAT_STATE_AFTER_FLAGS;
                    break;
                }
            }
            /*lint -fallthrough */
            case FORMAT_STATE_AFTER_FLAGS:
            {
                if (kzcStringFormatIsNumber_internal(character)) /* Width digit */
                {
                    kzInt number = (kzInt)(character - '0');
                    if (formatter.outputWidth == KZC_STRING_FORMATTER_UNSPECIFIED)
                    {
                        formatter.outputWidth = 0;
                    }
                    formatter.outputWidth = (formatter.outputWidth * 10) + number;
                    state = FORMAT_STATE_AFTER_FLAGS;
                    break;
                }
                if (character == '.') /* Precision separator */
                {
                    formatter.precision = 0;
                    state = FORMAT_STATE_PRECISION;
                    break;
                }
            }
            /*lint -fallthrough */
            case FORMAT_STATE_PRECISION_CONTINUE:
            {
                enum KzcStringFormatterType type;
                if (kzcStringFormatGetType_internal(character, &type)) /* Type */
                {
                    formatter.type = type;
                    if (formatter.type == KZC_STRING_FORMATTER_TYPE_FLOAT && formatter.precision == KZC_STRING_FORMATTER_UNSPECIFIED)
                    {
                        formatter.precision = KZC_STRING_FORMAT_DEFAULT_FLOAT_PRECISION;
                    }
                    state = FORMAT_STATE_DONE; /* Nothing more */
                    break;
                }
            }
            /*lint -fallthrough */
            case FORMAT_STATE_PRECISION:
            {
                if (kzcStringFormatIsNumber_internal(character)) /* Precision digit */
                {
                    kzInt number = (kzInt)(character - '0');
                    formatter.precision = (formatter.precision * 10) + number;
                    state = FORMAT_STATE_PRECISION_CONTINUE;
                    break;
                }
                kzsErrorThrow(KZC_ERROR_STRING_INVALID_FORMAT, "Invalid string format");
            }

            case FORMAT_STATE_DONE:
            {
                kzsAssert(KZ_FALSE);
            }
        }
    }

    kzsErrorTest(formatter.type != KZC_STRING_FORMATTER_TYPE_UNSPECIFIED, KZC_ERROR_STRING_INVALID_FORMAT, "Invalid string format");

    *out_formatter = formatter;
    kzsSuccess();
}

/** Checks that the parsed formatter is meaningful. */
static kzsError kzcStringFormatValidateFormatter_internal(struct KzcStringFormatter formatter)
{
    if (formatter.mode == KZC_STRING_FORMATTER_MODE_ESCAPED)
    {
        /* For escaped '%', all other format options are not possible. */
        kzsAssert(!formatter.leftAligned &&
                  !formatter.signedPositives &&
                  !formatter.zeroPadded &&
                  formatter.outputWidth == KZC_STRING_FORMATTER_UNSPECIFIED &&
                  formatter.precision == KZC_STRING_FORMATTER_UNSPECIFIED &&
                  formatter.length == 1);
    }
    else
    {
        /* Ensure that both left alignment and zero padding are not used. */
        kzsErrorTest(!(formatter.leftAligned && formatter.zeroPadded),
                     KZC_ERROR_STRING_INVALID_FORMAT, "Invalid string format: Can't enable both left alignment and zero padding.");

        /* Ensure that left alignment or zero padding is not used if outputWidth is unspecified. */
        kzsErrorTest(!(formatter.outputWidth == KZC_STRING_FORMATTER_UNSPECIFIED &&
                       (formatter.leftAligned || formatter.zeroPadded)),
                     KZC_ERROR_STRING_INVALID_FORMAT, "Invalid string format: Left alignment or zero padding can't be enabled if width is unspecified.");

        switch (formatter.type)
        {
            case KZC_STRING_FORMATTER_TYPE_SIGNED_INTEGER:
            case KZC_STRING_FORMATTER_TYPE_UNSIGNED_INTEGER:
            {
                /* Integers can not have precision. */
                kzsErrorTest(formatter.precision == KZC_STRING_FORMATTER_UNSPECIFIED,
                             KZC_ERROR_STRING_INVALID_FORMAT, "Invalid string format: Integers can't have precision.");
                break;
            }
            case KZC_STRING_FORMATTER_TYPE_FLOAT:
            {
                /* No illegal combinations */
                break;
            }
            case KZC_STRING_FORMATTER_TYPE_LOWERCASE_HEXADECIMAL:
            case KZC_STRING_FORMATTER_TYPE_UPPERCASE_HEXADECIMAL:
            {
                /* Hexadecimals can not have signed positives or precision. */
                kzsErrorTest(!formatter.signedPositives &&
                             formatter.precision == KZC_STRING_FORMATTER_UNSPECIFIED,
                             KZC_ERROR_STRING_INVALID_FORMAT, "Invalid string format: Hexadecimals can't have signed positives or precision.");
                break;
            }
            case KZC_STRING_FORMATTER_TYPE_STRING:
            case KZC_STRING_FORMATTER_TYPE_CHARACTER:
            case KZC_STRING_FORMATTER_TYPE_UNICODE_CHARACTER:
            case KZC_STRING_FORMATTER_TYPE_LOWERCASE_BOOLEAN:
            case KZC_STRING_FORMATTER_TYPE_UPPERCASE_BOOLEAN:
            case KZC_STRING_FORMATTER_TYPE_POINTER:
            {
                /* Non-numerics can not have signed positives, zero padding or precision. */
                kzsErrorTest(!formatter.signedPositives &&
                             !formatter.zeroPadded &&
                             formatter.precision == KZC_STRING_FORMATTER_UNSPECIFIED,
                             KZC_ERROR_STRING_INVALID_FORMAT, "Invalid string format: Only numerical types can have signed positives, zero padding or precision.");
                break;
            }

            case KZC_STRING_FORMATTER_TYPE_UNSPECIFIED:
            default:
            {
                kzsErrorThrow(KZC_ERROR_STRING_INVALID_FORMAT, "Invalid string format");
            }
        }
    }

    kzsSuccess();
}

/** Gets the character length of a signed integer value. */
static kzUint kzcStringFormatGetLengthSignedInteger_internal(kzInt value)
{
    kzUint length = 0;
    kzInt reducedValue = value;

    do
    {
        ++length;
        reducedValue /= 10;
    }
    while (reducedValue != 0);

    return length;
}

/** Gets the character length of an unsigned integer value. */
static kzUint kzcStringFormatGetLengthUnsignedInteger_internal(kzUint value)
{
    kzUint length = 0;
    kzUint reducedValue = value;

    do
    {
        ++length;
        reducedValue /= 10;
    }
    while (reducedValue != 0);

    return length;
}

/** Gets the character length of a float value. */
static kzUint kzcStringFormatGetLengthFloat_internal(kzFloat value, kzBool signedPositives, kzUint precision)
{
    kzUint length = (value < 0 || signedPositives) ? 1 : 0; /* One character for minus or plus sign */
    kzFloat reducedValue = value;

    /* Integer part */
    do
    {
        ++length;
        reducedValue /= 10;
    }
    while (reducedValue >= 1 || reducedValue <= -1);

    /* Decimal point and decimal digits */
    length += 1 + precision;

    return length;
}

/** Gets the character length of a hexadecimal integer value. */
static kzUint kzcStringFormatGetLengthHexadecimal_internal(kzUint value)
{
    kzUint length = 0;
    kzUint reducedValue = value;

    do
    {
        ++length;
        reducedValue /= 16;
    }
    while (reducedValue != 0);

    return length;
}

/** Gets the character length of a string value. */
static kzUint kzcStringFormatGetLengthString_internal(kzString value)
{
    return kzsStrlen(value);
}

/** Gets the character length of a unicode character value. */
static kzUint kzcStringFormatGetLengthUnicodeCharacter_internal(kzUnicodeChar value)
{
    return kzcStringGetUTF8ByteCount(value);
}

/** Gets the character length of a boolean value. */
static kzUint kzcStringFormatGetLengthBoolean_internal(kzBool value)
{
    return (value ? 4 : 5); /* strlen("true") or strlen("false") */
}

/** Gets the natural character length of a value with given formatting (that is, formatter.outputWidth is not considered). */
static kzUint kzcStringFormatGetTypeLength_internal(struct KzcStringFormatter formatter, kzArgList* arguments)
{
    kzUint length;

    switch (formatter.type)
    {
        case KZC_STRING_FORMATTER_TYPE_SIGNED_INTEGER:
        {
            kzInt value = KZ_READ_INT_FROM_ARGLIST(*arguments);
            length = kzcStringFormatGetLengthSignedInteger_internal(value);
            if (value < 0 || formatter.signedPositives)
            {
                ++length; /* One character for minus or plus sign */
            }
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_UNSIGNED_INTEGER:
        {
            kzUint value = KZ_READ_UINT_FROM_ARGLIST(*arguments);
            length = kzcStringFormatGetLengthUnsignedInteger_internal(value);
            if (formatter.signedPositives)
            {
                ++length; /* One character for plus sign */
            }
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_FLOAT:
        {
            kzFloat value = KZ_READ_FLOAT_FROM_ARGLIST(*arguments);
            length = kzcStringFormatGetLengthFloat_internal(value, formatter.signedPositives, (kzUint)formatter.precision);
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_LOWERCASE_HEXADECIMAL:
        case KZC_STRING_FORMATTER_TYPE_UPPERCASE_HEXADECIMAL:
        {
            kzUint value = KZ_READ_UINT_FROM_ARGLIST(*arguments);
            length = kzcStringFormatGetLengthHexadecimal_internal(value);
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_STRING:
        {
            kzString value = KZ_READ_STRING_FROM_ARGLIST(*arguments);
            length = kzcStringFormatGetLengthString_internal(value);
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_CHARACTER:
        {
            kzChar value = KZ_READ_CHAR_FROM_ARGLIST(*arguments);
            KZ_UNUSED_RETURN_VALUE(value); /* Just consumes the character, doesn't care what it is. */
            length = 1;
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_UNICODE_CHARACTER:
        {
            kzUnicodeChar value = (kzUnicodeChar)KZ_READ_UINT_FROM_ARGLIST(*arguments);
            length = kzcStringFormatGetLengthUnicodeCharacter_internal(value);
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_LOWERCASE_BOOLEAN:
        case KZC_STRING_FORMATTER_TYPE_UPPERCASE_BOOLEAN:
        {
            kzBool value = KZ_READ_BOOL_FROM_ARGLIST(*arguments);
            length = kzcStringFormatGetLengthBoolean_internal(value);
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_POINTER:
        {
            void* value = KZ_READ_POINTER_FROM_FROM_ARGLIST(*arguments);
            KZ_UNUSED_RETURN_VALUE(value); /* Just consumes the value, doesn't care what it is. */
            length = 1 + (sizeof(void*) * 2); /* Two characters per byte in hexadecimal format */
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_UNSPECIFIED:
        default:
        {
            kzsAssert(KZ_FALSE);
            length = 0;
        }
    }

    return length;
}

/** Gets the character length of formatted value. */
static kzUint kzcStringFormatGetFormattedLength_internal(struct KzcStringFormatter formatter, kzArgList* arguments)
{
    kzUint length;

    if (formatter.mode == KZC_STRING_FORMATTER_MODE_ESCAPED)
    {
        length = 1;
    }
    else
    {
        length = kzcStringFormatGetTypeLength_internal(formatter, arguments);

        if (formatter.outputWidth != KZC_STRING_FORMATTER_UNSPECIFIED)
        {
            length = kzsMaxU(length, (kzUint)formatter.outputWidth);
        }
    }

    return length;
}

/** Gets the length of the whole formatted string. */
static kzsError kzcStringFormatGetLength_internal(kzString format, kzArgList *arguments, kzUint* out_length)
{
    kzsError result;
    kzUint sourceIndex = 0;
    kzUint length = 0;
    kzUint formatterIndex;

    while (kzcStringFindNextCharacter(format, '%', sourceIndex, &formatterIndex))
    {
        kzUint offset = formatterIndex - sourceIndex;
        struct KzcStringFormatter formatter;
        kzUint formattedLength;

        result = kzcStringFormatParseFormatter_internal(format + formatterIndex, &formatter);
        kzsErrorForward(result);

        result = kzcStringFormatValidateFormatter_internal(formatter);
        kzsErrorForward(result);

        formattedLength = kzcStringFormatGetFormattedLength_internal(formatter, arguments);

        length += offset;
        sourceIndex += offset + formatter.length;
        length += formattedLength;
    }

    length += kzsStrlen(format + sourceIndex);

    *out_length = length;
    kzsSuccess();
}

/** Writes the prefix ('+' or '-') of the next argument using given formatter. */
static kzUint kzcStringFormatWritePrefix_internal(kzMutableString target, struct KzcStringFormatter formatter, kzArgList *arguments)
{
    kzUint offset = 0;
    kzBool negative = KZ_FALSE;
    kzBool signedPositive = KZ_FALSE;
    kzArgList argumentsCopy;
#ifdef __GNUC__
        va_copy(argumentsCopy, *arguments);
#else 
        argumentsCopy = *arguments; /* This is NOT ansi-c */
#endif
    


    switch (formatter.type)
    {
        case KZC_STRING_FORMATTER_TYPE_SIGNED_INTEGER:
        {
            kzInt value = KZ_READ_INT_FROM_ARGLIST(argumentsCopy);
            if (value < 0)
            {
                negative = KZ_TRUE;
            }
            else if (formatter.signedPositives)
            {
                signedPositive = KZ_TRUE;
            }
            break;
        }
        case KZC_STRING_FORMATTER_TYPE_UNSIGNED_INTEGER:
        {
            if (formatter.signedPositives)
            {
                signedPositive = KZ_TRUE;
            }
            break;
        }
        case KZC_STRING_FORMATTER_TYPE_FLOAT:
        {
            kzFloat value = KZ_READ_FLOAT_FROM_ARGLIST(argumentsCopy);
            if (value < 0)
            {
                negative = KZ_TRUE;
            }
            else if (formatter.signedPositives)
            {
                signedPositive = KZ_TRUE;
            }
            break;
        }
        case KZC_STRING_FORMATTER_TYPE_LOWERCASE_HEXADECIMAL:
        case KZC_STRING_FORMATTER_TYPE_UPPERCASE_HEXADECIMAL:
        case KZC_STRING_FORMATTER_TYPE_STRING:
        case KZC_STRING_FORMATTER_TYPE_CHARACTER:
        case KZC_STRING_FORMATTER_TYPE_UNICODE_CHARACTER:
        case KZC_STRING_FORMATTER_TYPE_LOWERCASE_BOOLEAN:
        case KZC_STRING_FORMATTER_TYPE_UPPERCASE_BOOLEAN:
        case KZC_STRING_FORMATTER_TYPE_POINTER:
        case KZC_STRING_FORMATTER_TYPE_UNSPECIFIED:
        default:
        {
            /* Do nothing */
        }
    }

    if (negative)
    {
        target[offset++] = '-';
    }
    if (signedPositive)
    {
        target[offset++] = '+';
    }

#ifdef __GNUC__
    va_end(argumentsCopy);
#endif
    return offset;
}

/** Writes a signed integer (without sign prefix) to the given target string. */
static kzUint kzcStringFormatWriteSignedInteger_internal(kzMutableString target, kzInt value)
{
    kzUint offset = 0;

    /* Write in reverse order */
    {
        kzUint maxLength = kzcStringFormatGetLengthSignedInteger_internal(value);

        /* Write the digits. */
        {
            kzInt reducedValue = value;
            kzUint i = 0;
            do
            {
                kzUint digit = kzsAbs(reducedValue % 10);
                reducedValue /= 10;
                target[maxLength - i - 1] = (kzChar)('0' + digit);
                ++i;
                ++offset;
            }
            while (reducedValue != 0);
        }
    }

    return offset;
}

/** Writes an unsigned integer (without optional '+' prefix) to the given target string. */
static kzUint kzcStringFormatWriteUnsignedInteger_internal(kzMutableString target, kzUint value)
{
    kzUint offset = 0;

    /* Write in reverse order */
    {
        kzUint maxLength = kzcStringFormatGetLengthUnsignedInteger_internal(value);

        /* Write the digits. */
        {
            kzUint reducedValue = value;
            kzUint i = 0;
            do
            {
                kzUint digit = reducedValue % 10;
                kzChar character = (kzChar)('0' + digit);
                reducedValue /= 10;
                target[maxLength - i - 1] = character;
                ++i;
                ++offset;
            }
            while (reducedValue != 0);
        }
    }

    return offset;
}

/** Writes a float (without sign prefix) to the given target string. */
static kzUint kzcStringFormatWriteFloat_internal(kzMutableString target, kzFloat value, kzUint precision)
{
    kzUint offset = 0;
    kzFloat integralPart;
    kzFloat fractionalPart;

    fractionalPart = kzsModf(value, &integralPart); /* Separate floating point value to integral and decimal parts. */

    /* Write integral part in reverse order */
    {
        kzUint maxLength = 0;
        /* Calculate maximum length. */
        {
            kzFloat reducedValue = integralPart;
            do
            {
                ++maxLength;
                reducedValue /= 10;
            }
            while (reducedValue >= 1 || reducedValue <= -1);
        }

        /* Write the digits. */
        {
            kzFloat reducedValue = integralPart;
            kzUint i = 0;
            do
            {
                kzInt modulo = (kzInt)kzsFmod(reducedValue, 10.0f);
                kzUint digit = kzsAbs(modulo);
                kzChar character = (kzChar)('0' + digit);
                target[maxLength - i - 1] = character;
                reducedValue /= 10.0f;
                ++i;
                ++offset;
            }
            while (reducedValue >= 1 || reducedValue <= -1);
        }
    }

    target[offset++] = '.';

    /* Write fractional part */
    {
        /* Write the digits. */
        {
            kzFloat reducedValue = fractionalPart;
            kzUint i;
            for (i = 0; i < precision; ++i)
            {
                kzUint digit;
                kzChar character;
                kzInt modulo;
                reducedValue *= 10.0f;
                modulo = (kzInt)kzsFmod(reducedValue, 10.0f);
                digit = kzsAbs(modulo);
                character = (kzChar)('0' + digit);
                reducedValue -= (kzFloat)digit;
                target[offset++] = character;
            }
        }
    }

    target[offset] = '\0';
    return offset;
}

/** Writes an unsigned integer as hexadecimal to the given target string. */
static kzUint kzcStringFormatWriteHexadecimal_internal(kzMutableString target, kzUint value, kzBool uppercase)
{
    kzUint offset = 0;

    /* Write in reverse order */
    {
        kzUint maxLength = 0;
        /* Calculate maximum length. */
        {
            kzUint reducedValue = value;
            do
            {
                ++maxLength;
                reducedValue /= 16;
            }
            while (reducedValue != 0);
        }

        /* Write the digits. */
        {
            kzUint reducedValue = value;
            kzUint i = 0;
            do
            {
                kzUint digit = reducedValue % 16;
                kzChar character;
                if (digit < 10)
                {
                    character = (kzChar)('0' + digit);
                }
                else
                {
                    if (uppercase)
                    {
                        character = (kzChar)('A' + digit - 10);
                    }
                    else
                    {
                        character = (kzChar)('a' + digit - 10);
                    }
                }
                target[maxLength - i - 1] = character;
                reducedValue /= 16;
                ++i;
                ++offset;
            }
            while (reducedValue != 0);
        }
    }

    return offset;
}

/** Writes a string to the given target string. */
static kzUint kzcStringFormatWriteString_internal(kzMutableString target, kzString value)
{
    kzUint length = kzsStrlen(value);

    kzsMemcpy(target, value, length);

    return length;
}

/** Writes a character to the given target string. */
static kzUint kzcStringFormatWriteCharacter_internal(kzMutableString target, kzChar value)
{
    target[0] = value;

    return 1;
}

/** Writes a unicode charater to the given target string. */
static kzUint kzcStringFormatWriteUnicodeCharacter_internal(kzMutableString target, kzUnicodeChar value)
{
    kzUint length;

    length = kzcStringWriteUnicodeCharacter(target, value);

    return length;
}

/** Writes a boolean to the given target string. */
static kzUint kzcStringFormatWriteBoolean_internal(kzMutableString target, kzBool value, kzBool uppercase)
{
    kzUint length;
    kzString stringValue;

    if (value)
    {
        stringValue = uppercase ? "True" : "true";
    }
    else
    {
        stringValue = uppercase ? "False" : "false";
    }
    length = kzcStringFormatWriteString_internal(target, stringValue);

    return length;
}

/** Writes a pointer to the given target string. */
static kzUint kzcStringFormatWritePointer_internal(kzMutableString target, const void* value)
{
    kzUint offset = 0;
    kzUint byteCount = sizeof(value);
    kzUint i;
    kzUint valueInt = (kzUint)value; /*lint !e923 Cast from pointer to unsigned int */

    target[offset++] = '#';

    for (i = 0; i < byteCount; ++i)
    {
        kzUint shiftOffset = ((byteCount - i - 1) * 8);
        kzUint byteValue = (valueInt >> shiftOffset) & 0xFF;

        offset += kzcStringFormatWriteHexadecimal_internal(target + offset, byteValue, KZ_TRUE);
    }

    return offset;
}

/** Writes the value of an argument using given formatter (without prefix or padding) to the given target string. */
static kzUint kzcStringFormatWriteType_internal(kzMutableString target, struct KzcStringFormatter formatter, kzArgList* arguments)
{
    kzUint offset;

    switch (formatter.type)
    {
        case KZC_STRING_FORMATTER_TYPE_SIGNED_INTEGER:
        {
            kzInt value = KZ_READ_INT_FROM_ARGLIST(*arguments);
            offset = kzcStringFormatWriteSignedInteger_internal(target, value);
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_UNSIGNED_INTEGER:
        {
            kzUint value = KZ_READ_UINT_FROM_ARGLIST(*arguments);
            offset = kzcStringFormatWriteUnsignedInteger_internal(target, value);
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_FLOAT:
        {
            kzFloat value = KZ_READ_FLOAT_FROM_ARGLIST(*arguments);
            offset = kzcStringFormatWriteFloat_internal(target, value, (kzUint)formatter.precision);
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_LOWERCASE_HEXADECIMAL:
        {
            kzUint value = KZ_READ_UINT_FROM_ARGLIST(*arguments);
            offset = kzcStringFormatWriteHexadecimal_internal(target, value, KZ_FALSE);
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_UPPERCASE_HEXADECIMAL:
        {
            kzUint value = KZ_READ_UINT_FROM_ARGLIST(*arguments);
            offset = kzcStringFormatWriteHexadecimal_internal(target, value, KZ_TRUE);
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_STRING:
        {
            kzString value = KZ_READ_STRING_FROM_ARGLIST(*arguments);
            offset = kzcStringFormatWriteString_internal(target, value);
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_CHARACTER:
        {
            kzChar value = KZ_READ_CHAR_FROM_ARGLIST(*arguments);
            offset = kzcStringFormatWriteCharacter_internal(target, value);
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_UNICODE_CHARACTER:
        {
            kzUnicodeChar value = (kzUnicodeChar)KZ_READ_UINT_FROM_ARGLIST(*arguments);
            offset = kzcStringFormatWriteUnicodeCharacter_internal(target, value);
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_LOWERCASE_BOOLEAN:
        {
            kzBool value = KZ_READ_BOOL_FROM_ARGLIST(*arguments);
            offset = kzcStringFormatWriteBoolean_internal(target, value, KZ_FALSE);
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_UPPERCASE_BOOLEAN:
        {
            kzBool value = KZ_READ_BOOL_FROM_ARGLIST(*arguments);
            offset = kzcStringFormatWriteBoolean_internal(target, value, KZ_TRUE);
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_POINTER:
        {
            void* value = KZ_READ_POINTER_FROM_FROM_ARGLIST(*arguments);
            offset = kzcStringFormatWritePointer_internal(target, value);
            break;
        }

        case KZC_STRING_FORMATTER_TYPE_UNSPECIFIED:
        default:
        {
            kzsAssert(KZ_FALSE);
            offset = 0;
        }
    }

    return offset;
}

/** Writes the given argument to the target string with given formatting. */
static kzUint kzcStringFormatWriteFormatter_internal(kzMutableString target, struct KzcStringFormatter formatter, kzArgList* arguments)
{
    kzUint offset = 0;

    if (formatter.mode == KZC_STRING_FORMATTER_MODE_ESCAPED)
    {
        target[offset++] = '%';
    }
    else
    {
    kzArgList argumentsCopy;
    kzUint width;
    kzUint paddingWidth = 0;
#ifdef __GNUC__
        va_copy(argumentsCopy, *arguments);
#else 
        argumentsCopy = *arguments; /* This is NOT ansi-c */
#endif
    width = kzcStringFormatGetTypeLength_internal(formatter, &argumentsCopy);

#ifdef __GNUC__
    va_end(argumentsCopy);
#endif
        if (formatter.outputWidth != KZC_STRING_FORMATTER_UNSPECIFIED && (kzUint)formatter.outputWidth > width)
        {
            paddingWidth = (kzUint)formatter.outputWidth - width;
        }

        if (formatter.leftAligned)
        {
            kzUint i;
            kzsAssert(!formatter.zeroPadded);

            /* Write prefix */
            offset += kzcStringFormatWritePrefix_internal(target, formatter, arguments);
            
            /* Write data */
            offset += kzcStringFormatWriteType_internal(target + offset, formatter, arguments);

            /* Write padding */
            for (i = 0; i < paddingWidth; ++i)
            {
                target[offset++] = ' ';
            }
        }
        else
        {
            if (formatter.zeroPadded)
            {
                kzUint i;

                /* Write prefix */
                offset += kzcStringFormatWritePrefix_internal(target, formatter, arguments);

                /* Write zero padding */
                for (i = 0; i < paddingWidth; ++i)
                {
                    target[offset++] = '0';
                }
            }
            else
            {
                kzUint i;

                /* Write empty padding */
                for (i = 0; i < paddingWidth; ++i)
                {
                    target[offset++] = ' ';
                }

                /* Write prefix */
                offset += kzcStringFormatWritePrefix_internal(target + offset, formatter, arguments);
            }

            /* Write data */
            offset += kzcStringFormatWriteType_internal(target + offset, formatter, arguments);
        }
    }

    return offset;
}

/** Writes the whole string format to the pre-allocated target string. */
static kzsError kzcStringFormatWrite_internal(kzMutableString target, kzString format, kzArgList *arguments)
{
    kzsError result;
    kzUint sourceIndex = 0;
    kzUint targetIndex = 0;
    kzUint formatterIndex;

    while (kzcStringFindNextCharacter(format, '%', sourceIndex, &formatterIndex))
    {
        kzUint offset = formatterIndex - sourceIndex;
        struct KzcStringFormatter formatter;
        kzUint formattedLength;

        kzsMemcpy(target + targetIndex, format + sourceIndex, offset);
        targetIndex += offset;
        sourceIndex += offset;

        result = kzcStringFormatParseFormatter_internal(format + formatterIndex, &formatter);
        kzsErrorForward(result);

        formattedLength = kzcStringFormatWriteFormatter_internal(target + targetIndex, formatter, arguments);

        sourceIndex += formatter.length;
        targetIndex += formattedLength;
    }

    kzsStrcpy(target + targetIndex, format + sourceIndex);
    kzsSuccess();
}

kzsError kzcStringFormatList(const struct KzcMemoryManager* memoryManager, kzString format, kzArgList *arguments, kzMutableString* out_string)
{
    kzsError result;
    kzUint stringLength;
    kzMutableString string;
    kzArgList argumentsCopy;
#ifdef __GNUC__
    va_copy(argumentsCopy, *arguments);
#else 
    argumentsCopy = *arguments; /* This is NOT ansi-c */
#endif
    result = kzcStringFormatGetLength_internal(format, &argumentsCopy, &stringLength);
    kzsErrorForward(result);
#ifdef __GNUC__
    va_end(argumentsCopy);
#endif
    result = kzcStringAllocate(memoryManager, stringLength, &string);
    kzsErrorForward(result);

    result = kzcStringFormatWrite_internal(string, format, arguments);
    kzsErrorForward(result);

    *out_string = string;
    kzsSuccess();
}

kzsError kzcStringFormat(const struct KzcMemoryManager* memoryManager, kzString format, kzMutableString* out_string, ...)
{
    kzsError result;
    kzArgList arguments;
    kzMutableString string;
    /*kzcStringAllocate(memoryManager, 1025, out_string);*/
    va_start(arguments, out_string);
/*    vsprintf(*out_string, format, arguments);*/
    result = kzcStringFormatList(memoryManager, format, &arguments, &string);
    kzsErrorForward(result);

    va_end(arguments);

    *out_string = string;
    kzsSuccess();
}

kzInt kzcStringCompare(kzString first, kzString second)
{
    return (first == second ? 0 : kzsStrcmp(first, second));
}

kzBool kzcStringIsEqual(kzString first, kzString second)
{
    return (first == second) || kzsStrcmp(first, second) == 0;
}

/* TODO: Add kzcStringCompareIgnoreCase */

kzU32 kzcStringGetHashCode(kzString string)
{
    kzU32 hash = 5381;
    kzUint i;

    /* djb2 hash algorithm */

    for (i = 0; string[i] != KZC_STRING_TERMINATION_SYMBOL; ++i)
    {
        hash = ((hash << 5) + hash) + (kzByte)string[i]; /* hash * 33 + c */
    }

    return hash;
}

kzUint kzcStringGetUnicodeLength(kzString string)
{
    kzUint length = 0;
    kzString tail = string;
    kzUint byteReadCount;
    while (kzcStringGetUnicodeCharacter(tail, &byteReadCount) != KZC_STRING_TERMINATION_SYMBOL)
    {
        tail += byteReadCount;
        ++length;
    }
    return length;
}

kzUint kzcStringGetUTF8ByteCount(kzUnicodeChar codePoint)
{
    kzUint byteCount;
    if (codePoint <= 0x0000007F)
    {
        byteCount = 1;
    }
    else if (codePoint <= 0x000007FF)
    {
        byteCount = 2;
    }
    else if (codePoint <= 0x0000FFFF)
    {
        byteCount = 3;
    }
    else if (codePoint <= 0x0010FFFF)
    {
        byteCount = 4;
    }
    else
    {
        byteCount = 0; /* Invalid character */
    }
    return byteCount;
}

kzUnicodeChar kzcStringGetUnicodeCharacter(kzString string, kzUint* out_byteReadCount)
{
    kzString tail = string;
    kzUint byteReadCount = 1;
    kzU8 first = (kzU8)*tail++;
    kzUnicodeChar codePoint = (kzUnicodeChar)(first & 0x7F);

    /* Not 7-bit ASCII character. */
    if ((first >> 7) == 1)
    {
        kzBool invalidCharacter = KZ_FALSE;
        kzUint otherCount;
        kzUint requiredCount;
        kzUint i;

        /* Calculate number of bytes for the character. */
        if ((first >> 5) == 0x06) /* 110xxxxx */
        {
            otherCount = 1;
            codePoint &= 0x1F; /* 00011111*/
        }
        else if ((first >> 4) == 0x0E) /* 1110xxxx */
        {
            otherCount = 2;
            codePoint &= 0x0F; /* 00001111*/
        }
        else if ((first >> 3) == 0x1E) /* 11110xxx */
        {
            otherCount = 3;
            codePoint &= 0x07; /* 0000111*/
        }
        else
        {
            otherCount = 0;
            invalidCharacter = KZ_TRUE;
        }

        /* Decode the character. */
        for (i = 0; i < otherCount; ++i)
        {
            kzU8 other = (kzU8)*tail++;
            if ((other >> 6) == 0x02) /* 10xxxxxx */
            {
                codePoint = (codePoint << 6) + (kzUnicodeChar)(other & 0x3F); /* 00111111 */
            }
            else
            {
                invalidCharacter = KZ_TRUE;
            }
        }
        byteReadCount += otherCount;

        /* Check for invalid character or character encoding. */
        requiredCount = kzcStringGetUTF8ByteCount(codePoint);
        if (requiredCount != (1 + otherCount))
        {
            invalidCharacter = KZ_TRUE;
        }

        if (invalidCharacter)
        {
            codePoint = KZC_STRING_UNICODE_REPLACEMENT_CHARACTER;
        }
    }

    *out_byteReadCount = byteReadCount;
    return codePoint;
}

kzUint kzcStringWriteUnicodeCharacter(kzMutableString string, kzUnicodeChar character)
{
    kzU8* bytes = (kzU8*)string;
    kzUint byteCount = 0;
    if (character <= 0x0000007F)
    {
        bytes[byteCount++] = (kzU8)character; /* 0xxxxxxx */
    }
    else if (character <= 0x000007FF)
    {
        bytes[byteCount++] = 0xC0 | (kzU8)(character >> 6); /* 110xxxxx */
        bytes[byteCount++] = 0x80 | (kzU8)(character & 0x0000003F); /* 10xxxxxx */
    }
    else if (character <= 0x0000FFFF)
    {
        bytes[byteCount++] = 0xE0 | (kzU8)(character >> 12); /* 1110xxxx */
        bytes[byteCount++] = 0x80 | (kzU8)((character >> 6) & 0x0000003F); /* 10xxxxxx */
        bytes[byteCount++] = 0x80 | (kzU8)(character & 0x0000003F); /* 10xxxxxx */
    }
    else if (character <= 0x0010FFFF)
    {
        bytes[byteCount++] = 0xF0 | (kzU8)(character >> 18); /* 11110xxx */
        bytes[byteCount++] = 0x80 | (kzU8)((character >> 12) & 0x0000003F); /* 10xxxxxx */
        bytes[byteCount++] = 0x80 | (kzU8)((character >> 6) & 0x0000003F); /* 10xxxxxx */
        bytes[byteCount++] = 0x80 | (kzU8)(character & 0x0000003F); /* 10xxxxxx */
    }
    else
    {
        /* Invalid character, don't write anything */
    }
    return byteCount;
}

kzsError kzcStringToUnicodeArray(const struct KzcMemoryManager* memoryManager, kzString string,
                                 kzUint* out_characterCount, kzUnicodeChar** out_unicodeCharacters)
{
    kzsError result;
    kzUint characterCount = kzcStringGetUnicodeLength(string);
    kzUnicodeChar* unicodeCharacters;

    result = kzcMemoryAllocArray(memoryManager, unicodeCharacters, characterCount, "Unicode character array");
    kzsErrorForward(result);

    {
        kzUint i = 0;
        struct KzcStringIterator it = kzcStringGetIterator(string);
        while (kzcStringIterate(it))
        {
            kzUnicodeChar character = kzcStringIteratorGetValue(it);
            unicodeCharacters[i++] = character;
        }
    }

    *out_characterCount = characterCount;
    *out_unicodeCharacters = unicodeCharacters;
    kzsSuccess();
}

kzsError kzcStringFromUnicodeArray(const struct KzcMemoryManager* memoryManager, kzUint arrayLength,
                                   const kzUnicodeChar* unicodeArray, kzMutableString* out_string)
{
    kzsError result;
    kzMutableString string;

    kzUint size = 0;
    {
        kzUint i;
        for (i = 0; i < arrayLength; ++i)
        {
            size += kzcStringGetUTF8ByteCount(unicodeArray[i]);
        }
    }

    result = kzcStringAllocate(memoryManager, size, &string);
    kzsErrorForward(result);

    {
        kzUint i;
        kzMutableString tail = string;
        for (i = 0; i < arrayLength; ++i)
        {
            tail += kzcStringWriteUnicodeCharacter(tail, unicodeArray[i]);
        }
    }

    *out_string = string;
    kzsSuccess();
}

struct KzcStringIterator kzcStringGetIterator(kzString string)
{
    struct KzcStringIterator iterator;
    iterator.tail_private = string;
    iterator.codePoint_private = '\0';
    return iterator;
}

kzBool kzcStringIterate_private(struct KzcStringIterator* iterator)
{
    kzUint byteReadCount;

    iterator->codePoint_private = kzcStringGetUnicodeCharacter(iterator->tail_private, &byteReadCount);
    iterator->tail_private += byteReadCount;

    return iterator->codePoint_private != KZC_STRING_TERMINATION_SYMBOL;
}

kzUnicodeChar kzcStringIteratorGetValue_private(const struct KzcStringIterator* iterator)
{
    return iterator->codePoint_private;
}

kzBool kzcStringIsSpace_internal(kzChar character) 
{
    kzBool isSpace;
    switch (character) 
    {
        case ' ':
        case '\f':
        case '\r':
        case '\n':
        case '\t':
        {
            isSpace = KZ_TRUE;
            break;
        }
        default:
        {
            isSpace = KZ_FALSE;
            break;
        }
    }
    return isSpace;
}


kzString kzcStringTrim(kzMutableString input) 
{
    kzChar* end;
    kzChar* ptr;
    kzChar* start = (kzChar*)input;

    while(kzcStringIsSpace_internal(*start))
    {
        start++;
    }

    ptr = start;
    end = start;
    while (*ptr++ != '\0')
    {
        if (!kzcStringIsSpace_internal(*ptr)) 
        {
            end = ptr;
        }
    }

    *end = '\0';
    return start;
}
