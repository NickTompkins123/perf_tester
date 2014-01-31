/**
* \file
* Project loader for project file.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_project.h"

#include "kzu_project_loader_general.h"
#include "kzu_project_loader.h"
#include "kzu_project_loader_property.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_collection.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_input_stream.h>


kzsException kzuProjectLoaderLoadProjectFile(struct KzuProject* project, const struct KzuBinaryFileInfo* file)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    
    /* Properties */
    {
        struct KzcInputStream* inputStream;

        result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
        kzsErrorForward(result);

        result = kzuProjectLoaderReadProperties(project, inputStream, file, project);
        kzsErrorForward(result);

        if(!kzuProjectHasPermanentFilePath(project, kzuBinaryFileInfoGetPath(file)))
        {
            result = kzuProjectLoaderSetFilePermanent(project, kzuBinaryFileInfoGetPath(file));
            kzsErrorForward(result);
        }

        result = kzcInputStreamDelete(inputStream);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsException kzuProjectLoaderLoadProjectFileFromPath(struct KzuProject* project, kzString filePath)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    
    kzsErrorTest(filePath != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load project file with null path");

    /* Get file info from directory corresponding the given path. */
    result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), filePath, &file);
    kzsExceptionForward(result);

    result = kzuProjectLoaderLoadProjectFile(project, file);
    kzsErrorForward(result);

    kzsSuccess();
}
