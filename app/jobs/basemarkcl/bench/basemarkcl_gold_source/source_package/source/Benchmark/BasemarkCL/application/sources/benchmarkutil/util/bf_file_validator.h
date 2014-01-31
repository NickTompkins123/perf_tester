/*
* OpenCL benchmark file hash validator
*
* Copyright 2011 by Rightware. All rights reserved.
*/

#ifndef BENCHMARK_FILE_VALIDATOR_H
#define BENCHMARK_FILE_VALIDATOR_H


/** Checks files md5 hash against given md5 hash. */
kzsError bfValidateFile(struct BenchmarkFramework* framework, kzString path, kzString hash, kzBool *is_valid);


#endif
