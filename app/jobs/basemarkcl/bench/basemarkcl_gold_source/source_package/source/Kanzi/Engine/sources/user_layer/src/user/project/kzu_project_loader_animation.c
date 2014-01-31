/**
* \file
* Project loader for animation files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_animation.h"

#include "kzu_project_loader.h"
#include "kzu_project_loader_general.h"
#include "kzu_project_loader_scene.h"
#include "kzu_project_loader_light.h"
#include "kzu_project_loader_material.h"
#include "kzu_project_loader_property.h"
#include "kzu_project_loader_texture.h"
#include "kzu_project_loader_object_node.h"
#include "kzu_project_loader_composer.h"
#include "kzu_project_loader_property_group.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/binary/kzu_binary_util.h>
#include <user/binary/kzu_binary_base.h>
#include <user/animation/kzu_animation.h>
#include <user/animation/kzu_animation_key.h>
#include <user/animation/kzu_animation_clip.h>
#include <user/animation/kzu_time_line_entry.h>
#include <user/animation/kzu_time_line_sequence.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_property_group.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_light.h>
#include <user/material/kzu_material.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_byte_buffer.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector2.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/util/string/kzc_string.h>
#include <core/util/collection/kzc_hash_map.h>

#include <system/debug/kzs_log.h>
#include <system/time/kzs_tick.h>


/** Animation interpolation type. */
enum KzuBinaryKeyFrameInterpolationType
{
    KZU_BINARY_KEY_FRAME_INTERPOLATION_TYPE_STEP = 0,
    KZU_BINARY_KEY_FRAME_INTERPOLATION_TYPE_LINEAR = 1,
    KZU_BINARY_KEY_FRAME_INTERPOLATION_TYPE_BEZIER = 2,
    KZU_BINARY_KEY_FRAME_INTERPOLATION_TYPE_HERMITE = 3
};

/** Type of the animation container. */
enum KzuBinaryAnimationType
{
    KZU_BINARY_ANIMATION_TYPE_FLOAT = 0,
    KZU_BINARY_ANIMATION_TYPE_TEXTURE = 1,
    KZU_BINARY_ANIMATION_TYPE_MATRIX4X4 = 2
};

/** Type of playback of time line sequence. */
enum KzuBinaryPlaybackMode
{
    KZU_BINARY_PLAYBACK_MODE_NORMAL = 0, 
    KZU_BINARY_PLAYBACK_MODE_REVERSE = 1,
    KZU_BINARY_PLAYBACK_MODE_PING_PONG = 2
};

/** Type of time line entry. */
enum KzuBinaryTimeLineEntryType
{
    KZU_BINARY_TIME_LINE_ENTRY_TYPE_TIME_LINE_SEQUENCE = 0, 
    KZU_BINARY_TIME_LINE_ENTRY_TYPE_ANIMATION_CLIP = 1 
};

/** Type of time line entry input. */
enum KzuBinaryTimeLineEntryInputType
{
    KZU_BINARY_TIME_LINE_ENTRY_INPUT_TYPE_TIME = 0,
    KZU_BINARY_TIME_LINE_ENTRY_INPUT_TYPE_PROPERTY = 1
};

static enum KzuTimeLineEntryPlaybackMode kzuProjectLoaderGetPlaybackMode_internal(enum KzuBinaryPlaybackMode type)
{
    enum KzuTimeLineEntryPlaybackMode resultType = KZU_TIME_LINE_ENTRY_PLAYBACK_MODE_NORMAL;
    switch(type)
    {
        case KZU_BINARY_PLAYBACK_MODE_NORMAL: resultType = KZU_TIME_LINE_ENTRY_PLAYBACK_MODE_NORMAL; break;
        case KZU_BINARY_PLAYBACK_MODE_REVERSE: resultType = KZU_TIME_LINE_ENTRY_PLAYBACK_MODE_REVERSE; break;
        case KZU_BINARY_PLAYBACK_MODE_PING_PONG: resultType = KZU_TIME_LINE_ENTRY_PLAYBACK_MODE_PINGPONG; break;
        default:
        {
            kzsAssert(KZ_FALSE);
            break;
        }
    }
    return resultType;
}

static enum KzuAnimationInterpolation kzuProjectLoaderGetInterpolation_internal(enum KzuBinaryKeyFrameInterpolationType interpolationType)
{
    enum KzuAnimationInterpolation interpolation = KZU_ANIMATION_INTERPOLATION_LINEAR;
    switch(interpolationType)
    {
        case KZU_BINARY_KEY_FRAME_INTERPOLATION_TYPE_STEP: interpolation = KZU_ANIMATION_INTERPOLATION_STEP; break;
        case KZU_BINARY_KEY_FRAME_INTERPOLATION_TYPE_LINEAR: interpolation = KZU_ANIMATION_INTERPOLATION_LINEAR; break;
        case KZU_BINARY_KEY_FRAME_INTERPOLATION_TYPE_BEZIER: interpolation = KZU_ANIMATION_INTERPOLATION_BEZIER; break;
        case KZU_BINARY_KEY_FRAME_INTERPOLATION_TYPE_HERMITE: interpolation = KZU_ANIMATION_INTERPOLATION_HERMITE; break;
        default:
        {
            kzsAssert(KZ_FALSE);
        }
    }
    return interpolation;
}

