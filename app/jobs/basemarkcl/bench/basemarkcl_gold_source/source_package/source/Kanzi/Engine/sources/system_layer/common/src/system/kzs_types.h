/**
 * \file
 * Kanzi types - integers, floats, doubles, strings, inlines
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZS_TYPES_H
#define KZS_TYPES_H


/** Boolean type. */
typedef int kzBool;

#define KZ_FALSE (kzBool)0 /**< False boolean value. */
#define KZ_TRUE (kzBool)1 /**< True boolean value. */

/** Char type. */
typedef char kzChar;
/** Unsigned char type. */
typedef unsigned int kzUnicodeChar;
/** Constant string type. */
typedef kzChar const* kzString;
/** Non-const string type. */
typedef kzChar* kzMutableString;

/** Signed integer type. */
typedef int kzInt;
/** Unsigned integer type. */
typedef unsigned int kzUint;

/** Float type. */
typedef float kzFloat;
/** Double precision float type. */
typedef double kzDouble;

/** Byte type. */
typedef unsigned char kzByte;

/** Unsigned 8-bit type. */
typedef unsigned char kzU8;
/** Signed 8-bit type. */
typedef signed char kzS8;

/** Unsigned 16-bit type. */
typedef unsigned short kzU16;
/** Signed 16-bit type. */
typedef signed short kzS16;

/** Unsigned 32-bit type. */
typedef unsigned long kzU32;
/** Signed 32-bit type. */
typedef signed long kzS32;

/** Null pointer value. */
#define KZ_NULL 0 /*lint --e(910,634)*/


#ifndef KZ_INLINE
#define KZ_INLINE static /**< Inline function definition. */
#endif


/** Macro for flagging unused function parameters. */
#define KZ_UNUSED_PARAMETER(param) (void)param

/** Macro for flagging unused return value. */
#define KZ_UNUSED_RETURN_VALUE(param) (void)param

/** Macro for flagging a function as a call-back function. */
#define KZ_CALLBACK /*lint -e{715, 818} Suppress warnings of unused parameters and parameter could be const pointer. */

/** \see KZ_MULTILINE_MACRO */
KZ_INLINE kzBool kzMultiLineMacroTest_private(void) { return KZ_FALSE; }
/** Beginning of a multiline macro. */
#define KZ_MULTILINE_MACRO do {
/** End of a multiline macro. */
#define KZ_MULTILINE_MACRO_END } while (kzMultiLineMacroTest_private())

/** Macro to use in empty source file to allow compilation. */
#define KZ_EMPTY_SOURCE_FILE static void* const unusedVariable = KZ_NULL /*lint --e{528}*/


#endif
