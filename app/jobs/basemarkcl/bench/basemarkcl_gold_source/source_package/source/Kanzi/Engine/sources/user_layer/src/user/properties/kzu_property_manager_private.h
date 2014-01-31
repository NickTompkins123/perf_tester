/**
* \file
* Property manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROPERTY_MANAGER_INTERNAL_H
#define KZU_PROPERTY_MANAGER_INTERNAL_H


#include <user/properties/kzu_property_base.h>
#include <user/scene_graph/kzu_object.h>

#include <core/util/math/kzc_vector2.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_vector4.h>
#include <core/util/math/kzc_matrix2x2.h>
#include <core/util/math/kzc_matrix3x3.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/color/kzc_color.h>
#include <core/util/string/kzc_string.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/** Common property data. */
struct KzuPropertyBaseStorage
{
    enum KzuPropertyPriority priority; /**< Property priority. */
    kzU32 flags; /**< Property flags. */
};

/** Float property data. */
struct KzuPropertyFloatStorage
{
    struct KzuPropertyBaseStorage baseStorage;

    kzFloat baseValue;           /**< Property base value. */
    kzFloat relativeValue;       /**< Property relative value. */
};

/** Int property data. */
struct KzuPropertyIntStorage
{
    struct KzuPropertyBaseStorage baseStorage;

    kzInt baseValue;           /**< Property base value. */
    kzInt relativeValue;       /**< Property relative value. */
};

/** Bool property data. */
struct KzuPropertyBoolStorage
{
    struct KzuPropertyBaseStorage baseStorage;

    kzBool baseValue;           /**< Property base value. */
    kzBool relativeValue;       /**< Property relative value. */
};

/** Color property data. */
struct KzuPropertyColorStorage
{
    struct KzuPropertyBaseStorage baseStorage;

    struct KzcColorRGBA baseValue;           /**< Property base value. */
    struct KzcColorRGBA relativeValue;       /**< Property relative value. */
};

/** Vector2 property data. */
struct KzuPropertyVector2Storage
{
    struct KzuPropertyBaseStorage baseStorage;

    struct KzcVector2 baseValue;           /**< Property base value. */
    struct KzcVector2 relativeValue;       /**< Property relative value. */
};

/** Vector3 property data. */
struct KzuPropertyVector3Storage
{
    struct KzuPropertyBaseStorage baseStorage;

    struct KzcVector3 baseValue;           /**< Property base value. */
    struct KzcVector3 relativeValue;       /**< Property relative value. */
};

/** Vector4 property data. */
struct KzuPropertyVector4Storage
{
    struct KzuPropertyBaseStorage baseStorage;

    struct KzcVector4 baseValue;           /**< Property base value. */
    struct KzcVector4 relativeValue;       /**< Property relative value. */
};

/** Matrix2x2 property data. */
struct KzuPropertyMatrix2x2Storage
{
    struct KzuPropertyBaseStorage baseStorage;

    struct KzcMatrix2x2 baseValue;           /**< Property base value. */
    struct KzcMatrix2x2 relativeValue;       /**< Property relative value. */
};

/** Matrix3x3 property data. */
struct KzuPropertyMatrix3x3Storage
{
    struct KzuPropertyBaseStorage baseStorage;

    struct KzcMatrix3x3 baseValue;           /**< Property base value. */
    struct KzcMatrix3x3 relativeValue;       /**< Property relative value. */
};

/** Matrix4x4 property data. */
struct KzuPropertyMatrix4x4Storage
{
    struct KzuPropertyBaseStorage baseStorage;

    struct KzcMatrix4x4 baseValue;           /**< Property base value. */
    struct KzcMatrix4x4 relativeValue;       /**< Property relative value. */
};

/** String property data. */
struct KzuPropertyStringStorage
{
    struct KzuPropertyBaseStorage baseStorage;

    kzMutableString baseValue;           /**< Property base value. */
    kzMutableString relativeValue;       /**< Property relative value. */
};

/** Void property data. */
struct KzuPropertyVoidStorage
{
    struct KzuPropertyBaseStorage baseStorage;

    void* baseValue;           /**< Property base value. */
    void* relativeValue;       /**< Property relative value. */
};

/** Type data. */
struct KzuPropertyTypeStorage
{
    const struct KzuPropertyType* type;
    struct KzcHashMap* propertyContainer;       /**< Property value storage. <void*, KzuPropertyBaseStorage*> */
};

/** Property manager. */
struct KzuPropertyManager
{
    struct KzcHashMap* typeContainer; /**< Map for types. <KzuPropertyType*, KzuPropertyTypeStorage*> */
    struct KzcHashMap* groupContainer; /**< Map for property groups. <void*, KzcDynamicArray*> */
    struct KzcHashMap* typesByNames; /**< Map for property types. <kzString, KzuPropertyType>. */
};


