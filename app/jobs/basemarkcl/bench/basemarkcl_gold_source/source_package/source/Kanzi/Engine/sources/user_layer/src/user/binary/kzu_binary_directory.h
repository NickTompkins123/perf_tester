/**
* \file
* Binary directory for .kzb format.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_BINARY_DIRECTORY_H
#define KZU_BINARY_DIRECTORY_H


#include "kzu_binary_base.h"

#include <core/util/io/kzc_io_stream.h>
#include <core/util/collection/kzc_hash_set.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/** Current binary format version. */
#define KZU_BINARY_FORMAT_VERSION 35


/* Forward declarations. */
struct KzuBinaryDirectoryFilter;
struct KzcMemoryManager;
struct KzcInputStream;
struct KzcDynamicArray;


#define KZU_BINARY_FILE_INFO_FLAG_LOAD_ON_DEMAND (1 << 1)
#define KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE (1 << 2)
#define KZU_BINARY_DIRECTORY_DEFAULT_FILE_FLAG_MASK 0xFF


/** Binary directory loading time measurement constants. */
enum KzuBinaryDirectoryLoadingMeasurement
{
    KZU_BINARY_DIRECTORY_LOADING_MEASUREMENT_FOLDERS, /**< Folders loading time. */
    KZU_BINARY_DIRECTORY_LOADING_MEASUREMENT_FILES, /**< Files loading time. */
    KZU_BINARY_DIRECTORY_LOADING_MEASUREMENT_EXTERNAL_FILES, /**< External files loading time. */
    KZU_BINARY_DIRECTORY_LOADING_MEASUREMENT_DELETED_FILES, /**< Deleted files loading time. */
    KZU_BINARY_DIRECTORY_LOADING_MEASUREMENT_SHORTCUTS, /**< Shortcuts loading time. */
    KZU_BINARY_DIRECTORY_LOADING_MEASUREMENT_SHORTCUTS_FOPEN, /**< Shortcuts loading time for fopen. */
    KZU_BINARY_DIRECTORY_LOADING_MEASUREMENT_SHORTCUTS_READ_REFERENCE, /**< Shortcuts loading time for read reference. */
    KZU_BINARY_DIRECTORY_LOADING_MEASUREMENT_HEADER /**< Header loading time. */
};


/**
 * \struct KzuBinarySource
 * Specifies a source where file content can be read with an input stream.
 */
struct KzuBinarySource;

/**
 * \struct KzuBinaryDirectoryFile
 * Represents a single file inside a directory.
 */
struct KzuBinaryFileInfo;

/**
 * \struct KzuBinaryDirectoryFolder
 * Represents a single folder inside a directory.
 */
struct KzuBinaryFolderInfo;

/**
 * \struct KzuBinaryDirectory
 * Directory of a single .kzb binary or a merged result of several .kzb binaries.
 */
struct KzuBinaryDirectory;


/* Binary source */

/**
 * Gets a full file path for the given reference index from the binary source.
 * \return Full file path of the reference with the given index or KZ_NULL if index == 0.
 */
kzString kzuBinarySourceGetReferencedFilePath(const struct KzuBinarySource* source, kzUint index);


/* Binary file info */

/** Gets the name of a file. */
kzString kzuBinaryFileInfoGetName(const struct KzuBinaryFileInfo* file);

/** Gets the binary source of a file. */
struct KzuBinarySource* kzuBinaryFileInfoGetSource(const struct KzuBinaryFileInfo* file);

/** Gets the size of a file. */
kzUint kzuBinaryFileInfoGetSize(const struct KzuBinaryFileInfo* file);

/** Gets the type of a file. */
enum KzuBinaryFileType kzuBinaryFileInfoGetType(const struct KzuBinaryFileInfo* file);

/** Gets the path of a file. */
kzString kzuBinaryFileInfoGetPath(const struct KzuBinaryFileInfo* file);

