/**
* \file
* Project loader for transition files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_transition.h"

#include "kzu_project_loader.h"
#include "kzu_project_loader_material.h"
#include "kzu_project_loader_property.h"
#include "kzu_project_loader_general.h"
#include "kzu_project.h"

#include <user/effect_system/transition/kzu_transition.h>
#include <user/effect_system/transition/kzu_transition_shader.h>
#include <user/binary/kzu_binary_directory.h>
#include <user/binary/kzu_binary_util.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_string_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>
#include <core/util/io/kzc_input_stream.h>


enum KzuBinaryTransitionType
{
    KZU_BINARY_TRANSITION_TYPE_SHADER = 0
};


kzsException kzuProjectLoaderLoadTransition(struct KzuProject* project, kzString path, struct KzuTransition** out_transition)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuPropertyManager* propertyManager = kzuProjectGetPropertyManager(project);
    struct KzuTransition* transition;

    kzsAssert(kzcIsValidPointer(project));
    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load transition with null path");

    /* Check if the scene is already loaded. */
    transition = (struct KzuTransition*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_TRANSITION, path);

    if (!kzuTransitionIsValid(transition))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_TRANSITION, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load transition file.");

        /* Load the object node of the file info. */
        {
            /* References */
            enum KzuBinaryTransitionType binaryDataType;            

            /* Read data */
            {
                struct KzcInputStream* inputStream;

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);

                result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&binaryDataType);
                kzsErrorForward(result);

                result = kzuProjectLoaderReadProperties(project, inputStream, file, transition);
                kzsErrorForward(result);

                /* Initialize object data. */
                {
                    switch(binaryDataType)
                    {
                        case KZU_BINARY_TRANSITION_TYPE_SHADER:
                        {
                            struct KzuTransitionShader* transitionShader;
                            /* Defaults values if property is not found for some reason. */
                            struct KzuMaterial* material = (struct KzuMaterial*)kzuPropertyManagerGetVoidDefault(propertyManager, transition, KZU_PROPERTY_TYPE_SHADER_TRANSITION_MATERIAL);
                            kzInt renderTargetWidth = kzuPropertyManagerGetIntDefault(propertyManager, transition, KZU_PROPERTY_TYPE_SHADER_TRANSITION_RENDER_TARGET_WIDTH);
                            kzInt renderTargetHeight = kzuPropertyManagerGetIntDefault(propertyManager, transition, KZU_PROPERTY_TYPE_SHADER_TRANSITION_RENDER_TARGET_HEIGHT);

                            result = kzuTransitionShaderCreate(memoryManager, propertyManager, kzuProjectGetMessageDispatcher(project),
                                                               kzuProjectGetResourceManager(project), kzuProjectGetRootSource(project),
                                                               (kzUint)renderTargetWidth, (kzUint)renderTargetHeight, 
                                                               material, &transitionShader);
                            kzsErrorForward(result);
                            transition = kzuTransitionShaderToTransition(transitionShader);

                            break;
                        }

                        default:
                        {
                            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid transition type");
                        }
                    }

                    result = kzcInputStreamDelete(inputStream);
                    kzsErrorForward(result);
                }
            }

            result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_TRANSITION, transition, kzuBinaryFileInfoGetPath(file), 
                (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&transition);
            kzsErrorForward(result);
        }
    }

    *out_transition = transition;
    kzsSuccess();
}

kzsException kzuProjectLoaderUnloadTransition(struct KzuProject* project, const struct KzuTransition* transition)
{
    kzsError result;
    kzString objectPath = kzuProjectGetObjectPath(project, transition);

    if(objectPath != KZ_NULL)
    {
        result = kzuProjectLoaderUnloadObject(project, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
