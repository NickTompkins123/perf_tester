/**
* \file
* Binary directory for .kzb format.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_binary_directory.h"

#include "kzu_binary_util.h"

#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/collection/kzc_hash_set.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/util/string/kzc_string.h>

#include <core/debug/kzc_log.h>

#include <system/debug/kzs_counter.h>
#include <system/wrappers/kzs_string.h>
#include <system/kzs_system.h>
#include <system/time/kzs_tick.h>


extern const kzByte kzuCoreAssetData[];
extern const kzUint kzuCoreAssetDataSize;


#define KZU_BINARY_DIRECTORY_SHORTCUTS_PATH "Shortcuts"
#define KZU_BINARY_DIRECTORY_PROPERTY_TYPE_LIBRARY_PATH "PropertyTypeLibrary/"
#define KZU_BINARY_DIRECTORY_ANIMATION_LIBRARY_PATH "Animation Data/"
#define KZU_BINARY_DIRECTORY_MESH_LIBRARY_PATH "Mesh Data/"

#define KZU_BINARY_DIRECTORY_FLAG_LITTLE_ENDIAN 1

/** Type of a binary source. */
enum KzuBinarySourceType {KZU_BINARY_SOURCE_TYPE_RESOURCE, KZU_BINARY_SOURCE_TYPE_MEMORY};


struct KzuBinarySource
{
    enum KzuBinarySourceType type; /**< Type of the source. */
    enum KzcIOStreamEndianness endianness; /**< Endianness of the source. */
    union
    {
        kzMutableString resourcePath; /**< Path for a resource file. Used if type == KZU_BINARY_SOURCE_TYPE_RESOURCE. */
        struct
        {
            kzUint size; /**< Size of the buffer. */
            const kzByte* buffer; /**< Memory buffer. */
        } memory; /**< Memory buffer. Used if type == KZU_BINARY_SOURCE_TYPE_MEMORY.*/
    } dataSource; /**< Data source of the binary source object. */
    kzMutableString* references; /**< Array of file names, which can be referenced by array index. */
    /* TODO: Optimize reading by leaving input stream open for faster sequential file reading. */
};

struct KzuBinaryFileInfo
{
    kzMutableString name; /**< Short name of the file. */
    struct KzuBinarySource* source; /**< Binary source which is used for reading the content of the file. */
    kzUint offset; /**< Offset in the binary source where the file is located. */
    kzUint size; /**< Size of the file. */
    enum KzuBinaryFileType type; /**< Type of the file. */
    kzString path; /**< Full path of the file (including file name). */
    kzU32 flags; /**< File flags. */
};

struct KzuBinaryFolderInfo
{
    kzMutableString name; /**< Short name of the folder. */
    struct KzcHashMap* subFolders; /**< Sub folders inside the folder. <kzString, KzuBinaryFolderInfo>. */
    struct KzcHashMap* files; /**< Files inside the folder. <kzString, KzuBinaryFileInfo>. */
};

struct KzuBinaryDirectoryLoading
{
    kzUint fileTime; /**< Time for files. */
    kzUint folderTime; /**< Time for folders. */
    kzUint externalFileTime; /**< Time for external files. */
    kzUint deletedFilesTime; /**< Time for deleted files */
    kzUint shortcutsTime; /**< Directory shortcuts time. */
    kzUint shortcutsTimeFileOpen; /**< Shortcuts time fopen. */
    kzUint shortcutsTimeReadReference; /**< Shortcuts time reference. */
    kzUint headerTime; /**< Directory header time. */
};

struct KzuBinaryDirectory
{
    struct KzuBinaryFolderInfo* rootFolder; /**< Root folder of the directory. */
    /* Enable the following two lines if/when there is use for the tool version or the flags. */
    /*kzU16 toolVersion[4]; *//**< Tool version used for creating the binary. */
    kzU32 flags; /**< General flags for the binary. See KZU_BINARY_DIRECTORY_FLAG_*. */
    struct KzcHashSet* sources; /**< All binary sources of this directory. <kzuBinarySource>. */
    struct KzcHashSet* deletedFiles; /**< Files marked as deleted. <kzMutableString>. */
    struct KzcHashMap* shortcuts; /** Shortcut file path mapping. <kzString, KzString>. */

    struct KzuBinaryFolderInfo* objectsFolder; /**< Folder containing objects. */
    struct KzuBinaryFolderInfo* propertyTypeFolder; /**< Folder containing property types. */
    struct KzuBinaryFolderInfo* animationsFolder; /**< Folder containing animations. */
    struct KzuBinaryFolderInfo* meshFolder; /**< Folder containing meshes. */
    kzU32 fileFlagMask; /**< Mask for file flags. When reading files, final flags used will be given by formula file.flag & directory.flagMask. */

    struct KzuBinaryDirectoryLoading loadingMeasurement; /**< Loading measurement for binary directory. */
};


/* Binary source */

/** Creates a binary source. KzuBinarySource.source remains uninitialized and must be initialized by the caller. */
static kzsError kzuBinarySourceCreate_internal(const struct KzcMemoryManager* memoryManager, enum KzuBinarySourceType type,
                                               struct KzuBinarySource** out_source)
{
    kzsError result;
    struct KzuBinarySource* source;
    
    result = kzcMemoryAllocVariable(memoryManager, source, "Binary directory binary source");
    kzsErrorForward(result);
    
    source->type = type;
    source->endianness = KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED;
    source->references = KZ_NULL; /* This will be initialized when loading directory. */

    *out_source = source;
    kzsSuccess();
}

