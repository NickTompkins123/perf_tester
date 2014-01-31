/**
* \file
* Level of detail selector node and data. Node inherited from KzuObjectNode.
* LOD selector node is pipeline element for selecting level of detail depending on camera distance / perspective size.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_LOD_SELECTOR_H
#define KZU_LOD_SELECTOR_H


#include <user/scene_graph/kzu_object_base.h>

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations. */
struct KzuObjectNode;
struct KzcMemoryManager;
struct KzuPropertyManager;
struct KzuMessageDispatcher;

/**
* \struct KzuLODSelectorNode
* Structure for level of detail selector node, inherited from KzuObjectNode.
*/
struct KzuLODSelectorNode;


/** List of available LOD types. */
enum KzuLODType
{
    KZU_LOD_TYPE_INVALID, /**< Invalid LOD type. */
    KZU_LOD_TYPE_DISTANCE, /**< Distance LOD, z distance from camera. */
    KZU_LOD_TYPE_SCREEN_AREA /**< Screen area LOD, size in screen space in percentages. */
};


/** Object type identifier for LOD selector objects. */
extern const KzuObjectType KZU_OBJECT_TYPE_LOD_SELECTOR;


/** Creates a new level of detail selector node. */
kzsError kzuLODSelectorNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                  kzString name, struct KzuLODSelectorNode** out_lodNode);

/** Deletes a level of detail selector node. */
kzsError kzuLODSelectorNodeDelete(struct KzuLODSelectorNode* lodSelectorNode);

/** Assigns distance LOD data for LOD selector levels. */
kzsError kzuLODSelectorNodeInitializeDistance(const struct KzuLODSelectorNode* lodSelectorNode, kzUint lodLevelCount,
                                              const kzFloat* constraints, struct KzuObjectNode* const* objectNodes);
/** Assigns screen area LOD data for LOD selector levels. */
kzsError kzuLODSelectorNodeInitializeScreenArea(const struct KzuLODSelectorNode* lodSelectorNode, kzUint lodLevelCount,
                                                const kzFloat* constraints, struct KzuObjectNode* const* objectNodes);


/** Returns LOD selector node from object node. */
struct KzuLODSelectorNode* kzuLODSelectorNodeFromObjectNode(const struct KzuObjectNode* objectNode);
/** Returns an object node from LOD selector node. */
struct KzuObjectNode* kzuLODSelectorNodeToObjectNode(struct KzuLODSelectorNode* lodNode);


/** Gets LOD type from LOD selector object. */
enum KzuLODType kzuLODSelectorGetLODType(const struct KzuLODSelectorNode* lodSelectorNode);


/** Gets number of LOD levels in this object. */
kzUint kzuLODSelectorNodeGetLevelCount(const struct KzuLODSelectorNode* lodSelectorNode);

/** Gets LOD level object with given constraint (distance, perspective size). */
struct KzuObjectNode* kzuLODSelectorNodeGetObjectAtConstraint(const struct KzuLODSelectorNode* lodSelectorNode, kzFloat constraint);

/** Gets object node at given index from LOD selector node. */
struct KzuObjectNode* kzuLODSelectorGetObjectNodeAtLevel(const struct KzuLODSelectorNode* lodSelectorNode, kzUint index);


#endif
