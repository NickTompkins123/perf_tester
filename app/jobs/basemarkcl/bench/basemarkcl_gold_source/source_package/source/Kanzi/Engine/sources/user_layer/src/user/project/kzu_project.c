/**
* \file
* Project structure containing all the scenes and their resources.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project.h"
#include "kzu_project_loader_material.h"

#include <user/scene_graph/kzu_scene.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_collection.h>
#include <user/properties/kzu_color_property.h>
#include <user/properties/kzu_matrix4x4_property.h>
#include <user/properties/kzu_vector3_property.h>
#include <user/properties/kzu_texture_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_light_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_property_group.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/material/kzu_material_type.h>
#include <user/material/kzu_material.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_mesh.h>
#include <user/scene_graph/kzu_light.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_lod_selector.h>
#include <user/renderer/kzu_composer.h>
#include <user/renderer/kzu_render_pass.h>
#include <user/filter/kzu_object_source.h>
#include <user/kzu_error_codes.h>
#include <user/animation/kzu_animation.h>
#include <user/animation/kzu_animation_clip.h>
#include <user/animation/kzu_time_line_sequence.h>
#include <user/ui/kzu_ui_component.h>
#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/kzu_ui_manager.h>
#include <user/effect_system/transition/kzu_transition.h>

#include <user/scripting/kzu_script.h>
#include <user/binary/kzu_binary_directory.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/resource_manager/kzc_resource_manager.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/resource_manager/frame_buffer/kzc_resource_frame_buffer.h>
#include <core/renderer/kzc_renderer.h>
#include <core/util/string/kzc_string.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/collection/kzc_hash_set.h>
#include <core/util/curve/kzc_trajectory.h>
#include <core/util/curve/kzc_spline.h>
#include <core/util/image/kzc_image.h>
#include <core/util/font/kzc_font.h>
#include <core/util/font/kzc_bitmap_font.h>
#include <core/util/font/kzc_freetype_font.h>
#include <core/util/font/kzc_truetype.h>
#include <core/debug/kzc_log.h>

#include <system/wrappers/kzs_opengl_base.h>
#include <system/debug/kzs_counter.h>
#include <system/time/kzs_tick.h>


/** Key structure for storing project objects. */
struct KzuProjectObjectKey
{
    enum KzuProjectObjectType type; /**< Type of the object. */
    kzString path; /**< Path of the object. */
};

/** Structure for project time measurement info. Used to detect time consumption of various resources. */
struct KzuProjectMeasurementInfo
{
    kzUint compressedImageCumulativeTime; /**< Compressed image cumulative time. */
    kzUint imageCumulativeTime; /**< Image cumulative time. */
    kzUint meshCumulativeTime; /**< Mesh cumulative time. */
    kzUint animationCumulativeTime; /**< Animation cumulative time. */
    kzUint objectCumulativeTime; /**< Object node cumulative time. */
    kzUint materialTypeCumulativeTime; /**< Material type cumulative time. */
};

struct KzuProject
{
    struct KzcHashMap* objects; /**< Project objects. <KzuProjectObjectKey, void>. */
    struct KzcHashMap* objectPaths; /**< Paths of project objects. <void, kzString>. */    
    kzInt anonymousId; /**< Automatically incrementing unique id for anonymous objects. */

    struct KzuPropertyManager* propertyManager; /**< Property manager. */
    struct KzuMessageDispatcher* messageDispatcher; /**< Message dispatcher. */

    struct KzcResourceManager* resourceManager; /**< Resource manager for the project. */
    struct KzuObjectSource* rootSource; /**< Root object source for this project. */
    struct KzuBinaryDirectory* directory; /**< Binary directory associated with the project. */

    struct KzuUiManager* uiManager; /**< User interface project. */
    
    struct KzcHashSet* permanentFilePaths; /**< Set of file paths that are marked as permanent, i.e. not deleted in ProjectClear. */

    struct KzcBitmapFontSystem* bitmapFontSystem; /** Bitmap font system used for loading bitmap fonts. */
    struct KzcTruetypeSystem* truetypeSystem; /** TrueType font system used for loading TrueType fonts. */
    kzBool defaultTruetypeSystem; /** Flag to indicate that the project uses default TrueType font system instead of custom one. */

    kzBool measurementEnabled; /**< Project time measurement enabled. */
    struct KzuProjectMeasurementInfo measurementInfo; /**< Project time measurement infos. */
};


/** Hash code calculation function for project object keys. */
static kzU32 kzuHashCodeFromObjectKey_internal(const void* pointer)
{
    struct KzuProjectObjectKey* objectKey;
    kzU32 typeHash;
    kzU32 pathHash;

    kzsAssert(pointer != KZ_NULL);

    objectKey = (struct KzuProjectObjectKey*)pointer;
    typeHash = kzcHashCodeFromInt(&objectKey->type);
    pathHash = kzcHashCodeFromString(objectKey->path);

    return typeHash ^ pathHash;
}

/** Comparison function for project object keys. */
static kzInt kzuCompareObjectKeys_internal(const void* first, const void* second)
{
    kzInt result;

    struct KzuProjectObjectKey* objectKeyFirst;
    struct KzuProjectObjectKey* objectKeySecond;
    kzInt typeResult;
    kzInt a;
    kzInt b;

    kzsAssert(first != KZ_NULL);
    kzsAssert(second != KZ_NULL);

    objectKeyFirst = (struct KzuProjectObjectKey*)first;
    objectKeySecond = (struct KzuProjectObjectKey*)second;
    a = *((kzInt*)&objectKeyFirst->type);
    b = *((kzInt*)&objectKeySecond->type);
    typeResult = ((a < b) ? -1 : ((a > b) ? 1 : 0));

    if (typeResult == 0)
    {
        kzInt pathResult = kzcStringCompare(objectKeyFirst->path, objectKeySecond->path);
        result = pathResult;
    }
    else
    {
        result = typeResult;
    }

    return result;
}

