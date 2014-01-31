/**
 * \file
 * System header.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZS_SYSTEM_H
#define KZS_SYSTEM_H


/* System */
#include <system/debug/kzs_error.h>


/** Initializes the system. */
kzsError kzsSystemInitialize(void);

/** Uninitializes the system. */
kzsError kzsSystemUninitialize(void);

/** Enables the reporting of leaked memory when uninitializing the application. */
void kzsSystemSetMemoryLeakReportEnabled(kzBool enabled);

/** Checks if the reporting of leaked memory when uninitializing the application is enabled. */
kzBool kzsSystemIsMemoryLeakReportEnabled(void);

/** Checks if the platform is big endian. */
kzBool kzsIsBigEndian(void);

/** Converts a 32-bit floating point number to 4 bytes in little-endian format. */
void kzsFloatToBytesLittleEndian(kzFloat floatValue, kzByte* out_floatBytes);

/** Converts a 32-bit floating point number to 4 bytes in big-endian format. */
void kzsFloatToBytesBigEndian(kzFloat floatValue, kzByte* out_floatBytes);

/** Converts 4 bytes of floating point value from little-endian format to a 32-bit floating point number. */
kzFloat kzsBytesToFloatLittleEndian(const kzByte* floatBytes);

/** Converts 4 bytes of floating point value from big-endian format to a 32-bit floating point number. */
kzFloat kzsBytesToFloatBigEndian(const kzByte* floatBytes);


#endif