/** Deletes a binary source. */
static kzsError kzuBinarySourceDelete_internal(struct KzuBinarySource* source)
{
    kzsError result;
    kzMutableString* references = source->references;
    kzUint referenceCount = kzcArrayLength(references);
    kzUint i;

    kzsAssert(kzcIsValidPointer(source));

    /* First item is NULL. */
    for (i = 1; i < referenceCount; ++i)
    {
        result = kzcStringDelete(references[i]);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeArray(references);
    kzsErrorForward(result);

    switch (source->type)
    {
        case KZU_BINARY_SOURCE_TYPE_RESOURCE:
        {
            result = kzcStringDelete(source->dataSource.resourcePath);
            kzsErrorForward(result);

            break;
        }

        case KZU_BINARY_SOURCE_TYPE_MEMORY:
        {
            break;
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid binary directory source type");
        }
    }
    result = kzcMemoryFreeVariable(source);
    kzsErrorForward(result);

    kzsSuccess();
}

/** Opens an input stream for reading the given binary source. */
static kzsError kzuBinarySourceCreateInputStream_internal(const struct KzcMemoryManager* memoryManager,
                                                          const struct KzuBinarySource* source,
                                                          enum KzcIOStreamEndianness endianness,
                                                          struct KzcInputStream** out_inputStream)
{
    kzsError result;
    struct KzcInputStream* inputStream;

    kzsAssert(kzcIsValidPointer(source));

    switch (source->type)
    {
        case KZU_BINARY_SOURCE_TYPE_MEMORY:
        {
            result = kzcInputStreamCreateFromMemory(memoryManager, source->dataSource.memory.buffer, source->dataSource.memory.size,
                                                    endianness == KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED ? source->endianness : endianness,
                                                    &inputStream);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_SOURCE_TYPE_RESOURCE:
        {
            result = kzcInputStreamCreateFromResource(memoryManager, source->dataSource.resourcePath,
                                                      endianness == KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED ? source->endianness : endianness,
                                                      &inputStream);
            kzsErrorForward(result);
            break;
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid binary directory source type");
        }
    }

    /* FIXME: This should not be enabled until it's fast enough (current implementation takes near twice as long with this enabled on QNX tegra2 for example. */
    /*result = kzcInputStreamInitBuffer(inputStream, KZC_INPUT_STREAM_BUFFER_DEFAULT_SIZE);
    kzsErrorForward(result);*/

    *out_inputStream = inputStream;
    kzsSuccess();
}

kzString kzuBinarySourceGetReferencedFilePath(const struct KzuBinarySource* source, kzUint index)
{
    kzsAssert(kzcIsValidPointer(source));
    kzsAssert(kzcIsValidArrayIndex(source->references, index));

    return source->references[index];
}


/* Binary file info */

/** Creates a new file info. */
static kzsError kzuBinaryFileInfoCreate_internal(const struct KzcMemoryManager* memoryManager, kzMutableString name,
                                                 enum KzuBinaryFileType type, struct KzuBinarySource* source,
                                                 kzUint offset, kzUint size, kzString path, kzU32 flags,
                                                 struct KzuBinaryFileInfo** out_file)
{
    kzsError result;
    struct KzuBinaryFileInfo* file;

    kzsAssert(kzcIsValidPointer(name));

    result = kzcMemoryAllocVariable(memoryManager, file, "Binary directory file");
    kzsErrorForward(result);

    file->name = name;
    file->type = type;
    file->source = source;
    file->offset = offset;
    file->size = size;
    file->path = path;
    file->flags = flags;

    *out_file = file;
    kzsSuccess();
}

/** Deletes a file info. */
static kzsError kzuBinaryFileInfoDelete_internal(struct KzuBinaryFileInfo* file)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(file));

    if (file->name != KZ_NULL) /* Can be null while merging directories. */
    {
        result = kzcStringDelete(file->name);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(file);
    kzsErrorForward(result);

    kzsSuccess();
}

kzString kzuBinaryFileInfoGetName(const struct KzuBinaryFileInfo* file)
{
    kzsAssert(kzcIsValidPointer(file));

    return file->name;
}

struct KzuBinarySource* kzuBinaryFileInfoGetSource(const struct KzuBinaryFileInfo* file)
{
    kzsAssert(kzcIsValidPointer(file));

    return file->source;
}

kzUint kzuBinaryFileInfoGetSize(const struct KzuBinaryFileInfo* file)
{
    kzsAssert(kzcIsValidPointer(file));

    return file->size;
}

enum KzuBinaryFileType kzuBinaryFileInfoGetType(const struct KzuBinaryFileInfo* file)
{
    kzsAssert(kzcIsValidPointer(file));

    return file->type;
}

kzString kzuBinaryFileInfoGetPath(const struct KzuBinaryFileInfo* file)
{
    kzsAssert(kzcIsValidPointer(file));

    return file->path;
}

kzU32 kzuBinaryFileInfoGetFlags(const struct KzuBinaryFileInfo* file)
{
    kzsAssert(kzcIsValidPointer(file));

    return file->flags;
}

/* Binary folder info */

/** Creates a new folder info. */
static kzsError kzuBinaryFolderInfoCreate_internal(const struct KzcMemoryManager* memoryManager, kzMutableString name,
                                                   struct KzuBinaryFolderInfo** out_folder)
{
    kzsError result;
    struct KzuBinaryFolderInfo* folder;

    result = kzcMemoryAllocVariable(memoryManager, folder, "Binary directory folder");
    kzsErrorForward(result);

    result = kzcHashMapCreateOrdered(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &folder->subFolders);
    kzsErrorForward(result);

    result = kzcHashMapCreateOrdered(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &folder->files);
    kzsErrorForward(result);

    folder->name = name;

    *out_folder = folder;
    kzsSuccess();
}

/** Deletes a folder info recursively with all sub folders and files in it. */
static kzsError kzuBinaryFolderInfoDelete_internal(struct KzuBinaryFolderInfo* folder)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(folder));

    /* Sub folders */
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(folder->subFolders);
        while (kzcHashMapIterate(it))
        {
            result = kzuBinaryFolderInfoDelete_internal((struct KzuBinaryFolderInfo*)kzcHashMapIteratorGetValue(it));
            kzsErrorForward(result);
        }

        result = kzcHashMapDelete(folder->subFolders);
        kzsErrorForward(result);
    }

    /* Files */
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(folder->files);
        while (kzcHashMapIterate(it))
        {
            result = kzuBinaryFileInfoDelete_internal((struct KzuBinaryFileInfo*)kzcHashMapIteratorGetValue(it));
            kzsErrorForward(result);
        }

        result = kzcHashMapDelete(folder->files);
        kzsErrorForward(result);
    }

    if (folder->name != KZ_NULL)
    {
        result = kzcStringDelete(folder->name);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(folder);
    kzsErrorForward(result);

    kzsSuccess();
}

kzString kzuBinaryFolderInfoGetName(const struct KzuBinaryFolderInfo* folder)
{
    kzsAssert(kzcIsValidPointer(folder));

    return folder->name;
}

struct KzcHashMap* kzuBinaryFolderInfoGetSubFolders(const struct KzuBinaryFolderInfo* folder)
{
    kzsAssert(kzcIsValidPointer(folder));

    return folder->subFolders;
}

struct KzcHashMap* kzuBinaryFolderInfoGetFiles(const struct KzuBinaryFolderInfo* folder)
{
    kzsAssert(kzcIsValidPointer(folder));

    return folder->files;
}

kzsError kzuBinaryFolderInfoGetAllFiles(const struct KzuBinaryFolderInfo* folder, struct KzcDynamicArray* files)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(folder));

    /* Sub-folders */
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(folder->subFolders);
        while (kzcHashMapIterate(it))
        {
            struct KzuBinaryFolderInfo* subFolder = (struct KzuBinaryFolderInfo*)kzcHashMapIteratorGetValue(it);

            /* Recursively handle sub-folder */
            result = kzuBinaryFolderInfoGetAllFiles(subFolder, files);
            kzsErrorForward(result);
        }
    }
    
    /* Files */
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(folder->files);
        while (kzcHashMapIterate(it))
        {
            struct KzuBinaryFileInfo* file = (struct KzuBinaryFileInfo*)kzcHashMapIteratorGetValue(it);

            kzsAssert(kzcIsValidPointer(file));

            result = kzcDynamicArrayAdd(files, file);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

/* Binary directory */

/** Creates a directory. */
static kzsError kzuBinaryDirectoryCreate_internal(const struct KzcMemoryManager* memoryManager, struct KzuBinaryDirectory** out_directory)
{
    kzsError result;
    struct KzuBinaryDirectory* directory;
    struct KzcHashSet* sources;
    struct KzcHashSet* deletedFiles;
    struct KzcHashMap* shortcuts;

    result = kzcMemoryAllocVariable(memoryManager, directory, "Binary directory");
    kzsErrorForward(result);

    result = kzcHashSetCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &sources);
    kzsErrorForward(result);

    result = kzcHashSetCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &deletedFiles);
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &shortcuts);
    kzsErrorForward(result);

    directory->fileFlagMask = KZU_BINARY_DIRECTORY_DEFAULT_FILE_FLAG_MASK;

    directory->rootFolder = KZ_NULL;
    directory->deletedFiles = deletedFiles;
    directory->sources = sources;
    directory->shortcuts = shortcuts;

    directory->objectsFolder = KZ_NULL;
    directory->propertyTypeFolder = KZ_NULL;
    directory->animationsFolder = KZ_NULL;
    directory->meshFolder = KZ_NULL;

    directory->loadingMeasurement.fileTime = 0;
    directory->loadingMeasurement.folderTime = 0;
    directory->loadingMeasurement.shortcutsTimeFileOpen = 0;
    directory->loadingMeasurement.shortcutsTimeReadReference = 0;

    *out_directory = directory;
    kzsSuccess();
}

kzsError kzuBinaryDirectoryCreateEmpty(const struct KzcMemoryManager* memoryManager, struct KzuBinaryDirectory** out_directory)
{
    kzsError result;
    struct KzuBinaryDirectory* directory;
    kzMutableString rootName;
    struct KzuBinaryFolderInfo* rootFolder;

    result = kzcStringCopy(memoryManager, "", &rootName);
    kzsErrorForward(result);

    result = kzuBinaryDirectoryCreate_internal(memoryManager, &directory);
    kzsErrorForward(result);

    result = kzuBinaryFolderInfoCreate_internal(memoryManager, rootName, &rootFolder);
    kzsErrorForward(result);

    directory->rootFolder = rootFolder;

    *out_directory = directory;
    kzsSuccess();
}

/** Creates a copy of the source string, where each occurence of slash (/) is replaced with double slash (//). */
static kzsError kzuBinaryNameEscape_internal(const struct KzcMemoryManager* memoryManager, kzString source, kzMutableString* out_target)
{
    kzsError result;
    kzMutableString target;

    kzsAssert(source != KZ_NULL);

    result = kzcStringReplace(memoryManager, source, "/", "//", &target);
    kzsErrorForward(result);

    *out_target = target;
    kzsSuccess();
}