kzsError kzuProjectCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                          struct KzuBinaryDirectory* directory, struct KzuProject** out_project)
{
    kzsError result;
    struct KzuProject* project;

    /* Hash map configuration for project object keys. */
    struct KzcHashMapConfiguration objectMapConfiguration = {kzuHashCodeFromObjectKey_internal, kzuCompareObjectKeys_internal};

    result = kzcMemoryAllocVariable(memoryManager, project, "Project");
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, objectMapConfiguration, &project->objects);
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &project->objectPaths);
    kzsErrorForward(result);

    project->propertyManager = propertyManager;
    project->messageDispatcher = messageDispatcher;

    project->directory = directory;
    project->anonymousId = 0;
    project->bitmapFontSystem = KZ_NULL;
    project->truetypeSystem = KZ_NULL;
    project->defaultTruetypeSystem = KZ_TRUE;

    project->measurementEnabled = KZ_FALSE;
    project->measurementInfo.compressedImageCumulativeTime = 0;
    project->measurementInfo.imageCumulativeTime = 0;
    project->measurementInfo.meshCumulativeTime = 0;
    project->measurementInfo.animationCumulativeTime = 0;
    project->measurementInfo.objectCumulativeTime = 0;
    project->measurementInfo.materialTypeCumulativeTime = 0;

    result = kzcResourceManagerCreate(memoryManager, &project->resourceManager);
    kzsErrorForward(result);

    /* Create root object source for project. */
    result = kzuRootObjectSourceCreate(memoryManager, &project->rootSource);
    kzsErrorForward(result);

    result = kzuUiManagerCreate(memoryManager, &project->uiManager);
    kzsErrorForward(result);

    result = kzcHashSetCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &project->permanentFilePaths);
    kzsErrorForward(result);

    *out_project = project;
    kzsSuccess();
}

static kzsError kzuProjectClear_internal(const struct KzuProject* project, kzBool cached)
{
    kzsError result;
    struct KzuPropertyManager* propertyManager = kzuProjectGetPropertyManager(project);

    kzsAssert(kzcIsValidPointer(project));

    /* Delete all project properties. */
    result = kzuPropertyManagerRemoveProperties(project->propertyManager, project);
    kzsErrorForward(result);

    /* Delete almost all objects in the project. */
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(project->objects);

        while (kzcHashMapIterate(it))
        {
            struct KzuProjectObjectKey* objectKey = (struct KzuProjectObjectKey*)kzcHashMapIteratorGetKey(it);

            kzsAssert(kzcIsValidPointer(objectKey));

            if (kzcHashSetContains(project->permanentFilePaths, objectKey->path) == cached)
            {
                if (objectKey->type != KZU_PROJECT_OBJECT_TYPE_PROPERTY_TYPE &&
                    objectKey->type != KZU_PROJECT_OBJECT_TYPE_UI_COMPONENT_TYPE &&
                    objectKey->type != KZU_PROJECT_OBJECT_TYPE_FONT)
                {
                    result = kzuProjectDeleteObject(project, objectKey->type, objectKey->path);
                    kzsErrorForward(result);
                }
            }
        }
    }

    /* Delete rest of the objects. */
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(project->objects);

        while (kzcHashMapIterate(it))
        {
            struct KzuProjectObjectKey* objectKey = (struct KzuProjectObjectKey*)kzcHashMapIteratorGetKey(it);

            kzsAssert(kzcIsValidPointer(objectKey));

            if (kzcHashSetContains(project->permanentFilePaths, objectKey->path) == cached)
            {
                if (objectKey->type == KZU_PROJECT_OBJECT_TYPE_PROPERTY_TYPE ||
                    objectKey->type == KZU_PROJECT_OBJECT_TYPE_UI_COMPONENT_TYPE ||
                    objectKey->type == KZU_PROJECT_OBJECT_TYPE_FONT)
                {
                    if (objectKey->type == KZU_PROJECT_OBJECT_TYPE_PROPERTY_TYPE)
                    {
                        struct KzuPropertyType* propertyType = (struct KzuPropertyType*)kzcHashMapIteratorGetValue(it);
                        result = kzuPropertyManagerUnregisterPropertyType(propertyManager, propertyType);
                        kzsErrorForward(result);
                    }

                    result = kzuProjectDeleteObject(project, objectKey->type, objectKey->path);
                    kzsErrorForward(result);
                }
            }
        }
    }

    {
        /* Delete all object containers in the project. */
        struct KzcHashMapIterator it = kzcHashMapGetIterator(project->objects);

        while (kzcHashMapIterate(it))
        {
            struct KzuProjectObjectKey* objectKey = (struct KzuProjectObjectKey*)kzcHashMapIteratorGetKey(it);

            kzsAssert(kzcIsValidPointer(objectKey));

            if (kzcHashSetContains(project->permanentFilePaths, objectKey->path) == cached)
            {
                void* value = kzcHashMapIteratorGetValue(it);

                result = kzcHashMapRemove(project->objects, objectKey);
                kzsErrorForward(result);

                result = kzcStringDelete((kzMutableString)objectKey->path);
                kzsErrorForward(result);

                result = kzcMemoryFreeVariable(objectKey);
                kzsErrorForward(result);

                result = kzcHashMapRemove(project->objectPaths, value);
                kzsErrorForward(result);

                if (value != kzuProjectGetRootSource(project))
                {
                    kzsAssert(kzcIsValidPointer(value));

                    /* TODO: Check if this should use kzuProjectDeleteObject */
                    result = kzcMemoryFreeVariable(value);
                    kzsErrorForward(result);
                }
            }
        }
    }

    kzsSuccess();
}

