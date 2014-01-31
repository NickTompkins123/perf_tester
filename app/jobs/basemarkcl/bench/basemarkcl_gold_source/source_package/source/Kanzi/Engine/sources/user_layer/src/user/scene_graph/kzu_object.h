/**
 * \file
 * Base structure for scene graph nodes. Contains information of node type (mesh, light, camera, ...), transformation,
 * list of child object nodes and set of properties.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZU_OBJECT_H
#define KZU_OBJECT_H


#include <user/scene_graph/kzu_object_base.h>

#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector3.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/** Object type identifier for empty objects. */
extern const KzuObjectType KZU_OBJECT_TYPE_EMPTY;
/** Unique object type ID for arbitrary objects. */
extern const KzuObjectType KZU_OBJECT_TYPE_ARBITRARY;
/** Object type identifier for all renderable object types. */
extern const KzuObjectType KZU_OBJECT_TYPE_RENDERABLE;


/* Forward declaration structures. */
struct KzuTransformedObjectNode;
struct KzuRenderer;
struct KzuProperty;
struct KzuPropertyType;
struct KzuPropertyManager;
struct KzuMessageDispatcher;
struct KzcDynamicArray;
struct KzcVolume;
struct KzcMemoryManager;


/** Horizontal alignment. */
enum KzuObjectNodeHorizontalAlignment
{
    KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_LEFT = 0,
    KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_RIGHT = 1,
    KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_CENTER = 2,
    KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_STRETCH = 3
};

/** Vertical alignment. */
enum KzuObjectNodeVerticalAlignment
{
    KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_BOTTOM = 0,
    KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_TOP = 1,
    KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_CENTER = 2,
    KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_STRETCH = 3
};

/** Depth alignment. */
enum KzuObjectNodeDepthAlignment
{
    KZU_OBJECT_NODE_DEPTH_ALIGNMENT_BACK = 0,
    KZU_OBJECT_NODE_DEPTH_ALIGNMENT_FRONT = 1,
    KZU_OBJECT_NODE_DEPTH_ALIGNMENT_CENTER = 2,
    KZU_OBJECT_NODE_DEPTH_ALIGNMENT_STRETCH = 3
};


/**
* \struct KzuObjectNode
* Base structure for object node. Contains information of node type (mesh, light, camera, ...), transformation,
* list of child object nodes and set of properties.
*/
struct KzuObjectNode;


/** Returns if given object type is renderable. */
kzBool kzuObjectTypeIsRenderable(KzuObjectType objectType);


/** Creates an empty object node. */
kzsError kzuEmptyNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                            kzString name, struct KzuObjectNode** out_emptyObjectNode);

/** Removes an object node data, data containing any inherited object node types. */
kzsError kzuObjectNodeDelete(struct KzuObjectNode* objectNode);

/**
 * Delete an object node and all its children recursively. *
 * Traverses the tree depth-first. UI components are uninitialized before deletion.
 */
kzsError kzuObjectNodeDeleteRecursive(struct KzuObjectNode* objectNode);

/**
 * Takes ownership of the object node container deletion. After this, when an object node is deleted,
 * only the data is actually deleted and the container remains alive.
 */
void kzuObjectNodeTakeOwnership(const struct KzuObjectNode* objectNode);
/** Checks if the object node is valid (pointer and the data are non-null). */
kzBool kzuObjectNodeIsValid(const struct KzuObjectNode* objectNode);
/** Moves the data from source object node to the target object node. */
void kzuObjectNodeTransferData(struct KzuObjectNode* targetObjectNode, struct KzuObjectNode* sourceObjectNode);

/** Get the property manager associated with an object node. */
struct KzuPropertyManager* kzuObjectNodeGetPropertyManager(const struct KzuObjectNode* objectNode);
/** Get the message dispatcher associated with an object node. */
struct KzuMessageDispatcher* kzuObjectNodeGetMessageDispatcher(const struct KzuObjectNode* objectNode);

/** 
* Creates an copy of given object node and all its ancestors. All properties in the objects are also copied. 
* New nodes will contain references to the data of the source objects.
*/
kzsError kzuObjectNodeCopyWithChildren(const struct KzcMemoryManager* memoryManager, const struct KzuObjectNode* sourceObject,
                                        struct KzuObjectNode** out_objectNode);

/** Gets the parent of an object node. */
struct KzuObjectNode* kzuObjectNodeGetParent(const struct KzuObjectNode* objectNode);
/** Tell whether a node is a parent (any level) of a specified node. */
kzBool kzuObjectnodeIsParentOf(const struct KzuObjectNode* parent, const struct KzuObjectNode* child);

