/**
* \file
* Property private members.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROPERTY_PRIVATE_H
#define KZU_PROPERTY_PRIVATE_H


#include "kzu_property_base.h"

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations. */
struct KzuPropertyType;
struct KzuColorPropertyType;


#define KZU_PROPERTY_FLAG_FROM_KZB                  (1 << 0)   /**< Set if a property was added to object through KZB. */
#define KZU_PROPERTY_FLAG_INHERITED                 (1 << 1)   /**< Set if a property participates in inheritance. */
#define KZU_PROPERTY_FLAG_INHERITANCE_OVERRIDEN     (1 << 2)   /**< Set if a property overrides the inheritance behavior from the property type. */


/** High-level structure for property. Contains priority, type. Data is contained in inherited properties such as color and scalar. */
struct KzuProperty
{
    enum KzuPropertyPriority        priority;     /**< Priority for property. */
    struct KzuPropertyType*         propertyType; /**< Type of this property. */
    kzU32                           flags;        /**< Property flags. */
};

/** Base structure for property type. */
struct KzuPropertyType
{
    struct KzuPropertyTypeData* data;
};

struct KzuPropertyTypeData
{
    kzBool selfOwned;
    kzMutableString name;                /**< Name of the property type. */
    enum KzuPropertyDataType dataType;   /**< Datatype for property type. */
    kzBool inherited;                    /**< Is the property inherited by default. */
};


/** Initializes property. */
void kzuPropertyInitialize_private(struct KzuProperty* property, struct KzuPropertyType* propertyType);

/** Creates a property type. */
kzsError kzuPropertyTypeCreate_private(const struct KzcMemoryManager* memoryManager, enum KzuPropertyDataType dataType,
                                       kzString name, struct KzuPropertyTypeData* propertyTypeData,
                                       struct KzuPropertyType** out_propertyType);

/** Deletes the base structure of property type. */
kzsError kzuPropertyTypeDeleteBase_private(struct KzuPropertyType* propertyType);


#endif