kzsError kzuProjectClear(const struct KzuProject* project)
{
    kzsError result;

    result = kzuProjectClear_internal(project, KZ_FALSE);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuProjectClearCache(const struct KzuProject* project)
{
    kzsError result;

    result = kzuProjectClear_internal(project, KZ_TRUE);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuProjectDelete(struct KzuProject* project)
{
    kzsError result;
  
    kzsAssert(kzcIsValidPointer(project));
    
    kzcResourceManagerRemoveRendererReferences(project->resourceManager);

    kzcHashSetClear(project->permanentFilePaths);

    result = kzuProjectClear(project);
    kzsErrorForward(result);

    result = kzcHashSetDelete(project->permanentFilePaths);
    kzsErrorForward(result);

    result = kzuUiManagerDelete(project->uiManager);
    kzsErrorForward(result);

    result = kzcHashMapDelete(project->objects);
    kzsErrorForward(result);

    result = kzcHashMapDelete(project->objectPaths);
    kzsErrorForward(result);

    result = kzuObjectSourceDelete(project->rootSource);
    kzsErrorForward(result);

    if (project->bitmapFontSystem != KZ_NULL)
    {
        result = kzcBitmapFontSystemDelete(project->bitmapFontSystem);
        kzsErrorForward(result);
    }

    if (project->truetypeSystem != KZ_NULL && project->defaultTruetypeSystem)
    {
        result = kzcTruetypeSystemDelete(project->truetypeSystem);
        kzsErrorForward(result);
    }

    result = kzcResourceManagerDelete(project->resourceManager);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(project);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzcResourceManager* kzuProjectGetResourceManager(const struct KzuProject* project)
{
    kzsAssert(kzcIsValidPointer(project));
    return project->resourceManager;
}

struct KzuPropertyManager* kzuProjectGetPropertyManager(const struct KzuProject* project)
{
    kzsAssert(kzcIsValidPointer(project));
    return project->propertyManager;
}

struct KzuMessageDispatcher* kzuProjectGetMessageDispatcher(const struct KzuProject* project)
{
    kzsAssert(kzcIsValidPointer(project));
    return project->messageDispatcher;
}

struct KzuBinaryDirectory* kzuProjectGetBinaryDirectory(const struct KzuProject* project)
{
    kzsAssert(kzcIsValidPointer(project));
    return project->directory;
}

static kzsError kzuProjectTransferData_internal(const struct KzuProject* project, enum KzuProjectObjectType type,
                                                void* object, void* storedObject)
{
    kzsError result;
    struct KzuPropertyManager* propertyManager;

    kzsAssert(kzcIsValidPointer(project));

    propertyManager = project->propertyManager;
    result = kzuPropertyManagerCopyProperties(propertyManager, object, storedObject);
    kzsErrorForward(result);

    result = kzuPropertyManagerRemoveProperties(propertyManager, object);
    kzsErrorForward(result);

    switch (type)
    {
        case KZU_PROJECT_OBJECT_TYPE_INVALID:
        {
            kzsAssert(KZ_FALSE);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_MATERIAL_TYPE:
        {
            struct KzuMaterialType* storedMaterialType = (struct KzuMaterialType*)storedObject;
            struct KzuMaterialType* materialType = (struct KzuMaterialType*)object;
            kzuMaterialTypeTransferData(storedMaterialType, materialType);
            result = kzcMemoryFreeVariable(materialType);
            kzsErrorForward(result);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_MATERIAL:
        {
            struct KzuMaterial* storedMaterial = (struct KzuMaterial*)storedObject;
            struct KzuMaterial* material = (struct KzuMaterial*)object;
            kzuMaterialTransferData(storedMaterial, material);
            result = kzcMemoryFreeVariable(material);
            kzsErrorForward(result);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE:
        {
            struct KzuObjectNode* storedObjectNode = (struct KzuObjectNode*)storedObject;
            struct KzuObjectNode* objectNode = (struct KzuObjectNode*)object;
            kzuObjectNodeTransferData(storedObjectNode, objectNode);
            result = kzcMemoryFreeVariable(objectNode);
            kzsErrorForward(result);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_SCENE:
        {
            struct KzuScene* storedScene = (struct KzuScene*)storedObject;
            struct KzuScene* scene = (struct KzuScene*)object;
            kzuSceneTransferData(storedScene, scene);
            result = kzcMemoryFreeVariable(scene);
            kzsErrorForward(result);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_TEXTURE:
        {
            struct KzcTexture* storedTexture = (struct KzcTexture*)storedObject;
            struct KzcTexture* texture = (struct KzcTexture*)object;
            result = kzcTextureTransferData(storedTexture, texture);
            kzsErrorForward(result);
            result = kzcMemoryFreeVariable(texture);
            kzsErrorForward(result);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_IMAGE:
        {
            struct KzcImage* storedImage = (struct KzcImage*)storedObject;
            struct KzcImage* image = (struct KzcImage*)object;
            kzcImageTransferData(storedImage, image);
            result = kzcMemoryFreeVariable(image);
            kzsErrorForward(result);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_COMPOSER:
        {
            struct KzuComposer* storedComposer = (struct KzuComposer*)storedObject;
            struct KzuComposer* composer = (struct KzuComposer*)object;
            kzuComposerTransferData(storedComposer, composer);
            result = kzcMemoryFreeVariable(composer);
            kzsErrorForward(result);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_MESH:
        {
            struct KzuMesh* storedMesh = (struct KzuMesh*)storedObject;
            struct KzuMesh* mesh = (struct KzuMesh*)object;
            kzuMeshTransferData(storedMesh, mesh);
            result = kzcMemoryFreeVariable(mesh);
            kzsErrorForward(result);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_OBJECT_SOURCE:
        {
            struct KzuObjectSource* objectSource = (struct KzuObjectSource*)object;

            if (objectSource != kzuProjectGetRootSource(project))
            {
                struct KzuObjectSource* storedObjectSource = (struct KzuObjectSource*)storedObject;
                kzuObjectSourceTransferData(storedObjectSource, objectSource);
                result = kzcMemoryFreeVariable(objectSource);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_PROPERTY_TYPE:
        {
            struct KzuPropertyType* storedPropertyType = (struct KzuPropertyType*)storedObject;
            struct KzuPropertyType* propertyType = (struct KzuPropertyType*)object;
            kzuPropertyTypeTransferData(storedPropertyType, propertyType);
            result = kzcMemoryFreeVariable(propertyType);
            kzsErrorForward(result);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_SHADER_SOURCE:
        {
            /* Do nothing, since shader sources are not stored in memory */
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_ANIMATION:
        {
            struct KzuAnimation* storedAnimation = (struct KzuAnimation*)storedObject;
            struct KzuAnimation* animation = (struct KzuAnimation*)object;
            kzuAnimationTransferData(storedAnimation, animation);
            result = kzcMemoryFreeVariable(animation);
            kzsErrorForward(result);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_ANIMATION_CLIP:
        {
            struct KzuAnimationClip* storedAnimationClip = (struct KzuAnimationClip*)storedObject;
            struct KzuAnimationClip* animationClip = (struct KzuAnimationClip*)object;
            kzuAnimationClipTransferData(storedAnimationClip, animationClip);
            result = kzcMemoryFreeVariable(animationClip);
            kzsErrorForward(result);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_TIMELINE_SEQUENCE:
        {
            struct KzuTimeLineSequence* storedSequence = (struct KzuTimeLineSequence*)storedObject;
            struct KzuTimeLineSequence* sequence = (struct KzuTimeLineSequence*)object;
            kzuTimeLineSequenceTransferData(storedSequence, sequence);
            result = kzcMemoryFreeVariable(sequence);
            kzsErrorForward(result);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_UI_COMPONENT_TYPE:
        {
            struct KzuUiComponentType* storedComponentType = (struct KzuUiComponentType*)storedObject;
            struct KzuUiComponentType* componentType = (struct KzuUiComponentType*)object;
            kzuUiComponentTypeTransferData(storedComponentType, componentType);
            result = kzcMemoryFreeVariable(componentType);
            kzsErrorForward(result);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_FONT:
        {
            struct KzcFont* storedFont = (struct KzcFont*)storedObject;
            struct KzcFont* font = (struct KzcFont*)object;
            kzcFontTransferData(storedFont, font);
            result = kzcMemoryFreeVariable(font);
            kzsErrorForward(result);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_TRANSITION:
        {
            struct KzuTransition* storedTransition = (struct KzuTransition*)storedObject;
            struct KzuTransition* transition = (struct KzuTransition*)object;
            kzuTransitionTransferData(storedTransition, transition);
            result = kzcMemoryFreeVariable(transition);
            kzsErrorForward(result);
            break;
        }
        case KZU_PROJECT_OBJECT_TYPE_SCRIPT:
        {
            struct KzuScript* storedScript = (struct KzuScript*)storedObject;
            struct KzuScript* script = (struct KzuScript*)object;
            kzuScriptTransferData(storedScript, script);
            result = kzcMemoryFreeVariable(script);
            kzsErrorForward(result);
            break;
        }
        case KZU_PROJECT_OBJECT_TYPE_TRAJECTORY:
        {
            struct KzcTrajectory* storedTrajectory = (struct KzcTrajectory*)storedObject;
            struct KzcTrajectory* trajectory = (struct KzcTrajectory*)object;
            kzcTrajectoryTransferData(storedTrajectory, trajectory);
            result = kzcMemoryFreeVariable(trajectory);
            kzsErrorForward(result);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_SPLINE:
        {
            struct KzcSpline* storedSpline = (struct KzcSpline*)storedObject;
            struct KzcSpline* spline = (struct KzcSpline*)object;
            kzcSplineTransferData(storedSpline, spline);
            result = kzcMemoryFreeVariable(spline);
            kzsErrorForward(result);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_PROPERTY_GROUP:
        {
            struct KzuPropertyGroup* storedGroup = (struct KzuPropertyGroup*)storedObject;
            struct KzuPropertyGroup* group = (struct KzuPropertyGroup*)object;
            kzuPropertyGroupTransferData(storedGroup, group);
            result = kzcMemoryFreeVariable(group);
            kzsErrorForward(result);
            break;
        }
    }

    kzsSuccess();
}

static void kzuProjectTakeObjectOwnership_internal(const struct KzuProject* project, enum KzuProjectObjectType type, void* object)
{
    switch (type)
    {
        case KZU_PROJECT_OBJECT_TYPE_INVALID:
        {
            kzsAssert(KZ_FALSE);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_MATERIAL_TYPE:
        {
            struct KzuMaterialType* materialType = (struct KzuMaterialType*)object;
            kzuMaterialTypeTakeOwnership(materialType);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_MATERIAL:
        {
            struct KzuMaterial* material = (struct KzuMaterial*)object;
            kzuMaterialTakeOwnership(material);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE:
        {
            struct KzuObjectNode* objectNode = (struct KzuObjectNode*)object;
            kzuObjectNodeTakeOwnership(objectNode);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_SCENE:
        {
            struct KzuScene* scene = (struct KzuScene*)object;
            kzuSceneTakeOwnership(scene);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_TEXTURE:
        {
            struct KzcTexture* texture = (struct KzcTexture*)object;
            kzcTextureTakeOwnership(texture);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_IMAGE:
        {
            struct KzcImage* image = (struct KzcImage*)object;
            kzcImageTakeOwnership(image);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_COMPOSER:
        {
            struct KzuComposer* composer = (struct KzuComposer*)object;
            kzuComposerTakeOwnership(composer);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_MESH:
        {
            struct KzuMesh* mesh = (struct KzuMesh*)object;
            kzuMeshTakeOwnership(mesh);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_OBJECT_SOURCE:
        {
            struct KzuObjectSource* objectSource = (struct KzuObjectSource*)object;

            if (objectSource != kzuProjectGetRootSource(project))
            {
                kzuObjectSourceTakeOwnership(objectSource);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_PROPERTY_TYPE:
        {
            struct KzuPropertyType* propertyType = (struct KzuPropertyType*)object;
            kzuPropertyTypeTakeOwnership(propertyType);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_SHADER_SOURCE:
        {
            /* Do nothing, since shader sources are not stored in memory */
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_ANIMATION:
        {
            struct KzuAnimation* animation = (struct KzuAnimation*)object;
            kzuAnimationTakeOwnership(animation);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_ANIMATION_CLIP:
        {
            struct KzuAnimationClip* animationClip = (struct KzuAnimationClip*)object;
            kzuAnimationClipTakeOwnership(animationClip);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_TIMELINE_SEQUENCE:
        {
            struct KzuTimeLineSequence* sequence = (struct KzuTimeLineSequence*)object;
            kzuTimeLineSequenceTakeOwnership(sequence);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_UI_COMPONENT_TYPE:
        {
            struct KzuUiComponentType* componentType = (struct KzuUiComponentType*)object;
            kzuUiComponentTypeTakeOwnership(componentType);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_FONT:
        {
            struct KzcFont* font = (struct KzcFont*)object;
            kzcFontTakeOwnership(font);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_TRANSITION:
        {
            struct KzuTransition* transition = (struct KzuTransition*)object;
            kzuTransitionTakeOwnership(transition);
            break;
        }
        case KZU_PROJECT_OBJECT_TYPE_SCRIPT:
        {
            struct KzuScript* script = (struct KzuScript*)object;
            kzuScriptTakeOwnership(script);
            break;
        }
        case KZU_PROJECT_OBJECT_TYPE_TRAJECTORY:
        {
            struct KzcTrajectory* trajectory = (struct KzcTrajectory*)object;
            kzcTrajectoryTakeOwnership(trajectory);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_SPLINE:
        {
            struct KzcSpline* spline = (struct KzcSpline*)object;
            kzcSplineTakeOwnership(spline);
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_PROPERTY_GROUP:
        {
            struct KzuPropertyGroup* propertyGroup = (struct KzuPropertyGroup*)object;
            kzuPropertyGroupTakeOwnership(propertyGroup);
            break;
        }
    }
}

kzsError kzuProjectAddObject(const struct KzuProject* project, enum KzuProjectObjectType type, void* object, kzString path)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuProjectObjectKey* key;
    kzMutableString pathCopy;

    kzsAssert(kzcIsValidPointer(project));
    kzsAssert(object != KZ_NULL);
    kzsAssert(path != KZ_NULL);

    result = kzcStringCopy(memoryManager, path, &pathCopy);
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, key, "Project objects map key");
    kzsErrorForward(result);

    key->type = type;
    key->path = pathCopy;

    kzsErrorTest(!kzcHashMapContains(project->objects, key), KZU_ERROR_PROJECT_OBJECT_ALREADY_EXISTS,
                 "Object with the given path already exists in the project.");

    result = kzcHashMapPut(project->objects, key, object);
    kzsErrorForward(result);

    result = kzcHashMapPut(project->objectPaths, object, (kzMutableString)path);
    kzsErrorForward(result);

    kzuProjectTakeObjectOwnership_internal(project, type, object);

    kzsSuccess();
}

kzsError kzuProjectRemoveObject(const struct KzuProject* project, enum KzuProjectObjectType type, kzString path)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuProjectObjectKey* key;

    kzsAssert(kzcIsValidPointer(project));
    kzsAssert(path != KZ_NULL);

    result = kzcMemoryAllocVariable(memoryManager, key, "Project objects map key");
    kzsErrorForward(result);

    key->type = type;
    key->path = path;

    kzsErrorTest(kzcHashMapContains(project->objects, key), KZU_ERROR_PROJECT_OBJECT_NOT_FOUND,
        "Object that was tried to be removed from project wasn't found");

    {
        struct KzuProjectObjectKey* storedKey = (struct KzuProjectObjectKey*)kzcHashMapGetStoredKey(project->objects, key);
        void* object;
        
        kzsAssert(storedKey != KZ_NULL);

        if (kzcHashMapGet(project->objects, key, &object))
        {
            result = kzcHashMapRemove(project->objectPaths, object);
            kzsErrorForward(result);
        }

        result = kzcHashMapRemove(project->objects, storedKey);
        kzsErrorForward(result);

        result = kzcStringDelete((kzMutableString)storedKey->path);
        kzsErrorForward(result);
        result = kzcMemoryFreeVariable(storedKey);
        kzsErrorForward(result);
    }
    
    result = kzcMemoryFreeVariable(key);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuProjectAddAnonymousObject(struct KzuProject* project, enum KzuProjectObjectType type, void* object)
{

    kzsError result;
    struct KzcMemoryManager* memoryManager;
    kzMutableString path;
    struct KzuProjectObjectKey* key;

    kzsAssert(kzcIsValidPointer(project));
    kzsAssert(object != KZ_NULL);

    memoryManager = kzcMemoryGetManager(project);

    result = kzcIntToString(memoryManager, project->anonymousId++, &path);
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, key, "Project objects anonymous map key");
    kzsErrorForward(result);

    key->type = type;
    key->path = path;

    result = kzcHashMapPut(project->objects, key, object);
    kzsErrorForward(result);

    result = kzcHashMapPut(project->objectPaths, object, path);
    kzsErrorForward(result);

    kzuProjectTakeObjectOwnership_internal(project, type, object);

    kzsSuccess();
}

kzsError kzuProjectObjectLoaded(const struct KzuProject* project, enum KzuProjectObjectType type, void* object,
                                kzString path, kzBool storeToResourceCache, void** out_storedObject)
{
    kzsError result;
    struct KzuProjectObjectKey key;
    void* storedObject;

    kzsAssert(kzcIsValidPointer(project));
    kzsAssert(object != KZ_NULL);
    kzsAssert(path != KZ_NULL);

    key.type = type;
    key.path = path;

    if (kzcHashMapGet(project->objects, &key, &storedObject))
    {
        result = kzuProjectTransferData_internal(project, type, object, storedObject);
        kzsErrorForward(result);

        kzuProjectTakeObjectOwnership_internal(project, type, storedObject);
    }
    else
    {
        storedObject = object;
        result = kzuProjectAddObject(project, type, storedObject, path);
        kzsErrorForward(result);
    }

    if(storeToResourceCache)
    {
        result = kzuProjectAddPermanentFilePath(project, path);
        kzsErrorForward(result);
    }

    *out_storedObject = storedObject;
    kzsSuccess();
}

static kzsError kzuProjectRemoveObjectFromStorage_internal(const struct KzuProject* project, enum KzuProjectObjectType type, kzString path, const void* object)
{
    kzsError result;
    struct KzuProjectObjectKey objectKeyTemp;
    struct KzuProjectObjectKey* objectKey;
    objectKeyTemp.path = path;
    objectKeyTemp.type = type;

    objectKey = (struct KzuProjectObjectKey*)kzcHashMapGetStoredKey(project->objects, &objectKeyTemp);
    kzsAssert(objectKey != KZ_NULL);

    result = kzcHashMapRemove(project->objects, objectKey);
    kzsErrorForward(result);

    result = kzcHashMapRemove(project->objectPaths, object);
    kzsErrorForward(result);

    result = kzcStringDelete((kzMutableString)objectKey->path);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(objectKey);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuProjectDeleteObject(const struct KzuProject* project, enum KzuProjectObjectType type, kzString path)
{
    kzsError result;
    void* object;
    kzBool remove = KZ_FALSE;

    kzsAssert(kzcIsValidPointer(project));
    kzsAssert(path != KZ_NULL);

    object = kzuProjectGetObject(project, type, path);
    kzsErrorTest(object != KZ_NULL, KZU_ERROR_PROJECT_OBJECT_NOT_FOUND, "Object with given name and type was not found from project.");

    switch (type)
    {
        case KZU_PROJECT_OBJECT_TYPE_IMAGE:
        {
            struct KzcImage* image = (struct KzcImage*)object;
            if (kzcImageIsValid(image))
            {
                result = kzcImageDelete(image);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_SCENE:
        {
            struct KzuScene* scene = (struct KzuScene*)object;
            if (kzuSceneIsValid(scene))
            {
                result = kzuSceneDelete(scene);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_PROPERTY_TYPE:
        {
            struct KzuPropertyType* propertyType = (struct KzuPropertyType*)object;
            if (kzuPropertyTypeIsValid(propertyType))
            {
                result = kzuPropertyTypeDelete(propertyType);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_MATERIAL_TYPE:
        {
            struct KzuMaterialType* materialType = (struct KzuMaterialType*)object;
            if (kzuMaterialTypeIsValid(materialType))
            {
                result = kzuMaterialTypeDelete(materialType);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_MATERIAL:
        {
            struct KzuMaterial* material = (struct KzuMaterial*)object;
            if (kzuMaterialIsValid(material))
            {
                result = kzuMaterialDelete(material);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE:
        {
            struct KzuObjectNode* objectNode = (struct KzuObjectNode*)object;
            if (kzuObjectNodeIsValid(objectNode))
            {
                /* TODO: This should not be done here! Add components to project separately!. */
                if (kzuObjectNodeGetType(objectNode) == KZU_OBJECT_TYPE_UI_COMPONENT)
                {
                    struct KzuUiComponentNode* componentNode = kzuUiComponentNodeFromObjectNode(objectNode);
                    
                    result = kzuUiComponentNodeUninitialize(componentNode);
                    kzsErrorForward(result);
                }

                result = kzuObjectNodeDelete(objectNode);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_MESH:
        {
            struct KzuMesh* mesh = (struct KzuMesh*)object;
            if (kzuMeshIsValid(mesh))
            {
                result = kzuMeshDelete(mesh);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_TEXTURE:
        {
            struct KzcTexture* texture = (struct KzcTexture*)object;
            if (kzcTextureIsValid(texture))
            {
                result = kzcTextureDelete(texture);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_SHADER_SOURCE:
        {
            result = kzcStringDelete((kzMutableString)object);
            kzsErrorForward(result);

            remove = KZ_TRUE;
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_COMPOSER:
        {
            struct KzuComposer* composer = (struct KzuComposer*)object;
            if (kzuComposerIsValid(composer))
            {
                result = kzuComposerDelete(composer);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_OBJECT_SOURCE:
        {
            struct KzuObjectSource* objectSource = (struct KzuObjectSource*)object;
            if (objectSource != kzuProjectGetRootSource(project) && kzuObjectSourceIsValid(objectSource))
            {
                result = kzuObjectSourceDelete(objectSource);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_ANIMATION:
        {
            struct KzuAnimation* animation = (struct KzuAnimation*)object;
            if (kzuAnimationIsValid(animation))
            {
                result = kzuAnimationDelete(animation);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_ANIMATION_CLIP:
        {
            struct KzuAnimationClip* clip = (struct KzuAnimationClip*)object;
            if (kzuAnimationClipIsValid(clip))
            {
                result = kzuAnimationClipDelete(clip);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_TIMELINE_SEQUENCE:
        {
            struct KzuTimeLineSequence* sequence = (struct KzuTimeLineSequence*)object;
            if (kzuTimeLineSequenceIsValid(sequence))
            {
                result = kzuTimeLineSequenceDelete(sequence);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_UI_COMPONENT_TYPE:
        {
            struct KzuUiComponentType* componentType = (struct KzuUiComponentType*)object;
            if (kzuUiComponentTypeIsValid(componentType))
            {
                result = kzuUiComponentTypeDelete(componentType);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_FONT:
        {
            struct KzcFont* font = (struct KzcFont*)object;
            if (kzcFontIsValid(font))
            {
                result = kzcFontDelete(font);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_TRANSITION:
        {
            struct KzuTransition* transition = (struct KzuTransition*)object;
            if (kzuTransitionIsValid(transition))
            {
                result = kzuTransitionDelete(transition);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_SCRIPT:
        {
            struct KzuScript* script = (struct KzuScript*)object;
            if (kzuScriptIsValid(script))
            {
                result = kzuScriptDelete(script);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_TRAJECTORY:
        {
            struct KzcTrajectory* trajectory = (struct KzcTrajectory*)object;
            if (kzcTrajectoryIsValid(trajectory))
            {
                result = kzcTrajectoryDelete(trajectory);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_PROPERTY_GROUP:
        {
            struct KzuPropertyGroup* group = (struct KzuPropertyGroup*)object;
            if (kzuPropertyGroupIsValid(group))
            {
                result = kzuPropertyGroupDelete(group);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_SPLINE:
        {
            struct KzcSpline* spline = (struct KzcSpline*)object;
            if (kzcSplineIsValid(spline))
            {
                result = kzcSplineDelete(spline);
                kzsErrorForward(result);
            }
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_INVALID:
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid project object type");
        }
    }

    /* Remove object */
    if (remove)
    {
        result = kzuProjectRemoveObjectFromStorage_internal(project, type, path, object);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

void* kzuProjectGetObject(const struct KzuProject* project, enum KzuProjectObjectType type, kzString path)
{
    void* object;
    struct KzuProjectObjectKey key;

    kzsAssert(kzcIsValidPointer(project));
    kzsAssert(path != KZ_NULL);

    key.type = type;
    key.path = path;

    if (!kzcHashMapGet(project->objects, &key, &object))
    {
        kzString shortcutTargetPath = kzuBinaryDirectoryGetActualPath(project->directory, path);
        if (shortcutTargetPath != path)
        {
            object = kzuProjectGetObject(project, type, shortcutTargetPath);
        }
    }

    return object;
}

kzString kzuProjectGetObjectPath(const struct KzuProject* project, const void* object)
{
    kzString path;

    kzsAssert(kzcIsValidPointer(project));

    if (!kzcHashMapGet(project->objectPaths, object, (void**)&path))
    {
        path = KZ_NULL;
    }

    return path;
}

kzsError kzuProjectSetViewport(const struct KzuProject* project, kzFloat x, kzFloat y, kzFloat width, kzFloat height, kzFloat aspectRatio,
                               kzBool relative)
{
    kzsError result;
    struct KzuPropertyManager* propertyManager = kzuProjectGetPropertyManager(project);

    result = kzuPropertyManagerSetFloat(propertyManager, project, KZU_PROPERTY_TYPE_WINDOW_X, x);
    kzsErrorForward(result);
    result = kzuPropertyManagerSetFloat(propertyManager, project, KZU_PROPERTY_TYPE_WINDOW_Y, y);
    kzsErrorForward(result);
    result = kzuPropertyManagerSetFloat(propertyManager, project, KZU_PROPERTY_TYPE_WINDOW_WIDTH, width);
    kzsErrorForward(result);
    result = kzuPropertyManagerSetFloat(propertyManager, project, KZU_PROPERTY_TYPE_WINDOW_HEIGHT, height);
    kzsErrorForward(result);
    result = kzuPropertyManagerSetFloat(propertyManager, project, KZU_PROPERTY_TYPE_WINDOW_ASPECT_RATIO, aspectRatio);
    kzsErrorForward(result);
    result = kzuPropertyManagerSetInt(propertyManager, project, KZU_PROPERTY_TYPE_WINDOW_METRICS_TYPE, relative ? 1 : 0);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuObjectSource* kzuProjectGetRootSource(const struct KzuProject* project)
{
    kzsAssert(kzcIsValidPointer(project));

    return project->rootSource;
}

kzsError kzuProjectAddPermanentFilePath(const struct KzuProject* project, kzString filePath)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(project));

    if(!kzcHashSetContains(project->permanentFilePaths, filePath))
    {
        result = kzcHashSetAdd(project->permanentFilePaths, (kzMutableString)filePath);
        kzsErrorForward(result);
    }

    kzsCounterIncrease("addFilePermanent");

    kzsSuccess();
}

struct KzcHashSet* kzuProjectGetPermanentFilePaths(const struct KzuProject* project)
{
    kzsAssert(kzcIsValidPointer(project));
    return project->permanentFilePaths;
}

kzBool kzuProjectHasPermanentFilePath(const struct KzuProject* project, kzString filePath)
{
    kzsAssert(kzcIsValidPointer(project));

    return kzcHashSetContains(project->permanentFilePaths, filePath);
}

kzsError kzuProjectRemovePermanentFilePath(const struct KzuProject* project, kzString filePath)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(project));

    result = kzcHashSetRemove(project->permanentFilePaths, (kzMutableString)filePath);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuPropertyType* kzuProjectGetPropertyType(const struct KzuProject* project, kzString name)
{
    struct KzuPropertyType* propertyType;

    kzsAssert(kzcIsValidPointer(project));
    kzsAssert(name != KZ_NULL);

    propertyType = kzuFixedPropertiesFindPropertyType(name);

    /* First try to find the property type from fixed property types. */
    if (propertyType == KZ_NULL)
    {
        /* If not found, get it as non-fixed property type. */
        propertyType = (struct KzuPropertyType*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_PROPERTY_TYPE, name);
    }

    return propertyType;
}

struct KzuProjectObjectIterator kzuProjectGetObjectsOfType(const struct KzuProject* project, enum KzuProjectObjectType type)
{
    struct KzuProjectObjectIterator iterator;

    kzsAssert(kzcIsValidPointer(project));

    iterator.type_private = type;
    iterator.mapIterator_private = kzcHashMapGetIterator(project->objects);

    return iterator;
}

kzBool kzuProjectObjectIterate_private(struct KzuProjectObjectIterator* iterator)
{
    kzBool result;
    kzBool correctType;

    kzsAssert(iterator != KZ_NULL);

    do 
    {
        struct KzuProjectObjectKey* key;
        result = kzcHashMapIterate(iterator->mapIterator_private);
        if (!result)
        {
            break;
        }
        key = (struct KzuProjectObjectKey*)kzcHashMapIteratorGetKey(iterator->mapIterator_private);
        kzsAssert(kzcIsValidPointer(key));
        correctType = (key->type == iterator->type_private);
    } while (!correctType);

    return result;
}

void* kzuProjectObjectIteratorGetValue_private(const struct KzuProjectObjectIterator* iterator)
{
    kzsAssert(iterator != KZ_NULL);

    return kzcHashMapIteratorGetValue(iterator->mapIterator_private);
}

struct KzuUiManager* kzuProjectGetUiProject(const struct KzuProject* project)
{
    kzsAssert(kzcIsValidPointer(project));
    return project->uiManager;
}

kzsError kzuProjectGetBitmapFontSystem(struct KzuProject* project, struct KzcBitmapFontSystem** out_bitmapFontSystem)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(project));

    if (project->bitmapFontSystem == KZ_NULL)
    {
        struct KzcBitmapFontSystem* bitmapFontSystem;

        result = kzcBitmapFontSystemCreate(kzcMemoryGetManager(project), &bitmapFontSystem);
        kzsErrorForward(result);
            
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
        {
            struct KzuMaterialType* fontMaterialType;

            result = kzuProjectLoaderLoadMaterialType(project, "Materials/FontSystemTruetype", &fontMaterialType);
            kzsExceptionCatch(result, KZU_EXCEPTION_FILE_NOT_FOUND)
            {
                kzsErrorThrow(KZU_ERROR_PROJECT_CORE_ASSET_NOT_FOUND, "CoreAssets didn't contain Truetype font");
            }
            result = kzuProjectAddPermanentFilePath(project, "Materials/FontSystemTruetype");
            kzsErrorForward(result);

            kzcBitmapFontSystemSetShader(bitmapFontSystem, kzuMaterialTypeGetShaderProgram(fontMaterialType));
        }
#endif

        project->bitmapFontSystem = bitmapFontSystem;
    }

    *out_bitmapFontSystem = project->bitmapFontSystem;
    kzsSuccess();
}

kzsError kzuProjectSetTruetypeSystem(struct KzuProject* project, struct KzcTruetypeSystem* truetypeSystem)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(project));

    if (project->truetypeSystem != KZ_NULL && project->defaultTruetypeSystem)
    {
        result = kzcTruetypeSystemDelete(project->truetypeSystem);
        kzsErrorForward(result);
    }

    project->truetypeSystem = truetypeSystem;
    if (truetypeSystem != KZ_NULL)
    {
        project->defaultTruetypeSystem = KZ_FALSE;
    }
    else
    {
        project->defaultTruetypeSystem = KZ_TRUE;
    }

    kzsSuccess();
}

kzsError kzuProjectGetTruetypeSystem(struct KzuProject* project, struct KzcTruetypeSystem** out_truetypeSystem)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(project));

    if (project->truetypeSystem == KZ_NULL)
    {
        struct KzcFreetypeSystem* freetypeSystem;

        result = kzcFreetypeSystemCreate(kzcMemoryGetManager(project), project->resourceManager, &freetypeSystem);
        kzsErrorForward(result);

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
        {
            struct KzuMaterialType* fontMaterialType;
            
            result = kzuProjectLoaderLoadMaterialType(project, "Materials/FontSystemTruetype", &fontMaterialType);
            kzsExceptionCatch(result, KZU_EXCEPTION_FILE_NOT_FOUND)
            {
                kzsErrorThrow(KZU_ERROR_PROJECT_CORE_ASSET_NOT_FOUND, "CoreAssets didn't contain Truetype font");
            }

            result = kzuProjectAddPermanentFilePath(project, "Materials/FontSystemTruetype");
            kzsErrorForward(result);

            kzcFreetypeSystemSetShader(freetypeSystem, kzuMaterialTypeGetShaderProgram(fontMaterialType));
        }
#endif

        project->truetypeSystem = kzcFreetypeSystemToTruetypeSystem(freetypeSystem);
    }

    *out_truetypeSystem = project->truetypeSystem;
    kzsSuccess();
}

void kzuProjectSetTimeMeasurementEnabled(struct KzuProject* project, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(project));
    project->measurementEnabled = enabled;
}

kzBool kzuProjectIsTimeMeasurementEnabled(const struct KzuProject* project)
{
    kzsAssert(kzcIsValidPointer(project));
    return project->measurementEnabled;
}

void kzuProjectAddMeasurementCumulativeTime_private(struct KzuProject* project, kzUint startTime, enum KzuProjectMeasurementType measurementType)
{
    kzsAssert(kzcIsValidPointer(project));

    if(project->measurementEnabled)
    {
        kzUint currentTime;
        currentTime = kzsTimeGetCurrentTimestamp();

        switch(measurementType)
        {
            case KZU_PROJECT_MEASUREMENT_IMAGE:
            {
                project->measurementInfo.imageCumulativeTime += (kzUint)(currentTime - startTime);
                break;
            }
            case KZU_PROJECT_MEASUREMENT_COMPRESSED_IMAGE:
            {
                project->measurementInfo.compressedImageCumulativeTime += (kzUint)(currentTime - startTime);
                break;
            }
            case KZU_PROJECT_MEASUREMENT_MESH:
            {
                project->measurementInfo.meshCumulativeTime += (kzUint)(currentTime - startTime);
                break;
            }
            case KZU_PROJECT_MEASUREMENT_ANIMATION:
            {
                project->measurementInfo.animationCumulativeTime += (kzUint)(currentTime - startTime);
                break;
            }
            case KZU_PROJECT_MEASUREMENT_OBJECT_NODE:
            {
                project->measurementInfo.objectCumulativeTime += (kzUint)(currentTime - startTime);
                break;
            }
            case KZU_PROJECT_MEASUREMENT_MATERIAL_TYPE:
            {
                project->measurementInfo.materialTypeCumulativeTime += (kzUint)(currentTime - startTime);
                break;
            }
            default:
            {
                kzsAssertText(KZ_FALSE, "Invalid type given for cumulative function calculation");
            }
        }
    }
}

kzsError kzuProjectPrintTimeMeasurementInfo(const struct KzuProject* project)
{
    kzsError result;
    kzMutableString infoString;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);

    result = kzcStringFormat(memoryManager, "Image time: %d", &infoString, project->measurementInfo.imageCumulativeTime);
    kzsErrorForward(result);
    kzsLog(KZS_LOG_LEVEL_DETAIL, infoString);
    result = kzcStringDelete(infoString);
    kzsErrorForward(result);

    result = kzcStringFormat(memoryManager, "Compressed image time: %d", &infoString, project->measurementInfo.compressedImageCumulativeTime);
    kzsErrorForward(result);
    kzsLog(KZS_LOG_LEVEL_DETAIL, infoString);
    result = kzcStringDelete(infoString);
    kzsErrorForward(result);

    result = kzcStringFormat(memoryManager, "Mesh time: %d", &infoString, project->measurementInfo.meshCumulativeTime);
    kzsErrorForward(result);
    kzsLog(KZS_LOG_LEVEL_DETAIL, infoString);
    result = kzcStringDelete(infoString);
    kzsErrorForward(result);

    result = kzcStringFormat(memoryManager, "Animation time: %d", &infoString, project->measurementInfo.animationCumulativeTime);
    kzsErrorForward(result);
    kzsLog(KZS_LOG_LEVEL_DETAIL, infoString);
    result = kzcStringDelete(infoString);
    kzsErrorForward(result);

    result = kzcStringFormat(memoryManager, "Object time: %d", &infoString, project->measurementInfo.objectCumulativeTime);
    kzsErrorForward(result);
    kzsLog(KZS_LOG_LEVEL_DETAIL, infoString);
    result = kzcStringDelete(infoString);
    kzsErrorForward(result);

    kzsSuccess();
}

kzUint kzuProjectGetTimeMeasurementInfo(const struct KzuProject* project, enum KzuProjectMeasurementType measurementType)
{
    kzUint value = 0;
    switch(measurementType)
    {
        case KZU_PROJECT_MEASUREMENT_MESH:
        {
            value = project->measurementInfo.meshCumulativeTime;
            break;
        }
        case KZU_PROJECT_MEASUREMENT_IMAGE:
        {
            value = project->measurementInfo.imageCumulativeTime;
            break;
        }
        case KZU_PROJECT_MEASUREMENT_COMPRESSED_IMAGE:
        {
            value = project->measurementInfo.compressedImageCumulativeTime;
            break;
        }
        case KZU_PROJECT_MEASUREMENT_ANIMATION:
        {
            value = project->measurementInfo.animationCumulativeTime;
            break;
        }
        case KZU_PROJECT_MEASUREMENT_OBJECT_NODE:
        {
            value = project->measurementInfo.objectCumulativeTime;
            break;
        }
        case KZU_PROJECT_MEASUREMENT_MATERIAL_TYPE:
        {
            value = project->measurementInfo.materialTypeCumulativeTime;
            break;
        }
        default:
        {
            kzsAssertText(KZ_FALSE, "Invalid measurement given");
        }
    }
    return value;
}