/** Reads a file object from the given input stream. This function also adds the file path to source->references. */
static kzsError kzuBinaryDirectoryReadFile_internal(const struct KzcMemoryManager* memoryManager,
                                                    struct KzcInputStream* inputStream,
                                                    struct KzuBinarySource* source, kzString parentPath,
                                                    kzUint* currentReferenceIndex, struct KzuBinaryFileInfo** out_file)
{
    kzsError result;
    struct KzuBinaryFileInfo* file;

    kzUint offset;
    kzUint size;
    enum KzuBinaryFileType type;
    kzU32 flags;
    kzMutableString name;
    kzMutableString filePath;

    result = kzcInputStreamReadU32Int(inputStream, &offset);
    kzsErrorForward(result);

    result = kzcInputStreamReadU32Int(inputStream, &size);
    kzsErrorForward(result);

    result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&type);
    kzsErrorForward(result);

    result = kzcInputStreamReadU32(inputStream, &flags);
    kzsErrorForward(result);

    result = kzuBinaryReadString(memoryManager, inputStream, &name);
    kzsErrorForward(result);

    /* If the file is in root folder, filePath == name. */
    if (parentPath == KZ_NULL)
    {
        result = kzcStringCopy(memoryManager, name, &filePath);
        kzsErrorForward(result);
    }
    /* Otherwise filePath == parentPath + name. Parent path already ends with slash (/). */
    else
    {
        kzMutableString escapedName;

        result = kzuBinaryNameEscape_internal(memoryManager, name, &escapedName);
        kzsErrorForward(result);

        result = kzcStringConcatenate(memoryManager, parentPath, escapedName, &filePath);
        kzsErrorForward(result);

        result = kzcStringDelete(escapedName);
        kzsErrorForward(result);
    }

    result = kzuBinaryFileInfoCreate_internal(memoryManager, name, type, source, offset, size, filePath, flags, &file);
    kzsErrorForward(result);

    source->references[(*currentReferenceIndex)++] = filePath;

    *out_file = file;
    kzsSuccess();
}

/** Reads a folder object from the given input stream. The folder is read recursively together with all sub folders and files in it. */
static kzsError kzuBinaryDirectoryReadFolder_internal(const struct KzcMemoryManager* memoryManager, struct KzuBinaryDirectory* directory,
                                                      struct KzcInputStream* inputStream,
                                                      struct KzuBinarySource* source, kzString parentPath, kzBool versionMatches,
                                                      kzUint* currentReferenceIndex, struct KzuBinaryFolderInfo** out_folder)
{
    kzsError result;
    struct KzuBinaryFolderInfo* folder;
    kzUint stamp;

    kzUint size;
    kzMutableString name;
    kzMutableString folderPath;

    kzUint startPosition = kzcInputStreamGetPosition(inputStream);
    kzUint relativePosition;
    kzUint numUnreadBytes;

    stamp = kzsTimeGetCurrentTimestamp();

    result = kzcInputStreamReadU32Int(inputStream, &size);
    kzsErrorForward(result);

    result = kzuBinaryReadString(memoryManager, inputStream, &name);
    kzsErrorForward(result);

    /* Full path of this folder is either empty string (for root folder) or <parentpath><foldername>/. */
    if (parentPath == KZ_NULL)
    {
        result = kzcStringCopy(memoryManager, "", &folderPath);
        kzsErrorForward(result);
    }
    else
    {
        kzMutableString escapedName;

        result = kzuBinaryNameEscape_internal(memoryManager, name, &escapedName);
        kzsErrorForward(result);

        result = kzcStringConcatenateMultiple(memoryManager, 3, &folderPath, parentPath, escapedName, "/");
        kzsErrorForward(result);

        result = kzcStringDelete(escapedName);
        kzsErrorForward(result);
    }

    result = kzuBinaryFolderInfoCreate_internal(memoryManager, name, &folder);
    kzsErrorForward(result);

    /* Sub folders */
    {
        kzUint subFolderCount;
        kzUint i;

        result = kzcInputStreamReadU32Int(inputStream, &subFolderCount);
        kzsErrorForward(result);

        directory->loadingMeasurement.folderTime += (kzsTimeGetCurrentTimestamp() - stamp);

        for (i = 0; i < subFolderCount; ++i)
        {
            struct KzuBinaryFolderInfo* subFolder;

            result = kzuBinaryDirectoryReadFolder_internal(memoryManager, directory, inputStream, source, folderPath, versionMatches,
                                                           currentReferenceIndex, &subFolder);
            kzsErrorForward(result);

            kzsAssert(!kzcHashMapContains(folder->subFolders, subFolder->name));

            result = kzcHashMapPut(folder->subFolders, subFolder->name, subFolder);
            kzsErrorForward(result);
        }
    }

    stamp = kzsTimeGetCurrentTimestamp();
    /* Files */
    {
        kzUint fileCount;
        kzUint i;

        result = kzcInputStreamReadU32Int(inputStream, &fileCount);
        kzsErrorForward(result);

        for (i = 0; i < fileCount; ++i)
        {
            struct KzuBinaryFileInfo* file;

            result = kzuBinaryDirectoryReadFile_internal(memoryManager, inputStream, source, folderPath, currentReferenceIndex, &file);
            kzsErrorForward(result);

            if (kzcHashMapContains(folder->files, file->name))
            {
                result = kzcLog(memoryManager, KZS_LOG_LEVEL_WARNING, "File with name %s already exists in hash map", file->name);
                kzsErrorForward(result);
                kzsAssert(KZ_FALSE);
            }

            result = kzcHashMapPut(folder->files, file->name, file);
            kzsErrorForward(result);
        }
    }

    result = kzcStringDelete(folderPath);
    kzsErrorForward(result);
    directory->loadingMeasurement.fileTime += (kzsTimeGetCurrentTimestamp() - stamp);

    /* Skip unread bytes in the folder, if there are any. This is to help forward compatibility of the binary format. */
    relativePosition = kzcInputStreamGetPosition(inputStream) - startPosition;
    if (versionMatches)
    {
        kzsErrorTest(relativePosition == size, KZU_ERROR_INVALID_BINARY_DATA, "Folder size was different than expected");
    }
    else
    {
        kzsErrorTest(relativePosition <= size, KZU_ERROR_INVALID_BINARY_DATA, "Folder size was smaller than expected");
    }

    numUnreadBytes = size - relativePosition;
    result = kzcInputStreamSkip(inputStream, numUnreadBytes);
    kzsErrorForward(result);

    *out_folder = folder;
    kzsSuccess();
}

static kzsError kzuBinaryDirectoryReadDirectory_internal(const struct KzcMemoryManager* memoryManager, struct KzuBinaryDirectory* directory,
                                                         struct KzcInputStream* inputStream, struct KzuBinarySource* source, kzBool versionMatches)
{
    kzsError result;
    kzUint stamp;

    /* Read directory */
    struct
    {
        kzUint directorySize;
        kzUint totalFileCount;
    } directoryHeader;
    kzUint startPosition = kzcInputStreamGetPosition(inputStream);
    kzUint relativePosition;
    kzUint unreadByteCount;
    kzUint currentReferenceIndex;

    result = kzcInputStreamReadU32Int(inputStream, &directoryHeader.directorySize);
    kzsErrorForward(result);

    result = kzcInputStreamReadU32Int(inputStream, &directoryHeader.totalFileCount);
    kzsErrorForward(result);

    result = kzcMemoryAllocArray(memoryManager, source->references, directoryHeader.totalFileCount + 1, "Binary source references array");
    kzsErrorForward(result);
    source->references[0] = KZ_NULL; /* First element is always NULL because reference with value 0 is specified as null reference. */
    currentReferenceIndex = 1;

    result = kzuBinaryDirectoryReadFolder_internal(memoryManager, directory, inputStream, source, KZ_NULL, versionMatches, &currentReferenceIndex, &directory->rootFolder);
    kzsErrorForward(result);

