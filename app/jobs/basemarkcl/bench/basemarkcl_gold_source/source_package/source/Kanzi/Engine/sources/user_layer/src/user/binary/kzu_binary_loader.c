/**
* \file
* Binary loader for .kzb format.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_binary_loader.h"

#include <user/binary/kzu_binary_directory.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_file.h>
#include <core/util/string/kzc_string.h>


/* Forward declarations */
struct KzuBinaryDirectory;


struct KzuBinaryLoader
{
    struct KzuBinaryDirectory* directory;
};


kzsError kzuBinaryLoaderCreate(const struct KzcMemoryManager* memoryManager, struct KzuBinaryLoader** out_binaryLoader)
{
    kzsError result;
    struct KzuBinaryLoader* binaryLoader;

    result = kzcMemoryAllocVariable(memoryManager, binaryLoader, "Binary loader");
    kzsErrorForward(result);

    result = kzuBinaryDirectoryCreateEmpty(memoryManager, &binaryLoader->directory);
    kzsErrorForward(result);

    *out_binaryLoader = binaryLoader;
    kzsSuccess();
}

kzsError kzuBinaryLoaderDelete(struct KzuBinaryLoader* binaryLoader)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(binaryLoader));

    result = kzuBinaryDirectoryDelete(binaryLoader->directory);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(binaryLoader);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuBinaryLoaderCreateFromConfig(const struct KzcMemoryManager* memoryManager, kzString configPath, struct KzuBinaryLoader** out_binaryLoader)
{
    kzsError result;
    struct KzuBinaryLoader* binaryLoader;
    kzUint lineCount;
    kzMutableString* lines;
    kzUint i;
    
    result = kzuBinaryLoaderCreate(memoryManager, &binaryLoader);
    kzsErrorForward(result);

    result = kzcFileReadTextResourceLines(memoryManager, configPath, &lines);
    kzsErrorForward(result);

    /* First the core assets are needed to be merged (if wanted to be overridden later). */
    {
        struct KzuBinaryDirectory* coreAssetsDirectory;

        result = kzuBinaryDirectoryCreateCoreAssets(memoryManager, &coreAssetsDirectory);
        kzsErrorForward(result);
        result = kzuBinaryDirectoryMerge(binaryLoader->directory, coreAssetsDirectory, KZ_NULL);
        kzsErrorForward(result);
    }

    lineCount = kzcArrayLength(lines);
    for (i = 0; i < lineCount; ++i)
    {
        kzMutableString binaryPath = lines[i];
        struct KzuBinaryDirectory* tempDirectory;

        if (kzcStringLength(binaryPath) > 0)
        {
            result = kzuBinaryDirectoryCreateFromFileResource(memoryManager, binaryPath, &tempDirectory);
            kzsErrorForward(result);

            result = kzuBinaryDirectoryMerge(binaryLoader->directory, tempDirectory, KZ_NULL);
            kzsErrorForward(result);
        }

        result = kzcStringDelete(binaryPath);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeArray(lines);
    kzsErrorForward(result);

    *out_binaryLoader = binaryLoader;
    kzsSuccess();
}

struct KzuBinaryDirectory* kzuBinaryLoaderGetDirectory(const struct KzuBinaryLoader* binaryLoader)
{
    kzsAssert(kzcIsValidPointer(binaryLoader));
    return binaryLoader->directory;
}

kzsError kzuBinaryLoaderAddBinaryFileResource(const struct KzuBinaryLoader* binaryLoader, kzString binaryPath)
{
    kzsError result;
    struct KzuBinaryDirectory* tempDirectory;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(binaryLoader);

    result = kzuBinaryDirectoryCreateFromFileResource(memoryManager, binaryPath, &tempDirectory);
    kzsErrorForward(result);

    result = kzuBinaryDirectoryMerge(binaryLoader->directory, tempDirectory, KZ_NULL);
    kzsErrorForward(result);

    kzsSuccess();
}
