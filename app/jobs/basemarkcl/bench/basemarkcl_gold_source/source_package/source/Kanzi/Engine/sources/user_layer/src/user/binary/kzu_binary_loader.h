/**
* \file
* Binary loader for .kzb format.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_BINARY_LOADER_H
#define KZU_BINARY_LOADER_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzcProject;
struct KzcInputStream;


/**
 * \struct KzuBinaryLoader
 * Binary loader structure.
 */
struct KzuBinaryLoader;


/**
 * Creates a new empty binary loader.
 */
kzsError kzuBinaryLoaderCreate(const struct KzcMemoryManager* memoryManager, struct KzuBinaryLoader** out_binaryLoader);

/**
 * Creates a new binary loader with the given configuration file.
 * First the configuration file is read as a resource to memory.
 * The configuration file must contain the paths of each binary file to load.
 * After the configuration is loaded, kzuBinaryDirectoryCreateFromFileResource is called for each file.
 */
kzsError kzuBinaryLoaderCreateFromConfig(const struct KzcMemoryManager* memoryManager, kzString configPath, 
                                         struct KzuBinaryLoader** out_binaryLoader);

/** Deletes a binary loader. */
kzsError kzuBinaryLoaderDelete(struct KzuBinaryLoader* binaryLoader);


/** Reads the directory structure of given file resource to memory and merges it with the current state. */
kzsError kzuBinaryLoaderAddBinaryFileResource(const struct KzuBinaryLoader* binaryLoader, kzString binaryPath);

/** Gets directory from binary loader. */
struct KzuBinaryDirectory* kzuBinaryLoaderGetDirectory(const struct KzuBinaryLoader* binaryLoader);


#endif
