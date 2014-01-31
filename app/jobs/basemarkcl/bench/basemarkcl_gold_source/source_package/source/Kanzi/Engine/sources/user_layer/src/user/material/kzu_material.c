/**
* \file
* Specifies material structure and its properties.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_material.h"

#include "kzu_material_type.h"

#include <user/properties/kzu_property_manager.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/string/kzc_string.h>

#include <system/debug/kzs_log.h>


struct KzuMaterial
{
    struct KzuMaterialData* data;
};

struct KzuMaterialData
{
    kzBool selfOwned;

    struct KzuPropertyManager* propertyManager; /**< Property manager used to create this material. */
    struct KzuMaterialType* type; /**< Material type, specifies set of properties that define the visual output of this material. */
    kzMutableString name; /**< Name for this material. */
};


kzsError kzuMaterialCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMaterialType* type, kzString name, struct KzuMaterial** out_material)
{
    kzsError result;
    struct KzuMaterial* material;
    struct KzuMaterialData* materialData;

    kzsAssert(kzcIsValidPointer(type));

    kzsAssert(propertyManager != KZ_NULL);
    
    /* This error is valid for ES1 also at the time of writing. ES1 material type contains a shader NONE. */
    kzsErrorTest(kzuMaterialTypeIsShaderSetCalled(type), KZS_ERROR_ILLEGAL_ARGUMENT, "Before material type can be used to construct material kzuMaterialTypeSetShader must be called.");

    /* Allocate memory for material */
    result = kzcMemoryAllocVariable(memoryManager, material, "Material");
    kzsErrorForward(result);

    /* Allocate memory for material data */
    result = kzcMemoryAllocVariable(memoryManager, materialData, "Material data");
    kzsErrorForward(result);

    material->data = materialData;
    materialData->selfOwned = KZ_TRUE;

    materialData->propertyManager = propertyManager;
    materialData->type = type;
    
    if(name != KZ_NULL)
    {
        result = kzcStringCopy(memoryManager, name, &materialData->name);
        kzsErrorForward(result);
    }
    else
    {
        materialData->name = KZ_NULL;
    }

    *out_material = material;
    kzsSuccess();
}

kzsError kzuMaterialDelete(struct KzuMaterial* material)
{
    kzsError result;
    struct KzuMaterialData* materialData;
    kzBool selfOwned;

    kzsAssert(kzuMaterialIsValid(material));

    materialData = material->data;
    selfOwned = materialData->selfOwned;

    result = kzuPropertyManagerRemoveProperties(materialData->propertyManager, material);
    kzsErrorForward(result);

    if (materialData->name != KZ_NULL)
    {
        result = kzcStringDelete(materialData->name);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(materialData);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(material);
        kzsErrorForward(result);
    }
    else
    {
        material->data = KZ_NULL;
    }

    kzsSuccess();
}

void kzuMaterialTakeOwnership(const struct KzuMaterial* material)
{
    kzsAssert(kzuMaterialIsValid(material));

    material->data->selfOwned = KZ_FALSE;
}

kzBool kzuMaterialIsValid(const struct KzuMaterial* material)
{
    return material != KZ_NULL && material->data != KZ_NULL;
}

void kzuMaterialTransferData(struct KzuMaterial* targetMaterial, struct KzuMaterial* sourceMaterial)
{
    kzsAssert(kzcIsValidPointer(targetMaterial));
    kzsAssert(targetMaterial->data == KZ_NULL);
    kzsAssert(kzuMaterialIsValid(sourceMaterial));

    targetMaterial->data = sourceMaterial->data;
    sourceMaterial->data = KZ_NULL;
}

kzsError kzuMaterialCopy(const struct KzcMemoryManager* memoryManager, const struct KzuMaterial* sourceMaterial, struct KzuMaterial** out_material)
{
    kzsError result;
    struct KzuMaterial* material;

    kzsAssert(kzuMaterialIsValid(sourceMaterial));

    result = kzuMaterialCreate(memoryManager, sourceMaterial->data->propertyManager, sourceMaterial->data->type, sourceMaterial->data->name, &material);
    kzsErrorForward(result);

    result = kzuPropertyManagerCopyProperties(sourceMaterial->data->propertyManager, sourceMaterial, material);
    kzsErrorForward(result);

    /* Copy material name if it is valid. */
    if(sourceMaterial->data->name != KZ_NULL)
    {
        result = kzcStringCopy(memoryManager, sourceMaterial->data->name, &material->data->name);
        kzsErrorForward(result);
    }

    *out_material = material;
    kzsSuccess();
}

kzString kzuMaterialGetName(const struct KzuMaterial* material)
{
    kzsAssert(kzuMaterialIsValid(material));

    return material->data->name;
}

struct KzuMaterialType* kzuMaterialGetMaterialType(const struct KzuMaterial* material)
{
    kzsAssert(kzuMaterialIsValid(material));

    return material->data->type;
}

struct KzuPropertyManager* kzuMaterialGetPropertyManager(const struct KzuMaterial* material)
{
    kzsAssert(kzuMaterialIsValid(material));

    return material->data->propertyManager;
}
