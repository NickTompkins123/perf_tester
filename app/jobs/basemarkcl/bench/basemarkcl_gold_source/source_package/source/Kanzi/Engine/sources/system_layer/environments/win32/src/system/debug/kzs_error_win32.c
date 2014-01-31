/**
 * \file
 * Error.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzs_error_win32.h"

#include <system/debug/kzs_error.h>

#include <windows.h>
#include <intrin.h>
#include <crtdbg.h> /*lint -efile(766,crtdbg.h) This file is referenced inside macros. */
#include <assert.h> /*lint -efile(766,assert.h) This file is referenced inside macros. */


static kzBool kzsErrorBreakEnabled = KZ_TRUE;


void kzsErrorSetBreakEnabled(kzBool enabled)
{
    kzsErrorBreakEnabled = enabled;
}

void kzsAssert_private(void)
{
    if (kzsErrorBreakEnabled)
    {
         _CrtDbgBreak();
    }
}

void kzsErrorBreak_private(void)
{
    if (kzsErrorBreakEnabled)
    {
         _CrtDbgBreak();
    }
}

void kzsDebugBreak_private(void)
{
    if (IsDebuggerPresent() && kzsErrorBreakEnabled)
    {
        _CrtDbgBreak();
    }
}
