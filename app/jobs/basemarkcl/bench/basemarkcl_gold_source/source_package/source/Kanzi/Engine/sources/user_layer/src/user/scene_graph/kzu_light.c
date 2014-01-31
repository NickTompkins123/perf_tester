/**
 * \file
 * Light node and data.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzu_light.h"

#include "kzu_object.h"
#include "kzu_object_private.h"

#include <user/properties/kzu_light_property.h>
#include <user/properties/kzu_color_property.h>
#include <user/properties/kzu_vector3_property.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_matrix4x4_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/color/kzc_color.h>

/* System */
#include <system/debug/kzs_log.h>


/** Object node class for light. */
extern const struct KzuObjectNodeClass KZU_LIGHT_NODE_CLASS;

/* Unique object type for Light is the memory address of the type. */
const KzuObjectType KZU_OBJECT_TYPE_LIGHT = (void*)&KZU_LIGHT_NODE_CLASS;


/* Fixed-function pipeline light property types. */
const kzString KZU_DIRECTIONAL_LIGHT_PROPERTY               = "DirectionalLight";

const kzString KZU_POINT_LIGHT_PROPERTY                  = "PointLight";

const kzString KZU_SPOT_LIGHT_PROPERTY                   = "SpotLight";


struct KzuLightNode
{
    struct KzuObjectNode objectNode;
};

struct KzuLightNodeData
{
    struct KzuObjectNodeData objectNodeData;        /**< Inherited from object node, that contains transformation and child data. */ /*lint -esym(754, KzuLightNodeData::objectNodeData) Suppressed as this member is required but not referenced. */ 
    struct KzuLightPropertyType* lightPropertyType; /**< Properties that light contains. */
};


static struct KzuLightNodeData* kzuLightNodeGetData_internal(const struct KzuLightNode* lightNode)
{
    kzsAssert(kzuObjectNodeIsValid(&lightNode->objectNode));

    return (struct KzuLightNodeData*)lightNode->objectNode.data;
}

static kzsError kzuLightCreateObjectNodeData_internal(const struct KzcMemoryManager* memoryManager, struct KzuObjectNodeData** out_lightNodeData)
{
    kzsError result;
    struct KzuLightNodeData* lightNodeData;

    result = kzcMemoryAllocVariable(memoryManager, lightNodeData, "Light node data");
    kzsErrorForward(result);

    lightNodeData->lightPropertyType = KZ_NULL;

    *out_lightNodeData = &lightNodeData->objectNodeData;
    kzsSuccess();
}

kzsError kzuLightNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                            kzString name, struct KzuLightNode** out_lightNode)
{
    kzsError result;
    struct KzuObjectNodeData* objectNodeData;
    struct KzuObjectNode* objectNode;
    struct KzuLightNode* lightNode;

    result = kzuLightCreateObjectNodeData_internal(memoryManager, &objectNodeData);
    kzsErrorForward(result);

    result = kzuObjectNodeCreate_private(memoryManager, propertyManager, messageDispatcher, &KZU_LIGHT_NODE_CLASS, name, objectNodeData, &objectNode);
    kzsErrorForward(result);

    lightNode = (struct KzuLightNode*)objectNode;

    *out_lightNode = lightNode;
    kzsSuccess();
}

kzsError kzuLightNodeDelete(struct KzuLightNode* lightNode)
{
    kzsError result;

    result = kzuObjectNodeDeleteBase_private(&lightNode->objectNode);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuLightNodeCreateDirectional(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                       kzString name, struct KzuLightNode** out_lightNode)
{
    kzsError result;
    struct KzuLightNode* lightNode;

    result = kzuLightNodeCreate(memoryManager, propertyManager, messageDispatcher, name, &lightNode);
    kzsErrorForward(result);

    kzuLightNodeInitialize(lightNode, kzuLightPropertyTypeFromPropertyType(KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT));

    *out_lightNode = lightNode;
    kzsSuccess();
}

kzsError kzuLightNodeCreatePoint(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                 kzString name, struct KzuLightNode** out_lightNode)
{
    kzsError result;
    struct KzuLightNode* lightNode;

    result = kzuLightNodeCreate(memoryManager, propertyManager, messageDispatcher, name, &lightNode);
    kzsErrorForward(result);

    kzuLightNodeInitialize(lightNode, kzuLightPropertyTypeFromPropertyType(KZU_PROPERTY_TYPE_POINT_LIGHT));

    *out_lightNode = lightNode;
    kzsSuccess();
}

kzsError kzuLightNodeCreateSpot(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                kzString name, struct KzuLightNode** out_lightNode)
{
    kzsError result;
    struct KzuLightNode* lightNode;

    result = kzuLightNodeCreate(memoryManager, propertyManager, messageDispatcher, name, &lightNode);
    kzsErrorForward(result);

    kzuLightNodeInitialize(lightNode, kzuLightPropertyTypeFromPropertyType(KZU_PROPERTY_TYPE_SPOT_LIGHT));

    *out_lightNode = lightNode;
    kzsSuccess();
}

void kzuLightNodeInitialize(const struct KzuLightNode* lightNode, struct KzuLightPropertyType* lightPropertyType)
{
    struct KzuLightNodeData* lightNodeData = kzuLightNodeGetData_internal(lightNode);

    kzsAssert(kzcIsValidPointer(lightNodeData));

    lightNodeData->lightPropertyType = lightPropertyType;
}

struct KzuLightNode* kzuLightNodeFromObjectNode(const struct KzuObjectNode* objectNode)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));
    kzsAssert(objectNode->data->objectNodeClass == &KZU_LIGHT_NODE_CLASS);

    return (struct KzuLightNode*)objectNode;
}

struct KzuObjectNode* kzuLightNodeToObjectNode(const struct KzuLightNode* lightNode)
{
    return (struct KzuObjectNode*)lightNode;
}

struct KzuLightPropertyType* kzuLightNodeGetLightPropertyType(const struct KzuLightNode* lightNode)
{
    struct KzuLightNodeData* lightNodeData = kzuLightNodeGetData_internal(lightNode);

    kzsAssert(kzcIsValidPointer(lightNodeData));

    return lightNodeData->lightPropertyType;
}

KZ_CALLBACK static kzsError kzuLightNodeDeleteImplementation_internal(struct KzuObjectNode* objectNode)
{
    kzsError result = kzuLightNodeDelete(kzuLightNodeFromObjectNode(objectNode));
    kzsErrorForward(result);
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuLightNodeCopyImplementation_internal(const struct KzcMemoryManager* memoryManager,
                                                                    const struct KzuObjectNode* sourceObjectNode,
                                                                    struct KzuObjectNode** out_objectNode)
{
    kzsError result;
    struct KzuObjectNodeData* objectNodeData;
    struct KzuObjectNode* objectNode;

    result = kzuLightCreateObjectNodeData_internal(memoryManager, &objectNodeData);
    kzsErrorForward(result);

    result = kzuObjectNodeCopy_private(memoryManager, sourceObjectNode, objectNodeData, &objectNode);
    kzsErrorForward(result);

    /* TODO: Initialize sourceLightNodeData->lightProperty */

    *out_objectNode = objectNode;
    kzsSuccess();
}

const struct KzuObjectNodeClass KZU_LIGHT_NODE_CLASS =
{
    kzuLightNodeDeleteImplementation_internal,
    kzuLightNodeCopyImplementation_internal,
    KZ_NULL,
    KZ_NULL
};
