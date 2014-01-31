/**
* \file
* Transformed object node. Created from original scene graph object node (KzuObjectNode).
* Contains all characteristics as object node, as transformed object node owns instance to one.
* Matrix in calculated world coordinates after extracted in pipeline (kzuSceneExtract / kzuEngineExtractScene called).
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_TRANSFORMED_OBJECT_H
#define KZU_TRANSFORMED_OBJECT_H


#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector2.h>

#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzuObjectNode;
struct KzuPropertyType;
struct KzuProperty;
struct KzcMemoryManager;
struct KzcDynamicArray;
struct KzcVector2;
struct KzcVector3;


/** 
* \struct KzuTransformedObjectNode
* Structure for transformed object node that contains reference to original object in scene graph and transformation
* in calculated world coordinates (affected by hierarchy).
*/
struct KzuTransformedObjectNode;


/** Creates new Transformed object node from regular scene graph object. Saves the reference to original scene graph object
    and copies the matrix data. */
kzsError kzuTransformedObjectNodeCreateFromObjectNode(const struct KzcMemoryManager* memoryManager, struct KzuObjectNode* object, 
                                                      struct KzuTransformedObjectNode** out_object);
/** Adds a child transformed object for parent. */
struct KzuTransformedObjectNode* kzuTransformedObjectNodeGetParent(const struct KzuTransformedObjectNode* transformedObjectNode);
/** Adds a child transformed object for parent. */
kzsError kzuTransformedObjectNodeAddChildNode(struct KzuTransformedObjectNode* parentObject, 
                                              struct KzuTransformedObjectNode* childNode);
/** Removes transformed object node from parent and moves its children upper in hierarchy. */
kzsError kzuTransformedObjectNodeRemove(struct KzuTransformedObjectNode* parentObject, 
                                        const struct KzuTransformedObjectNode* destObject);
/** Copies a object without children to the new node. */
kzsError kzuTransformedObjectNodeCopyWithoutChildren(const struct KzcMemoryManager* memoryManager, 
                                                     const struct KzuTransformedObjectNode* sourceObject, 
                                                     struct KzuTransformedObjectNode** out_object);
/** Creates an array from tree. */
kzsError kzuTransformedObjectTreeToDynamicArray(const struct KzcMemoryManager* memoryManager, struct KzuTransformedObjectNode* rootNode, 
                                                    struct KzcDynamicArray** out_dynamicArray);
/** Returns the child count. */
kzUint kzuTransformedObjectNodeGetChildCount(const struct KzuTransformedObjectNode* transformedObjectNode);
/** Returns a children of transformed object node as iterator (list of KzuTransformedObjectNode:s). */
struct KzcDynamicArrayIterator kzuTransformedObjectNodeGetChildren(const struct KzuTransformedObjectNode* transformedObjectNode);
/** Returns a child from given index from object node. */
struct KzuTransformedObjectNode* kzuTransformedObjectNodeGetChildAtIndex(const struct KzuTransformedObjectNode* transformedObjectNode, kzUint index);
/** Returns a KzuObjectNode owned by transformed object node. */
struct KzuObjectNode* kzuTransformedObjectNodeGetObjectNode(const struct KzuTransformedObjectNode* transformedObjectNode);
/** Finds transformed object node from destination object node's direct children. NULL if nothing was found. */
struct KzuTransformedObjectNode* kzuTransformedObjectNodeFindChildObjectNode(const struct KzuTransformedObjectNode* transformedObjectNode,
                                                                             const struct KzuObjectNode* objectToFind);
/** Finds transformed object node from destination object node's direct descendants. NULL if nothing was found. */
struct KzuTransformedObjectNode* kzuTransformedObjectNodeFindChildObjectNodeFromDescendants(const struct KzuTransformedObjectNode* transformedObjectNode,
                                                                                            const struct KzuObjectNode* objectToFind);
/** Finds given transformed object node from destination transformed object node's descendants. Returns KZ_NULL if nothing was found. */
struct KzuTransformedObjectNode* kzuTransformedObjectNodeFindChildObjectTransformedNodeFromDescendants(const struct KzuTransformedObjectNode* transformedObjectNode,
                                                                                                       const struct KzuTransformedObjectNode* transformedObjectNodeToFind);


/** Sets a matrix for transformed object node. */
void kzuTransformedObjectNodeSetMatrix(struct KzuTransformedObjectNode* transformedObjectNode, const struct KzcMatrix4x4* matrix);
/** Returns a matrix of transformed object node */
struct KzcMatrix4x4 kzuTransformedObjectNodeGetMatrix(const struct KzuTransformedObjectNode* transformedObjectNode);
/** Calculates view transformation for transformed object node. */
void kzuTransformedObjectNodeCalculateViewMatrix(struct KzuTransformedObjectNode* transformedObjectNode, const struct KzcMatrix4x4* viewTransformation);


