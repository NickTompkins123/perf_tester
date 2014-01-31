/**
* \file
* Specifies material structure and its properties.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_MATERIAL_H
#define KZU_MATERIAL_H


#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/color/kzc_color.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzuMaterialType;
struct KzuProperty;
struct KzuPropertyType;
struct KzcMemoryManager;
struct KzuPropertyManager;


/**
* \struct KzuMaterial
* Structure for material. Contains specific visual properties and material type (shader).
*/
struct KzuMaterial;


/** Allocates memory and returns a new material. */
kzsError kzuMaterialCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMaterialType* type, kzString name, struct KzuMaterial** out_material);
/** Frees the memory allocated for the material. */
kzsError kzuMaterialDelete(struct KzuMaterial* material);

/**
 * Takes ownership of the material container deletion. After this, when a material is deleted,
 * only the data is actually deleted and the container remains alive.
 */
void kzuMaterialTakeOwnership(const struct KzuMaterial* material);
/** Checks if the material is valid (pointer and the data are non-null). */
kzBool kzuMaterialIsValid(const struct KzuMaterial* material);
/** Moves the data from source material to the target material. */
void kzuMaterialTransferData(struct KzuMaterial* targetMaterial, struct KzuMaterial* sourceMaterial);

/** Creates a copy of given material. */
kzsError kzuMaterialCopy(const struct KzcMemoryManager* memoryManager, const struct KzuMaterial* materialToCopy, struct KzuMaterial** out_material);

/** Returns the material property manager. */
struct KzuPropertyManager* kzuMaterialGetPropertyManager(const struct KzuMaterial* material);

/** Returns material name from material. */
kzString kzuMaterialGetName(const struct KzuMaterial* material);
/** Returns material type from material. */
struct KzuMaterialType* kzuMaterialGetMaterialType(const struct KzuMaterial* material);


#endif
