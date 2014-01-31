/**
 * \file
 * CL Benchmark version info.
 * 
 * Copyright 2011 by Rightware. All rights reserved.
 */
#include "bf_version.h"


/* Major.minor.[-r|-rc|-alpha|-beta]build. E.g. 1.0-b3 (=third beta build). */
const kzString bfBenchmarkVersionString = "1.0.1-4";


kzString bfGetVersionString()
{
    return bfBenchmarkVersionString;

}
