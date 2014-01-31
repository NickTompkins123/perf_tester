/**
* \file
* Standard library string function wrappers.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_STRING_H
#define KZS_STRING_H


#include <system/kzs_types.h>

/* Standard library */
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


/** Return a pointer to the first occurrence of string2 in string1. */
KZ_INLINE kzString kzsStrstr(kzString string1, kzString string2)
{
    return (kzString)strstr(string1, string2);
}

/** Return a pointer to the first occurrence of ch in string1. */
KZ_INLINE kzString kzsStrchr(kzString string1, kzChar ch)
{
    return (kzString)strchr(string1, ch);
}

/** Concatenate tail onto the end of head. */
KZ_INLINE void kzsStrcat(kzMutableString head, kzString tail)
{
    strcat(head, tail);
}

/** Return the length of the string. */
KZ_INLINE kzUint kzsStrlen(kzString string)
{
    return (kzUint)strlen(string);
}

/** Concatenate tail onto the end of head with at most count characters. */
KZ_INLINE void kzsStrncat(kzMutableString head, kzString tail, kzUint count)
{
    strncat(head, tail, count);
}

/** Copy the source string to the destination. */
KZ_INLINE void kzsStrcpy(kzMutableString destination, kzString source)
{
    strcpy(destination, source);
}

/** Copy the source string to the destination. */
KZ_INLINE void kzsStrncpy(kzMutableString destination, kzString source, kzUint n)
{
    strncpy(destination, source, n);
}

/** Convert a string into an integer. */
KZ_INLINE kzInt kzsAtoi(kzString string)
{
    return (kzInt)atoi(string);
}

/** Convert a string into a float. */
KZ_INLINE kzFloat kzsAtof(kzString string)
{
    return (kzFloat)atof(string);
}

/** Compare two strings. Return value is 0 if the strings are equal. */
KZ_INLINE kzInt kzsStrcmp(kzString first, kzString second)
{
    return (kzInt)strcmp(first, second);
}

/** Return true if the string is a whitespace. */
KZ_INLINE kzBool kzsIsSpace(const kzChar string)
{
    return isspace(string) != 0;
}

/** Return true if the character is a digit. */
KZ_INLINE kzBool kzsIsDigit(kzChar ch)
{
    return isdigit(ch) != 0;
}

/*
* Splits string into tokens. 
* On a first call, the function expects a string as argument for string, whose first character is used as the starting location to scan for tokens. 
* In subsequent calls, the function expects a null pointer and uses the position right after the end of last token as the new starting location for scanning.
* \param string String to split.
* \param delimiters Delimiters used to split the string to tokens
* \return Returns a pointer to the last token found in string 
*/
KZ_INLINE kzString kzsStrtok(kzMutableString string, kzString delimiters)
{
    return (kzString)strtok(string, delimiters);
}


#endif