    stamp = kzsTimeGetCurrentTimestamp();
    /* External files */
    {
        kzUint externalFileCount;
        kzUint i;
        result = kzcInputStreamReadU32Int(inputStream, &externalFileCount);
        kzsErrorForward(result);

        for (i = 0; i <externalFileCount; ++i)
        {
            kzMutableString filePath;
            result = kzuBinaryReadString(memoryManager, inputStream, &filePath);
            kzsErrorForward(result);

            source->references[currentReferenceIndex++] = filePath;
        }
    }
    directory->loadingMeasurement.externalFileTime = (kzsTimeGetCurrentTimestamp() - stamp);

    stamp = kzsTimeGetCurrentTimestamp();
    /* Deleted files */
    {
        kzUint deletedFileCount;
        kzUint i;
        result = kzcInputStreamReadU32Int(inputStream, &deletedFileCount);
        kzsErrorForward(result);

        for (i = 0; i < deletedFileCount; ++i)
        {
            kzMutableString filePath;

            result = kzuBinaryReadString(memoryManager, inputStream, &filePath);
            kzsErrorForward(result);

            result = kzcHashSetAdd(directory->deletedFiles, filePath);
            kzsErrorForward(result);
        }
    }
    directory->loadingMeasurement.deletedFilesTime = (kzsTimeGetCurrentTimestamp() - stamp);

    /* Skip unread bytes in the directory, if there are any. This is to help forward compatibility of the binary format. */
    relativePosition = kzcInputStreamGetPosition(inputStream) - startPosition;
    kzsErrorTest(relativePosition <= directoryHeader.directorySize, KZU_ERROR_INVALID_BINARY_DATA, "Directory size was smaller than expected");

    unreadByteCount = directoryHeader.directorySize - relativePosition;
    result = kzcInputStreamSkip(inputStream, unreadByteCount);
    kzsErrorForward(result);

    kzsSuccess();
}

/* Reads a directory from the given input stream. This includes whole folder and file hierarchy inside it. */
static kzsError kzuBinaryDirectoryRead_internal(const struct KzcMemoryManager* memoryManager, struct KzuBinaryDirectory* directory,
                                                struct KzcInputStream* inputStream, struct KzuBinarySource* source)
{
    kzsError result;
    kzUint stamp;

    struct
    {
        kzU8 magicNumber[4];
        kzUint fileSize;
        kzUint headerSize;
        kzUint fileFormatVersion;
        kzU16 toolVersion[4];
        kzU32 flags;
        kzU8 licenseId[16];
    } kzbHeader;
    kzBool versionMatches = KZ_FALSE; /* Indicates that the binary file version matches exactly the one supported by the engine. */

    stamp = kzsTimeGetCurrentTimestamp();
    /* Read kzb header */
    {
        kzUint relativePosition;
        kzUint numUnreadBytes;
        kzUint i;

        for (i = 0; i < 4; ++i)
        {
            result = kzcInputStreamReadU8(inputStream, &kzbHeader.magicNumber[i]);
            kzsErrorForward(result);

            kzsErrorTest(kzbHeader.magicNumber[i] == (kzU8)".KZB"[i], KZU_ERROR_INVALID_BINARY_DATA, "Binary file is corrupted");
        }

        result = kzcInputStreamReadU32Int(inputStream, &kzbHeader.fileSize);
        kzsErrorForward(result);

        result = kzcInputStreamReadU32Int(inputStream, &kzbHeader.headerSize);
        kzsErrorForward(result);

        result = kzcInputStreamReadU32Int(inputStream, &kzbHeader.fileFormatVersion);
        kzsErrorForward(result);

        if (kzbHeader.fileFormatVersion < KZU_BINARY_FORMAT_VERSION)
        {
            result = kzcLog(memoryManager, KZS_LOG_LEVEL_WARNING, "Binary file format version (%i) is older than currently supported (%i).", kzbHeader.fileFormatVersion, KZU_BINARY_FORMAT_VERSION);
            kzsErrorForward(result);
        }
        else if (kzbHeader.fileFormatVersion > KZU_BINARY_FORMAT_VERSION)
        {
            result = kzcLog(memoryManager, KZS_LOG_LEVEL_WARNING, "Binary file format version (%i) is newer than currently supported (%i).", kzbHeader.fileFormatVersion, KZU_BINARY_FORMAT_VERSION);
            kzsErrorForward(result);
        }
        else
        {
            versionMatches = KZ_TRUE;
        }

        for (i = 0; i < 4; ++i)
        {
            result = kzcInputStreamReadU16(inputStream, &kzbHeader.toolVersion[i]);
            kzsErrorForward(result);
        }

        result = kzcInputStreamReadU32(inputStream, &kzbHeader.flags);
        kzsErrorForward(result);
        
        directory->flags = kzbHeader.flags;

        result = kzcInputStreamReadBytes(inputStream, 16, kzbHeader.licenseId);
        kzsErrorForward(result);

        /* Skip unread bytes in the header, if there are any. This is to help forward compatibility of the binary format. */
        relativePosition = kzcInputStreamGetPosition(inputStream);
        kzsErrorTest(relativePosition <= kzbHeader.headerSize, KZU_ERROR_INVALID_BINARY_DATA, "Header size was smaller than expected");

        numUnreadBytes = kzbHeader.headerSize - relativePosition;
        result = kzcInputStreamSkip(inputStream, numUnreadBytes);
        kzsErrorForward(result);
    }

    {
        kzBool littleEndian = (directory->flags & KZU_BINARY_DIRECTORY_FLAG_LITTLE_ENDIAN) == KZU_BINARY_DIRECTORY_FLAG_LITTLE_ENDIAN;
        enum KzcIOStreamEndianness streamEndianness = littleEndian ? KZC_IO_STREAM_ENDIANNESS_LITTLE_ENDIAN : KZC_IO_STREAM_ENDIANNESS_BIG_ENDIAN;
        enum KzcIOStreamEndianness platformEndianness = kzsIsBigEndian() ? KZC_IO_STREAM_ENDIANNESS_BIG_ENDIAN : KZC_IO_STREAM_ENDIANNESS_LITTLE_ENDIAN;

        if (streamEndianness == platformEndianness)
        {
            streamEndianness = KZC_IO_STREAM_ENDIANNESS_PLATFORM;
        }

        source->endianness = streamEndianness;
        
        directory->loadingMeasurement.headerTime = (kzsTimeGetCurrentTimestamp() - stamp);

        result = kzuBinaryDirectoryReadDirectory_internal(memoryManager, directory, inputStream, source, versionMatches);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

/** Creates a binary directory from the given binary source. */
static kzsError kzuBinaryDirectoryCreateFromSource_internal(const struct KzcMemoryManager* memoryManager, struct KzuBinarySource* source,
                                                            struct KzuBinaryDirectory** out_directory)
{
    kzsException result;
    kzUint stamp;
    struct KzuBinaryDirectory* directory;

    /* Read directory. */
    {
        struct KzcInputStream* inputStream;

        result = kzuBinaryDirectoryCreate_internal(memoryManager, &directory);
        kzsErrorForward(result);

        result = kzcHashSetAdd(directory->sources, source);
        kzsErrorForward(result);

        result = kzuBinarySourceCreateInputStream_internal(memoryManager, source, KZC_IO_STREAM_ENDIANNESS_BIG_ENDIAN, &inputStream);
        kzsErrorForward(result);

        result = kzuBinaryDirectoryRead_internal(memoryManager, directory, inputStream, source);
        kzsErrorForward(result);

        result = kzcInputStreamDelete(inputStream);
        kzsErrorForward(result);
    }

    /* Read all shortcuts. */
    stamp = kzsTimeGetCurrentTimestamp();
    {
        struct KzuBinaryFolderInfo* rootFolder = directory->rootFolder;
        struct KzuBinaryFolderInfo* shortcutsFolder;
        kzUint stamp2; /* TODO: Rename */

        if (kzcHashMapGet(rootFolder->subFolders, KZU_BINARY_DIRECTORY_SHORTCUTS_PATH, (void**)&shortcutsFolder))
        {
            struct KzcHashMapIterator it = kzcHashMapGetIterator(shortcutsFolder->files);
            while (kzcHashMapIterate(it))
            {
                struct KzuBinaryFileInfo* file = (struct KzuBinaryFileInfo*)kzcHashMapIteratorGetValue(it);
                kzString shortcutName;
                struct KzcInputStream* inputStream;
                kzString targetPath;

                kzsAssert(kzcIsValidPointer(file));

                kzsErrorTest(file->type == KZU_BINARY_FILE_TYPE_SHORTCUT, KZU_ERROR_WRONG_BINARY_FILE_TYPE, "Files in shortcut folder must be shortcut type");

                shortcutName = file->name;

                stamp2 = kzsTimeGetCurrentTimestamp();
                result = kzuBinaryDirectoryOpenFile(kzcMemoryGetManager(directory), file, &inputStream);
                kzsErrorForward(result);
                directory->loadingMeasurement.shortcutsTimeFileOpen += (kzsTimeGetCurrentTimestamp() - stamp2);

                stamp2 = kzsTimeGetCurrentTimestamp();
                result = kzuBinaryDirectoryReadReference(inputStream, file, &targetPath);
                kzsErrorForward(result);
                directory->loadingMeasurement.shortcutsTimeReadReference += (kzsTimeGetCurrentTimestamp() - stamp2);

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);

                result = kzcHashMapPut(directory->shortcuts, shortcutName, (kzMutableString)targetPath);
                kzsErrorForward(result);
            }
        }
    }
    directory->loadingMeasurement.shortcutsTime = (kzsTimeGetCurrentTimestamp() - stamp);

    *out_directory = directory;
    kzsSuccess();
}

kzsError kzuBinaryDirectoryCreateFromFileResource(const struct KzcMemoryManager* memoryManager, kzString resourcePath,
                                                  struct KzuBinaryDirectory** out_directory)
{
    kzsError result;
    struct KzuBinarySource* source;
    struct KzuBinaryDirectory* directory;

