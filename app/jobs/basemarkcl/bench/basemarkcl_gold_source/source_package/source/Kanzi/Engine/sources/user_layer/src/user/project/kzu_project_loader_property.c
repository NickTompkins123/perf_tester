/**
* \file
* Project loader for properties and property type files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_property.h"

#include "kzu_project_loader.h"
#include "kzu_project_loader_general.h"
#include "kzu_project_loader_texture.h"
#include "kzu_project_loader_object_node.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/binary/kzu_binary_util.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_collection.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_color_property.h>
#include <user/properties/kzu_string_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_matrix2x2_property.h>
#include <user/properties/kzu_matrix3x3_property.h>
#include <user/properties/kzu_matrix4x4_property.h>
#include <user/properties/kzu_vector2_property.h>
#include <user/properties/kzu_vector3_property.h>
#include <user/properties/kzu_vector4_property.h>
#include <user/properties/kzu_texture_property.h>
#include <user/properties/kzu_void_property.h>
#include <user/properties/kzu_light_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_light.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/util/string/kzc_string.h>

#include <system/debug/kzs_log.h>


/** Texture operations in kzb file. */
enum KzuBinaryTextureOperation
{
    KZU_BINARY_TEXTURE_OPERATION_REPLACE = 0,
    KZU_BINARY_TEXTURE_OPERATION_MULTIPLY = 1,
    KZU_BINARY_TEXTURE_OPERATION_ADD = 2,
    KZU_BINARY_TEXTURE_OPERATION_DECAL = 3,
    KZU_BINARY_TEXTURE_OPERATION_CUSTOM = 4
};

/** Type of the property in kzb file. */
enum KzuBinaryPropertyType
{
    KZU_BINARY_PROPERTY_TYPE_COLOR = 0,
    KZU_BINARY_PROPERTY_TYPE_STRING = 1,
    KZU_BINARY_PROPERTY_TYPE_INT32 = 2,
    KZU_BINARY_PROPERTY_TYPE_FLOAT = 3,
    KZU_BINARY_PROPERTY_TYPE_VECTOR2D = 4,
    KZU_BINARY_PROPERTY_TYPE_VECTOR3D = 5,
    KZU_BINARY_PROPERTY_TYPE_VECTOR4D = 6,
    KZU_BINARY_PROPERTY_TYPE_MATRIX2D = 7,
    KZU_BINARY_PROPERTY_TYPE_MATRIX3D = 8,
    KZU_BINARY_PROPERTY_TYPE_MATRIX4D = 9,
    KZU_BINARY_PROPERTY_TYPE_ENUM = 10,
    KZU_BINARY_PROPERTY_TYPE_LIGHT = 12,
    KZU_BINARY_PROPERTY_TYPE_CAMERA = 13,
    KZU_BINARY_PROPERTY_TYPE_TEXTURE = 14,
    KZU_BINARY_PROPERTY_TYPE_BOOLEAN = 15,
    KZU_BINARY_PROPERTY_TYPE_REFERENCE = 101
};


