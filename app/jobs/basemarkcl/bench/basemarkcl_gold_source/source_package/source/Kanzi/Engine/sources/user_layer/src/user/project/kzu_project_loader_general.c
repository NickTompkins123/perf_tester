/**
* \file
* Project loader general purpose functions.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_general.h"

#include <user/binary/kzu_binary_directory.h>

#include <core/util/io/kzc_input_stream.h>
#include <core/util/string/kzc_string.h>

#include <system/debug/kzs_counter.h>


kzsException kzuProjectLoaderReadTextFile(const struct KzcMemoryManager* memoryManager,
                                          const struct KzuBinaryFileInfo* file, kzMutableString* out_text)
{
    kzsError result;
    struct KzcInputStream* inputStream;
    kzUint textLength;
    kzMutableString text;

    textLength = kzuBinaryFileInfoGetSize(file);

    result = kzcStringAllocate(memoryManager, textLength, &text);
    kzsErrorForward(result);

    result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
    kzsErrorForward(result);

    result = kzcInputStreamReadBytes(inputStream, textLength, (kzByte*)text);
    kzsErrorForward(result);

    text[textLength] = KZC_STRING_TERMINATION_SYMBOL;

    result = kzcInputStreamDelete(inputStream);
    kzsErrorForward(result);

    *out_text = text;
    kzsSuccess();
}

enum KzuProjectObjectType kzuProjectLoaderGetObjectType(enum KzuBinaryFileType binaryType)
{
    enum KzuProjectObjectType objectType;

    switch (binaryType)
    {
        case KZU_BINARY_FILE_TYPE_SCENE: objectType = KZU_PROJECT_OBJECT_TYPE_SCENE; break;
        case KZU_BINARY_FILE_TYPE_EMPTY_NODE: objectType = KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE; break;
        case KZU_BINARY_FILE_TYPE_MESH_NODE: objectType = KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE; break;
        case KZU_BINARY_FILE_TYPE_CAMERA_NODE: objectType = KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE; break;
        case KZU_BINARY_FILE_TYPE_LIGHT_NODE: objectType = KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE; break;
        case KZU_BINARY_FILE_TYPE_LOD_SELECTOR_NODE: objectType = KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE; break;
        case KZU_BINARY_FILE_TYPE_UI_COMPONENT_NODE: objectType = KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE; break;
        case KZU_BINARY_FILE_TYPE_INSTANCIATOR_NODE: objectType = KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE; break;
        case KZU_BINARY_FILE_TYPE_TEXTURE: objectType = KZU_PROJECT_OBJECT_TYPE_TEXTURE; break;
        case KZU_BINARY_FILE_TYPE_MATERIAL_TYPE: objectType = KZU_PROJECT_OBJECT_TYPE_MATERIAL_TYPE; break;
        case KZU_BINARY_FILE_TYPE_MATERIAL: objectType = KZU_PROJECT_OBJECT_TYPE_MATERIAL; break;
        case KZU_BINARY_FILE_TYPE_COMPOSER: objectType = KZU_PROJECT_OBJECT_TYPE_COMPOSER; break;
        case KZU_BINARY_FILE_TYPE_PROPERTY_TYPE: objectType = KZU_PROJECT_OBJECT_TYPE_PROPERTY_TYPE; break;
        case KZU_BINARY_FILE_TYPE_MESH: objectType = KZU_PROJECT_OBJECT_TYPE_MESH; break;
        case KZU_BINARY_FILE_TYPE_MESH_SPHERE: objectType = KZU_PROJECT_OBJECT_TYPE_MESH; break;
        case KZU_BINARY_FILE_TYPE_MESH_BOX: objectType = KZU_PROJECT_OBJECT_TYPE_MESH; break;
        case KZU_BINARY_FILE_TYPE_MESH_PLANE: objectType = KZU_PROJECT_OBJECT_TYPE_MESH; break;
        case KZU_BINARY_FILE_TYPE_IMAGE_PNG: objectType = KZU_PROJECT_OBJECT_TYPE_IMAGE; break;
        case KZU_BINARY_FILE_TYPE_IMAGE_JPEG: objectType = KZU_PROJECT_OBJECT_TYPE_IMAGE; break;
        case KZU_BINARY_FILE_TYPE_IMAGE_ETC: objectType = KZU_PROJECT_OBJECT_TYPE_IMAGE; break;
        case KZU_BINARY_FILE_TYPE_IMAGE_DXT: objectType = KZU_PROJECT_OBJECT_TYPE_IMAGE; break;
        case KZU_BINARY_FILE_TYPE_IMAGE_MIPMAP: objectType = KZU_PROJECT_OBJECT_TYPE_IMAGE; break;

        case KZU_BINARY_FILE_TYPE_SHADER_SOURCE: objectType = KZU_PROJECT_OBJECT_TYPE_SHADER_SOURCE; break;
        case KZU_BINARY_FILE_TYPE_OBJECT_SOURCE: objectType = KZU_PROJECT_OBJECT_TYPE_OBJECT_SOURCE; break;
        case KZU_BINARY_FILE_TYPE_ANIMATION_DATA: objectType = KZU_PROJECT_OBJECT_TYPE_ANIMATION; break;
        case KZU_BINARY_FILE_TYPE_ANIMATION_CLIP: objectType = KZU_PROJECT_OBJECT_TYPE_ANIMATION_CLIP; break;
        case KZU_BINARY_FILE_TYPE_TIMELINE_SEQUENCE: objectType = KZU_PROJECT_OBJECT_TYPE_TIMELINE_SEQUENCE; break;
        case KZU_BINARY_FILE_TYPE_UI_COMPONENT_TYPE: objectType = KZU_PROJECT_OBJECT_TYPE_UI_COMPONENT_TYPE; break;
        case KZU_BINARY_FILE_TYPE_BITMAP_FONT: objectType = KZU_PROJECT_OBJECT_TYPE_FONT; break;
        case KZU_BINARY_FILE_TYPE_TRUETYPE_FONT: objectType = KZU_PROJECT_OBJECT_TYPE_FONT; break;
        case KZU_BINARY_FILE_TYPE_TRANSITION: objectType = KZU_PROJECT_OBJECT_TYPE_TRANSITION; break;
        case KZU_BINARY_FILE_TYPE_SCRIPT: objectType = KZU_PROJECT_OBJECT_TYPE_SCRIPT; break;
        case KZU_BINARY_FILE_TYPE_TRAJECTORY: objectType = KZU_PROJECT_OBJECT_TYPE_TRAJECTORY; break;
        case KZU_BINARY_FILE_TYPE_PROPERTY_GROUP: objectType = KZU_PROJECT_OBJECT_TYPE_PROPERTY_GROUP; break;
        case KZU_BINARY_FILE_TYPE_SPLINE: objectType = KZU_PROJECT_OBJECT_TYPE_SPLINE; break;

        case KZU_BINARY_FILE_TYPE_BINARY:
        case KZU_BINARY_FILE_TYPE_UTF8_TEXT:
        case KZU_BINARY_FILE_TYPE_ASCII_TEXT:
        case KZU_BINARY_FILE_TYPE_SHORTCUT:
        case KZU_BINARY_FILE_TYPE_IMAGE_RAW_1D:
        case KZU_BINARY_FILE_TYPE_IMAGE_RAW_2D:
        case KZU_BINARY_FILE_TYPE_IMAGE_RAW_3D:
        case KZU_BINARY_FILE_TYPE_PROJECT:
        {
            objectType = KZU_PROJECT_OBJECT_TYPE_INVALID;
            break;
        }

        case KZU_BINARY_FILE_TYPE_SHADER_BINARY:
        {
            objectType = KZU_PROJECT_OBJECT_TYPE_INVALID;
            kzsAssertText(KZ_FALSE, "Binary shaders not supported yet.");
            break;
        }

        case KZU_BINARY_FILE_TYPE_INVALID:
        default:
        {
            objectType = KZU_PROJECT_OBJECT_TYPE_INVALID;
            kzsAssertText(KZ_FALSE, "Invalid binary file type");
        }
    }

    return objectType;
}