    result = kzuBinarySourceCreate_internal(memoryManager, KZU_BINARY_SOURCE_TYPE_RESOURCE, &source);
    kzsErrorForward(result);

    result = kzcStringCopy(memoryManager, resourcePath, &source->dataSource.resourcePath);
    kzsErrorForward(result);

    result = kzuBinaryDirectoryCreateFromSource_internal(memoryManager, source, &directory);
    kzsErrorForward(result);

    *out_directory = directory;
    kzsSuccess();
}

kzsError kzuBinaryDirectoryCreateFromMemory(const struct KzcMemoryManager* memoryManager, const kzByte* buffer,
                                            kzUint size, struct KzuBinaryDirectory** out_directory)
{
    kzsError result;
    struct KzuBinarySource* source;
    struct KzuBinaryDirectory* directory;

    result = kzuBinarySourceCreate_internal(memoryManager, KZU_BINARY_SOURCE_TYPE_MEMORY, &source);
    kzsErrorForward(result);
    source->dataSource.memory.size = size;
    source->dataSource.memory.buffer = buffer;

    result = kzuBinaryDirectoryCreateFromSource_internal(memoryManager, source, &directory);
    kzsErrorForward(result);

    *out_directory = directory;
    kzsSuccess();
}

kzsError kzuBinaryDirectoryDelete(struct KzuBinaryDirectory* directory)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(directory));

    /* Delete all shortcuts in the directory. */
    {
        result = kzcHashMapDelete(directory->shortcuts);
        kzsErrorForward(result);
    }


    /* Delete all sources in the directory. */
    {
        struct KzcHashSetIterator it = kzcHashSetGetIterator(directory->sources);
        while (kzcHashSetIterate(it))
        {
            struct KzuBinarySource* source = (struct KzuBinarySource*)kzcHashSetIteratorGetValue(it);

            result = kzuBinarySourceDelete_internal(source);
            kzsErrorForward(result);
        }

        result = kzcHashSetDelete(directory->sources);
        kzsErrorForward(result);
    }

    result = kzcHashSetDelete(directory->deletedFiles);
    kzsErrorForward(result);

    /* Recursively delete the folder structure of the directory. */
    result = kzuBinaryFolderInfoDelete_internal(directory->rootFolder);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(directory);
    kzsErrorForward(result);

    kzsSuccess();
}

/** Merges two binary folders together. Source folder is emptied as a side effect. */
static kzsError kzuBinaryDirectoryMergeFolder_internal(struct KzuBinaryDirectory* targetDirectory, const struct KzuBinaryFolderInfo* targetFolder,
                                                       const struct KzuBinaryFolderInfo* sourceFolder, struct KzcDynamicArray* mergedFiles)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(targetDirectory));
    kzsAssert(kzcIsValidPointer(targetFolder));
    kzsAssert(kzcIsValidPointer(sourceFolder));
    
    /* Sub-folders */
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(sourceFolder->subFolders);
        while (kzcHashMapIterate(it))
        {
            struct KzuBinaryFolderInfo* subFolder = (struct KzuBinaryFolderInfo*)kzcHashMapIteratorGetValue(it);
            kzMutableString subFolderName;
            struct KzuBinaryFolderInfo* targetSubFolder;
            kzBool deleteSourceName = KZ_FALSE;

            kzsAssert(kzcIsValidPointer(subFolder));
            subFolderName = subFolder->name;

            /* Find matching sub-folder from targetFolder or create a new one in it */
            if (kzcHashMapGet(targetFolder->subFolders, subFolderName, (void**)&targetSubFolder))
            {
                deleteSourceName = KZ_TRUE;
            }
            else
            {
                result = kzuBinaryFolderInfoCreate_internal(kzcMemoryGetManager(targetDirectory), subFolderName, &targetSubFolder);
                kzsErrorForward(result);

                result = kzcHashMapPut(targetFolder->subFolders, subFolderName, targetSubFolder);
                kzsErrorForward(result);
            }

            kzsAssert(kzcIsValidPointer(targetSubFolder));

            /* Recursively merge sub-folder */
            result = kzuBinaryDirectoryMergeFolder_internal(targetDirectory, targetSubFolder, subFolder, mergedFiles);
            kzsErrorForward(result);

            /* Remove source sub-folder. */
            subFolder->name = KZ_NULL;
            result = kzuBinaryFolderInfoDelete_internal(subFolder);
            kzsErrorForward(result);

            result = kzcHashMapRemove(sourceFolder->subFolders, subFolderName);
            kzsErrorForward(result);

            if (deleteSourceName)
            {
                result = kzcStringDelete(subFolderName);
                kzsErrorForward(result);
            }
        }

        kzsAssert(kzcHashMapIsEmpty(sourceFolder->subFolders));
    }
    
    /* Files */
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(sourceFolder->files);
        while (kzcHashMapIterate(it))
        {
            struct KzuBinaryFileInfo* file = (struct KzuBinaryFileInfo*)kzcHashMapIteratorGetValue(it);
            kzMutableString fileName;
            struct KzuBinaryFileInfo* targetFile;

            kzsAssert(kzcIsValidPointer(file));
            fileName = file->name;

            /* Find matching file from targetFolder or create a new one in it */
            if (kzcHashMapGet(targetFolder->files, fileName, (void**)&targetFile))
            {
                result = kzcHashMapRemove(targetFolder->files, fileName);
                kzsErrorForward(result);

                result = kzcStringDelete(targetFile->name);
                kzsErrorForward(result);

                *targetFile = *file;
            }
            else
            {
                result = kzuBinaryFileInfoCreate_internal(kzcMemoryGetManager(targetDirectory), fileName, file->type,
                                                          file->source, file->offset, file->size, file->path, file->flags & targetDirectory->fileFlagMask, &targetFile);
                kzsErrorForward(result);
            }

            result = kzcHashMapPut(targetFolder->files, fileName, targetFile);
            kzsErrorForward(result);

            if (mergedFiles != KZ_NULL)
            {
                result = kzcDynamicArrayAdd(mergedFiles, targetFile);
                kzsErrorForward(result);
            }

            kzsAssert(kzcIsValidPointer(targetFile));

            /* Remove old file. */
            file->name = KZ_NULL;
            result = kzuBinaryFileInfoDelete_internal(file);
            kzsErrorForward(result);

            result = kzcHashMapRemove(sourceFolder->files, fileName);
            kzsErrorForward(result);
        }

        kzsAssert(kzcHashMapIsEmpty(sourceFolder->files));
    }

    kzsSuccess();
}


