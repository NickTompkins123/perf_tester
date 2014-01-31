/**
* \file
* Project loader for scene files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_scene.h"

#include "kzu_project_loader.h"
#include "kzu_project_loader_animation.h"
#include "kzu_project_loader_general.h"
#include "kzu_project_loader_property.h"
#include "kzu_project_loader_object_node.h"
#include "kzu_project_loader_composer.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_object.h>
#include <user/ui/kzu_ui_screen.h>
#include <user/kzu_error_codes.h>
#include <user/animation/kzu_animation_player.h>
#include <user/animation/kzu_time_line_entry.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_input_stream.h>


kzsException kzuProjectLoaderLoadScene(struct KzuProject* project, kzString path, struct KzuScene** out_scene)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuScene* scene;

    kzsAssert(kzcIsValidPointer(project));
    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load scene with null path");

    /* Check if the scene is already loaded. */
    scene = (struct KzuScene*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_SCENE, path);

    if (!kzuSceneIsValid(scene))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_SCENE, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load scene file.");

        result = kzuSceneCreate(memoryManager, kzuProjectGetPropertyManager(project), kzuProjectGetMessageDispatcher(project), kzuBinaryFileInfoGetName(file), &scene);
        kzsErrorForward(result);
            
        result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_SCENE, scene, kzuBinaryFileInfoGetPath(file), 
            (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&scene);
        kzsErrorForward(result);

        /* Load the scene of the file info. */
        {
            /* References */
            kzString rootObjectPath;
            kzString defaultComposerPath;
            kzString viewCameraPath;
            kzString viewCamera2DPath;
            kzString defaultAnimationPath;
            struct KzuObjectNode* rootObject;
            struct KzcInputStream* inputStream;

            result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
            kzsErrorForward(result);

            result = kzuBinaryDirectoryReadReference(inputStream, file, &rootObjectPath);
            kzsErrorForward(result);

            result = kzuBinaryDirectoryReadReference(inputStream, file, &defaultComposerPath);
            kzsErrorForward(result);

            result = kzuBinaryDirectoryReadReference(inputStream, file, &viewCameraPath);
            kzsErrorForward(result);

            result = kzuBinaryDirectoryReadReference(inputStream, file, &viewCamera2DPath);
            kzsErrorForward(result);

            result = kzuBinaryDirectoryReadReference(inputStream, file, &defaultAnimationPath);
            kzsErrorForward(result);

            /* Load root object */
            kzsErrorTest(rootObjectPath != KZ_NULL, KZU_ERROR_INVALID_BINARY_DATA, "Root node of a scene cannot be null");

            result = kzuProjectLoaderLoadObjectNode(project, rootObjectPath, &rootObject);
            kzsErrorForward(result);

            result = kzuSceneSetRootNode(scene, rootObject);
            kzsErrorForward(result);

            /* Properties */
            result = kzuProjectLoaderReadProperties(project, inputStream, file, scene);
            kzsErrorForward(result);

            result = kzcInputStreamDelete(inputStream);
            kzsErrorForward(result);

            if (defaultComposerPath != KZ_NULL)
            {
                struct KzuComposer* defaultComposer;

                result = kzuProjectLoaderLoadComposer(project, defaultComposerPath, &defaultComposer);
                kzsErrorForward(result);

                kzuSceneSetComposer(scene, defaultComposer);
            }

            if (viewCameraPath != KZ_NULL)
            {
                struct KzuObjectNode* viewCameraObjectNode;
                struct KzuCameraNode* viewCameraNode;

                result = kzuProjectLoaderLoadObjectNode(project, viewCameraPath, &viewCameraObjectNode);
                kzsErrorForward(result);

                kzsErrorTest(kzuObjectNodeGetType(viewCameraObjectNode) == KZU_OBJECT_TYPE_CAMERA, KZU_ERROR_INVALID_BINARY_DATA,
                             "Wrong type of object used as view camera in a scene");

                viewCameraNode = kzuCameraNodeFromObjectNode(viewCameraObjectNode);

                kzuSceneSetViewCamera(scene, viewCameraNode);
            }

            if (viewCamera2DPath != KZ_NULL)
            {
                struct KzuObjectNode* viewCameraObjectNode;
                struct KzuCameraNode* viewCameraNode;
                struct KzuUiScreen* screen;

                result = kzuProjectLoaderLoadObjectNode(project, viewCamera2DPath, &viewCameraObjectNode);
                kzsErrorForward(result);

                kzsErrorTest(kzuObjectNodeGetType(viewCameraObjectNode) == KZU_OBJECT_TYPE_CAMERA, KZU_ERROR_INVALID_BINARY_DATA,
                             "Wrong type of object used as 2D view camera in a scene");

                viewCameraNode = kzuCameraNodeFromObjectNode(viewCameraObjectNode);

                kzuSceneSetViewCamera2D(scene, viewCameraNode);
                screen = kzuSceneGetUiScreen(scene);
                kzuUiScreenSet2DPickingCamera(screen, viewCameraNode);
            }

            if (defaultAnimationPath != KZ_NULL)
            {
                struct KzuAnimationPlayer* animationPlayer = kzuSceneGetAnimationPlayer(scene);
                struct KzuTimeLineSequence* sequence;
                struct KzuTimeLineEntry* timeLineEntry;

                result = kzuProjectLoaderLoadTimeLineSequence(project, defaultAnimationPath, &sequence);
                kzsErrorForward(result);

                result = kzuTimeLineEntryCreateSequence(memoryManager, 0.0f, sequence, &timeLineEntry);
                kzsErrorForward(result);

                result = kzuAnimationPlayerAddTimeLineEntry(animationPlayer, timeLineEntry);
                kzsErrorForward(result);
            }
       }
    }

    *out_scene = scene;
    kzsSuccess();
}

kzsException kzuProjectLoaderUnloadScene(struct KzuProject* project, const struct KzuScene* scene)
{
    kzsError result;
    kzString objectPath = kzuProjectGetObjectPath(project, scene);

    if(objectPath != KZ_NULL)
    {
        result = kzuProjectLoaderUnloadObject(project, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