static kzsError kzuProjectLoaderLoadAnimationGetAnimationType_internal(const struct KzuPropertyType* propertyType, 
                                                                       enum KzuBinaryAnimationTargetAttribute targetAttribute,
                                                                       enum KzuBinaryAnimationType* out_animationType)
{
    enum KzuBinaryAnimationType animationType = KZU_BINARY_ANIMATION_TYPE_FLOAT;
    enum KzuPropertyDataType propertyDataType;

    propertyDataType = kzuPropertyTypeGetDataType(propertyType);
    KZ_UNUSED_PARAMETER(targetAttribute);

    switch(propertyDataType)
    {
        case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
        {
            /*
            TODO: Re-enable this when possible. E.g. Tool does not incorrectly feed this property.
            if(targetAttribute == KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_WHOLE_PROPERTY)
            {
                animationType = KZU_BINARY_ANIMATION_TYPE_MATRIX4X4;
            }
            */
            animationType = KZU_BINARY_ANIMATION_TYPE_FLOAT;
            break;
        }
        case KZU_PROPERTY_DATA_TYPE_TEXTURE:
        {
            animationType = KZU_BINARY_ANIMATION_TYPE_TEXTURE;
            break;
        }
        case KZU_PROPERTY_DATA_TYPE_FLOAT:
        case KZU_PROPERTY_DATA_TYPE_INT:
        case KZU_PROPERTY_DATA_TYPE_COLOR:
        case KZU_PROPERTY_DATA_TYPE_VECTOR2:
        case KZU_PROPERTY_DATA_TYPE_VECTOR3:
        case KZU_PROPERTY_DATA_TYPE_VECTOR4:
        case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
        case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
        case KZU_PROPERTY_DATA_TYPE_STRING:
        case KZU_PROPERTY_DATA_TYPE_LIGHT:
        {
            animationType = KZU_BINARY_ANIMATION_TYPE_FLOAT;
            break;
        }
        case KZU_PROPERTY_DATA_TYPE_BOOL:
        case KZU_PROPERTY_DATA_TYPE_VOID:
        {
            kzsErrorThrow(KZU_ERROR_INVALID_PROPERTY, "Bool or void property animation is not supported.");
        }
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid enum value.");
        }
    }
        
    *out_animationType = animationType;
    kzsSuccess();
}

kzsError kzuProjectLoaderLoadAnimationConvertAttribute(const struct KzuPropertyType* propertyType,
                                                       enum KzuBinaryAnimationTargetAttribute binaryTargetAttribute,
                                                       enum KzuAnimationTargetAttribute* out_targetAttribute)
{
    kzsError result;
    enum KzuAnimationTargetAttribute targetAttribute = KZU_ANIMATION_TARGET_NO_PROPERTY;
    enum KzuBinaryAnimationType animationType;
    enum KzuPropertyDataType propertyDataType;