/** Returns a position of this transformed object node */
void kzuTransformedObjectNodeGetPosition(const struct KzuTransformedObjectNode* transformedObjectNode, struct KzcVector3* out_position);
/** Finds transformed object node from object list. */
struct KzuTransformedObjectNode* kzuTransformedObjectListFindObject(const struct KzcDynamicArray* objectList,
                                                                    const struct KzuObjectNode* objectToFind);


/** Allocates UI data for the transformed object. */
kzsError kzuTransformedObjectNodeAllocateUiData(const struct KzcMemoryManager* memoryManager, struct KzuTransformedObjectNode* node);
/** Returns KZ_TRUE if UI data has been allocated for the object, KZ_FALSE otherwise. */
kzBool kzuTransformedObjectNodeHasUiData(const struct KzuTransformedObjectNode* transformedObjectNode);

/** Set the clipping box for an object. */
void kzuTransformedObjectNodeSetClipBox(const struct KzuTransformedObjectNode* node, const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax);
/** Get the clipping box of an object. */
void kzuTransformedObjectNodeGetClipBox(const struct KzuTransformedObjectNode* node, struct KzcVector3* aabbMin, struct KzcVector3* aabbMax);

/** Check if clipping is enabled for the node. */
kzBool kzuTransformedObjectNodeIsClippingEnabled(const struct KzuTransformedObjectNode* node);
/** Enable or disable clipping for the node. */
void kzuTransformedObjectNodeSetClippingEnabled(const struct KzuTransformedObjectNode* node, kzBool enabled);

/** Check if the node has been clipped. */
kzBool kzuTransformedObjectNodeIsClipped(const struct KzuTransformedObjectNode* node);
/** Set if the node has been clipped. */
void kzuTransformedObjectNodeSetClipped(const struct KzuTransformedObjectNode* node, kzBool enabled);

/** Check if the node is a layout. */
kzBool kzuTransformedObjectNodeIsLayout(const struct KzuTransformedObjectNode* node);

/** Check if clipping is enabled for the children of the node. */
kzBool kzuTransformedObjectNodeIsClipChildrenEnabled(const struct KzuTransformedObjectNode* node);

/** Gets horizontal margin value, if one exists. */
struct KzcVector2 kzuTransformedObjectNodeGetHorizontalMargin(const struct KzuTransformedObjectNode* transformedObjectNode);
/** Gets vertical margin value, if one exists. */
struct KzcVector2 kzuTransformedObjectNodeGetVerticalMargin(const struct KzuTransformedObjectNode* transformedObjectNode);
/** Gets depth margin value, if one exists. */
struct KzcVector2 kzuTransformedObjectNodeGetDepthMargin(const struct KzuTransformedObjectNode* transformedObjectNode);

/** Set content size for an object. Object can store their content size for layout calculations. */
void kzuTransformedObjectNodeSetContentSize(const struct KzuTransformedObjectNode* node, const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax);
/** Get content size of an object. Object can store their content size for layout calculations. */
void kzuTransformedObjectNodeGetContentSize(const struct KzuTransformedObjectNode* node, struct KzcVector3* aabbMin, struct KzcVector3* aabbMax);
/** Set desired size for an object. Object must specify their size for layout calculations. */ 
void kzuTransformedObjectNodeSetDesiredSize(const struct KzuTransformedObjectNode* node, const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax);
/** Get desired size of an object. Object must specify their size for layout calculations. */ 
void kzuTransformedObjectNodeGetDesiredSize(const struct KzuTransformedObjectNode* node, struct KzcVector3* aabbMin, struct KzcVector3* aabbMax);
/** Set layout size for an object. Object receive this size from the layout system. */
void kzuTransformedObjectNodeSetLayoutSize(const struct KzuTransformedObjectNode* node, const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax);
/** Get layout size of an object. Object receive this size from the layout system. */
void kzuTransformedObjectNodeGetLayoutSize(const struct KzuTransformedObjectNode* node, struct KzcVector3* aabbMin, struct KzcVector3* aabbMax);
/** Set actual size for an object. Object can store actual size for layout calculations. */
void kzuTransformedObjectNodeSetActualSize(const struct KzuTransformedObjectNode* node, const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax);
/** Get actual size of an object. Object can store actual size for layout calculations. */
void kzuTransformedObjectNodeGetActualSize(const struct KzuTransformedObjectNode* node, struct KzcVector3* aabbMin, struct KzcVector3* aabbMax);

/** Perform the default arrange step. */
void kzuTransformedObjectNodeDefaultArrange(struct KzuTransformedObjectNode* node);

/** Calculates a new desired bounding box by applying properties like transformation and margins. */
void kzuTransformedObjectNodeCalculateDesiredSize(const struct KzuTransformedObjectNode* node,
                                                  const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax,
                                                  struct KzcVector3* out_aabbMin, struct KzcVector3* out_aabbMax);

/** Perform clipping on a transformed node. */
void kzuTransformedObjectNodeClip(const struct KzuTransformedObjectNode* transformedNode, const struct KzcVector3* offset);


#endif