static kzsError kzuBinaryDirectoryHandleDeletedFiles_internal(const struct KzuBinaryFolderInfo* targetFolder,
                                                              struct KzcHashSet* deletedFiles)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(targetFolder));
    
    /* Sub-folders */
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(targetFolder->subFolders);
        while (kzcHashMapIterate(it))
        {
            struct KzuBinaryFolderInfo* subFolder = (struct KzuBinaryFolderInfo*)kzcHashMapIteratorGetValue(it);

            /* Recursively handle sub-folder */
            result = kzuBinaryDirectoryHandleDeletedFiles_internal(subFolder, deletedFiles);
            kzsErrorForward(result);
        }
    }
    
    /* Files */
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(targetFolder->files);
        while (kzcHashMapIterate(it))
        {
            struct KzuBinaryFileInfo* file = (struct KzuBinaryFileInfo*)kzcHashMapIteratorGetValue(it);
            kzString filePath;

            kzsAssert(kzcIsValidPointer(file));
            filePath = file->path;

            if (kzcHashSetContains(deletedFiles, filePath))
            {
                kzMutableString storedFilePath = (kzMutableString)kzcHashSetGetStoredValue(deletedFiles, filePath);

                result = kzcHashSetRemove(deletedFiles, filePath);
                kzsErrorForward(result);

                result = kzcStringDelete(storedFilePath);
                kzsErrorForward(result);

                result = kzcHashMapRemove(targetFolder->files, file->name);
                kzsErrorForward(result);

                result = kzuBinaryFileInfoDelete_internal(file);
                kzsErrorForward(result);
            }
        }
    }

    kzsSuccess();
}

kzsError kzuBinaryDirectoryMerge(struct KzuBinaryDirectory* targetDirectory, struct KzuBinaryDirectory* sourceDirectory,
                                 struct KzcDynamicArray* mergedFiles)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(targetDirectory));
    kzsAssert(kzcIsValidPointer(sourceDirectory));

    /* Merge root and all ancestors. */
    result = kzuBinaryDirectoryMergeFolder_internal(targetDirectory, targetDirectory->rootFolder,
                                                    sourceDirectory->rootFolder, mergedFiles);
    kzsErrorForward(result);

    /* Handle deleted files. */
    /* TODO: Add callback call for deleted files also */
    result = kzuBinaryDirectoryHandleDeletedFiles_internal(targetDirectory->rootFolder, sourceDirectory->deletedFiles);
    kzsErrorForward(result);

    kzsErrorTest(kzcHashSetIsEmpty(sourceDirectory->deletedFiles), KZU_ERROR_PROJECT_OBJECT_NOT_FOUND,
                 "Patch data contains deletion of nonexistent object(s)");

    /* Merge sources. */
    {
        struct KzcHashSetIterator it = kzcHashSetGetIterator(sourceDirectory->sources);

        while (kzcHashSetIterate(it))
        {
            struct KzuBinarySource* source = kzcHashSetIteratorGetValue(it);

            result = kzcHashSetAdd(targetDirectory->sources, source);
            kzsErrorForward(result);
        }

        kzcHashSetClear(sourceDirectory->sources);
        kzsErrorForward(result);
    }

    /* Merge shortcuts. */
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(sourceDirectory->shortcuts);

        while (kzcHashMapIterate(it))
        {
            result = kzcHashMapPut(targetDirectory->shortcuts, kzcHashMapIteratorGetKey(it), kzcHashMapIteratorGetValue(it));
            kzsErrorForward(result);
        }
    }

    result = kzuBinaryDirectoryDelete(sourceDirectory);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuBinaryFolderInfo* kzuBinaryDirectoryGetRootFolder(const struct KzuBinaryDirectory* directory)
{
    kzsAssert(kzcIsValidPointer(directory));

    return directory->rootFolder;
}

struct KzcHashSetIterator kzuBinaryDirectoryGetDeletedFiles(const struct KzuBinaryDirectory* directory)
{
    kzsAssert(kzcIsValidPointer(directory));

    return kzcHashSetGetIterator(directory->deletedFiles);
}

/*
 * Calculates the number of path elements in the given path string.
 * Path elements are separated with a single slash (/), but not with a double slash (//).
 */
static kzUint kzuBinaryDirectoryGetPathElementCount_internal(kzString path)
{
    kzUint pathElementCount = 0;
    kzUint i = 0;
    kzChar ch;

    kzsAssert(path != KZ_NULL);

    while ((ch = path[i++]) != KZC_STRING_TERMINATION_SYMBOL)
    {
        if (ch == KZU_BINARY_PATH_SEPARATOR)
        {
            if (path[i] == KZU_BINARY_PATH_SEPARATOR) /* Check if it's an escaped slash instead of path separator. */
            {
                ++i;
            }
            else
            {
                ++pathElementCount;
            }
        }
    }

    return pathElementCount + 1;
}

/**
 * Gets the next path element from the given path string. A new string is allocated for the path element.
 * Tail of the original path string is returned as well; that is the location where to keep looking for more path elements.
 */
static kzsError kzuBinaryDirectoryGetNextPathElement_internal(const struct KzcMemoryManager* memoryManager, kzString path,
                                                              kzString* out_pathTail, kzMutableString* out_pathElement)
{
    kzsError result;

    kzUint nameLength = 0;
    kzMutableString name;
    kzString pathTail = KZ_NULL;

    kzsAssert(path != KZ_NULL);

    /* Count the length of the next path element. Escaped slashes are treated as one character. */
    {
        kzChar ch;
        kzUint i = 0;

        while ((ch = path[i++]) != KZC_STRING_TERMINATION_SYMBOL) /* Loop until single slash is found or end of string. */
        {
            if (ch == KZU_BINARY_PATH_SEPARATOR &&
                path[i++] != KZU_BINARY_PATH_SEPARATOR) /* Check if it's a path separator instead of an escaped slash. */
            {
                break;
            }
            ++nameLength;
        }
    }

    result = kzcStringAllocate(memoryManager, nameLength, &name);
    kzsErrorForward(result);

    /* Read the next path element. */
    {
        kzChar ch;
        kzUint i = 0;
        kzUint j = 0;

        while ((ch = path[i++]) != KZC_STRING_TERMINATION_SYMBOL) /* Loop until single slash is found or end of string. */
        {
            if (ch == KZU_BINARY_PATH_SEPARATOR &&
                path[i++] != KZU_BINARY_PATH_SEPARATOR) /* Check if it's a path separator instead of an escaped slash. */
            {
                pathTail = path + i - 1;
                break;
            }
            name[j++] = ch;
        }
        name[j] = KZC_STRING_TERMINATION_SYMBOL;
    }

    kzsCounterIncrease("binaryDirectoryGetNextPathElement_internal");

    *out_pathTail = pathTail;
    *out_pathElement = name;
    kzsSuccess();
}

kzsException kzuBinaryDirectoryGetFolder(const struct KzuBinaryDirectory* directory, kzString folderPath, struct KzuBinaryFolderInfo** out_folder)
{
    kzsError result;
    const struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(directory);
    struct KzuBinaryFolderInfo* folder = directory->rootFolder;
    kzUint folderCount;
    kzString filePathTail;

    folderCount = kzuBinaryDirectoryGetPathElementCount_internal(folderPath) - 1;

    filePathTail = folderPath;

    /* Loop through the folders */
    {
        kzUint i;
        for (i = 0; i < folderCount; ++i)
        {
            kzMutableString folderName;
            kzBool subFolderFound;
            struct KzuBinaryFolderInfo* subFolder;

            result = kzuBinaryDirectoryGetNextPathElement_internal(memoryManager, filePathTail, &filePathTail, &folderName);
            kzsErrorForward(result);

            subFolderFound = kzcHashMapGet(folder->subFolders, folderName, (void**)&subFolder);

            result = kzcStringDelete(folderName);
            kzsErrorForward(result);

            kzsExceptionTest(subFolderFound, KZU_EXCEPTION_FILE_NOT_FOUND, "Folder not found from the binary");

            folder = subFolder;
        }
    }

    *out_folder = folder;
    kzsSuccess();
}

static kzsException kzuBinaryFolderGetFile_internal(const struct KzuBinaryFolderInfo* folder, kzString fileName, struct KzuBinaryFileInfo** out_file)
{
    struct KzuBinaryFileInfo* file = KZ_NULL;
    kzBool fileFound;

    kzsAssert(kzcIsValidPointer(folder));

    fileFound = kzcHashMapGet(folder->files, fileName, (void**)&file);

    kzsExceptionTest(fileFound, KZU_EXCEPTION_FILE_NOT_FOUND, "File not found from folder");

    *out_file = file;
    kzsSuccess();
}