/** Gets flags from binary file. */
kzU32 kzuBinaryFileInfoGetFlags(const struct KzuBinaryFileInfo* file);


/* Binary folder info */

/** Gets the name of a folder. */
kzString kzuBinaryFolderInfoGetName(const struct KzuBinaryFolderInfo* folder);

/**
 * Gets the sub folders of a folder.
 * \return Hash map of kzString -> KzuBinaryDirectoryFolder entries.
 */
struct KzcHashMap* kzuBinaryFolderInfoGetSubFolders(const struct KzuBinaryFolderInfo* folder);

/**
 * Gets the files of a folder.
 * \return Hash map of kzString -> KzuBinaryDirectoryFile entries.
 */
struct KzcHashMap* kzuBinaryFolderInfoGetFiles(const struct KzuBinaryFolderInfo* folder);

/** Gets all files recursively from a folder. The files are added to the given dynamic array. */
kzsError kzuBinaryFolderInfoGetAllFiles(const struct KzuBinaryFolderInfo* folder, struct KzcDynamicArray* files);


/* Binary directory */

/** Creates an empty directory. Content can be added to the directory with kzuBinaryDirectoryMerge(). */
kzsError kzuBinaryDirectoryCreateEmpty(const struct KzcMemoryManager* memoryManager, struct KzuBinaryDirectory** out_directory);

/** Creates a directory and fills the content from a .kzb resource file. */
kzsError kzuBinaryDirectoryCreateFromFileResource(const struct KzcMemoryManager* memoryManager, kzString resourcePath, 
                                                  struct KzuBinaryDirectory** out_directory);

/** Creates a directory and fills the content from memory buffer with .kzb content.*/
kzsError kzuBinaryDirectoryCreateFromMemory(const struct KzcMemoryManager* memoryManager, const kzByte* buffer, kzUint size, 
                                            struct KzuBinaryDirectory** out_directory);

/** Deletes a directory. */
kzsError kzuBinaryDirectoryDelete(struct KzuBinaryDirectory* directory);


/**
 * Merges the content of two directories.
 * Note: This function will completely delete sourceDirectory rendering any references to it invalid.
 * This is to ensure that each directory entry is owned by only one directory.
 * \param mergedFiles If this parameter is not NULL, all merged files are recorded in this dynamic array of KzuBinaryFileInfo objects.
 */
kzsError kzuBinaryDirectoryMerge(struct KzuBinaryDirectory* targetDirectory, struct KzuBinaryDirectory* sourceDirectory,
                                 struct KzcDynamicArray* mergedFiles);

/** Gets the root folder of a directory. */
struct KzuBinaryFolderInfo* kzuBinaryDirectoryGetRootFolder(const struct KzuBinaryDirectory* directory);

/** Gets an iterator of all deleted files in a directory. */
struct KzcHashSetIterator kzuBinaryDirectoryGetDeletedFiles(const struct KzuBinaryDirectory* directory);

/** Gets a file with the given shortcut or full path from a binary directory. Note that because of shortcut objects, the returned file may exist in different path than requested. */
kzsException kzuBinaryDirectoryGetFile(const struct KzuBinaryDirectory* directory, kzString filePath, struct KzuBinaryFileInfo** out_file);

/** Gets flags from binary directory. */
kzU32 kzuBinaryDirectoryGetFlags(const struct KzuBinaryDirectory* directory);

/** Gets a folder with given path from binary directory. E.g. "Objects/" would return Object library. */
kzsException kzuBinaryDirectoryGetFolder(const struct KzuBinaryDirectory* directory, kzString folderPath, struct KzuBinaryFolderInfo** out_folder);

/** Opens a given file for reading. */
kzsError kzuBinaryDirectoryOpenFile(const struct KzcMemoryManager* memoryManager, const struct KzuBinaryFileInfo* file,
                                    struct KzcInputStream** out_inputStream);