/** Initializes base property storage. */
void kzuPropertyManagerInitializeBaseProperty_private(struct KzuPropertyBaseStorage* property);

/** Finds property type storage. Returns KZ_NULL if the type storage is not found. */
struct KzuPropertyTypeStorage* kzuPropertyManagerFindTypeStorage_private(const struct KzuPropertyManager* propertyManager, const struct KzuPropertyType* propertyType);

/** Finds property storage. Returns KZ_NULL if the storage is not found. */
struct KzuPropertyBaseStorage* kzuPropertyManagerFindPropertyStorage_private(const void* object, const struct KzuPropertyTypeStorage* typeStorage);
/** Finds property storage taking groups into account. Returns KZ_NULL if the storage is not found. */
struct KzuPropertyBaseStorage* kzuPropertyManagerFindPropertyStorageWithGroups_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage);


/** Removes property storage. */
kzsError kzuPropertyManagerRemovePropertyStorage_private(const void* object, const struct KzuPropertyTypeStorage* typeStorage);

/** Gets a property type storage if it exists, or creates new storage if it is not found. */
kzsError kzuPropertyManagerAcquireTypeStorage_private(const struct KzuPropertyManager* propertyManager, const struct KzuPropertyType* propertyType, struct KzuPropertyTypeStorage** out_storage);


/** Gets a float property storage if it exists, or creates new storage if it is not found. */
kzsError kzuPropertyManagerAcquireFloatStorage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage,
                                                       struct KzuPropertyFloatStorage** out_propertyStorage);

/** Gets a int property storage if it exists, or creates new storage if it is not found. */
kzsError kzuPropertyManagerAcquireIntStorage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage,
                                                     struct KzuPropertyIntStorage** out_propertyStorage);

/** Gets a bool property storage if it exists, or creates new storage if it is not found. */
kzsError kzuPropertyManagerAcquireBoolStorage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage,
                                                      struct KzuPropertyBoolStorage** out_propertyStorage);

/** Gets a color property storage if it exists, or creates new storage if it is not found. */
kzsError kzuPropertyManagerAcquireColorStorage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage,
                                                       struct KzuPropertyColorStorage** out_propertyStorage);

/** Gets a vector2 property storage if it exists, or creates new storage if it is not found. */
kzsError kzuPropertyManagerAcquireVector2Storage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage,
                                                         struct KzuPropertyVector2Storage** out_propertyStorage);

/** Gets a vector3 property storage if it exists, or creates new storage if it is not found. */
kzsError kzuPropertyManagerAcquireVector3Storage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage,
                                                         struct KzuPropertyVector3Storage** out_propertyStorage);

/** Gets a vector4 property storage if it exists, or creates new storage if it is not found. */
kzsError kzuPropertyManagerAcquireVector4Storage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage,
                                                         struct KzuPropertyVector4Storage** out_propertyStorage);

/** Gets a matrix2x2 property storage if it exists, or creates new storage if it is not found. */
kzsError kzuPropertyManagerAcquireMatrix2x2Storage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage,
                                                           struct KzuPropertyMatrix2x2Storage** out_propertyStorage);

/** Gets a matrix3x3 property storage if it exists, or creates new storage if it is not found. */
kzsError kzuPropertyManagerAcquireMatrix3x3Storage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage,
                                                           struct KzuPropertyMatrix3x3Storage** out_propertyStorage);

/** Gets a matrix4x4 property storage if it exists, or creates new storage if it is not found. */
kzsError kzuPropertyManagerAcquireMatrix4x4Storage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage,
                                                           struct KzuPropertyMatrix4x4Storage** out_propertyStorage);

/** Gets a void property storage if it exists, or creates new storage if it is not found. */
kzsError kzuPropertyManagerAcquireVoidStorage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage,
                                                      struct KzuPropertyVoidStorage** out_floatStorage);

/** Gets a string property storage if it exists, or creates new storage if it is not found. */
kzsError kzuPropertyManagerAcquireStringStorage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage,
                                                        struct KzuPropertyStringStorage** out_floatStorage);

/* TODO: Move this to kzu_object_node.c */
/** Finds property storage for an object node. Takes groups into account. Returns KZ_FALSE if the storage is not found. */
kzBool kzuObjectNodeFindPropertyStorage_private(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzuPropertyBaseStorage** out_propertyStorage);
struct KzuPropertyBaseStorage* kzuObjectNodeFindPropertyStorageWithGroups_private(const struct KzuObjectNode* objectNode, const struct KzuPropertyTypeStorage* typeStorage,
                                                                                  const struct KzuObjectNode** out_objectNode);

/** Copy a string to base value. */
kzsError kzuPropertyManagerAssignBaseString_internal(struct KzuPropertyStringStorage* stringStorage, kzString string);
/** Copy a string to relative value. */
kzsError kzuPropertyManagerAssignRelativeString_internal(struct KzuPropertyStringStorage* stringStorage, kzString string);


#endif
