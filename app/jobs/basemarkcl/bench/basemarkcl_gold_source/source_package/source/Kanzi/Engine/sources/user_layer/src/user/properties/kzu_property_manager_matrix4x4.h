/**
* \file
* Property manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROPERTY_MANAGER_MATRIX4X4_H
#define KZU_PROPERTY_MANAGER_MATRIX4X4_H


#include <core/util/math/kzc_matrix4x4.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuPropertyType;
struct KzuObjectNode;


/** Set a property of propertyType associated with an object. */
kzsError kzuPropertyManagerSetMatrix4x4(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, const struct KzcMatrix4x4* value);
/** Set the relative value of a property of propertyType associated with an object. */
kzsError kzuPropertyManagerSetRelativeMatrix4x4(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, const struct KzcMatrix4x4* value);

/**
 * Get the value of a property of propertyType associated with an object. Returns KZ_TRUE if the property is found and KZ_FALSE otherwise.
 * The return value is calculated based on the following steps:
 * 1. If an animation is applied to the property, the animated value is returned. It is calculated from the base and relative values of the property.
 * 2. The value associated with the object is returned.
 * 3. If the property is not associated with the object, the property groups associated with the object are queried:
 *  3.1 Property groups associated with the object are queried in the reverse order than they were added.
 * 4. If the property can not be deduced, the output value is not modified and KZ_FALSE is returned.
 */
kzBool kzuPropertyManagerGetMatrix4x4(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcMatrix4x4* out_value);
/**
 * Get the base value of a property of propertyType associated with an object. Returns KZ_TRUE if the property is found and KZ_FALSE otherwise.
 */
kzBool kzuPropertyManagerGetBaseMatrix4x4(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcMatrix4x4* out_value);
/**
 * Get the relative value of a property of propertyType associated with an object. Returns KZ_TRUE if the property is found and KZ_FALSE otherwise.
 */
kzBool kzuPropertyManagerGetRelativeMatrix4x4(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcMatrix4x4* out_value);
/**
  * Get a property of propertyType associated with an object and return the default value from the property type if not found.
  * If the property value can be deduced with kzuPropertyManagerGetMatrix4x4 then that value is returned.
  * If the property can not be deduced, the default value from the property type is returned.
  */
struct KzcMatrix4x4 kzuPropertyManagerGetMatrix4x4Default(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType);


/** Set a property on an object node.  */
kzsError kzuObjectNodeSetMatrix4x4Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, const struct KzcMatrix4x4* value);

/**
 * Get the value of a property of propertyType associated with an object node. Returns KZ_TRUE if the property is found and KZ_FALSE otherwise.
 * The return value is calculated based on the following steps:
 * 1. kzuPropertyManagerGetMatrix4x4 is used for getting the property value.
 * 2. If the property value can not be deduced from the object node, the same query is performed for the parent of the current object node.
 * 3. If the property is not found until the root is reached, the output value is not modified and KZ_FALSE is returned.
 */
kzBool kzuObjectNodeGetMatrix4x4Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcMatrix4x4* out_value);

/**
 * Get the base value of a property of propertyType associated with an object node.
 * Behaves like kzuObjectNodeGetMatrix4x4Property but does not take animation into account.
 */
kzBool kzuObjectNodeGetBaseMatrix4x4Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcMatrix4x4* out_value);

/**
 * Get the value of a property associated with an object node.
 * Behaves like kzuObjectNodeGetMatrix4x4Property, but if the property value can not be deduced, the default value from the property type is returned.
 */
struct KzcMatrix4x4 kzuObjectNodeGetMatrix4x4PropertyDefault(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType);


#endif
