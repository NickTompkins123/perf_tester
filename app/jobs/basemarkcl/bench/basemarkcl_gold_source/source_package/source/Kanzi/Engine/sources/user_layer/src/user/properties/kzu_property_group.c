/**
* \file
* Property group. Group object that contains set of objects, materials, render passes and scenes, and properties for them.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_property_group.h"

#include <user/properties/kzu_property_collection.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/scene_graph/kzu_object.h>
#include <user/material/kzu_material.h>
#include <user/renderer/kzu_render_pass.h>
#include <user/scene_graph/kzu_scene.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_hash_set.h>
#include <core/util/collection/kzc_hash_map.h>


struct KzuPropertyGroupData
{
    kzBool selfOwned; /**< Data owned by object itself or not. */

    struct KzuPropertyManager* propertyManager; /**< Property manager used to create this material. */
    kzBool overrideProperties; /**< Override properties in the property group or not. */
};

struct KzuPropertyGroup
{
    struct KzuPropertyGroupData* data; /**< Reference to group data. */
};


kzsError kzuPropertyGroupCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager,
                                struct KzuPropertyGroup** out_propertyGroup)
{
    kzsError result;
    struct KzuPropertyGroup* group;
    struct KzuPropertyGroupData* groupData;

    result = kzcMemoryAllocVariable(memoryManager, group, "PropertyGroup");
    kzsErrorForward(result);
    result = kzcMemoryAllocVariable(memoryManager, groupData, "PropertyGroupData");
    kzsErrorForward(result);

    group->data = groupData;
    groupData->selfOwned = KZ_TRUE;

    groupData->propertyManager = propertyManager;
    groupData->overrideProperties = KZ_FALSE;

    *out_propertyGroup = group;
    kzsSuccess();
}

kzsError kzuPropertyGroupDelete(struct KzuPropertyGroup* propertyGroup)
{
    kzsError result;
    struct KzuPropertyGroupData* data;
    kzBool selfOwned;

    kzsAssert(kzcIsValidPointer(propertyGroup));

    data = propertyGroup->data;
    selfOwned = data->selfOwned;

    result = kzuPropertyManagerRemoveProperties(data->propertyManager, propertyGroup);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(data);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(propertyGroup);
        kzsErrorForward(result);
    }
    else
    {
        propertyGroup->data = KZ_NULL;
    }

    kzsSuccess();
}

void kzuPropertyGroupTakeOwnership(const struct KzuPropertyGroup* group)
{
    kzsAssert(kzuPropertyGroupIsValid(group));

    group->data->selfOwned = KZ_FALSE;
}

kzBool kzuPropertyGroupIsValid(const struct KzuPropertyGroup* group)
{
    return group != KZ_NULL && group->data != KZ_NULL;
}

void kzuPropertyGroupTransferData(struct KzuPropertyGroup* targetGroup, struct KzuPropertyGroup* sourceGroup)
{
    kzsAssert(kzcIsValidPointer(targetGroup));
    kzsAssert(targetGroup->data == KZ_NULL);
    kzsAssert(kzuPropertyGroupIsValid(sourceGroup));

    targetGroup->data = sourceGroup->data;
    sourceGroup->data = KZ_NULL;
}

void kzuPropertyGroupSetOverrideProperties(const struct KzuPropertyGroup* group, kzBool overrideProperties)
{
    kzsAssert(kzuPropertyGroupIsValid(group));
    group->data->overrideProperties = overrideProperties;
}

kzBool kzuPropertyGroupIsOverrideProperties(const struct KzuPropertyGroup* group)
{
    kzsAssert(kzuPropertyGroupIsValid(group));
    return group->data->overrideProperties;
}