/** Opens a given file for reading with the specified endianness. */
kzsError kzuBinaryDirectoryOpenFileWithEndianness(const struct KzcMemoryManager* memoryManager, const struct KzuBinaryFileInfo* file,
                                                  enum KzcIOStreamEndianness endianness, struct KzcInputStream** out_inputStream);

/** Returns binary data from file.*/
kzsError kzuBinaryDirectoryReadFile(const struct KzcMemoryManager* memoryManager, const struct KzuBinaryFileInfo* file, kzByte** out_data);

/**
 * Opens a given file for reading with reference positions returned in an array.
 * The references in the file can be queried with the functions kzuBinaryFileInfoGetSource() and kzuBinarySourceGetReferencedFilePath().
 * \param out_referencePositions This array will be filled with positions of each reference in the file.
 *                               If this parameter is KZ_NULL, the position information is ignored and the array is not created.
 *                               The caller must free the array with kzcMemoryFreeArray afterwards.
 */
kzsError kzuBinaryDirectoryOpenFileWithReferences(const struct KzcMemoryManager* memoryManager,
                                                  const struct KzuBinaryFileInfo* file, kzUint** out_referencePositions,
                                                  enum KzcIOStreamEndianness endianness, struct KzcInputStream** out_inputStream);

/**
 * Gets all file paths referenced by the given file. The paths are returned in an array of strings, which the caller
 * must delete after done using it.
 */
kzsError kzuBinaryDirectoryGetFileReferences(const struct KzcMemoryManager* memoryManager,
                                             const struct KzuBinaryFileInfo* file, kzString** out_references);

/** Gets a string description of a given file. This is for debugging purposes only. */
kzsError kzuBinaryDirectoryGetFileDebugDescription(const struct KzuBinaryFileInfo* file, kzMutableString* out_description);

/** Search for the given file path from shortcuts. If it is found, return the target path of the shortcut. Otherwise return filePath. */
kzString kzuBinaryDirectoryGetActualPath(const struct KzuBinaryDirectory* directory, kzString filePath);

/** Gets file from property type library, using optimized search. */
kzsException kzuBinaryDirectoryGetFileFromPropertyTypeLibrary(const struct KzuBinaryDirectory* directory, kzString filePath,
                                                              struct KzuBinaryFileInfo** out_file);
/** Gets file from animation library, using optimized search. */
kzsException kzuBinaryDirectoryGetFileFromAnimationLibrary(const struct KzuBinaryDirectory* directory, kzString filePath,
                                                           struct KzuBinaryFileInfo** out_file);
/** Gets file from mesh library, using optimized search. */
kzsException kzuBinaryDirectoryGetFileFromMeshLibrary(const struct KzuBinaryDirectory* directory, kzString filePath,
                                                      struct KzuBinaryFileInfo** out_file);

/** Loads reference. */
kzsError kzuBinaryDirectoryReadReference(struct KzcInputStream* inputStream, const struct KzuBinaryFileInfo* file, kzString* out_path);
/** Sets file flag mask for binary directory files. Results loaded files to have flags with formula: flags = file.flags & directory.mask */
void kzuBinaryDirectorySetFileFlagMask(struct KzuBinaryDirectory* binaryDirectory, kzU32 fileFlagMask);
/** Gets file flag mask from directory. */
kzU32 kzuBinaryDirectoryGetFileFlagMask(const struct KzuBinaryDirectory* directory);


/** Gets loading time measurement from binary directory. */
kzUint kzuBinaryDirectoryGetLoadingTime(const struct KzuBinaryDirectory* engine, enum KzuBinaryDirectoryLoadingMeasurement measurement);


/** Creates core assets binary directory. */
kzsError kzuBinaryDirectoryCreateCoreAssets(const struct KzcMemoryManager* memoryManager, struct KzuBinaryDirectory** out_coreAssetsDirectory);
/** Merges core assets to binary directory. */
kzsError kzuBinaryDirectoryMergeCoreAssets(struct KzuBinaryDirectory* directory);


#endif