/** Gets the root object node that is the topmost ancestor of an object node. */
struct KzuObjectNode* kzuObjectNodeGetRootNode(struct KzuObjectNode* objectNode);

/** Adds a child node for object node. */
kzsError kzuObjectNodeAddChild(struct KzuObjectNode* parentObject, struct KzuObjectNode* childNode);
/** Adds a child node for object node to given index. */
kzsError kzuObjectNodeAddChildAtIndex(struct KzuObjectNode* parentObject, kzUint index, struct KzuObjectNode* childNode);
/** Removes child node from parent object. */
kzsError kzuObjectNodeRemoveChild(const struct KzuObjectNode* parentObject, const struct KzuObjectNode *childObject);
/** Removes all child nodes from parent object. */
kzsError kzuObjectNodeRemoveAllChildren(const struct KzuObjectNode* parentObject);
/** Checks if parent object node contains child object node. */
kzBool kzuObjectNodeHasChild(const struct KzuObjectNode* parentObjectNode, const struct KzuObjectNode* childObjectNode);
/** Gets child count from object. */
kzUint kzuObjectNodeGetChildCount(const struct KzuObjectNode* parentObject);
/** Returns child objects as iterator from object node */
struct KzcDynamicArrayIterator kzuObjectNodeGetChildren(const struct KzuObjectNode* objectNode);
/** Returns a child from given index from object node. */
struct KzuObjectNode* kzuObjectNodeGetChildAtIndex(const struct KzuObjectNode* objectNode, kzUint index);
/** Returns a child from with given name from object node. */
struct KzuObjectNode* kzuObjectNodeGetChildByName(const struct KzuObjectNode* objectNode, kzString name);


/** Sets a name for object node */
kzsError kzuObjectNodeSetName(const struct KzuObjectNode* objectNode, kzString name);
/** Returns an object node's name. */
kzString kzuObjectNodeGetName(const struct KzuObjectNode* objectNode);

/** Returns an object node's type */ 
KzuObjectType kzuObjectNodeGetType(const struct KzuObjectNode* objectNode);


/** Returns the position of the object node as vector3. */
struct KzcVector3 kzuObjectNodeGetPositionVector(const struct KzuObjectNode* objectNode);


/** Sets an identity matrix for object node. */
kzsError kzuObjectNodeSetIdentityTransformation(const struct KzuObjectNode* node);
/** Sets a top left 3x3 part of object node transformation as identity (clear rotation & scaling). 
    Leaves other parts of matrix untouched. */
kzsError kzuObjectNodeSetIdentity3x3Transformation(const struct KzuObjectNode* node);
/** Translates object node. */
kzsError kzuObjectNodeTranslate(const struct KzuObjectNode* node, kzFloat x, kzFloat y, kzFloat z);
/** Sets position for object node. */
kzsError kzuObjectNodeSetPosition(const struct KzuObjectNode* node, kzFloat x, kzFloat y, kzFloat z);
/** Rotates object node by specific axis. */
kzsError kzuObjectNodeRotate(const struct KzuObjectNode* node, kzFloat angle, kzFloat x, kzFloat y, kzFloat z);
/** Scales object node uniformly. */
kzsError kzuObjectNodeScale(const struct KzuObjectNode* node, kzFloat scale);
/** Scales object node non-uniformly. */
kzsError kzuObjectNodeScaleXYZ(const struct KzuObjectNode* node, kzFloat scaleX, kzFloat scaleY, kzFloat scaleZ);


/** Finds given object from root nodes hierarchy */
kzBool kzuObjectNodeFindObjectNode(const struct KzuObjectNode* rootNode, const struct KzuObjectNode* objectToFind);
/** Finds object node from root nodes' hiearchy by object name. KZ_NULL if no object was found. */
struct KzuObjectNode* kzuObjectNodeFindObjectNodeByName(struct KzuObjectNode* rootObjectNode, kzString objectName);
/** Finds first object node from root nodes' hierarchy. KZ_NULL if no object was found. */
struct KzuObjectNode* kzuObjectNodeFindObjectNodeContainingPropertyType(struct KzuObjectNode* rootObjectNode, const struct KzuPropertyType* propertyType);


/** Sets object visible / invisible. */
kzsError kzuObjectNodeSetVisible(const struct KzuObjectNode* objectNode, kzBool visible);
/** Returns if object node is visible / invisible. */
kzBool kzuObjectNodeIsVisible(const struct KzuObjectNode* objectNode);

