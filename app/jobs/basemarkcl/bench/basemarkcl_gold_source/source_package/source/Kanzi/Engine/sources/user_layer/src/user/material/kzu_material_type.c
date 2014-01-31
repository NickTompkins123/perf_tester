/**
* \file
* Specifies material type. Material type is set of property types that
* define material (e.g. textured, 2 lights).
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_material_type.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_light_property.h>
#include <user/properties/kzu_property_collection.h>
#include <user/kzu_error_codes.h>

#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/string/kzc_string.h>
#include <core/memory/kzc_memory_manager.h>

#include <system/wrappers/kzs_opengl_base.h>
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
#include <core/resource_manager/kzc_resource_manager.h>
#include <core/resource_manager/shader/kzc_resource_shader.h>
#endif


struct KzuMaterialType
{
    struct KzuMaterialTypeData* data;
};

struct KzuMaterialTypeData
{
    kzBool selfOwned;
    struct KzuPropertyTypeCollection* propertyTypes;   /**< List of property types that define this material type. */
    struct KzcHashMap* lightPropertyTypes;  /**< List of light property types that define lights for this material. <KzuLightPropertyType, uint(lightCount)>.*/

    struct KzcDynamicArray* vertexAttributes;    /**< Shader attributes for this material type. <String(attributeName)>. */

    kzBool validShader;                         /**< Boolean for indicating if this shader (material type) is valid. */
    kzBool shaderSetCalled;                   /**< Boolean for indicating if shader set has been called. */

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    struct KzcShader* shaderProgram;         /**< Shader program linked to this material type. */
#endif
};


kzsError kzuMaterialTypeCreate(const struct KzcMemoryManager* memoryManager, struct KzuMaterialType** out_materialType)
{
    kzsError result;
    struct KzuMaterialType* materialType;
    struct KzuMaterialTypeData* materialTypeData;
    
    result = kzcMemoryAllocVariable(memoryManager, materialType, "Material type");
    kzsErrorForward(result);
    
    result = kzcMemoryAllocVariable(memoryManager, materialTypeData, "Material type data");
    kzsErrorForward(result);

    materialType->data = materialTypeData;
    materialTypeData->selfOwned = KZ_TRUE;

    /* Allocate memory for material property types. */
    result = kzuPropertyTypeCollectionCreate(memoryManager, &materialTypeData->propertyTypes);
    kzsErrorForward(result);
    
    /* Allocates memory for material lights. */
    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &materialTypeData->lightPropertyTypes);
    kzsErrorForward(result);

    materialTypeData->vertexAttributes = KZ_NULL;
    materialTypeData->validShader = KZ_FALSE;
    materialTypeData->shaderSetCalled = KZ_FALSE;
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    materialTypeData->shaderProgram = KZ_NULL;
#endif

    *out_materialType = materialType;
    kzsSuccess();
}

kzsError kzuMaterialTypeDelete(struct KzuMaterialType* materialType)
{
    kzsError result;
    struct KzuMaterialTypeData* materialTypeData;
    kzBool selfOwned;
    struct KzcHashMapIterator hashMapIterator;
    kzsAssert(kzuMaterialTypeIsValid(materialType));

    materialTypeData = materialType->data;
    selfOwned = materialTypeData->selfOwned;

    /* Delete light count variables. */
    hashMapIterator = kzcHashMapGetIterator(materialTypeData->lightPropertyTypes);
    while(kzcHashMapIterate(hashMapIterator))
    {
        kzUint* lightCount = (kzUint*)kzcHashMapIteratorGetValue(hashMapIterator);

        result = kzcMemoryFreeVariable(lightCount);
        kzsErrorForward(result);
    }

    if(materialTypeData->vertexAttributes != KZ_NULL)
    {
        struct KzcDynamicArrayIterator it;

        it = kzcDynamicArrayGetIterator(materialTypeData->vertexAttributes);
        while(kzcDynamicArrayIterate(it))
        {
            kzMutableString attributeName = (kzMutableString)kzcDynamicArrayIteratorGetValue(it);

            result = kzcStringDelete(attributeName);
            kzsErrorForward(result);
        }
        result = kzcDynamicArrayDelete(materialTypeData->vertexAttributes);
        kzsErrorForward(result);
    }

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    if (materialType->data->shaderProgram != KZ_NULL)
    {
        result = kzcShaderDelete(materialTypeData->shaderProgram);
        kzsErrorForward(result);
    }
#endif

    /* Delete light property types hash map. */
    result = kzcHashMapDelete(materialTypeData->lightPropertyTypes);
    kzsErrorForward(result);
    /* Delete material property types dynamic array. */
    result = kzuPropertyTypeCollectionDelete(materialTypeData->propertyTypes);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(materialTypeData);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(materialType);
        kzsErrorForward(result);
    }
    else
    {
        materialType->data = KZ_NULL;
    }

    kzsSuccess();
}

