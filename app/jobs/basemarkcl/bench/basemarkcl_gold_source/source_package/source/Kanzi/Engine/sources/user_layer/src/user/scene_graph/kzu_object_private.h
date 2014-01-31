/**
* \file
* Object node private members.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_OBJECT_PRIVATE_H
#define KZU_OBJECT_PRIVATE_H


#include <user/scene_graph/kzu_object_base.h>

#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/collection/kzc_dynamic_array.h>

#include <system/kzs_types.h>


/* Forward declarations */
struct KzuPropertyManager;
struct KzuBoundingVolume;
struct KzuRenderer;
struct KzuTransformedObjectNode;


/**
 * Base structure for object node. Contains information of node type (mesh, light, camera, ...), transformation,
 * list of child object nodes and set of properties.
 */
struct KzuObjectNode
{
    struct KzuObjectNodeData* data;         /**< Data for object node. */
};

/** Defines functions for different object node types. */
struct KzuObjectNodeClass
{
    /** Deletor function for the object node. */
    kzsError (*kzuObjectNodeDeletor)(struct KzuObjectNode* objectNode);
    
    /** Copier function for the object node. */
    kzsError (*kzuObjectNodeCopier)(const struct KzcMemoryManager* memoryManager, const struct KzuObjectNode* sourceObjectNode,
                                    struct KzuObjectNode** out_objectNode);

    /** Renderer function for the object node. Can be KZ_NULL. */
    kzsError (*kzuObjectNodeRenderer)(struct KzuRenderer* renderer, const struct KzuTransformedObjectNode* transformedObjectNode);

    /** Function for getting bounding volume from object node. Can be KZ_NULL. */
    struct KzuBoundingVolume* (*kzuObjectNodeBoundingVolumeGetter)(const struct KzuObjectNode* objectNode);
};

/** Object node data. */
struct KzuObjectNodeData
{
    kzBool selfOwned; /**< Data owned by object itself or not. */
    const struct KzuObjectNodeClass* objectNodeClass; /**< Class of the object node. */
    struct KzuObjectNode* parent; /**< Parent of the object. */
    struct KzcDynamicArray* children; /**< Direct children of the object. <KzuObjectNode> */
    struct KzcHashMap* childrenFromKzb; /**< Hash map telling child reference is set in Kanzi Tool <KzuObjectNode, kzBool>. */
    kzMutableString name; /**< Name of the object node. */
    struct KzuPropertyManager* propertyManager; /**< Object node property manager. */
    struct KzuMessageDispatcher* messageDispatcher; /**< Object node message dispatcher. */
};


/** Creates a new object node. */
kzsError kzuObjectNodeCreate_private(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                     const struct KzuObjectNodeClass* objectNodeClass, kzString name, struct KzuObjectNodeData* objectNodeData, struct KzuObjectNode** out_objectNode);

/** Deletes the base structures of object node. */
kzsError kzuObjectNodeDeleteBase_private(struct KzuObjectNode* objectNode);

/** Creates a copy of object node. */
kzsError kzuObjectNodeCopy_private(const struct KzcMemoryManager* memoryManager, const struct KzuObjectNode* sourceObjectNode,
                                   struct KzuObjectNodeData* objectNodeData, struct KzuObjectNode** out_objectNode);

void kzuObjectNodeSetParent_private(const struct KzuObjectNode* objectNode, struct KzuObjectNode* parentObjectNode);


#endif
