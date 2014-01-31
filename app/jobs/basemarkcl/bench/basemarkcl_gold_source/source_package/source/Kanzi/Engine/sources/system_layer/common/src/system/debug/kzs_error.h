/**
 * \file
 * Error handling functions.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZS_ERROR_H
#define KZS_ERROR_H


#include <system/kzs_types.h>


/** Error code type definition. */
typedef int kzsError;
/** Exception code type definition, extends error. */
typedef int kzsException;


#ifdef WIN32
#define KZS_EXCEPTION_STORE_TO_THREAD_LOCAL_STORAGE
#endif


/*
      0 -  9999: Generic error codes
  10000 - 19999: System layer error codes
  20000 - 29999: Core layer error codes
  30000 - 39999: User layer error codes
  40000 - 49999: Framework error codes
 100000 -      : Application error codes
*/

/** Success code. */
#define KZS_SUCCESS (kzsError)0 /**< No error. */

/* Generic error codes. (1 - ) */
#define KZS_ERROR_ASSERTION_FAILED (kzsError)1 /**< Assertion failed. */
#define KZS_ERROR_NOT_IMPLEMENTED_YET (kzsError)2 /**< Functionality is not yet implemented. */
#define KZS_ERROR_ENUM_OUT_OF_RANGE (kzsError)3 /**< Invalid value for enum. */
#define KZS_ERROR_ILLEGAL_OPERATION (kzsError)4 /**< Trying to perform illegal operation. */
#define KZS_ERROR_ILLEGAL_ARGUMENT (kzsError)5 /**< An argument was invalid for the given context. */
#define KZS_ERROR_MALFORMED_DATA (kzsError)6 /**< Data is malformed. */
#define KZS_ERROR_ARRAY_OUT_OF_BOUNDS (kzsError)7 /**< Trying to access array with invalid index. */

/** Return from the function without any error code. */
#define kzsSuccess() return KZS_SUCCESS

#ifdef _DEBUG

/** Asserts the given test parameter. If test evaluates to true, nothing happens. Only effective in debug build. */
#define kzsAssertText(test, text) KZ_MULTILINE_MACRO                        \
    kzBool assertResult = (test);                                           \
    if (!(assertResult))                                                    \
    {                                                                       \
        kzsErrorLog(KZS_ERROR_ASSERTION_FAILED, "Assertion failed: " text); \
        kzsAssert_private();                                                \
    }                                                                       \
KZ_MULTILINE_MACRO_END