kzsException kzuBinaryDirectoryGetFile(const struct KzuBinaryDirectory* directory, kzString filePath, struct KzuBinaryFileInfo** out_file)
{
    kzsError result;
    struct KzuBinaryFileInfo* file;
    const struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(directory);
    struct KzuBinaryFolderInfo* folder = directory->rootFolder;
    kzUint folderCount;
    kzString filePathTail;

    filePath = kzuBinaryDirectoryGetActualPath(directory, filePath);

    folderCount = kzuBinaryDirectoryGetPathElementCount_internal(filePath) - 1;

    filePathTail = filePath;

    /* Loop through the folders */
    {
        kzUint i;
        for (i = 0; i < folderCount; ++i)
        {
            kzMutableString folderName;
            kzBool subFolderFound;
            struct KzuBinaryFolderInfo* subFolder;

            result = kzuBinaryDirectoryGetNextPathElement_internal(memoryManager, filePathTail, &filePathTail, &folderName);
            kzsErrorForward(result);

            subFolderFound = kzcHashMapGet(folder->subFolders, folderName, (void**)&subFolder);

            result = kzcStringDelete(folderName);
            kzsErrorForward(result);

            kzsExceptionTest(subFolderFound, KZU_EXCEPTION_FILE_NOT_FOUND, "Folder not found from the binary");

            folder = subFolder;
        }
    }

    /* Get the file */
    {
        kzMutableString fileName;

        result = kzuBinaryDirectoryGetNextPathElement_internal(memoryManager, filePathTail, &filePathTail, &fileName);
        kzsErrorForward(result);

        kzsAssert(filePathTail == KZ_NULL);

        result = kzuBinaryFolderGetFile_internal(folder, fileName, &file);
        kzsExceptionForward(result);

        result = kzcStringDelete(fileName);
        kzsErrorForward(result);
    }

    kzsCounterIncrease("binaryDirectoryGetFile");

    *out_file = file;
    kzsSuccess();
}

kzU32 kzuBinaryDirectoryGetFlags(const struct KzuBinaryDirectory* directory)
{
    kzsAssert(kzcIsValidPointer(directory));
    return directory->flags;
}

kzsError kzuBinaryDirectoryOpenFile(const struct KzcMemoryManager* memoryManager, const struct KzuBinaryFileInfo* file,
                                    struct KzcInputStream** out_inputStream)
{
    kzsError result;
    struct KzcInputStream* inputStream;

    result = kzuBinaryDirectoryOpenFileWithReferences(memoryManager, file, KZ_NULL, KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED, &inputStream);
    kzsErrorForward(result);
    
    *out_inputStream = inputStream;
    kzsSuccess();
}

kzsError kzuBinaryDirectoryOpenFileWithEndianness(const struct KzcMemoryManager* memoryManager, const struct KzuBinaryFileInfo* file,
                                                  enum KzcIOStreamEndianness endianness, struct KzcInputStream** out_inputStream)
{
    kzsError result;
    struct KzcInputStream* inputStream;

    result = kzuBinaryDirectoryOpenFileWithReferences(memoryManager, file, KZ_NULL, endianness, &inputStream);
    kzsErrorForward(result);
    
    *out_inputStream = inputStream;
    kzsSuccess();
}

kzsError kzuBinaryDirectoryReadFile(const struct KzcMemoryManager* memoryManager, const struct KzuBinaryFileInfo* file, kzByte** out_data)
{
    kzsError result;
    struct KzcInputStream* inputStream;
    kzByte* data;

    result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
    kzsErrorForward(result);

    result = kzcMemoryAllocArray(memoryManager, data, file->size, "File data");
    kzsErrorForward(result);

    result = kzcInputStreamReadBytes(inputStream, file->size, data);
    kzsErrorForward(result);

    result = kzcInputStreamDelete(inputStream);
    kzsErrorForward(result);

    *out_data = data;
    kzsSuccess();
}

kzsError kzuBinaryDirectoryOpenFileWithReferences(const struct KzcMemoryManager* memoryManager,
                                                  const struct KzuBinaryFileInfo* file, kzUint** out_referencePositions,
                                                  enum KzcIOStreamEndianness endianness, struct KzcInputStream** out_inputStream)
{
    kzsError result;
    struct KzcInputStream* inputStream;
    kzUint* referencePositions = KZ_NULL; /* The value assigned here is never used, but data flow analysis isn't able to detect that. */

    kzsAssert(kzcIsValidPointer(file));

    result = kzuBinarySourceCreateInputStream_internal(memoryManager, file->source, endianness, &inputStream);
    kzsErrorForward(result);

    result = kzcInputStreamSkip(inputStream, file->offset);
    kzsErrorForward(result);

    /* Read or skip reference list */
    {
        kzUint referencesSize;
        kzByte* referenceData;
        kzUint numReferences;

        result = kzcInputStreamReadU32Int(inputStream, &referencesSize);
        kzsErrorForward(result);

        if (out_referencePositions == KZ_NULL)
        {
            result = kzcInputStreamSkip(inputStream, referencesSize);
            kzsErrorForward(result);
        }
        else
        {
            if (referencesSize == 0) /* Optimize by not allocating temporary data array */
            {
                result = kzcMemoryAllocArray(memoryManager, referencePositions, 0, "Binary file reading reference positions");
                kzsErrorForward(result);
            }
            else
            {
                result = kzcMemoryAllocArray(memoryManager, referenceData, referencesSize, "Binary file reading reference positions data");
                kzsErrorForward(result);

                result = kzcInputStreamReadBytes(inputStream, referencesSize, referenceData);
                kzsErrorForward(result);

                /* Count the number of references */
                {
                    kzUint i;
                    numReferences = 0;
                    for (i = 0; i < referencesSize; ++i)
                    {
                        if ((referenceData[i] >> 7) == 0)
                        {
                            ++numReferences;
                        }
                    }
                }

                result = kzcMemoryAllocArray(memoryManager, referencePositions, numReferences, "Binary file reading reference positions");
                kzsErrorForward(result);

                /* Uncompress the references */
                {
                    kzUint i;
                    kzUint offset = 0;
                    kzUint lastPosition = 0;
                    for (i = 0; i < numReferences; ++i)
                    {
                        kzBool isHighBitSet;
                        kzUint position = 0;
                        do 
                        {
                            kzByte value = referenceData[offset++];
                            isHighBitSet = (value >> 7) > 0;
                            position += (kzUint)(value & 0x7F);
                            if (isHighBitSet)
                            {
                                position = (position + 1) << 7;
                            }
                        } while (isHighBitSet);
                        lastPosition += position;

                        referencePositions[i] = lastPosition;

                        lastPosition += 4; /* Size of the reference */
                    }
                }

                result = kzcMemoryFreeArray(referenceData);
                kzsErrorForward(result);
            }
        }
    }

    if (out_referencePositions != KZ_NULL)
    {
        kzsAssert(referencePositions != KZ_NULL);
        *out_referencePositions = referencePositions;
    }
    *out_inputStream = inputStream;
    kzsSuccess();
}

kzsError kzuBinaryDirectoryGetFileReferences(const struct KzcMemoryManager* memoryManager,
                                             const struct KzuBinaryFileInfo* file, kzString** out_references)
{
    kzsError result;
    kzUint* referencePositions;
    struct KzcInputStream* inputStream;
    kzString* references;
    kzUint referenceCount;
    kzUint offset;
    kzUint i;

    result = kzuBinaryDirectoryOpenFileWithReferences(memoryManager, file, &referencePositions,
                                                      KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED, &inputStream);
    kzsErrorForward(result);

    referenceCount = kzcArrayLength(referencePositions);

    result = kzcMemoryAllocArray(memoryManager, references, referenceCount, "Binary file reference paths");
    kzsErrorForward(result);

    offset = 0;
    for (i = 0; i < referenceCount; ++i)
    {
        kzUint nextOffset = referencePositions[i];
        kzUint skipAmount = nextOffset - offset;
        kzUint referenceIndex;

        result = kzcInputStreamSkip(inputStream, skipAmount);
        kzsErrorForward(result);

        result = kzcInputStreamReadU32Int(inputStream, &referenceIndex);
        kzsErrorForward(result);

        offset += skipAmount + 4;

        references[i] = kzuBinarySourceGetReferencedFilePath(file->source, referenceIndex);
    }

    result = kzcInputStreamDelete(inputStream);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray(referencePositions);
    kzsErrorForward(result);

    *out_references = references;
    kzsSuccess();
}