/** Gets bounding volume from object node. Returns KZ_NULL if the object is not renderable. */
struct KzuBoundingVolume* kzuObjectNodeGetBoundingVolume(const struct KzuObjectNode* objectNode);

/** Gets bounding volume union from object node and its children. Result in objectNode:s local coordinates. */
void kzuObjectNodeGetBoundingVolumeUnion(const struct KzuObjectNode* objectNode, struct KzcVolume* out_volume);

/** Renders the object if the object type has specified a renderer function. */
kzsError kzuObjectNodeRender(struct KzuRenderer* renderer, const struct KzuTransformedObjectNode* transformedObjectNode);


/**
 * Calculates relative transformation for single ancestor object of rootObjectNode.
 * If the object exists, true is returned and the relative transformation is written to out_transformation.
 * Otherwise, false is returned and identity matrix is written to out_transformation.
 */
kzBool kzuObjectNodeCalculateRelativeTransformation(const struct KzuObjectNode* rootObjectNode,
                                                    const struct KzuObjectNode* objectNode,
                                                    struct KzcMatrix4x4* out_transformation);
/**
 * Calculates world transformation for an object. The transformation is calculated as the relative transformation
 * to the root object node.
 */
void kzuObjectNodeCalculateWorldTransformation(const struct KzuObjectNode* objectNode,
                                               struct KzcMatrix4x4* out_transformation);
                                                    
/** Sets object node pickable. */
kzsError kzuObjectNodeSetPickable(const struct KzuObjectNode* objectNode, kzBool pickable);
/** Returns true if object node is pickable, false if not. */
kzBool kzuObjectNodeIsPickable(const struct KzuObjectNode* objectNode);
/** Returns true if object node is pickable in 2D space, false if not. */
kzBool kzuObjectNodeIsPickable2D(const struct KzuObjectNode* objectNode);
/** Sets 2D object node pickable. */
kzsError kzuObjectNodeSetPickable2D(const struct KzuObjectNode* objectNode, kzBool pickable);

/** Returns true if child node is added to parent through tool. */
kzBool kzuObjectNodeIsChildFromKzb(const struct KzuObjectNode* parent, const struct KzuObjectNode* child);

/** Sets given childs state to be added from tool. */
kzsError kzuObjectNodeSetChildFromKzb(const struct KzuObjectNode* parent, const struct KzuObjectNode* child);

/** Returns the size of given object node. */
struct KzcVector3 kzuObjectNodeGetSize(const struct KzuObjectNode* node);
/** Returns the minimum coordinates for given object nodes bounding volume. */
struct KzcVector3 kzuObjectNodeGetMaximumBoundingBoxCoordinates(const struct KzuObjectNode* node);
/** Returns the maximum coordinates for given object nodes bounding volume. */
struct KzcVector3 kzuObjectNodeGetMinimumBoundingBoxCoordinates(const struct KzuObjectNode* node);

/** Check if the node is a layout. */
kzBool kzuObjectNodeIsLayout(const struct KzuObjectNode* node);
/** Gets layout width value, if one exists. */
kzBool kzuObjectNodeGetLayoutWidth(const struct KzuObjectNode* objectNode, kzFloat* width);
/** Gets layout height value, if one exists. */
kzBool kzuObjectNodeGetLayoutHeight(const struct KzuObjectNode* objectNode, kzFloat* height);
/** Gets layout depth value, if one exists. */
kzBool kzuObjectNodeGetLayoutDepth(const struct KzuObjectNode* objectNode, kzFloat* depth);
/** Gets layout horizontal alignment. */
enum KzuObjectNodeHorizontalAlignment kzuObjectNodeGetHorizontalAlignment(const struct KzuObjectNode* objectNode);
/** Gets layout vertical alignment. */
enum KzuObjectNodeVerticalAlignment kzuObjectNodeGetVerticalAlignment(const struct KzuObjectNode* objectNode);
/** Gets layout depth alignment. */
enum KzuObjectNodeDepthAlignment kzuObjectNodeGetDepthAlignment(const struct KzuObjectNode* objectNode);

/** Calculates a new bounding box by applying user defined size restrictions, if set. */
void kzuObjectNodeCalculateUserRestrictedSize(const struct KzuObjectNode* objectNode,
                                              const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax,
                                              struct KzcVector3* out_aabbMin, struct KzcVector3* out_aabbMax);


#include "kzu_object_properties.h"


#endif