/** Asserts the given test parameter. If test evaluates to true, nothing happens. Only effective in debug build. */
#define kzsAssert(test) KZ_MULTILINE_MACRO                                      \
    kzBool assertResult = (test);                                               \
    if (!(assertResult))                                                        \
    {                                                                           \
        kzsErrorLog(KZS_ERROR_ASSERTION_FAILED, "Assertion failed: " # test);   \
        kzsAssert_private();                                                    \
    }                                                                           \
KZ_MULTILINE_MACRO_END

/** Terminates the application and traps the debugger if possible. Only effective in debug build. */
#define kzsErrorBreak() kzsErrorBreak_private()

/** Traps the debugger if possible. Only effective in debug build. */
#define kzsDebugBreak() kzsDebugBreak_private()

#else

/** Asserts the given test parameter. If test evaluates to true, nothing happens. Only effective in debug build. */
#define kzsAssertText(test, text) (void)0

/** Asserts the given test parameter. If test evaluates to true, nothing happens. Only effective in debug build. */
#define kzsAssert(test) (void)0

/** Terminates the application and traps the debugger if possible. Only effective in debug build. */
#define kzsErrorBreak() (void)0

/** Traps the debugger if possible. Only effective in debug build. */
#define kzsDebugBreak() (void)0

#endif

/** Return from the function with the given error code. */
#define kzsErrorThrow(new_error, logMessage) KZ_MULTILINE_MACRO \
    kzsErrorLog(new_error, logMessage);                         \
    kzsErrorBreak();                                            \
    /*lint -e{904}*/                                            \
    return new_error;                                           \
KZ_MULTILINE_MACRO_END

/** Return from the function with the given exception code. */
#define kzsExceptionThrow(new_exception, logMessage) KZ_MULTILINE_MACRO \
    /*lint -e{904}*/                                                    \
    kzsExceptionLog_private(logMessage);                                \
    return new_exception;                                               \
KZ_MULTILINE_MACRO_END

/** Return from the function with the given error code, if the test is false. Otherwise continue. */
#define kzsErrorTest(test, new_error, logMessage) KZ_MULTILINE_MACRO    \
    if (!(test))                                                        \
    {                                                                   \
        kzsErrorThrow(new_error, logMessage);                           \
    }                                                                   \
KZ_MULTILINE_MACRO_END

/** Return from the function with the given exception code, if the test is false. Otherwise continue. */
#define kzsExceptionTest(test, new_exception, logMessage) KZ_MULTILINE_MACRO    \
    if (!(test))                                                                \
    {                                                                           \
        /*lint -e{904}*/                                                        \
        kzsExceptionThrow((new_exception), logMessage);                         \
    }                                                                           \
KZ_MULTILINE_MACRO_END

/** Return from the function with the given error code, if it is one. Otherwise continue. */
#define kzsErrorForward(error) KZ_MULTILINE_MACRO                   \
    if ((error) != KZS_SUCCESS)                                     \
    {                                                               \
        if ((error) < KZS_SUCCESS)                                  \
        {                                                           \
            /*lint -e{904}*/                                        \
            kzsErrorThrow(error, "Unhandled exception occurred");   \
        }                                                           \
        else                                                        \
        {                                                           \
            /*lint -e{904}*/                                        \
            return error;                                           \
        }                                                           \
    }                                                               \
KZ_MULTILINE_MACRO_END

/** Return from the function with the given exception code, if it is one. Otherwise continue. */
#define kzsExceptionForward(exception) KZ_MULTILINE_MACRO   \
    if ((exception) != KZS_SUCCESS)                         \
    {                                                       \
        /*lint -e{904}*/                                    \
        return exception;                                   \
    }                                                       \
KZ_MULTILINE_MACRO_END

/**
 * If the given exception code is an error, pass it forward. If it is an exception, substitute it with the given error.
 * Otherwise continue. */
#define kzsExceptionForwardAsError(exception, new_error, logMessage) KZ_MULTILINE_MACRO \
    if ((exception) < KZS_SUCCESS)                                                      \
    {                                                                                   \
        kzsErrorThrow(new_error, logMessage);                                           \
    }                                                                                   \
    else if ((exception) > KZS_SUCCESS)                                                 \
    {                                                                                   \
        /*lint -e{904}*/                                                                \
        return (kzsError)(exception);                                                   \
    }                                                                                   \
KZ_MULTILINE_MACRO_END

/**
 * Return from the function with the given new error code,
 * if the given old code is also an error. Otherwise continue.
 */
#define kzsErrorSubstitute(error, new_error, logMessage) KZ_MULTILINE_MACRO \
    if ((error) != KZS_SUCCESS)                                             \
    {                                                                       \
        kzsErrorLog(new_error, logMessage);                                 \
        /*lint -e{904}*/                                                    \
        return new_error;                                                   \
    }                                                                       \
KZ_MULTILINE_MACRO_END

/**
 * Return from the function with the given new exception code,
 * if the given old code is also an exception. Otherwise continue.
 */
#define kzsExceptionSubstitute(error, new_exception, logMessage) KZ_MULTILINE_MACRO \
    if ((error) != KZS_SUCCESS)                                                     \
    {                                                                               \
        /*lint -e{904}*/                                                            \
        return new_exception;                                                       \
    }                                                                               \
KZ_MULTILINE_MACRO_END

/** Test if the given error code is an error */
#define kzsErrorIf(error) if ((error) != KZS_SUCCESS)

/** Test if the given exception code is an exception */
#define kzsExceptionCatch(exception, exception_code) if ((exception) == (exception_code))

/** Macro to use to indicate that some code is not yet implemented. */
/* #define KZS_NOT_IMPLEMENTED_YET kzsErrorLog(KZS_ERROR_NOT_IMPLEMENTED_YET, "Not implemented yet.") *//*lint --e{715,818}*/
#define KZS_NOT_IMPLEMENTED_YET
/** Macro to use to indicate and throw error that some code is not yet implemented. */
#define KZS_NOT_IMPLEMENTED_YET_ERROR kzsErrorThrow(KZS_ERROR_NOT_IMPLEMENTED_YET, "Not implemented yet.") /*lint --e{715,818}*/
/** Macro to use to indicate and throw error with an explanation, that some code is not yet implemented. */
#define KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT(message) kzsErrorThrow(KZS_ERROR_NOT_IMPLEMENTED_YET, message) /*lint --e{715,818}*/


/** Logs the given error code with given message. */
#define kzsErrorLog(errorCode, message) kzsErrorLog_private(errorCode, message, __FILE__, __LINE__)

/** \see kzsErrorLog */
void kzsErrorLog_private(kzsError errorCode, kzString message, kzString file, kzUint line);
/** \see kzsAssert */
void kzsAssert_private(void);
/** \see kzsErrorBreak */
void kzsErrorBreak_private(void);
/** \see kzsDebugBreak */
void kzsDebugBreak_private(void);
/** Exception log. */
void kzsExceptionLog_private(kzString message);
/** Throws exception message when forwarding error of type exception. */
kzsError kzsExceptionThrowMessage(kzsError errorCode);

/** Initializes error handling for thread. */
kzsError kzsErrorInitializeThread(void);
/** Denitializes error handling for thread. */
kzsError kzsErrorDeinitializeThread(void);


/** Initialize a global variable for error message storage. */
kzsError kzsErrorInitialize(void);
/** Initialize a global variable for error message storage. */
kzsError kzsErrorUninitialize(void);

/** Gets last error message or empty string if there is no error. */
kzString kzsErrorGetLastErrorMessage(void);


#endif
