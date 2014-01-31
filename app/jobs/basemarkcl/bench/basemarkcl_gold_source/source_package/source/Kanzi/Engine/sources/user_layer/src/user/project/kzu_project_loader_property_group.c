/**
* \file
* Project loader for property group.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_property_group.h"

#include "kzu_project_loader.h"
#include "kzu_project_loader_general.h"
#include "kzu_project_loader_property.h"
#include "kzu_project_loader.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_group.h>
#include <user/properties/kzu_property_manager.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_input_stream.h>


kzsException kzuProjectLoaderLoadPropertyGroupFromFileInfo(struct KzuProject* project, const struct KzuBinaryFileInfo* file,
                                                           struct KzuPropertyGroup** out_propertyGroup)
{
    kzsException result;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuPropertyManager* propertyManager = kzuProjectGetPropertyManager(project);
    struct KzuPropertyGroup* group;

    kzsAssert(file != KZ_NULL);

    /* Check if the property group is already loaded. */
    group = (struct KzuPropertyGroup*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_PROPERTY_GROUP, kzuBinaryFileInfoGetPath(file));

    if (!kzuPropertyGroupIsValid(group))
    {
        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_PROPERTY_GROUP, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
            "Wrong file type encountered while trying to load property group file.");

        result = kzuPropertyGroupCreate(memoryManager, kzuProjectGetPropertyManager(project), &group);
        kzsErrorForward(result);

        /* Read properties. */
        {
            struct KzcInputStream* inputStream;
            kzUint itemCount;
            kzUint i;
            kzBool overrideProperties;

            result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
            kzsErrorForward(result);

            result = kzcInputStreamReadBoolean(inputStream, &overrideProperties);
            kzsErrorForward(result);

            kzuPropertyGroupSetOverrideProperties(group, overrideProperties);

            result = kzuProjectLoaderReadProperties(project, inputStream, file, group);
            kzsErrorForward(result);

            result = kzcInputStreamReadU32Int(inputStream, &itemCount);
            kzsErrorForward(result);

            for(i = 0; i < itemCount; ++i)
            {
                struct KzuBinaryFileInfo* groupFile;
                kzString itemPath;
                enum KzuBinaryFileType fileType;
                void* targetObject = KZ_NULL;

                result = kzuBinaryDirectoryReadReference(inputStream, file, &itemPath);
                kzsErrorForward(result);
                result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), itemPath, &groupFile);
                kzsErrorForward(result);

                fileType = kzuBinaryFileInfoGetType(groupFile);

                result = kzuProjectLoaderLoadObject(project, fileType, itemPath, &targetObject);
                kzsErrorForward(result);

                if (targetObject != KZ_NULL)
                {
                    result = kzuPropertyManagerAddPropertyGroup(propertyManager, targetObject, group);
                    kzsErrorForward(result);
                }
            }

            result = kzcInputStreamDelete(inputStream);
            kzsErrorForward(result);
        }

        result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_PROPERTY_GROUP, group, kzuBinaryFileInfoGetPath(file),
            (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&group);
        kzsErrorForward(result);
    }

    *out_propertyGroup = group;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadPropertyGroup(struct KzuProject* project, kzString path, struct KzuPropertyGroup** out_propertyGroup)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzuPropertyGroup* group;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load property group with null path");

    /* Get file info from directory corresponding the given path. */
    result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
    kzsExceptionForward(result);

    result = kzuProjectLoaderLoadPropertyGroupFromFileInfo(project, file, &group);
    kzsErrorForward(result);

    *out_propertyGroup = group;
    kzsSuccess();
}

kzsException kzuProjectLoaderUnloadPropertyGroup(struct KzuProject* project, const struct KzuPropertyGroup* propertyGroup)
{
    kzsError result;
    kzString objectPath = kzuProjectGetObjectPath(project, propertyGroup);

    if(objectPath != KZ_NULL)
    {
        result = kzuProjectLoaderUnloadObject(project, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