/* TODO: Read default values for property types. */
static kzsError kzuProjectLoaderReadColorPropertyType_internal(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                               kzString name, struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuColorPropertyType* colorPropertyType;
    struct KzuPropertyType* propertyType;

    KZ_UNUSED_PARAMETER(inputStream); /* This parameter is just to make the interface uniform between all read functions. */
    
    {
        result = kzuColorPropertyTypeCreate(memoryManager, name, KZC_COLOR_BLACK, &colorPropertyType);
        kzsErrorForward(result);

        propertyType = kzuColorPropertyTypeToPropertyType(colorPropertyType);
    }

    *out_propertyType = propertyType;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadColorProperty_internal(struct KzcInputStream* inputStream, struct KzcColorRGBA* out_value)
{
    kzsError result;
    struct KzcColorRGBA value;
    
    result = kzcInputStreamReadFloat(inputStream, &value.red);
    kzsErrorForward(result);
    result = kzcInputStreamReadFloat(inputStream, &value.green);
    kzsErrorForward(result);
    result = kzcInputStreamReadFloat(inputStream, &value.blue);
    kzsErrorForward(result);
    result = kzcInputStreamReadFloat(inputStream, &value.alpha);
    kzsErrorForward(result);

    *out_value = value;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadStringPropertyType_internal(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                                kzString name, struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuStringPropertyType* stringPropertyType;
    struct KzuPropertyType* propertyType;

    KZ_UNUSED_PARAMETER(inputStream); /* This parameter is just to make the interface uniform between all read functions. */
    
    {
        result = kzuStringPropertyTypeCreate(memoryManager, name, KZ_NULL, &stringPropertyType);
        kzsErrorForward(result);

        propertyType = kzuStringPropertyTypeToPropertyType(stringPropertyType);
    }

    *out_propertyType = propertyType;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadStringProperty_internal(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                            kzMutableString* out_value)
{
    kzsError result;
    kzMutableString value;
    
    result = kzuBinaryReadString(memoryManager, inputStream, &value);
    kzsErrorForward(result);

    *out_value = value;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadIntPropertyType_internal(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                             kzString name, struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuIntPropertyType* intPropertyType;
    struct KzuPropertyType* propertyType;
    
    {
        kzInt lowerBound; /* These are not used in the engine. */
        kzInt upperBound;

        result = kzcInputStreamReadS32Int(inputStream, &lowerBound);
        kzsErrorForward(result);

        result = kzcInputStreamReadS32Int(inputStream, &upperBound);
        kzsErrorForward(result);

        result = kzuIntPropertyTypeCreate(memoryManager, name, 0, &intPropertyType);
        kzsErrorForward(result);

        propertyType = kzuIntPropertyTypeToPropertyType(intPropertyType);
    }

    *out_propertyType = propertyType;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadIntProperty_internal(struct KzcInputStream* inputStream, kzInt* out_value)
{
    kzsError result;
    kzInt value;
    
    result = kzcInputStreamReadS32Int(inputStream, &value);
    kzsErrorForward(result);

    *out_value = value;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadFloatPropertyType_internal(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                               kzString name, struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuFloatPropertyType* floatPropertyType;
    struct KzuPropertyType* propertyType;
    
    {
        kzFloat lowerBound; /* These are not used in the engine. */
        kzFloat upperBound;

        result = kzcInputStreamReadFloat(inputStream, &lowerBound);
        kzsErrorForward(result);

        result = kzcInputStreamReadFloat(inputStream, &upperBound);
        kzsErrorForward(result);

        result = kzuFloatPropertyTypeCreate(memoryManager, name, 0.0f, &floatPropertyType);
        kzsErrorForward(result);

        propertyType = kzuFloatPropertyTypeToPropertyType(floatPropertyType);
    }

    *out_propertyType = propertyType;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadFloatProperty_internal(struct KzcInputStream* inputStream, kzFloat* out_value)
{
    kzsError result;
    kzFloat value;
    
    result = kzcInputStreamReadFloat(inputStream, &value);
    kzsErrorForward(result);

    *out_value = value;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadVector2PropertyType_internal(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                                 kzString name, struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuVector2PropertyType* vectorPropertyType;
    struct KzuPropertyType* propertyType;

    KZ_UNUSED_PARAMETER(inputStream); /* This parameter is just to make the interface uniform between all read functions. */
    
    {
        result = kzuVector2PropertyTypeCreate(memoryManager, name, KZC_VECTOR2_ZERO, &vectorPropertyType);
        kzsErrorForward(result);

        propertyType = kzuVector2PropertyTypeToPropertyType(vectorPropertyType);
    }

    *out_propertyType = propertyType;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadVector2Property_internal(struct KzcInputStream* inputStream, struct KzcVector2* out_value)
{
    kzsError result;
    struct KzcVector2 value;
    kzFloat x;
    kzFloat y;

    result = kzcInputStreamReadFloat(inputStream, &x);
    kzsErrorForward(result);

    result = kzcInputStreamReadFloat(inputStream, &y);
    kzsErrorForward(result);

    kzcVector2Set(&value, x, y);

    *out_value = value;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadVector3PropertyType_internal(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                                 kzString name, struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuVector3PropertyType* vectorPropertyType;
    struct KzuPropertyType* propertyType;

    KZ_UNUSED_PARAMETER(inputStream); /* This parameter is just to make the interface uniform between all read functions. */
    
    {
        result = kzuVector3PropertyTypeCreate(memoryManager, name, KZU_VECTOR3_TYPE_NORMAL, KZC_VECTOR3_ZERO, &vectorPropertyType);
        kzsErrorForward(result);

        propertyType = kzuVector3PropertyTypeToPropertyType(vectorPropertyType);
    }

    *out_propertyType = propertyType;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadVector3Property_internal(struct KzcInputStream* inputStream, struct KzcVector3* out_value)
{
    kzsError result;
    struct KzcVector3 value;
    kzFloat x;
    kzFloat y;
    kzFloat z;

    result = kzcInputStreamReadFloat(inputStream, &x);
    kzsErrorForward(result);

    result = kzcInputStreamReadFloat(inputStream, &y);
    kzsErrorForward(result);

    result = kzcInputStreamReadFloat(inputStream, &z);
    kzsErrorForward(result);

    kzcVector3Set(&value, x, y, z);

    *out_value = value;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadVector4PropertyType_internal(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                                 kzString name, struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuVector4PropertyType* vectorPropertyType;
    struct KzuPropertyType* propertyType;

    KZ_UNUSED_PARAMETER(inputStream); /* This parameter is just to make the interface uniform between all read functions. */
    
    {
        result = kzuVector4PropertyTypeCreate(memoryManager, name, KZC_VECTOR4_ZERO, &vectorPropertyType);
        kzsErrorForward(result);

        propertyType = kzuVector4PropertyTypeToPropertyType(vectorPropertyType);
    }

    *out_propertyType = propertyType;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadVector4Property_internal(struct KzcInputStream* inputStream, struct KzcVector4* out_value)
{
    kzsError result;
    struct KzcVector4 value;
    kzFloat x;
    kzFloat y;
    kzFloat z;
    kzFloat w;

    result = kzcInputStreamReadFloat(inputStream, &x);
    kzsErrorForward(result);

    result = kzcInputStreamReadFloat(inputStream, &y);
    kzsErrorForward(result);

    result = kzcInputStreamReadFloat(inputStream, &z);
    kzsErrorForward(result);

    result = kzcInputStreamReadFloat(inputStream, &w);
    kzsErrorForward(result);

    kzcVector4Set(&value, x, y, z, w);

    *out_value = value;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadMatrix2x2PropertyType_internal(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                                   kzString name, struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuMatrix2x2PropertyType* matrixPropertyType;
    struct KzuPropertyType* propertyType;

    KZ_UNUSED_PARAMETER(inputStream); /* This parameter is just to make the interface uniform between all read functions. */

    {
        result = kzuMatrix2x2PropertyTypeCreate(memoryManager, name, KZC_MATRIX2X2_IDENTITY, &matrixPropertyType);
        kzsErrorForward(result);

        propertyType = kzuMatrix2x2PropertyTypeToPropertyType(matrixPropertyType);
    }

    *out_propertyType = propertyType;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadMatrix2x2Property_internal(struct KzcInputStream* inputStream, struct KzcMatrix2x2* out_value)
{
    kzsError result;
    struct KzcMatrix2x2 value = {{0.0f}}; /* Initialize here to prevent compiler warning of potentially uninitialized variable. The variable is initialized in a for-loop below. */
    kzUint i;

    for (i = 0; i < 4; ++i)
    {
        result = kzcInputStreamReadFloat(inputStream, &value.data[i]);
        kzsErrorForward(result);
    }

    *out_value = value;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadMatrix3x3PropertyType_internal(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                                   kzString name, struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuMatrix3x3PropertyType* matrixPropertyType;
    struct KzuPropertyType* propertyType;

    KZ_UNUSED_PARAMETER(inputStream); /* This parameter is just to make the interface uniform between all read functions. */

    {
        result = kzuMatrix3x3PropertyTypeCreate(memoryManager, name, KZC_MATRIX3X3_IDENTITY, &matrixPropertyType);
        kzsErrorForward(result);

        propertyType = kzuMatrix3x3PropertyTypeToPropertyType(matrixPropertyType);
    }

    *out_propertyType = propertyType;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadMatrix3x3Property_internal(struct KzcInputStream* inputStream, struct KzcMatrix3x3* out_value)
{
    kzsError result;
    struct KzcMatrix3x3 value = {{0.0f}}; /* Initialize here to prevent compiler warning of potentially uninitialized variable. The variable is initialized in a for-loop below. */
    kzUint i;

    for (i = 0; i < 9; ++i)
    {
        result = kzcInputStreamReadFloat(inputStream, &value.data[i]);
        kzsErrorForward(result);
    }

    *out_value = value;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadMatrix4x4PropertyType_internal(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                                   kzString name, struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuMatrix4x4PropertyType* matrixPropertyType;
    struct KzuPropertyType* propertyType;

    KZ_UNUSED_PARAMETER(inputStream); /* This parameter is just to make the interface uniform between all read functions. */
    
    {
        result = kzuMatrix4x4PropertyTypeCreate(memoryManager, name, KZC_MATRIX4X4_IDENTITY, &matrixPropertyType);
        kzsErrorForward(result);

        propertyType = kzuMatrix4x4PropertyTypeToPropertyType(matrixPropertyType);
    }

    *out_propertyType = propertyType;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadMatrix4x4Property_internal(struct KzcInputStream* inputStream, struct KzcMatrix4x4* out_value)
{
    kzsError result;
    struct KzcMatrix4x4 value = {{0.0f}}; /* Initialize here to prevent compiler warning of potentially uninitialized variable. The variable is initialized in a for-loop below. */
    kzUint i;

    for (i = 0; i < 16; ++i)
    {
        result = kzcInputStreamReadFloat(inputStream, &value.data[i]);
        kzsErrorForward(result);
    }

    *out_value = value;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadEnumPropertyType_internal(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                              kzString name, struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuIntPropertyType* enumPropertyType;
    struct KzuPropertyType* propertyType;

    KZ_UNUSED_PARAMETER(inputStream); /* This parameter is just to make the interface uniform between all read functions. */
    
    {
        result = kzuIntPropertyTypeCreate(memoryManager, name, 0, &enumPropertyType);
        kzsErrorForward(result);

        propertyType = kzuIntPropertyTypeToPropertyType(enumPropertyType);
    }

    *out_propertyType = propertyType;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadLightPropertyType_internal(const struct KzcMemoryManager* memoryManager, struct KzuProject* project,
                                                               struct KzcInputStream* inputStream, const struct KzuBinaryFileInfo* file,
                                                               kzString name, struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuLightPropertyType* lightPropertyType;
    struct KzuPropertyType* propertyType;

    {
        kzString fixedLightPath;
        kzUint propertyTypeCount;
        kzUint i;

        result = kzuBinaryDirectoryReadReference(inputStream, file, &fixedLightPath);
        kzsErrorForward(result);

        kzsErrorTest(fixedLightPath == KZ_NULL, KZU_ERROR_INVALID_BINARY_DATA, "Fixed lights are not supported");

        result = kzuLightPropertyTypeCreate(memoryManager, name, &lightPropertyType);
        kzsErrorForward(result);

        result = kzcInputStreamReadU32Int(inputStream, &propertyTypeCount);
        kzsErrorForward(result);

        for (i = 0; i < propertyTypeCount; ++i)
        {
            kzString propertyTypePath;
            struct KzuPropertyType* childPropertyType;

            result = kzuBinaryDirectoryReadReference(inputStream, file, &propertyTypePath);
            kzsErrorForward(result);

            result = kzuProjectLoaderLoadPropertyType(project, propertyTypePath, &childPropertyType);
            kzsErrorForward(result);

            if (childPropertyType == KZ_NULL)
            {
                /* Unknown property type is not an error due to flexible support for future binary formats as well. */
                kzsLog(KZS_LOG_LEVEL_WARNING, "Unable to load property due to unknown property type");
            }
            else
            {
                result = kzuLightPropertyTypeAddProperty(lightPropertyType, childPropertyType);
                kzsErrorForward(result);
            }
        }

        propertyType = kzuLightPropertyTypeToPropertyType(lightPropertyType);
    }

    *out_propertyType = propertyType;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadLightProperties_internal(struct KzuProject* project, struct KzcInputStream* inputStream,
                                                             const struct KzuBinaryFileInfo* file, const void* object)
{
    kzsError result;

    result = kzuProjectLoaderReadProperties(project, inputStream, file, object);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzuProjectLoaderGetTextureOperation_internal(enum KzuBinaryTextureOperation binaryTextureOperation,
                                                             enum KzuTextureOperation* out_textureOperation)
{
    enum KzuTextureOperation textureOperation;

    switch (binaryTextureOperation)
    {
        case KZU_BINARY_TEXTURE_OPERATION_REPLACE: textureOperation = KZU_TEXTURE_OPERATION_REPLACE; break;
        case KZU_BINARY_TEXTURE_OPERATION_MULTIPLY: textureOperation = KZU_TEXTURE_OPERATION_MODULATE; break;
        case KZU_BINARY_TEXTURE_OPERATION_ADD: textureOperation = KZU_TEXTURE_OPERATION_ADD; break;
        case KZU_BINARY_TEXTURE_OPERATION_DECAL: textureOperation = KZU_TEXTURE_OPERATION_DECAL; break;
        case KZU_BINARY_TEXTURE_OPERATION_CUSTOM: KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("Texture Operation Custom not supported yet.");
        default: kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid texture operation");
    }

    *out_textureOperation = textureOperation;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadTexturePropertyType_internal(const struct KzcMemoryManager* memoryManager,
                                                                 struct KzuProject* project,
                                                                 struct KzcInputStream* inputStream, kzString name,
                                                                 const struct KzuBinaryFileInfo* file,
                                                                 struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuTexturePropertyType* texturePropertyType;
    struct KzuPropertyType* propertyType;
    
    {
        enum KzuBinaryTextureOperation binaryTextureOperation;
        enum KzuTextureOperation textureOperation;
        struct KzuMatrix4x4PropertyType* matrixPropertyType = KZ_NULL;
        kzUint textureUnitId;
        kzString textureMatrixPropertyTypePath;

        result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&binaryTextureOperation);
        kzsErrorForward(result);

        result = kzcInputStreamReadU32Int(inputStream, &textureUnitId);
        kzsErrorForward(result);

        /* TODO: use the texture matrix property type path to set the texture matrix to this texture property type. */ 
        result = kzuBinaryDirectoryReadReference(inputStream, file, &textureMatrixPropertyTypePath);
        kzsErrorForward(result);

        result = kzuProjectLoaderGetTextureOperation_internal(binaryTextureOperation, &textureOperation);
        kzsErrorForward(result);

        if(textureMatrixPropertyTypePath != KZ_NULL)
        {
            struct KzuPropertyType* referencePropertyType = KZ_NULL;
            result = kzuProjectLoaderLoadPropertyType(project, textureMatrixPropertyTypePath, &referencePropertyType);
            kzsErrorForward(result);

            matrixPropertyType = kzuMatrix4x4PropertyTypeFromPropertyType(referencePropertyType);
        }

        result = kzuTexturePropertyTypeCreate(memoryManager, name, textureOperation, textureUnitId, matrixPropertyType,
                                              KZ_NULL, &texturePropertyType);
        kzsErrorForward(result);

        propertyType = kzuTexturePropertyTypeToPropertyType(texturePropertyType);
    }

    *out_propertyType = propertyType;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadTextureProperty_internal(const struct KzuProject* project, struct KzcInputStream* inputStream,
                                                             const struct KzuBinaryFileInfo* file, struct KzcTexture** out_value)
{
    kzsError result;
    struct KzcTexture* value;
    kzString texturePath;
    
    result = kzuBinaryDirectoryReadReference(inputStream, file, &texturePath);
    kzsErrorForward(result);

    if (texturePath != KZ_NULL)
    {
        result = kzuProjectLoaderLoadTexture(project, texturePath, &value);
        kzsErrorForward(result);
    }
    else
    {
        value = KZ_NULL;
    }

    *out_value = value;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadBoolPropertyType_internal(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                              kzString name, struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuBoolPropertyType* boolPropertyType;
    struct KzuPropertyType* propertyType;
    
    KZ_UNUSED_PARAMETER(inputStream); /* This parameter is just to make the interface uniform between all read functions. */
    {
        result = kzuBoolPropertyTypeCreate(memoryManager, name, KZ_FALSE, &boolPropertyType);
        kzsErrorForward(result);

        propertyType = kzuBoolPropertyTypeToPropertyType(boolPropertyType);
    }

    *out_propertyType = propertyType;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadBoolProperty_internal(struct KzcInputStream* inputStream, kzBool* out_value)
{   
    kzsError result; 
    kzBool value;
    
    result = kzcInputStreamReadBoolean(inputStream, &value);
    kzsErrorForward(result);
    result = kzcInputStreamSkip(inputStream, 3);
    kzsErrorForward(result);

    *out_value = value;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadReferencePropertyType_internal(const struct KzcMemoryManager* memoryManager, struct KzuProject* project,
                                                                   struct KzcInputStream* inputStream, kzString name,
                                                                   const struct KzuBinaryFileInfo* file,
                                                                   struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuVoidPropertyType* voidPropertyType;
    struct KzuPropertyType* propertyType;

    {
        /* TODO: Default value */
        /*kzString referencePath;

        result = kzuProjectLoaderReadReference(inputStream, file, &referencePath);
        kzsErrorForward(result);*/
        KZ_UNUSED_PARAMETER(inputStream); /* TODO: Remove when default value is implemented */
        KZ_UNUSED_PARAMETER(file); /* TODO: Remove when default value is implemented */
        KZ_UNUSED_PARAMETER(project); /* TODO: Remove when default value is implemented */
        KZ_UNUSED_PARAMETER(project); /* TODO: Remove when default value is implemented */

        result = kzuVoidPropertyTypeCreate(memoryManager, name, KZ_NULL, &voidPropertyType);
        kzsErrorForward(result);

        propertyType = kzuVoidPropertyTypeToPropertyType(voidPropertyType);
    }

    *out_propertyType = propertyType;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadVoidProperty_internal(struct KzuProject* project,
                                                          struct KzcInputStream* inputStream, const struct KzuBinaryFileInfo* file,
                                                          const struct KzuPropertyType* propertyType,
                                                          void** out_value)
{
    kzsError result;
    void* value = KZ_NULL;
    kzString referencePath;

    kzsAssert(kzcIsValidPointer(propertyType));
    
    result = kzuBinaryDirectoryReadReference(inputStream, file, &referencePath);
    kzsErrorForward(result);

    if (referencePath != KZ_NULL)
    {
        struct KzuBinaryFileInfo* referenceFile;

        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), referencePath, &referenceFile);
        kzsErrorForward(result);

        result = kzuProjectLoaderLoadObject(project, kzuBinaryFileInfoGetType(referenceFile), referencePath, &value);
        kzsErrorForward(result);
    }
    else
    {
        value = KZ_NULL;
    }

    *out_value = value;
    kzsSuccess();
}

kzsError kzuProjectLoaderReadProperty(struct KzuProject* project, struct KzcInputStream* inputStream,
                                      const struct KzuBinaryFileInfo* file, const void* object,
                                      struct KzuPropertyType** out_propertyType)
{
    kzsError result;

    /* TODO: Consider moving variable declarations near using .. */
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuPropertyManager* propertyManager = kzuProjectGetPropertyManager(project);
    kzUint startPosition = kzcInputStreamGetPosition(inputStream);
    kzUint relativePosition;
    kzUint unreadByteCount;
    kzUint size;
    kzString propertyTypePath;
    struct KzuPropertyType* propertyType;
    enum KzuPropertyDataType propertyDataType;
    enum KzuPropertyPriority priority;
    kzUint propertyFlags;

    result = kzcInputStreamReadU32Int(inputStream, &size);
    kzsErrorForward(result);

    kzsErrorTest(size >= 4, KZU_ERROR_INVALID_BINARY_DATA, "Invalid property size, must be at least 4 bytes");

    /* TODO: Currently loads propertyTypes directly, which might cause multiple input streams being opened simultaneously. */
    result = kzuBinaryDirectoryReadReference(inputStream, file, &propertyTypePath);
    kzsErrorForward(result);

    result = kzuProjectLoaderLoadPropertyType(project, propertyTypePath, &propertyType);
    kzsErrorForward(result);

    if (propertyType == KZ_NULL)
    {
        /* Unknown property type is not an error due to flexible support for future binary formats as well. */
        kzsLog(KZS_LOG_LEVEL_WARNING, "Unable to load property due to unknown property type");
    }
    else
    {
        propertyDataType = kzuPropertyTypeGetDataType(propertyType);

        result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&priority);
        kzsErrorForward(result);

        result = kzcInputStreamReadU32Int(inputStream, &propertyFlags);
        kzsErrorForward(result);

        switch (propertyDataType)
        {
            case KZU_PROPERTY_DATA_TYPE_FLOAT:
            {
                kzFloat value;
                result = kzuProjectLoaderReadFloatProperty_internal(inputStream, &value);
                kzsErrorForward(result);
                result = kzuPropertyManagerSetFloat(propertyManager, object, propertyType, value);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_INT:
            {
                kzInt value;
                result = kzuProjectLoaderReadIntProperty_internal(inputStream, &value);
                kzsErrorForward(result);
                result = kzuPropertyManagerSetInt(propertyManager, object, propertyType, value);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_BOOL:
            {
                kzBool value;
                result = kzuProjectLoaderReadBoolProperty_internal(inputStream, &value);
                kzsErrorForward(result);
                result = kzuPropertyManagerSetBool(propertyManager, object, propertyType, value);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_COLOR:
            {
                struct KzcColorRGBA value;
                result = kzuProjectLoaderReadColorProperty_internal(inputStream, &value);
                kzsErrorForward(result);
                result = kzuPropertyManagerSetColor(propertyManager, object, propertyType, value);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_VECTOR2:
            {
                struct KzcVector2 value;
                result = kzuProjectLoaderReadVector2Property_internal(inputStream, &value);
                kzsErrorForward(result);
                result = kzuPropertyManagerSetVector2(propertyManager, object, propertyType, value);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_VECTOR3:
            {
                struct KzcVector3 value;
                result = kzuProjectLoaderReadVector3Property_internal(inputStream, &value);
                kzsErrorForward(result);
                result = kzuPropertyManagerSetVector3(propertyManager, object, propertyType, value);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_VECTOR4:
            {
                struct KzcVector4 value;
                result = kzuProjectLoaderReadVector4Property_internal(inputStream, &value);
                kzsErrorForward(result);
                result = kzuPropertyManagerSetVector4(propertyManager, object, propertyType, value);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
            {
                struct KzcMatrix2x2 value;
                result = kzuProjectLoaderReadMatrix2x2Property_internal(inputStream, &value);
                kzsErrorForward(result);
                result = kzuPropertyManagerSetMatrix2x2(propertyManager, object, propertyType, &value);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
            {
                struct KzcMatrix3x3 value;
                result = kzuProjectLoaderReadMatrix3x3Property_internal(inputStream, &value);
                kzsErrorForward(result);
                result = kzuPropertyManagerSetMatrix3x3(propertyManager, object, propertyType, &value);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
            {
                struct KzcMatrix4x4 value;
                result = kzuProjectLoaderReadMatrix4x4Property_internal(inputStream, &value);
                kzsErrorForward(result);
                result = kzuPropertyManagerSetMatrix4x4(propertyManager, object, propertyType, &value);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_STRING:
            {
                kzMutableString value;
                result = kzuProjectLoaderReadStringProperty_internal(memoryManager, inputStream, &value);
                kzsErrorForward(result);
                result = kzuPropertyManagerSetString(propertyManager, object, propertyType, value);
                kzsErrorForward(result);
                result = kzcStringDelete(value);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_VOID:
            {
                void* value;
                result = kzuProjectLoaderReadVoidProperty_internal(project, inputStream, file, propertyType, &value);
                kzsErrorForward(result);
                result = kzuPropertyManagerSetVoid(propertyManager, object, propertyType, value);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_LIGHT:
            {
                result = kzuProjectLoaderReadLightProperties_internal(project, inputStream, file, object);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_TEXTURE:
            {
                struct KzcTexture* value;
                result = kzuProjectLoaderReadTextureProperty_internal(project, inputStream, file, &value);
                kzsErrorForward(result);
                result = kzuPropertyManagerSetVoid(propertyManager, object, propertyType, value);
                kzsErrorForward(result);
                break;
            }

            default:
            {
                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid property type data type");
            }
        }

        {
            kzBool set;
            set = kzuPropertyManagerSetPropertyPriority(propertyManager, object, propertyType, priority);
            KZ_UNUSED_RETURN_VALUE(set);
            set = kzuPropertyManagerSetPropertyFlag(propertyManager, object, propertyType, KZU_PROPERTY_MANAGER_FLAG_FROM_KZB);
            KZ_UNUSED_RETURN_VALUE(set);
        }
    }

    /* Skip unread bytes in the property, if there are any. This is to help forward compatibility of the binary format. */
    relativePosition = kzcInputStreamGetPosition(inputStream) - startPosition; /* TODO: Exact match if file version matches */
    kzsErrorTest(relativePosition <= size, KZU_ERROR_INVALID_BINARY_DATA, "Property size was smaller than expected");

    unreadByteCount = size - relativePosition;
    result = kzcInputStreamSkip(inputStream, unreadByteCount);
    kzsErrorForward(result);

    *out_propertyType = propertyType;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadColorProperty_old(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                      struct KzuPropertyType* propertyType, struct KzuProperty** out_property)
{
    kzsError result;
    struct KzuColorProperty* colorProperty;
    struct KzuProperty* property;

    /* TODO: Consider removing unnecessary curly braces */
    {
        struct KzcColorRGBA color;

        result = kzcInputStreamReadFloat(inputStream, &color.red);
        kzsErrorForward(result);
        result = kzcInputStreamReadFloat(inputStream, &color.green);
        kzsErrorForward(result);
        result = kzcInputStreamReadFloat(inputStream, &color.blue);
        kzsErrorForward(result);
        result = kzcInputStreamReadFloat(inputStream, &color.alpha);
        kzsErrorForward(result);

        result = kzuColorPropertyCreate(memoryManager, kzuColorPropertyTypeFromPropertyType(propertyType), color, &colorProperty);
        kzsErrorForward(result);

        property = kzuColorPropertyToProperty(colorProperty);
    }

    *out_property = property;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadStringProperty_old(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                       struct KzuPropertyType* propertyType, struct KzuProperty** out_property)
{
    kzsError result;
    struct KzuStringProperty* stringProperty;
    struct KzuProperty* property;

    /* TODO: Consider removing unnecessary curly braces */
    {
        kzMutableString string;

        result = kzuBinaryReadString(memoryManager, inputStream, &string);
        kzsErrorForward(result);

        result = kzuStringPropertyCreate(memoryManager, kzuStringPropertyTypeFromPropertyType(propertyType), string, &stringProperty);
        kzsErrorForward(result);

        property = kzuStringPropertyToProperty(stringProperty);

        result = kzcStringDelete(string);
        kzsErrorForward(result);
    }

    *out_property = property;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadIntProperty_old(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                    struct KzuPropertyType* propertyType, struct KzuProperty** out_property)
{
    kzsError result;
    struct KzuIntProperty* intProperty;
    struct KzuProperty* property;

    {
        kzInt value;

        result = kzcInputStreamReadS32Int(inputStream, &value);
        kzsErrorForward(result);

        result = kzuIntPropertyCreate(memoryManager, kzuIntPropertyTypeFromPropertyType(propertyType), value, &intProperty);
        kzsErrorForward(result);

        property = kzuIntPropertyToProperty(intProperty);
    }

    *out_property = property;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadVector2Property_old(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                        struct KzuPropertyType* propertyType, struct KzuProperty** out_property)
{
    kzsError result;
    struct KzuVector2Property* vectorProperty;
    struct KzuProperty* property;

    {
        struct KzcVector2 value;
        kzFloat x;
        kzFloat y;

        result = kzcInputStreamReadFloat(inputStream, &x);
        kzsErrorForward(result);

        result = kzcInputStreamReadFloat(inputStream, &y);
        kzsErrorForward(result);

        kzcVector2Set(&value, x, y);

        result = kzuVector2PropertyCreate(memoryManager, kzuVector2PropertyTypeFromPropertyType(propertyType), value,
            &vectorProperty);
        kzsErrorForward(result);

        property = kzuVector2PropertyToProperty(vectorProperty);
    }

    *out_property = property;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadVector3Property_old(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                        struct KzuPropertyType* propertyType, struct KzuProperty** out_property)
{
    kzsError result;
    struct KzuVector3Property* vectorProperty;
    struct KzuProperty* property;

    {
        struct KzcVector3 value;
        kzFloat x;
        kzFloat y;
        kzFloat z;

        result = kzcInputStreamReadFloat(inputStream, &x);
        kzsErrorForward(result);

        result = kzcInputStreamReadFloat(inputStream, &y);
        kzsErrorForward(result);

        result = kzcInputStreamReadFloat(inputStream, &z);
        kzsErrorForward(result);

        kzcVector3Set(&value, x, y, z);

        result = kzuVector3PropertyCreate(memoryManager, kzuVector3PropertyTypeFromPropertyType(propertyType), value,
            &vectorProperty);
        kzsErrorForward(result);

        property = kzuVector3PropertyToProperty(vectorProperty);
    }

    *out_property = property;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadVector4Property_old(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                        struct KzuPropertyType* propertyType, struct KzuProperty** out_property)
{
    kzsError result;
    struct KzuVector4Property* vectorProperty;
    struct KzuProperty* property;

    {
        struct KzcVector4 value;
        kzFloat x;
        kzFloat y;
        kzFloat z;
        kzFloat w;

        result = kzcInputStreamReadFloat(inputStream, &x);
        kzsErrorForward(result);

        result = kzcInputStreamReadFloat(inputStream, &y);
        kzsErrorForward(result);

        result = kzcInputStreamReadFloat(inputStream, &z);
        kzsErrorForward(result);

        result = kzcInputStreamReadFloat(inputStream, &w);
        kzsErrorForward(result);

        kzcVector4Set(&value, x, y, z, w);

        result = kzuVector4PropertyCreate(memoryManager, kzuVector4PropertyTypeFromPropertyType(propertyType), value,
            &vectorProperty);
        kzsErrorForward(result);

        property = kzuVector4PropertyToProperty(vectorProperty);
    }

    *out_property = property;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadMatrix2x2Property_old(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                          struct KzuPropertyType* propertyType, struct KzuProperty** out_property)
{
    kzsError result;
    struct KzuMatrix2x2Property* matrixProperty;
    struct KzuProperty* property;

    {
        struct KzcMatrix2x2 value = {{0.0f}}; /* Initialize here to prevent compiler warning of potentially uninitialized variable. The variable is initialized in a for-loop below. */
        kzUint i;

        for (i = 0; i < 4; ++i)
        {
            result = kzcInputStreamReadFloat(inputStream, &value.data[i]);
            kzsErrorForward(result);
        }

        /* TODO: Initialize transformation matrix - struct members */
        result = kzuMatrix2x2PropertyCreate(memoryManager, kzuMatrix2x2PropertyTypeFromPropertyType(propertyType), &value, &matrixProperty);
        kzsErrorForward(result);

        property = kzuMatrix2x2PropertyToProperty(matrixProperty);
    }

    *out_property = property;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadMatrix3x3Property_old(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                          struct KzuPropertyType* propertyType, struct KzuProperty** out_property)
{
    kzsError result;
    struct KzuMatrix3x3Property* matrixProperty;
    struct KzuProperty* property;

    {
        struct KzcMatrix3x3 value = {{0.0f}}; /* Initialize here to prevent compiler warning of potentially uninitialized variable. The variable is initialized in a for-loop below. */
        kzUint i;

        for (i = 0; i < 9; ++i)
        {
            result = kzcInputStreamReadFloat(inputStream, &value.data[i]);
            kzsErrorForward(result);
        }

        /* TODO: Initialize transformation matrix - struct members */
        result = kzuMatrix3x3PropertyCreate(memoryManager, kzuMatrix3x3PropertyTypeFromPropertyType(propertyType), &value, &matrixProperty);
        kzsErrorForward(result);

        property = kzuMatrix3x3PropertyToProperty(matrixProperty);
    }

    *out_property = property;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadMatrix4x4Property_old(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                          struct KzuPropertyType* propertyType, struct KzuProperty** out_property)
{
    kzsError result;
    struct KzuMatrix4x4Property* matrixProperty;
    struct KzuProperty* property;

    {
        struct KzcMatrix4x4 value = {{0.0f}}; /* Initialize here to prevent compiler warning of potentially uninitialized variable. The variable is initialized in a for-loop below. */
        kzUint i;

        for (i = 0; i < 16; ++i)
        {
            result = kzcInputStreamReadFloat(inputStream, &value.data[i]);
            kzsErrorForward(result);
        }

        /* TODO: Initialize transformation matrix - struct members */
        result = kzuMatrix4x4PropertyCreate(memoryManager, kzuMatrix4x4PropertyTypeFromPropertyType(propertyType), &value, &matrixProperty);
        kzsErrorForward(result);

        property = kzuMatrix4x4PropertyToProperty(matrixProperty);
    }

    *out_property = property;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadFloatProperty_old(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                      struct KzuPropertyType* propertyType, struct KzuProperty** out_property)
{
    kzsError result;
    struct KzuFloatProperty* floatProperty;
    struct KzuProperty* property;

    {
        kzFloat value;

        result = kzcInputStreamReadFloat(inputStream, &value);
        kzsErrorForward(result);

        result = kzuFloatPropertyCreate(memoryManager, kzuFloatPropertyTypeFromPropertyType(propertyType), value, &floatProperty);
        kzsErrorForward(result);

        property = kzuFloatPropertyToProperty(floatProperty);
    }

    *out_property = property;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadBoolProperty_old(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                                                     struct KzuPropertyType* propertyType, struct KzuProperty** out_property)
{   
    kzsError result; 
    struct KzuBoolProperty* boolProperty;
    struct KzuProperty* property;

    {
        kzBool value;

        result = kzcInputStreamReadBoolean(inputStream, &value);
        kzsErrorForward(result);
        result = kzcInputStreamSkip(inputStream, 3);
        kzsErrorForward(result);

        result = kzuBoolPropertyCreate(memoryManager, kzuBoolPropertyTypeFromPropertyType(propertyType), value, &boolProperty);
        kzsErrorForward(result);

        property = kzuBoolPropertyToProperty(boolProperty);
    }

    *out_property = property;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadVoidProperty_old(const struct KzcMemoryManager* memoryManager, struct KzuProject* project,
                                                     struct KzcInputStream* inputStream, const struct KzuBinaryFileInfo* file,
                                                     struct KzuPropertyType* propertyType, struct KzuProperty** out_property)
{
    kzsError result;
    struct KzuProperty* property;
    kzString referencePath;
    struct KzuVoidProperty* voidProperty;
    void* object;

    kzsAssert(kzcIsValidPointer(propertyType));

    result = kzuBinaryDirectoryReadReference(inputStream, file, &referencePath);
    kzsErrorForward(result);

    if (referencePath != KZ_NULL)
    {
        struct KzuBinaryFileInfo* referenceFile;

        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), referencePath, &referenceFile);
        kzsErrorForward(result);

        result = kzuProjectLoaderLoadObject(project, kzuBinaryFileInfoGetType(referenceFile), referencePath, &object);
        kzsErrorForward(result);
    }
    else
    {
        object = KZ_NULL;
    }

    result = kzuVoidPropertyCreate(memoryManager, kzuVoidPropertyTypeFromPropertyType(propertyType), object, &voidProperty);
    kzsErrorForward(result);

    property = kzuVoidPropertyToProperty(voidProperty);

    *out_property = property;
    kzsSuccess();
}

static kzsError kzuProjectLoaderReadTextureProperty_old(const struct KzcMemoryManager* memoryManager, const struct KzuProject* project,
                                                        struct KzcInputStream* inputStream, const struct KzuBinaryFileInfo* file,
                                                        struct KzuPropertyType* propertyType, struct KzuProperty** out_property)
{
    kzsError result;
    struct KzuTextureProperty* textureProperty;
    struct KzuProperty* property;

    {
        kzString texturePath;
        struct KzcTexture* texture = KZ_NULL;

        result = kzuBinaryDirectoryReadReference(inputStream, file, &texturePath);
        kzsErrorForward(result);

        if (texturePath != KZ_NULL) /* TODO: How to handle this? */
        {
            result = kzuProjectLoaderLoadTexture(project, texturePath, &texture);
            kzsErrorForward(result);
        }

        result = kzuTexturePropertyCreate(memoryManager, kzuTexturePropertyTypeFromPropertyType(propertyType), texture, &textureProperty);
        kzsErrorForward(result);

        property = kzuTexturePropertyToProperty(textureProperty);
    }

    *out_property = property;
    kzsSuccess();
}

kzsError kzuProjectLoaderReadProperty_old(struct KzuProject* project, struct KzcInputStream* inputStream,
                                          const struct KzuBinaryFileInfo* file, struct KzuProperty** out_property)
{
    kzsError result;
    struct KzuProperty* property;

    /* TODO: Consider moving variable declarations near using .. */
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    kzUint startPosition = kzcInputStreamGetPosition(inputStream);
    kzUint relativePosition;
    kzUint unreadByteCount;
    kzUint size;
    kzString propertyTypePath;
    struct KzuPropertyType* propertyType;
    enum KzuPropertyDataType propertyDataType;

    result = kzcInputStreamReadU32Int(inputStream, &size);
    kzsErrorForward(result);

    kzsErrorTest(size >= 4, KZU_ERROR_INVALID_BINARY_DATA, "Invalid property size, must be at least 4 bytes");

    /* TODO: Currently loads propertyTypes directly, which might cause multiple input streams being opened simultaneously. */
    result = kzuBinaryDirectoryReadReference(inputStream, file, &propertyTypePath);
    kzsErrorForward(result);

    result = kzuProjectLoaderLoadPropertyType(project, propertyTypePath, &propertyType);
    kzsErrorForward(result);

    if (propertyType == KZ_NULL)
    {
        /* Unknown property type is not an error due to flexible support for future binary formats as well. */
        kzsLog(KZS_LOG_LEVEL_WARNING, "Unable to load property due to unknown property type");
        property = KZ_NULL;
    }
    else
    {
        enum KzuPropertyPriority priority;
        kzBool inherited;

        propertyDataType = kzuPropertyTypeGetDataType(propertyType);

        result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&priority);
        kzsErrorForward(result);

        result = kzcInputStreamReadBoolean(inputStream, &inherited);
        kzsErrorForward(result);

        result = kzcInputStreamSkip(inputStream, 3);
        kzsErrorForward(result);

        switch (propertyDataType)
        {
            case KZU_PROPERTY_DATA_TYPE_FLOAT:
            {
                result = kzuProjectLoaderReadFloatProperty_old(memoryManager, inputStream, propertyType, &property);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_INT:
            {
                result = kzuProjectLoaderReadIntProperty_old(memoryManager, inputStream, propertyType, &property);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_BOOL:
            {
                result = kzuProjectLoaderReadBoolProperty_old(memoryManager, inputStream, propertyType, &property);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_COLOR:
            {
                result = kzuProjectLoaderReadColorProperty_old(memoryManager, inputStream, propertyType, &property);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_VECTOR2:
            {
                result = kzuProjectLoaderReadVector2Property_old(memoryManager, inputStream, propertyType, &property);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_VECTOR3:
            {
                result = kzuProjectLoaderReadVector3Property_old(memoryManager, inputStream, propertyType, &property);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_VECTOR4:
            {
                result = kzuProjectLoaderReadVector4Property_old(memoryManager, inputStream, propertyType, &property);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
            {
                result = kzuProjectLoaderReadMatrix2x2Property_old(memoryManager, inputStream, propertyType, &property);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
            {
                result = kzuProjectLoaderReadMatrix3x3Property_old(memoryManager, inputStream, propertyType, &property);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
            {
                result = kzuProjectLoaderReadMatrix4x4Property_old(memoryManager, inputStream, propertyType, &property);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_STRING:
            {
                result = kzuProjectLoaderReadStringProperty_old(memoryManager, inputStream, propertyType, &property);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_VOID:
            {
                result = kzuProjectLoaderReadVoidProperty_old(memoryManager, project, inputStream, file, propertyType, &property);
                kzsErrorForward(result);
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_LIGHT:
            {
                /* This is disabled and will be removed after message refactoring. If this is encountered inform Alex. */
                kzsAssert(KZ_FALSE);
                property = KZ_NULL;
                break;
            }

            case KZU_PROPERTY_DATA_TYPE_TEXTURE:
            {
                result = kzuProjectLoaderReadTextureProperty_old(memoryManager, project, inputStream, file, propertyType, &property);
                kzsErrorForward(result);
                break;
            }

            default:
            {
                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid property type data type");
            }
        }

        /* Set common data. */
        kzuPropertySetPriority(property, priority);
        kzuPropertySetInherited(property, inherited);

        /* Set kzb flag. */
        kzuPropertySetFromKzb(property);
    }

    /* Skip unread bytes in the property, if there are any. This is to help forward compatibility of the binary format. */
    relativePosition = kzcInputStreamGetPosition(inputStream) - startPosition; /* TODO: Exact match if file version matches */
    kzsErrorTest(relativePosition <= size, KZU_ERROR_INVALID_BINARY_DATA, "Property size was smaller than expected");

    unreadByteCount = size - relativePosition;
    result = kzcInputStreamSkip(inputStream, unreadByteCount);
    kzsErrorForward(result);

    *out_property = property;
    kzsSuccess();
}

kzsError kzuProjectLoaderReadProperties(struct KzuProject* project, struct KzcInputStream* inputStream,
                                        const struct KzuBinaryFileInfo* file, const void* object)
{
    kzsError result;
    kzUint propertyCount;
    kzUint i;

    result = kzcInputStreamReadU32Int(inputStream, &propertyCount);
    kzsErrorForward(result);

    for (i = 0; i < propertyCount; ++i)
    {
        struct KzuPropertyType* propertyType;
        result = kzuProjectLoaderReadProperty(project, inputStream, file, object, &propertyType);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuProjectLoaderReadPropertyType(struct KzuProject* project, struct KzcInputStream* inputStream,
                                          const struct KzuBinaryFileInfo* file, struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuPropertyType* propertyType;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    enum KzuBinaryPropertyType dataType;
    kzString name = kzuBinaryFileInfoGetName(file);
    kzUint inherited;

    result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&dataType);
    kzsErrorForward(result);

    result = kzcInputStreamReadU32Int(inputStream, &inherited);
    kzsErrorForward(result);

    switch (dataType)
    {
        case KZU_BINARY_PROPERTY_TYPE_COLOR:
        {
            result = kzuProjectLoaderReadColorPropertyType_internal(memoryManager, inputStream, name, &propertyType);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_PROPERTY_TYPE_STRING:
        {
            result = kzuProjectLoaderReadStringPropertyType_internal(memoryManager, inputStream, name, &propertyType);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_PROPERTY_TYPE_INT32:
        {
            result = kzuProjectLoaderReadIntPropertyType_internal(memoryManager, inputStream, name, &propertyType);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_PROPERTY_TYPE_FLOAT:
        {
            result = kzuProjectLoaderReadFloatPropertyType_internal(memoryManager, inputStream, name, &propertyType);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_PROPERTY_TYPE_VECTOR2D:
        {
            result = kzuProjectLoaderReadVector2PropertyType_internal(memoryManager, inputStream, name, &propertyType);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_PROPERTY_TYPE_VECTOR3D:
        {
            result = kzuProjectLoaderReadVector3PropertyType_internal(memoryManager, inputStream, name, &propertyType);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_PROPERTY_TYPE_VECTOR4D:
        {
            result = kzuProjectLoaderReadVector4PropertyType_internal(memoryManager, inputStream, name, &propertyType);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_PROPERTY_TYPE_MATRIX2D:
        {
            result = kzuProjectLoaderReadMatrix2x2PropertyType_internal(memoryManager, inputStream, name, &propertyType);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_PROPERTY_TYPE_MATRIX3D:
        {
            result = kzuProjectLoaderReadMatrix3x3PropertyType_internal(memoryManager, inputStream, name, &propertyType);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_PROPERTY_TYPE_MATRIX4D:
        {
            result = kzuProjectLoaderReadMatrix4x4PropertyType_internal(memoryManager, inputStream, name, &propertyType);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_PROPERTY_TYPE_ENUM:
        {
            result = kzuProjectLoaderReadEnumPropertyType_internal(memoryManager, inputStream, name, &propertyType);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_PROPERTY_TYPE_LIGHT:
        {
            result = kzuProjectLoaderReadLightPropertyType_internal(memoryManager, project, inputStream, file, name, &propertyType);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_PROPERTY_TYPE_CAMERA:
        {
            KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("Camera property not supported");
        }

        case KZU_BINARY_PROPERTY_TYPE_TEXTURE:
        {
            result = kzuProjectLoaderReadTexturePropertyType_internal(memoryManager, project, inputStream, name, file, &propertyType);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_PROPERTY_TYPE_BOOLEAN:
        {
            result = kzuProjectLoaderReadBoolPropertyType_internal(memoryManager, inputStream, name, &propertyType);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_PROPERTY_TYPE_REFERENCE:
        {
            result = kzuProjectLoaderReadReferencePropertyType_internal(memoryManager, project, inputStream, name, file, &propertyType);
            kzsErrorForward(result);
            break;
        }

        default:
        {
            /* Unknown property type is not an error due to flexible support for future binary formats as well. */
            kzsLog(KZS_LOG_LEVEL_WARNING, "Unknown property type data type");
            kzsDebugBreak();
            propertyType = KZ_NULL;
        }
    }

    kzuPropertyTypeSetInherited(propertyType, inherited != 0);

    *out_propertyType = propertyType;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadPropertyType(struct KzuProject* project, kzString path, struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuPropertyType* propertyType;
    kzString name;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load property type with null path");

    /* Check if the property type is already loaded. */
    /* Special handling for property types, since the name is meaningful and not just the path */
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFileFromPropertyTypeLibrary(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_PROPERTY_TYPE,
                     KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load property type file.");

        name = kzuBinaryFileInfoGetName(file);
    }

    propertyType = kzuProjectGetPropertyType(project, name);

    if (!kzuPropertyTypeIsValid(propertyType))
    {
        /* Load the property type of the file info. */
        {
            /* Read data */
            {
                struct KzcInputStream* inputStream;

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);
    
                result = kzuProjectLoaderReadPropertyType(project, inputStream, file, &propertyType);
                kzsErrorForward(result);

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);
            }

            if (propertyType != KZ_NULL)
            {
                result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_PROPERTY_TYPE, propertyType, name,
                    (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&propertyType);
                kzsErrorForward(result);
            }
        }
    }

    *out_propertyType = propertyType;
    kzsSuccess();
}
kzsException kzuProjectLoaderUnloadPropertyType(struct KzuProject* project, const struct KzuPropertyType* propertyType)
{
    kzsError result;
    kzString objectPath = kzuProjectGetObjectPath(project, propertyType);

    if(objectPath != KZ_NULL)
    {
        result = kzuProjectLoaderUnloadObject(project, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