void kzuMaterialTypeTakeOwnership(const struct KzuMaterialType* materialType)
{
    kzsAssert(kzuMaterialTypeIsValid(materialType));

    materialType->data->selfOwned = KZ_FALSE;
}

kzBool kzuMaterialTypeIsValid(const struct KzuMaterialType* materialType)
{
    return materialType != KZ_NULL && materialType->data != KZ_NULL;
}

void kzuMaterialTypeTransferData(struct KzuMaterialType* targetMaterialType, struct KzuMaterialType* sourceMaterialType)
{
    kzsAssert(kzcIsValidPointer(targetMaterialType));
    kzsAssert(targetMaterialType->data == KZ_NULL);
    kzsAssert(kzuMaterialTypeIsValid(sourceMaterialType));

    targetMaterialType->data = sourceMaterialType->data;
    sourceMaterialType->data = KZ_NULL;
}

kzsError kzuMaterialTypeAddPropertyType(const struct KzuMaterialType* materialType, struct KzuPropertyType* propertyType)
{
    kzsError result;
    
    kzsAssert(kzuMaterialTypeIsValid(materialType));
    kzsAssert(kzcIsValidPointer(propertyType));

    if (kzuPropertyTypeGetDataType(propertyType) == KZU_PROPERTY_DATA_TYPE_LIGHT)
    {
        result = kzuMaterialTypeAddLightPropertyType(materialType, kzuLightPropertyTypeFromPropertyType(propertyType));
        kzsErrorForward(result);
    }
    else
    {
        result = kzuPropertyTypeCollectionAddPropertyType(materialType->data->propertyTypes, propertyType);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuMaterialTypeAddLightPropertyType(const struct KzuMaterialType* materialType, const struct KzuLightPropertyType* propertyType)
{
    kzsError result;
    kzBool elementFound;
    kzUint* lightPropertyTypeCount;
    struct KzcMemoryManager* memoryManager;

    kzsAssert(kzuMaterialTypeIsValid(materialType));
    memoryManager = kzcMemoryGetManager(materialType);

    elementFound = kzcHashMapGet(materialType->data->lightPropertyTypes, propertyType, (void**)&lightPropertyTypeCount);

    if(!elementFound)
    {
        result = kzcMemoryAllocVariable(memoryManager, lightPropertyTypeCount, "MaterialTypeLightPropertyCount");
        kzsErrorForward(result);

        result = kzcHashMapPut(materialType->data->lightPropertyTypes, propertyType, lightPropertyTypeCount);
        kzsErrorForward(result);

        *lightPropertyTypeCount = 0;
    }
    ++(*lightPropertyTypeCount);

    kzsSuccess();
}

struct KzuPropertyTypeCollection* kzuMaterialTypeGetPropertyTypeCollection(const struct KzuMaterialType* materialType)
{
    kzsAssert(kzuMaterialTypeIsValid(materialType));

    return materialType->data->propertyTypes;
}

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
struct KzcShader* kzuMaterialTypeGetShaderProgram(const struct KzuMaterialType* materialType)
{
    kzsAssert(kzuMaterialTypeIsValid(materialType));
    return materialType->data->shaderProgram;
}
#endif

kzsError kzuMaterialTypeSetShader(const struct KzuMaterialType* materialType, struct KzcResourceManager* resourceManager,
                                  enum KzcResourceMemoryType memoryType, const struct KzuMaterialTypeShaderInfo* shaderInfo, kzBool* out_shaderValid)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    struct KzcDynamicArrayIterator it;
    kzBool shaderValid = KZ_TRUE;
    
    kzsAssert(shaderInfo != KZ_NULL);

#if !defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    KZ_UNUSED_PARAMETER(memoryType);
#endif

    kzsAssert(kzuMaterialTypeIsValid(materialType));
    materialType->data->shaderSetCalled = KZ_TRUE;
    memoryManager = kzcMemoryGetManager(materialType);

    /* Create array for attributes. */
    result = kzcDynamicArrayCreate(memoryManager, &materialType->data->vertexAttributes);
    kzsErrorForward(result);

    it = kzcDynamicArrayGetIterator(shaderInfo->vertexAttributes);
    while(kzcDynamicArrayIterate(it))
    {
        kzString attributeName = (kzString)kzcDynamicArrayIteratorGetValue(it);
        kzMutableString newAttributeName;

        result = kzcStringCopy(memoryManager, attributeName, &newAttributeName);
        kzsErrorForward(result);
        result = kzcDynamicArrayAdd(materialType->data->vertexAttributes, newAttributeName);
        kzsErrorForward(result);
    }

    switch(shaderInfo->type)
    {
        case KZU_SHADER_TYPE_NONE:
        {
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
            kzsErrorThrow(KZU_ERROR_INVALID_SHADER_TYPE, "Invalid shader type assigned for material! (fixed for es2)");
#else
            break;
#endif
        }
        case KZU_SHADER_TYPE_SOURCE:
        {
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
            result = kzcShaderSourceCreate(resourceManager, memoryType,
                                           shaderInfo->shaderData.code.vertex, shaderInfo->shaderData.code.fragment,
                                           materialType->data->vertexAttributes, &shaderValid, &materialType->data->shaderProgram);
            kzsErrorForward(result);

            break;
#else
            kzsErrorThrow(KZU_ERROR_INVALID_SHADER_TYPE, "Invalid shader type assigned for material! (shader program for non-es1)");
#endif
        }
        case KZU_SHADER_TYPE_BINARY_SEPARATED:
        {
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
            result = kzcShaderBinarySeparatedCreate(resourceManager, memoryType,
                    shaderInfo->shaderData.binarySeparated.vertexData,
                    shaderInfo->shaderData.binarySeparated.vertexDataSize,
                    shaderInfo->shaderData.binarySeparated.fragmentData,
                    shaderInfo->shaderData.binarySeparated.fragmentDataSize,
                    materialType->data->vertexAttributes,
                    &shaderValid, &materialType->data->shaderProgram);
            kzsErrorForward(result);
            break;
#else
            kzsErrorThrow(KZU_ERROR_INVALID_SHADER_TYPE, "Invalid shader type assigned for material! (shader program for non-es1)");
#endif
        }
        case KZU_SHADER_TYPE_BINARY_COMBINED:
        {
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
            result = kzcShaderBinaryCombinedCreate(resourceManager, memoryType,
                    shaderInfo->shaderData.binaryCombined.data,
                    shaderInfo->shaderData.binaryCombined.size,
                    materialType->data->vertexAttributes,
                    &shaderValid, &materialType->data->shaderProgram);
            kzsErrorForward(result);
            break;
#else
            kzsErrorThrow(KZU_ERROR_INVALID_SHADER_TYPE, "Invalid shader type assigned for material! (shader program for non-es1)");
#endif
        }
        default:
        {
            kzsErrorThrow(KZU_ERROR_INVALID_SHADER_TYPE, "Invalid shader type assigned for material! (no support for binary shaders / invalid shader)");
        }
    }

#if !defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    KZ_UNUSED_PARAMETER(resourceManager);   /* No resource manager needed if shader is not created. */
#endif

    materialType->data->validShader = shaderValid;
    *out_shaderValid = shaderValid;
    kzsSuccess();
}

struct KzcDynamicArray* kzuMaterialTypeGetVertexAttributes(const struct KzuMaterialType* materialType)
{
    kzsAssert(kzuMaterialTypeIsValid(materialType));

    return materialType->data->vertexAttributes;
}

struct KzcDynamicArray* kzuMaterialTypeGetPropertyTypes(const struct KzuMaterialType* materialType)
{
    kzsAssert(kzuMaterialTypeIsValid(materialType));

    return kzuPropertyTypeCollectionGetPropertyTypes(materialType->data->propertyTypes);
}

struct KzcHashMap* kzuMaterialTypeGetLightPropertyTypes(const struct KzuMaterialType* materialType)
{
    kzsAssert(kzuMaterialTypeIsValid(materialType));

    return materialType->data->lightPropertyTypes;
}

struct KzuPropertyType* kzuMaterialTypeGetPropertyTypeByName(const struct KzuMaterialType* materialType, kzString propertyTypeName)
{
    struct KzcDynamicArrayIterator it;
    struct KzuPropertyType* foundPropertyType = KZ_NULL;

    kzsAssert(kzuMaterialTypeIsValid(materialType));

    /* TODO: Use kzuPropertyTypeCollectionGetPropertyTypeByName instead. */
    it = kzuPropertyTypeCollectionGetIterator(materialType->data->propertyTypes);

    while (kzcDynamicArrayIterate(it))
    {
        struct KzuPropertyType* propertyType = (struct KzuPropertyType*)kzcDynamicArrayIteratorGetValue(it);
        if(kzcStringIsEqual(propertyTypeName, kzuPropertyTypeGetName(propertyType)))
        {
            foundPropertyType = propertyType;
            break;
        }
    }
    return foundPropertyType;
}

kzBool kzuMaterialTypeContainsPropertyType(const struct KzuMaterialType* materialType, const struct KzuPropertyType* propertyType)
{
    struct KzcDynamicArrayIterator it;
    kzBool found = KZ_FALSE;

    kzsAssert(kzuMaterialTypeIsValid(materialType));

    it = kzuPropertyTypeCollectionGetIterator(materialType->data->propertyTypes);

    while (kzcDynamicArrayIterate(it))
    {
        struct KzuPropertyType* materialPropertyType = (struct KzuPropertyType*)kzcDynamicArrayIteratorGetValue(it);
        if(propertyType == materialPropertyType)
        {
            found = KZ_TRUE;
            break;
        }
    }
    return found;
}

kzBool kzuMaterialTypeIsValidShader(const struct KzuMaterialType* materialType)
{
    kzsAssert(kzuMaterialTypeIsValid(materialType));

    return materialType->data->validShader;
}

kzBool kzuMaterialTypeIsShaderSetCalled(const struct KzuMaterialType* materialType)
{
    kzsAssert(kzuMaterialTypeIsValid(materialType));
    return materialType->data->shaderSetCalled;
}

void kzuMaterialTypeShaderInfoSetSourceShader(struct KzuMaterialTypeShaderInfo* shaderInfo, kzString vertexShaderSourceCode,
                                              kzString fragmentShaderSourceCode, struct KzcDynamicArray* uniforms,
                                              struct KzcDynamicArray* vertexAttributes)
{
    shaderInfo->type = KZU_SHADER_TYPE_SOURCE;
    shaderInfo->shaderData.code.vertex = vertexShaderSourceCode;
    shaderInfo->shaderData.code.fragment = fragmentShaderSourceCode;
    shaderInfo->shaderData.code.uniforms = uniforms;
    shaderInfo->vertexAttributes = vertexAttributes;
}

void kzuMaterialTypeShaderInfoSetBinaryShaderSeparated(struct KzuMaterialTypeShaderInfo* shaderInfo, void* vertexShaderData, kzUint vertexShaderLength,
                                                       void* fragmentShaderData, kzUint fragmentShaderLength, struct KzcDynamicArray* uniforms,
                                                       struct KzcDynamicArray* vertexAttributes)
{
    shaderInfo->type = KZU_SHADER_TYPE_BINARY_SEPARATED;
    shaderInfo->shaderData.binarySeparated.vertexData = vertexShaderData;
    shaderInfo->shaderData.binarySeparated.vertexDataSize = vertexShaderLength;
    shaderInfo->shaderData.binarySeparated.fragmentData = fragmentShaderData;
    shaderInfo->shaderData.binarySeparated.fragmentDataSize = fragmentShaderLength;
    shaderInfo->shaderData.binarySeparated.uniforms = uniforms;
    shaderInfo->vertexAttributes = vertexAttributes;
}

void kzuMaterialTypeShaderInfoSetBinaryShaderCombined(struct KzuMaterialTypeShaderInfo* shaderInfo, void* shaderData, kzUint shaderLength,
                                                      struct KzcDynamicArray* uniforms, struct KzcDynamicArray* vertexAttributes)
{
    shaderInfo->type = KZU_SHADER_TYPE_BINARY_COMBINED;
    shaderInfo->shaderData.binaryCombined.data = shaderData;
    shaderInfo->shaderData.binaryCombined.size = shaderLength;
    shaderInfo->shaderData.binarySeparated.uniforms = uniforms;
    shaderInfo->vertexAttributes = vertexAttributes;
}

void kzuMaterialTypeShaderInfoSetNoShader(struct KzuMaterialTypeShaderInfo* shaderInfo, struct KzcDynamicArray* vertexAttributes)
{
    shaderInfo->type = KZU_SHADER_TYPE_NONE;
    shaderInfo->vertexAttributes = vertexAttributes;
}