    if (propertyType != KZ_NULL)
    {
        propertyDataType = kzuPropertyTypeGetDataType(propertyType);

        result = kzuProjectLoaderLoadAnimationGetAnimationType_internal(propertyType, binaryTargetAttribute, &animationType);
        kzsErrorForward(result);

        switch(animationType)
        {
            case KZU_BINARY_ANIMATION_TYPE_MATRIX4X4:
            {
                switch(binaryTargetAttribute)
                {
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_TRANSLATION_X: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_X; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_TRANSLATION_Y: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_Y; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_TRANSLATION_Z: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_Z; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_SCALE_X: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_X; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_SCALE_Y: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_Y; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_SCALE_Z: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_Z; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_ROTATION_X: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_X; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_ROTATION_Y: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_Y; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_ROTATION_Z: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_Z; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_WHOLE_PROPERTY: targetAttribute = KZU_ANIMATION_TARGET_WHOLE_PROPERTY; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_COLOR_R:
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_COLOR_G:
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_COLOR_B:
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_COLOR_A:
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_VECTOR_X:
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_VECTOR_Y:
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_VECTOR_Z:
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_VECTOR_W:
                    default:
                    {
                        kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Not valid target.");
                    }
                }
                break;
            }
            case KZU_BINARY_ANIMATION_TYPE_TEXTURE:
            {
                targetAttribute = KZU_ANIMATION_TARGET_WHOLE_PROPERTY;
                break;
            }
            case KZU_BINARY_ANIMATION_TYPE_FLOAT:
            {
                switch(binaryTargetAttribute)
                {
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_TRANSLATION_X: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_X; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_TRANSLATION_Y: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_Y; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_TRANSLATION_Z: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_Z; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_SCALE_X: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_X; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_SCALE_Y: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_Y; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_SCALE_Z: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_Z; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_ROTATION_X: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_X; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_ROTATION_Y: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_Y; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_ROTATION_Z: targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_Z; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_WHOLE_PROPERTY: targetAttribute = KZU_ANIMATION_TARGET_WHOLE_PROPERTY; break;
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_COLOR_R:
                    {
                        targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_R; 
                        break;
                    }
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_COLOR_G:
                    {
                        targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_G; 
                        break;
                    }
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_COLOR_B:
                    {
                        targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_B; 
                        break;
                    }
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_COLOR_A:
                    {
                        targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_A; 
                        break;
                    }
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_VECTOR_X:
                    {
                        switch(propertyDataType)
                        {
                            case KZU_PROPERTY_DATA_TYPE_VECTOR4:
                            {
                                targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_X;
                                break;
                            }
                            case KZU_PROPERTY_DATA_TYPE_VECTOR3:
                            {
                                targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_X;
                                break;
                            }
                            case KZU_PROPERTY_DATA_TYPE_VECTOR2:
                            {
                                targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR2_X;
                                break;
                            }
                            case KZU_PROPERTY_DATA_TYPE_COLOR:
                            case KZU_PROPERTY_DATA_TYPE_FLOAT:
                            case KZU_PROPERTY_DATA_TYPE_TEXTURE:
                            case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
                            case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
                            case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
                            case KZU_PROPERTY_DATA_TYPE_INT:
                            case KZU_PROPERTY_DATA_TYPE_LIGHT:
                            case KZU_PROPERTY_DATA_TYPE_STRING:
                            case KZU_PROPERTY_DATA_TYPE_BOOL:
                            case KZU_PROPERTY_DATA_TYPE_VOID:
                            default:
                            {
                                kzsAssert(KZ_FALSE);
                                break;
                            }
                        }
                        break;
                    }
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_VECTOR_Y:
                    {
                        switch(propertyDataType)
                        {
                            case KZU_PROPERTY_DATA_TYPE_VECTOR4:
                            {
                                targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_Y;
                                break;
                            }
                            case KZU_PROPERTY_DATA_TYPE_VECTOR3:
                            {
                                targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_Y;
                                break;
                            }
                            case KZU_PROPERTY_DATA_TYPE_VECTOR2:
                            {
                                targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR2_Y;
                                break;
                            }
                            case KZU_PROPERTY_DATA_TYPE_COLOR:
                            case KZU_PROPERTY_DATA_TYPE_FLOAT:
                            case KZU_PROPERTY_DATA_TYPE_TEXTURE:
                            case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
                            case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
                            case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
                            case KZU_PROPERTY_DATA_TYPE_INT:
                            case KZU_PROPERTY_DATA_TYPE_LIGHT:
                            case KZU_PROPERTY_DATA_TYPE_STRING:
                            case KZU_PROPERTY_DATA_TYPE_BOOL:
                            case KZU_PROPERTY_DATA_TYPE_VOID:
                            default:
                            {
                                kzsAssert(KZ_FALSE);
                                break;
                            }
                        }
                        break;
                    }
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_VECTOR_Z:
                    {
                        switch(propertyDataType)
                        {
                            case KZU_PROPERTY_DATA_TYPE_VECTOR4:
                            {
                                targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_Z;
                                break;
                            }
                            case KZU_PROPERTY_DATA_TYPE_VECTOR3:
                            {
                                targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_Z;
                                break;
                            }
                            case KZU_PROPERTY_DATA_TYPE_VECTOR2:
                            case KZU_PROPERTY_DATA_TYPE_COLOR:
                            case KZU_PROPERTY_DATA_TYPE_FLOAT:
                            case KZU_PROPERTY_DATA_TYPE_TEXTURE:
                            case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
                            case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
                            case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
                            case KZU_PROPERTY_DATA_TYPE_INT:
                            case KZU_PROPERTY_DATA_TYPE_LIGHT:
                            case KZU_PROPERTY_DATA_TYPE_STRING:
                            case KZU_PROPERTY_DATA_TYPE_BOOL:
                            case KZU_PROPERTY_DATA_TYPE_VOID:
                            default:
                            {
                                kzsAssert(KZ_FALSE);
                                break;
                            }
                        }
                        break;
                    }
                    case KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_VECTOR_W:
                    {
                        switch(propertyDataType)
                        {
                            case KZU_PROPERTY_DATA_TYPE_VECTOR4:
                            {
                                targetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_W;
                                break;
                            }
                            case KZU_PROPERTY_DATA_TYPE_VECTOR3:
                            case KZU_PROPERTY_DATA_TYPE_VECTOR2:
                            case KZU_PROPERTY_DATA_TYPE_COLOR:
                            case KZU_PROPERTY_DATA_TYPE_FLOAT:
                            case KZU_PROPERTY_DATA_TYPE_TEXTURE:
                            case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
                            case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
                            case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
                            case KZU_PROPERTY_DATA_TYPE_INT:
                            case KZU_PROPERTY_DATA_TYPE_LIGHT:
                            case KZU_PROPERTY_DATA_TYPE_STRING:
                            case KZU_PROPERTY_DATA_TYPE_BOOL:
                            case KZU_PROPERTY_DATA_TYPE_VOID:
                            default:
                            {
                                kzsAssert(KZ_FALSE);
                                break;
                            }
                        }
                        break;
                    }
                    default:
                    {
                        kzsAssert(KZ_FALSE);
                        break;
                    }
                }
                break;
            }
            default:
            {
                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid animation type");
            }
        }
    }

    *out_targetAttribute = targetAttribute;
    kzsSuccess();
}

static kzsError kzuProjectLoaderAnimationLoadAnimationTarget_internal(struct KzuBinaryFileInfo* file, kzString pathToContainerFile, 
                                                                      struct KzuProject* project, const void** out_object)
{
    kzsError result;
    const void* object = KZ_NULL;

    /** TODO: replace with ProjectLoaderLoadObject or something */

