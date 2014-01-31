/**
 * \file
 * Represents light in scene. Contains constructor for directional, point and spot light. Node inherited from KzuObjectNode.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZU_LIGHT_H
#define KZU_LIGHT_H


#include <user/scene_graph/kzu_object_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzuObjectNode;
struct KzuLightPropertyType;
struct KzuLightProperty;
struct KzcMemoryManager;
struct KzuPropertyManager;
struct KzuMessageDispatcher;

/**
* \struct KzuLightNode
* Structure for the light node, inherited from KzuObjectNode.
*/
struct KzuLightNode;


/* Fixed light property type names. */

extern const kzString KZU_DIRECTIONAL_LIGHT_PROPERTY;               /**< Directional light. */

extern const kzString KZU_POINT_LIGHT_PROPERTY;                     /**< Point light. */

extern const kzString KZU_SPOT_LIGHT_PROPERTY;                      /**< Spot light. */


/** Object type identifier for Light objects. */
extern const KzuObjectType KZU_OBJECT_TYPE_LIGHT;


/** Creates a new light node. */
kzsError kzuLightNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                            kzString name, struct KzuLightNode** out_lightNode);
/** Deletes a light object node. */
kzsError kzuLightNodeDelete(struct KzuLightNode* lightNode);

/** Creates new directional light. */
kzsError kzuLightNodeCreateDirectional(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                       kzString name, struct KzuLightNode** out_lightNode);
/** Creates new point light. */
kzsError kzuLightNodeCreatePoint(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                 kzString name, struct KzuLightNode** out_lightNode);
/** Creates new spot light. */
kzsError kzuLightNodeCreateSpot(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                kzString name, struct KzuLightNode** out_lightNode);

/** Initializes the light node. */
void kzuLightNodeInitialize(const struct KzuLightNode* lightNode, struct KzuLightPropertyType* lightPropertyType);


/** Get light object node from object node. */
struct KzuLightNode* kzuLightNodeFromObjectNode(const struct KzuObjectNode* objectNode);
/** Returns an object node from light node. */
struct KzuObjectNode* kzuLightNodeToObjectNode(const struct KzuLightNode* lightNode);


/** Returns light property type from light. */
struct KzuLightPropertyType* kzuLightNodeGetLightPropertyType(const struct KzuLightNode* lightNode);


#include "kzu_light_properties.h"


#endif
