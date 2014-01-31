/**
* \file
* Property manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROPERTY_MANAGER_H
#define KZU_PROPERTY_MANAGER_H


#include <user/properties/kzu_property_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuPropertyType;
struct KzuObjectNode;
struct KzcDynamicArray;
struct KzcMemoryManager;
struct KzuProperty;

/** Property manager object. */
struct KzuPropertyManager;


/**
 * \struct KzuPropertyGroup
 * Property groups can be associated to any object. They contain properties just like any other objects and when a property
 * value is queried for an object, all the associated property groups are considered as well.
 */
struct KzuPropertyGroup;


#define KZU_PROPERTY_MANAGER_FLAG_FROM_KZB 1


/** Create a property manager. */
kzsError kzuPropertyManagerCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager** out_propertyManager);
/** Delete a property manager. */
kzsError kzuPropertyManagerDelete(struct KzuPropertyManager* propertyManager);


/** Remove a property of propertyType associated with an object. */
kzsError kzuPropertyManagerRemoveProperty(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType);

/** Remove all properties associated with an object. */
kzsError kzuPropertyManagerRemoveProperties(const struct KzuPropertyManager* propertyManager, const void* object);

/** Get all property types associated with an object. */
kzsError kzuPropertyManagerGetPropertyTypes(const struct KzuPropertyManager* propertyManager, const struct KzcMemoryManager* memoryManager, const void* object, struct KzcDynamicArray** out_propertyTypes);

/** Check if a property of propertyType is associated with an object. */
kzBool kzuPropertyManagerHasProperty(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType);


/**
 * Compare the property value of two objects.
 * If properties are equal or not associated with both of the object, KZ_TRUE is returned.
 * Otherwise KZ_FALSE is returned.
 */
kzBool kzuPropertyManagerIsPropertyEqual(const struct KzuPropertyManager* propertyManager, const void* object1, const void* object2, const struct KzuPropertyType* propertyType);

/** Copy a property of given type from source object to target object. */
kzsError kzuPropertyManagerCopyProperty(const struct KzuPropertyManager* propertyManager, const void* sourceObject, const void* targetOject, const struct KzuPropertyType* propertyType);
/** Copy all properties from source object to target object. */
kzsError kzuPropertyManagerCopyProperties(const struct KzuPropertyManager* propertyManager, const void* sourceObject, const void* targetObject);


/** Set the priority of an object property. Returns KZ_TRUE if the property is found and KZ_FALSE otherwise. */
kzBool kzuPropertyManagerSetPropertyPriority(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, enum KzuPropertyPriority priority);
/** Get the priority of an object property. Returns KZ_TRUE if the property is found and KZ_FALSE otherwise. */
kzBool kzuPropertyManagerGetPropertyPriority(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, enum KzuPropertyPriority* out_priority);

/** Set a flag for a property. Returns KZ_TRUE if the property is found and KZ_FALSE otherwise. */
kzBool kzuPropertyManagerSetPropertyFlag(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzU32 flag);
/** Clears a flag for a property. Returns KZ_TRUE if the property is found and KZ_FALSE otherwise. */
kzBool kzuPropertyManagerClearPropertyFlag(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzU32 flag);
/** Checks if a flag is set for a property. Returns KZ_TRUE if the property is found and KZ_FALSE otherwise. */
kzBool kzuPropertyManagerIsPropertyFlagSet(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzU32 flag, kzBool* out_isSet);


/**
 * Register a property type in the property manager. After a property type is registered to the manager,
 * it is possible to find it by its name.
 */
kzsError kzuPropertyManagerRegisterPropertyType(const struct KzuPropertyManager* propertyManager, const struct KzuPropertyType* propertyType);
/**
 * Unregister a property type from the property manager. It is the responsibility of the caller to ensure,
 * that all properties with specified type have been removed prior to the call.
 */
kzsError kzuPropertyManagerUnregisterPropertyType(const struct KzuPropertyManager* propertyManager, const struct KzuPropertyType* propertyType);
/** Finds a property type by name from the property manager. Returns KZ_NULL if the type is not found. */
const struct KzuPropertyType* kzuPropertyManagerFindPropertyType(const struct KzuPropertyManager* propertyManager, kzString propertyTypeName);


/** Associate a property group with a given object. */
kzsError kzuPropertyManagerAddPropertyGroup(const struct KzuPropertyManager* propertyManager, const void* object, struct KzuPropertyGroup* propertyGroup);

/** Unassociate a property group from a given object. */
kzsError kzuPropertyManagerRemovePropertyGroup(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyGroup* propertyGroup);

/** Unassociate all property groups from a given object. */
kzsError kzuPropertyManagerRemovePropertyGroups(const struct KzuPropertyManager* propertyManager, const void* object);

/** Get all property groups associated with an object. */
kzsError kzuPropertyManagerGetPropertyGroups(const struct KzuPropertyManager* propertyManager, const struct KzcMemoryManager* memoryManager, const void* object, struct KzcDynamicArray** out_propertyGroups);

/** Copy all property group associations from source object to target object. */
kzsError kzuPropertyManagerCopyProperyGroups(const struct KzuPropertyManager* propertyManager, const void* sourceObject, const void* targetOject);


/** Check if an object node has or inherits a property. */
kzBool kzuObjectNodeHasProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType);
/** Find an object node with the highest priority. */
struct KzuObjectNode* kzuObjectNodeFindHighestPriorityPropertySource(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType);


/* TODO: remove when message system has been refactored. */
/** Get a property as a property object. This function is for compatibility with old code and should not be used. */
kzsError kzuPropertyManagerGetPropertyByName_old(const struct KzuPropertyManager* propertyManager, const void* object, kzString propertyName, struct KzuProperty** out_property);


#if 0
/** Add a property change notification handler */
kzsError kzuPropertyManagerAddNotificationHandler(const void* object, const struct KzuPropertyType* propertyType, Handler, void* userData);
/** Remove a property change notification handler. Searches for the handler by both Handler and UserData */
kzsError kzuPropertyManagerRemoveNotificationHandler(const void* object, const struct KzuPropertyType* propertyType, Handler, void* userData);
#endif


#if 0
/** Add a property change notification handler */
kzsError kzuObjectNodeAddPropertyNotification(const void* object, const struct KzuPropertyType* propertyType, Handler, void* UserData);
/** Remove a property change notification handler. Searches for the handler by both Handler and UserData */
kzsError kzuObjectNodeRemovePropertyNotification(const void* object, const struct KzuPropertyType* propertyType, Handler, void* UserData);
#endif


#include "kzu_property_manager_float.h"
#include "kzu_property_manager_int.h"
#include "kzu_property_manager_bool.h"
#include "kzu_property_manager_color.h"
#include "kzu_property_manager_vector2.h"
#include "kzu_property_manager_vector3.h"
#include "kzu_property_manager_vector4.h"
#include "kzu_property_manager_matrix2x2.h"
#include "kzu_property_manager_matrix3x3.h"
#include "kzu_property_manager_matrix4x4.h"
#include "kzu_property_manager_void.h"
#include "kzu_property_manager_string.h"


#endif