    if (pathToContainerFile != KZ_NULL)
    {
        enum KzuBinaryFileType fileType;
        enum KzuProjectObjectType projectFileType;

        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), pathToContainerFile, &file);
        kzsExceptionForward(result);
        fileType = kzuBinaryFileInfoGetType(file);
        projectFileType = kzuProjectLoaderGetObjectType(fileType);

        /*lint -e{788} Suppress warning about not listing all possible enum values. */
        switch(projectFileType)
        {
            case KZU_PROJECT_OBJECT_TYPE_SCENE:
            {
                struct KzuScene* scene;
                result = kzuProjectLoaderLoadScene(project, pathToContainerFile, &scene);
                kzsErrorForward(result);
                object = scene;
                break;
            }
            case KZU_PROJECT_OBJECT_TYPE_MATERIAL:
            {
                struct KzuMaterial* material;
                result = kzuProjectLoaderLoadMaterial(project, pathToContainerFile, &material);
                kzsErrorForward(result);
                object = material;
                break;
            }
            case KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE:
            {
                struct KzuObjectNode* objectNode;
                result = kzuProjectLoaderLoadObjectNode(project, pathToContainerFile, &objectNode);
                kzsErrorForward(result);
                object = objectNode;
                break;
            }
            case KZU_PROJECT_OBJECT_TYPE_COMPOSER:
            {
                struct KzuComposer* composer;
                result = kzuProjectLoaderLoadComposer(project, pathToContainerFile, &composer);
                kzsErrorForward(result);
                object = composer;
                break;
            }
            case KZU_PROJECT_OBJECT_TYPE_PROPERTY_GROUP:
            {
                struct KzuPropertyGroup* group;
                result = kzuProjectLoaderLoadPropertyGroup(project, pathToContainerFile, &group);
                kzsErrorForward(result);
                object = group;
                break; 
            }
            default:
            {
                kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Animation target object must have properties.");
            }
        }

        if (object == KZ_NULL)
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "Could not find target object.");
        }
    }

    *out_object = object;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadAnimation(struct KzuProject* project, kzString path, struct KzuAnimation** out_animation)
{
    kzsError result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuPropertyManager* propertyManager = kzuProjectGetPropertyManager(project);
    struct KzuAnimation* animation;
    kzUint measurementTime = kzsTimeGetCurrentTimestamp();

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load animation with null path");

    /* Check if the animation is already loaded. */
    animation = (struct KzuAnimation*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_ANIMATION, path);

    if (!kzuAnimationIsValid(animation))
    {
        /* Get file info from directory corresponding the given path. */
        /*result = kzuBinaryDirectoryGetFile(directory, path, &file);
        kzsErrorForward(result);*/
        result = kzuBinaryDirectoryGetFileFromAnimationLibrary(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_ANIMATION_DATA, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load animation file.");

        /* Load the animation. */
        {
            kzString* childPaths;
            kzUint childAnimationCount;
            kzString pathToContainerFile;
            kzString pathToPropertyType;
            struct KzuPropertyType* propertyType;
            enum KzuBinaryAnimationTargetAttribute targetAttribute;
            enum KzuBinaryAnimationType animationType;

            /* Read data */
            {
                enum KzuAnimationTargetAttribute engineAttributeType;
                struct KzcInputStream* inputStream;
                const void* targetObject = KZ_NULL;

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);

                result = kzuBinaryDirectoryReadReference(inputStream, file, &pathToPropertyType);
                kzsErrorForward(result);
                result = kzuProjectLoaderLoadPropertyType(project, pathToPropertyType, &propertyType);
                kzsErrorForward(result);

                result = kzuBinaryDirectoryReadReference(inputStream, file, &pathToContainerFile);
                kzsErrorForward(result);

                /* Get target property from the reference container. */
                result = kzuProjectLoaderAnimationLoadAnimationTarget_internal(file, pathToContainerFile, project, &targetObject);
                kzsErrorForward(result);

                result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&targetAttribute);
                kzsErrorForward(result);     

                result = kzuProjectLoaderLoadAnimationGetAnimationType_internal(propertyType, targetAttribute, &animationType);
                kzsErrorForward(result);     

                result = kzuProjectLoaderLoadAnimationConvertAttribute(propertyType, targetAttribute, &engineAttributeType);
                kzsErrorForward(result); 

                /* TODO: This can be removed after bug https://rightware.bugtrack.net/viewbug.aspx?id=817757 is fixed. */
                {
                    enum KzuPropertyDataType propertyDataType = kzuPropertyTypeGetDataType(propertyType);
                    if(propertyDataType == KZU_PROPERTY_DATA_TYPE_MATRIX4X4 && engineAttributeType == KZU_ANIMATION_TARGET_WHOLE_PROPERTY)
                    {
                        engineAttributeType = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_X;
                    }
                }

                switch(animationType)
                {
                    case KZU_BINARY_ANIMATION_TYPE_FLOAT:
                    {
                        result = kzuAnimationCreateFloatTarget(memoryManager, propertyManager, propertyType, targetObject, engineAttributeType, &animation);
                        kzsErrorForward(result);
                        break;
                    }
                    case KZU_BINARY_ANIMATION_TYPE_TEXTURE:
                    {
                        result = kzuAnimationCreateTexture(memoryManager, propertyManager, propertyType, targetObject, &animation);
                        kzsErrorForward(result);
                        break;
                    }
                    case KZU_BINARY_ANIMATION_TYPE_MATRIX4X4:
                    {
                        result = kzuAnimationCreateMatrix(memoryManager, propertyManager, propertyType, targetObject, &animation);
                        kzsErrorForward(result);
                        break;
                    }
                    default:
                    {
                        kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unknown animation type.");
                    }
                }

                /* Read key frames. */
                {
                    kzUint i;
                    kzUint keyFrameCount;
                    
                    result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&keyFrameCount);
                    kzsErrorForward(result);

                    for(i = 0; i < keyFrameCount; ++i)
                    {
                        kzFloat time;
                        struct KzuAnimationKey* animationKey;
                        enum KzuBinaryKeyFrameInterpolationType interpolationType;
                        kzFloat tangents[4]; /* 0=inX, 1=inY, 2=outX, 3=outY */
                        enum KzuAnimationInterpolation animationInterpolation;

                        result = kzcInputStreamReadFloat(inputStream, &time);
                        kzsErrorForward(result);

                        result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&interpolationType);
                        kzsErrorForward(result);

                        switch(interpolationType)
                        {
                            case KZU_BINARY_KEY_FRAME_INTERPOLATION_TYPE_STEP:
                            case KZU_BINARY_KEY_FRAME_INTERPOLATION_TYPE_LINEAR:
                            {
                               break;
                            }
                            case KZU_BINARY_KEY_FRAME_INTERPOLATION_TYPE_BEZIER:
                            case KZU_BINARY_KEY_FRAME_INTERPOLATION_TYPE_HERMITE:
                            {
                                result = kzcInputStreamReadFloatArray(inputStream, 4, tangents);
                                kzsErrorForward(result);
                                break;
                            }
                            default:
                            {
                                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid interpolation im animation keyframe.");
                            }
                        }
                        
                        animationInterpolation = kzuProjectLoaderGetInterpolation_internal(interpolationType);
                        switch(animationType)
                        {
                            case KZU_BINARY_ANIMATION_TYPE_FLOAT:
                            {
                                kzFloat floatValue;

                                result = kzcInputStreamReadFloat(inputStream, &floatValue);
                                kzsErrorForward(result);

                                switch(interpolationType)
                                {
                                    case KZU_BINARY_KEY_FRAME_INTERPOLATION_TYPE_STEP:
                                    case KZU_BINARY_KEY_FRAME_INTERPOLATION_TYPE_LINEAR:
                                    {
                                        struct KzuAnimationKeyFloat* keyFloat;
                                        result = kzuAnimationKeyFloatCreate(memoryManager, time, floatValue, animationInterpolation, &keyFloat);
                                        kzsErrorForward(result);
                                        animationKey = kzuAnimationKeyFloatToAnimationKey(keyFloat);
                                        break;
                                    }
                                    case KZU_BINARY_KEY_FRAME_INTERPOLATION_TYPE_BEZIER:
                                    case KZU_BINARY_KEY_FRAME_INTERPOLATION_TYPE_HERMITE:
                                    {
                                        struct KzuAnimationKeyFloatAdvanced* keyFloat;
                                        result = kzuAnimationKeyFloatAdvancedCreate(memoryManager, time, floatValue, animationInterpolation, 
                                            /*lint -e{644}*/kzcVector2(tangents[0], tangents[1]), kzcVector2(tangents[2], tangents[3]), &keyFloat); 
                                        kzsErrorForward(result);
                                        animationKey = kzuAnimationKeyFloatAdvancedToAnimationKey(keyFloat);
                                        break;
                                    }
                                    default:
                                    {
                                        kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid interpolation im animation keyframe.");
                                    }
                                }
                                break;
                            }
                            case KZU_BINARY_ANIMATION_TYPE_TEXTURE:
                            {
                                struct KzuAnimationKeyTexture* keyTexture;
                                struct KzcTexture* textureValue = KZ_NULL;
                                kzString texturePath;

                                result = kzuBinaryDirectoryReadReference(inputStream, file, &texturePath);
                                kzsErrorForward(result);

                                if (texturePath != KZ_NULL)
                                {
                                    result = kzuProjectLoaderLoadTexture(project, texturePath, &textureValue);
                                    kzsErrorForward(result);
                                }

                                result = kzuAnimationKeyTextureCreate(memoryManager, time, textureValue, &keyTexture);
                                kzsErrorForward(result);

                                animationKey = kzuAnimationKeyTextureToAnimationKey(keyTexture);

                                break;
                            }
                            case KZU_BINARY_ANIMATION_TYPE_MATRIX4X4:
                            {
                                struct KzuAnimationKeyMatrix4x4* keyMatrix;
                                struct KzcMatrix4x4 matrixValue;
                                
                                result = kzcInputStreamReadFloatArray(inputStream, 16, matrixValue.data);
                                kzsErrorForward(result);

                                result = kzuAnimationKeyMatrix4x4Create(memoryManager, time, &matrixValue, &keyMatrix);
                                kzsErrorForward(result);

                                animationKey = kzuAnimationKeyMatrix4x4ToAnimationKey(keyMatrix);

                                break;
                            }
                            default:
                            {
                                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unknown animation type.");
                            }
                        }       

                        result = kzuAnimationAddKeyFrame(animation, animationKey);
                        kzsErrorForward(result);

                    } /* End of key loop. */
                }
                    
                /* Child animations. */
                {
                    kzUint i;

                    result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&childAnimationCount);
                    kzsErrorForward(result);           

                    result = kzcMemoryAllocArray(memoryManager, childPaths, childAnimationCount, "Animation child reference paths");
                    kzsErrorForward(result);

                    for (i = 0; i < childAnimationCount; ++i)
                    {
                        result = kzuBinaryDirectoryReadReference(inputStream, file, &childPaths[i]);
                        kzsErrorForward(result);
                    }
                }

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);
            }

            result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_ANIMATION, animation, kzuBinaryFileInfoGetPath(file),
                (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&animation);
            kzsErrorForward(result);

            kzuProjectAddMeasurementCumulativeTime_private(project, measurementTime, KZU_PROJECT_MEASUREMENT_ANIMATION);

            /* Load references */
            {
                kzUint i;
                for (i = 0; i < childAnimationCount; ++i)
                {
                    struct KzuAnimation* childAnimation;

                    result = kzuProjectLoaderLoadAnimation(project, childPaths[i], &childAnimation);
                    kzsErrorForward(result);

                    result = kzuAnimationAddChild(animation, childAnimation);
                    kzsErrorForward(result);

                    /* TODO: Implement ChildFromKzb to prevent issues with patching. Check object node for reference. */
                }
            }

            result = kzcMemoryFreeArray((kzMutableString*)childPaths);
            kzsErrorForward(result);
        }
    }

    *out_animation = animation;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadAnimationClip(struct KzuProject* project, kzString path, struct KzuAnimationClip** out_animationClip)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuAnimationClip* animationClip;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load animation clip with null path");

    /* Check if the animation clip is already loaded. */
    animationClip = (struct KzuAnimationClip*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_ANIMATION_CLIP, path);

    if (!kzuAnimationClipIsValid(animationClip))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsExceptionTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_ANIMATION_CLIP, KZU_EXCEPTION_WRONG_BINARY_FILE_TYPE,
            "Wrong file type encountered while trying to load time line sequence file.");

        /* Load the animation clip. */
        {
            kzString* referencePaths;
            kzUint referenceAnimationCount;
            kzFloat startTime;
            kzFloat endTime;
            kzBool relative;

            /* Read data */
            {
                struct KzcInputStream* inputStream;

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);

                result = kzcInputStreamReadFloat(inputStream, &startTime);
                kzsErrorForward(result);

                result = kzcInputStreamReadFloat(inputStream, &endTime);
                kzsErrorForward(result);
                
                result = kzcInputStreamReadBoolean(inputStream, &relative);
                kzsErrorForward(result);
                result = kzcInputStreamSkip(inputStream, 3);
                kzsErrorForward(result);

                /* Reference animations. */
                {
                    kzUint i;

                    result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&referenceAnimationCount);
                    kzsErrorForward(result);           

                    result = kzcMemoryAllocArray(memoryManager, referencePaths, referenceAnimationCount, "Animation reference paths");
                    kzsErrorForward(result);

                    for (i = 0; i < referenceAnimationCount; ++i)
                    {
                        result = kzuBinaryDirectoryReadReference(inputStream, file, &referencePaths[i]);
                        kzsErrorForward(result);
                    }
                }

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);
            }

            if(startTime > endTime)
            {
                kzsLog(KZS_LOG_LEVEL_USER_WARNING, "Animation clip starting time was after ending time. Forcing the animation clip times safe range.");
                endTime = startTime + 1.0f;
            }

            result = kzuAnimationClipCreate(memoryManager, startTime, endTime, &animationClip);
            kzsErrorForward(result);

            kzuAnimationClipSetRelative(animationClip, relative);

            result = kzuAnimationClipSetName(animationClip, kzuBinaryFileInfoGetName(file));
            kzsErrorForward(result);

            result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_ANIMATION_CLIP, animationClip, kzuBinaryFileInfoGetPath(file),
                (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&animationClip);
            kzsErrorForward(result);

            /* Load references */
            {
                kzUint i;

                for (i = 0; i < referenceAnimationCount; ++i)
                {
                    struct KzuAnimation* referencedAnimation;

                    result = kzuProjectLoaderLoadAnimation(project, referencePaths[i], &referencedAnimation);
                    kzsErrorForward(result);

                    result = kzuAnimationClipAddAnimation(animationClip, referencedAnimation);
                    kzsErrorForward(result);
                }
            }

            result = kzcMemoryFreeArray((kzMutableString*)referencePaths);
            kzsErrorForward(result);
        }
    }

    *out_animationClip = animationClip;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadTimeLineSequence(struct KzuProject* project, kzString path, struct KzuTimeLineSequence** out_timeLineSequence)
{
    kzsException result;
    struct KzuTimeLineSequence* timeLineSequence;

    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load time line sequence with null path");

    /* Check if the sequence is already loaded. */
    timeLineSequence = (struct KzuTimeLineSequence*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_TIMELINE_SEQUENCE, path);

    if (!kzuTimeLineSequenceIsValid(timeLineSequence))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsExceptionTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_TIMELINE_SEQUENCE, KZU_EXCEPTION_WRONG_BINARY_FILE_TYPE,
            "Wrong file type encountered while trying to load time line sequence file.");

        /* Load the time line sequence. */
        {
            kzUint timeLineEntryCount;

            /* Read data */
            {
                struct KzcInputStream* inputStream;

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);

                result = kzcInputStreamReadU32Int(inputStream, (kzUint*)&timeLineEntryCount);
                kzsErrorForward(result);

                result = kzuTimeLineSequenceCreate(memoryManager, &timeLineSequence);
                kzsErrorForward(result);           

                /* Time line entries. */
                {
                    kzUint i;
                    for (i = 0; i < timeLineEntryCount; ++i)
                    {
                        struct KzuPropertyType* inputPropertyType = KZ_NULL;
                        void* inputPropertyObject = KZ_NULL;
                        enum KzuAnimationTargetAttribute inputTargetAttribute = KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_FLOAT;
                        kzFloat startTime = 0.0f;
                        kzFloat scalingFactor = 1.0f;
                        kzInt repeatCount = 1;
                        enum KzuBinaryPlaybackMode playbackType = KZU_BINARY_PLAYBACK_MODE_NORMAL;
                        kzUint overrideCount;
                        enum KzuBinaryTimeLineEntryInputType inputType;
                        enum KzuBinaryTimeLineEntryType type;
                        struct KzuTimeLineEntry* timeLineEntry = KZ_NULL;
                        
                        result = kzcInputStreamReadFloat(inputStream, &scalingFactor);
                        kzsErrorForward(result);
                        
                        result = kzcInputStreamReadU32Int(inputStream, (kzUint*)&inputType);
                        kzsErrorForward(result);

                        switch (inputType)
                        {
                            case KZU_BINARY_TIME_LINE_ENTRY_INPUT_TYPE_TIME:
                            {
                                result = kzcInputStreamReadFloat(inputStream, &startTime);
                                kzsErrorForward(result);

                                result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&repeatCount);
                                kzsErrorForward(result);
                                repeatCount = kzsMax(repeatCount, -1);

                                result = kzcInputStreamReadU32Int(inputStream, (kzUint*)&playbackType);
                                kzsErrorForward(result);

                                break;
                            }

                            case KZU_BINARY_TIME_LINE_ENTRY_INPUT_TYPE_PROPERTY:
                            {
                                kzString propertyTypePath;
                                kzString propertyHostReference;
                                enum KzuBinaryAnimationTargetAttribute targetAttribute;

                                result = kzuBinaryDirectoryReadReference(inputStream, file, &propertyTypePath);
                                kzsErrorForward(result);
                                result = kzuBinaryDirectoryReadReference(inputStream, file, &propertyHostReference);
                                kzsErrorForward(result);
                                result = kzcInputStreamReadU32Int(inputStream, (kzUint*)&targetAttribute);
                                kzsErrorForward(result);

                                result = kzuProjectLoaderLoadPropertyType(project, propertyTypePath, &inputPropertyType);
                                kzsErrorForward(result);

                                result = kzuProjectLoaderAnimationLoadAnimationTarget_internal(file, propertyHostReference, project, (const void**)&inputPropertyObject);
                                kzsErrorForward(result);
                                result = kzuProjectLoaderLoadAnimationConvertAttribute(inputPropertyType, targetAttribute, &inputTargetAttribute);
                                kzsErrorForward(result);

                                break;
                            }

                            default:
                            {
                                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid enum value for time line entry input type in animation sequence.");
                            }
                        }
                        
                        result = kzcInputStreamReadU32Int(inputStream, (kzUint*)&type);
                        kzsErrorForward(result);

                        switch(type)
                        {
                            case KZU_BINARY_TIME_LINE_ENTRY_TYPE_ANIMATION_CLIP:
                            {
                                kzsException e;
                                kzString referencePath;
                                struct KzuAnimationClip* containedClip;
                                result = kzuBinaryDirectoryReadReference(inputStream, file, &referencePath);
                                kzsErrorForward(result);
                                e = kzuProjectLoaderLoadAnimationClip(project, referencePath, &containedClip);
                                kzsErrorTest(e == KZS_SUCCESS, KZU_ERROR_PROJECT_LOADING_FAILED, "Loading animation clip failed.");
                                result = kzuTimeLineEntryCreateClip(memoryManager, startTime, containedClip, &timeLineEntry);
                                kzsErrorForward(result);
                                break;
                            }

                            case KZU_BINARY_TIME_LINE_ENTRY_TYPE_TIME_LINE_SEQUENCE:
                            {
                                kzsException e;
                                kzString referencePath;
                                struct KzuTimeLineSequence* containedSequence;

                                result = kzuBinaryDirectoryReadReference(inputStream, file, &referencePath);
                                kzsErrorForward(result);

                                kzsErrorTest(!kzcStringIsEqual(kzuBinaryFileInfoGetPath(file), referencePath), KZU_ERROR_INVALID_BINARY_DATA, "Animation sequence contains a loop. A sequence is not allowed to contain itself.");

                                e = kzuProjectLoaderLoadTimeLineSequence(project, referencePath, &containedSequence);
                                kzsErrorTest(e == KZS_SUCCESS, KZU_ERROR_WRONG_BINARY_FILE_TYPE, "Loading animation clip failed.");

                                result = kzuTimeLineEntryCreateSequence(memoryManager, startTime, containedSequence, &timeLineEntry);
                                kzsErrorForward(result);

                                break;
                            }

                            default:
                            {
                                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid enum value for time line entry type in animation sequence.");
                            }
                        }

                        kzuTimeLineEntrySetInputType(timeLineEntry, kzuProjectGetPropertyManager(project), 
                            inputPropertyType, inputPropertyObject, inputTargetAttribute);
                        
                        kzuTimeLineEntrySetDurationScale(timeLineEntry, scalingFactor);
                        kzuTimeLineEntrySetRepeatCount(timeLineEntry, repeatCount);

                        {
                            enum KzuTimeLineEntryPlaybackMode mode = kzuProjectLoaderGetPlaybackMode_internal(playbackType);
                            kzuTimeLineEntrySetPlaybackMode(timeLineEntry, mode);
                        }


                        result = kzcInputStreamReadU32Int(inputStream, (kzUint*)&overrideCount);
                        kzsErrorForward(result);

                        {
                            kzUint n;
                            for(n = 0; n < overrideCount; ++n)
                            {
                                struct KzuAnimationOverride source;
                                struct KzuAnimationOverride target;

                                {
                                    enum KzuBinaryAnimationType animationType;
                                    kzString pathToContainerFile;
                                    kzString sourcePath;
                                    const void* object;
                                    struct KzuPropertyType* propertyType;
                                    enum KzuBinaryAnimationTargetAttribute targetAttribute;
                                    enum KzuAnimationTargetAttribute engineAttributeType;

                                    result = kzuBinaryDirectoryReadReference(inputStream, file, &sourcePath);
                                    kzsErrorForward(result);
                                    result = kzuBinaryDirectoryReadReference(inputStream, file, &pathToContainerFile);
                                    kzsErrorForward(result);
                                    result = kzcInputStreamReadU32Int(inputStream, (kzUint*)&targetAttribute);
                                    kzsErrorForward(result);

                                    result = kzuProjectLoaderLoadPropertyType(project, sourcePath, &propertyType);
                                    kzsErrorForward(result);
                                    result = kzuProjectLoaderAnimationLoadAnimationTarget_internal(file, pathToContainerFile, project, &object);
                                    kzsErrorForward(result);
                                    /* TODO: Per attribute override not yet implemented in tool or engine. */
                                    result = kzuProjectLoaderLoadAnimationGetAnimationType_internal(propertyType, targetAttribute, &animationType);
                                    kzsErrorForward(result);
                                    result = kzuProjectLoaderLoadAnimationConvertAttribute(propertyType, targetAttribute, &engineAttributeType);
                                    kzsErrorForward(result);

                                    source.object = object;
                                    source.propertyType = propertyType;
                                }

                                {
                                    enum KzuBinaryAnimationType animationType;
                                    kzString pathToContainerFile;
                                    kzString sourcePath;
                                    const void* object;
                                    struct KzuPropertyType* propertyType;
                                    enum KzuBinaryAnimationTargetAttribute targetAttribute;
                                    enum KzuAnimationTargetAttribute engineAttributeType;

                                    result = kzuBinaryDirectoryReadReference(inputStream, file, &sourcePath);
                                    kzsErrorForward(result);
                                    result = kzuBinaryDirectoryReadReference(inputStream, file, &pathToContainerFile);
                                    kzsErrorForward(result);
                                    result = kzcInputStreamReadU32Int(inputStream, (kzUint*)&targetAttribute);
                                    kzsErrorForward(result);     

                                    result = kzuProjectLoaderLoadPropertyType(project, sourcePath, &propertyType);
                                    kzsErrorForward(result);
                                    result = kzuProjectLoaderAnimationLoadAnimationTarget_internal(file, pathToContainerFile, project, &object);
                                    kzsErrorForward(result);

                                    /* TODO: Per attribute override not yet implemented in tool or engine. */
                                    result = kzuProjectLoaderLoadAnimationGetAnimationType_internal(propertyType, targetAttribute, &animationType);
                                    kzsErrorForward(result);
                                    result = kzuProjectLoaderLoadAnimationConvertAttribute(propertyType, targetAttribute, &engineAttributeType);
                                    kzsErrorForward(result);

                                    target.object = object;
                                    target.propertyType = propertyType;
                                }
                                
                                result = kzuTimeLineEntryAddOverride(timeLineEntry, source, target);
                                kzsErrorForward(result);
                            }
                        }


                        result = kzuTimeLineSequenceAddEntry(timeLineSequence, timeLineEntry);
                        kzsErrorForward(result);
                    }
                }

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);
            }

            result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_TIMELINE_SEQUENCE, timeLineSequence, kzuBinaryFileInfoGetPath(file),
                (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&timeLineSequence);
            kzsErrorForward(result);
        }
    }

    *out_timeLineSequence = timeLineSequence;
    kzsSuccess();
}

kzsException kzuProjectLoaderUnloadAnimation(struct KzuProject* project, const struct KzuAnimation* animation)
{
    kzsError result;
    kzString objectPath = kzuProjectGetObjectPath(project, animation);

    if(objectPath != KZ_NULL)
    {
        result = kzuProjectLoaderUnloadObject(project, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsException kzuProjectLoaderUnloadAnimationClip(struct KzuProject* project, const struct KzuAnimationClip* animationClip)
{
    kzsError result;
    kzString objectPath = kzuProjectGetObjectPath(project, animationClip);

    if(objectPath != KZ_NULL)
    {
        result = kzuProjectLoaderUnloadObject(project, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsException kzuProjectLoaderUnloadTimeLineSequence(struct KzuProject* project, const struct KzuTimeLineSequence* timeLineSequence)
{
    kzsError result;
    kzString objectPath = kzuProjectGetObjectPath(project, timeLineSequence);

    if(objectPath != KZ_NULL)
    {
        result = kzuProjectLoaderUnloadObject(project, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
