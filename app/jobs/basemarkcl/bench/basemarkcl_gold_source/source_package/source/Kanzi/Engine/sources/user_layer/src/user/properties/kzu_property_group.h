/**
* \file
* Property group. Group object that contains set of objects, materials, render passes and scenes, and properties for them.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROPERTY_GROUP_H
#define KZU_PROPERTY_GROUP_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations. */
struct KzuPropertyManager;
struct KzcMemoryManager;


/** 
* \struct KzuPropertyGroup
* Structure for property group, that specifies set of objects and properties.
*/
struct KzuPropertyGroup;


/** Creates new property group. */
kzsError kzuPropertyGroupCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager,
                                struct KzuPropertyGroup** out_propertyGroup);
/** Deletes property group. */
kzsError kzuPropertyGroupDelete(struct KzuPropertyGroup* propertyGroup);


/**
 * Takes ownership of the property group data deletion. After this, when a group is deleted,
 * only the data is actually deleted and the container remains alive.
 */
void kzuPropertyGroupTakeOwnership(const struct KzuPropertyGroup* group);
/** Is this property group valid. */
kzBool kzuPropertyGroupIsValid(const struct KzuPropertyGroup* group);
/** Moves the data from source group to the target group. */
void kzuPropertyGroupTransferData(struct KzuPropertyGroup* targetGroup, struct KzuPropertyGroup* sourceGroup);


/** Sets property override enabled or disabled. */
void kzuPropertyGroupSetOverrideProperties(const struct KzuPropertyGroup* group, kzBool overrideProperties);
/** Returns is property override on or off. */
kzBool kzuPropertyGroupIsOverrideProperties(const struct KzuPropertyGroup* group);


#endif