kzsError kzuBinaryDirectoryGetFileDebugDescription(const struct KzuBinaryFileInfo* file, kzMutableString* out_description)
{
    kzsError result;

    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(file);
    struct KzuBinarySource* source = file->source;
    kzMutableString description;
    kzMutableString sourceDescription;

    switch (source->type)
    {
        case KZU_BINARY_SOURCE_TYPE_MEMORY:
        {
            result = kzcStringFormat(memoryManager, "buffer[%u]", &sourceDescription, source->dataSource.memory.size);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_SOURCE_TYPE_RESOURCE:
        {
            result = kzcStringFormat(memoryManager, "%s", &sourceDescription, source->dataSource.resourcePath);
            kzsErrorForward(result);
            break;
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid binary directory source type");
        }
    }

    result = kzcStringFormat(memoryManager, "%s (Source: %s:%u, size: %u)", &description, file->name, sourceDescription, file->offset, file->size);
    kzsErrorForward(result);

    result = kzcStringDelete(sourceDescription);
    kzsErrorForward(result);

    *out_description = description;
    kzsSuccess();
}

kzString kzuBinaryDirectoryGetActualPath(const struct KzuBinaryDirectory* directory, kzString filePath)
{
    kzString newFilePath;

    if (!kzcHashMapGet(directory->shortcuts, filePath, (void**)&newFilePath))
    {
        newFilePath = filePath;
    }

    return newFilePath;
}

kzsException kzuBinaryDirectoryGetFileFromPropertyTypeLibrary(const struct KzuBinaryDirectory* directory, kzString filePath,
                                                              struct KzuBinaryFileInfo** out_file)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    kzString newFilePath;

    if (directory->propertyTypeFolder == KZ_NULL)
    {
        result = kzuBinaryDirectoryGetFolder(directory, KZU_BINARY_DIRECTORY_PROPERTY_TYPE_LIBRARY_PATH, (struct KzuBinaryFolderInfo**)&directory->propertyTypeFolder);
        kzsExceptionForward(result);
    }

    /* TODO: Use kzcStringLength instead of kzsStrlen. Also don't calculate the length every time, create a constant for it. */
    newFilePath = filePath + kzsStrlen(KZU_BINARY_DIRECTORY_PROPERTY_TYPE_LIBRARY_PATH);

    result = kzuBinaryFolderGetFile_internal(directory->propertyTypeFolder, newFilePath, &file);
    kzsExceptionForward(result);

    *out_file = file;
    kzsSuccess();
}

kzsException kzuBinaryDirectoryGetFileFromAnimationLibrary(const struct KzuBinaryDirectory* directory, kzString filePath,
                                                           struct KzuBinaryFileInfo** out_file)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    kzString newFilePath;

    if (directory->animationsFolder == KZ_NULL)
    {
        result = kzuBinaryDirectoryGetFolder(directory, KZU_BINARY_DIRECTORY_ANIMATION_LIBRARY_PATH, (struct KzuBinaryFolderInfo**)&directory->animationsFolder);
        kzsExceptionForward(result);
    }

    newFilePath = filePath + kzsStrlen(KZU_BINARY_DIRECTORY_ANIMATION_LIBRARY_PATH);

    result = kzuBinaryFolderGetFile_internal(directory->animationsFolder, newFilePath, &file);
    kzsExceptionForward(result);

    *out_file = file;
    kzsSuccess();
}

kzsException kzuBinaryDirectoryGetFileFromMeshLibrary(const struct KzuBinaryDirectory* directory, kzString filePath,
                                                      struct KzuBinaryFileInfo** out_file)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    kzString newFilePath;

    if (directory->meshFolder == KZ_NULL)
    {
        result = kzuBinaryDirectoryGetFolder(directory, KZU_BINARY_DIRECTORY_MESH_LIBRARY_PATH, (struct KzuBinaryFolderInfo**)&directory->meshFolder);
        kzsExceptionForward(result);
    }

    newFilePath = filePath + kzsStrlen(KZU_BINARY_DIRECTORY_MESH_LIBRARY_PATH);

    result = kzuBinaryFolderGetFile_internal(directory->meshFolder, newFilePath, &file);
    kzsExceptionForward(result);

    *out_file = file;
    kzsSuccess();
}

kzsError kzuBinaryDirectoryReadReference(struct KzcInputStream* inputStream, const struct KzuBinaryFileInfo* file, kzString* out_path)
{
    kzsError result;
    kzUint referenceIndex;
    struct KzuBinarySource* source;
    kzString path;

    result = kzcInputStreamReadU32Int(inputStream, &referenceIndex);
    kzsErrorForward(result);

    source = kzuBinaryFileInfoGetSource(file);

    path = kzuBinarySourceGetReferencedFilePath(source, referenceIndex);

    kzsCounterIncrease("readReference");

    *out_path = path;
    kzsSuccess();
}

void kzuBinaryDirectorySetFileFlagMask(struct KzuBinaryDirectory* binaryDirectory, kzU32 fileFlagMask)
{
    kzsAssert(kzcIsValidPointer(binaryDirectory));

    binaryDirectory->fileFlagMask = fileFlagMask;
}

kzU32 kzuBinaryDirectoryGetFileFlagMask(const struct KzuBinaryDirectory* directory)
{
    kzsAssert(kzcIsValidPointer(directory));

    return directory->fileFlagMask;
}

kzUint kzuBinaryDirectoryGetLoadingTime(const struct KzuBinaryDirectory* directory, enum KzuBinaryDirectoryLoadingMeasurement measurement)
{
    kzUint time = 0;
    switch(measurement)
    {
        case KZU_BINARY_DIRECTORY_LOADING_MEASUREMENT_FILES: time = directory->loadingMeasurement.fileTime; break;
        case KZU_BINARY_DIRECTORY_LOADING_MEASUREMENT_FOLDERS: time = directory->loadingMeasurement.folderTime; break;
        case KZU_BINARY_DIRECTORY_LOADING_MEASUREMENT_EXTERNAL_FILES: time = directory->loadingMeasurement.externalFileTime; break;
        case KZU_BINARY_DIRECTORY_LOADING_MEASUREMENT_DELETED_FILES: time = directory->loadingMeasurement.deletedFilesTime; break;
        case KZU_BINARY_DIRECTORY_LOADING_MEASUREMENT_SHORTCUTS: time = directory->loadingMeasurement.shortcutsTime; break;
        case KZU_BINARY_DIRECTORY_LOADING_MEASUREMENT_SHORTCUTS_FOPEN: time = directory->loadingMeasurement.shortcutsTimeFileOpen; break;
        case KZU_BINARY_DIRECTORY_LOADING_MEASUREMENT_SHORTCUTS_READ_REFERENCE: time = directory->loadingMeasurement.shortcutsTimeReadReference; break;
        case KZU_BINARY_DIRECTORY_LOADING_MEASUREMENT_HEADER: time = directory->loadingMeasurement.headerTime; break;
        default: kzsAssert(KZ_FALSE);
    }

    return time;
}

kzsError kzuBinaryDirectoryCreateCoreAssets(const struct KzcMemoryManager* memoryManager, struct KzuBinaryDirectory** out_coreAssetsDirectory)
{
    kzsError result;
    struct KzuBinaryDirectory* tempDirectory;

    result = kzuBinaryDirectoryCreateFromMemory(memoryManager, kzuCoreAssetData, kzuCoreAssetDataSize,
        &tempDirectory);
    kzsErrorForward(result);

    *out_coreAssetsDirectory = tempDirectory;
    kzsSuccess();
}

kzsError kzuBinaryDirectoryMergeCoreAssets(struct KzuBinaryDirectory* directory)
{
    kzsError result;
    struct KzuBinaryDirectory* tempDirectory;

    result = kzuBinaryDirectoryCreateCoreAssets(kzcMemoryGetManager(directory), &tempDirectory);
    kzsErrorForward(result);

    result = kzuBinaryDirectoryMerge(directory, tempDirectory, KZ_NULL);
    kzsErrorForward(result);

    kzsSuccess();
}
