/**
* \file
* Base structure for scene graph nodes. Contains information of node type (mesh, light, camera, ...), transformation,
* list of child object nodes and set of properties.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_object_private.h"
#include "kzu_object.h"

#include "kzu_mesh.h"
#include "kzu_transformed_object.h"
#include "bounding_volume/kzu_bounding_volume.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_void_property.h>
#include <user/properties/kzu_vector2_property.h>
#include <user/properties/kzu_matrix4x4_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/ui/message/kzu_message_dispatcher.h>
#include <user/ui/kzu_ui_component.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_vector2.h>
#include <core/util/math/kzc_volume.h>
#include <core/util/string/kzc_string.h>

#include <system/debug/kzs_log.h>


/* TODO: Move empty object to own file. */
/** Object node class for empty object. */
extern const struct KzuObjectNodeClass KZU_EMPTY_NODE_CLASS;

const KzuObjectType KZU_OBJECT_TYPE_EMPTY = &KZU_EMPTY_NODE_CLASS;

const KzuObjectType KZU_OBJECT_TYPE_ARBITRARY = (void*)&KZU_OBJECT_TYPE_ARBITRARY;
const KzuObjectType KZU_OBJECT_TYPE_RENDERABLE = (void*)&KZU_OBJECT_TYPE_RENDERABLE;


kzBool kzuObjectTypeIsRenderable(KzuObjectType objectType)
{
    return objectType == KZU_OBJECT_TYPE_MESH || ((struct KzuObjectNodeClass*)objectType)->kzuObjectNodeRenderer != KZ_NULL;
}

/** Internal function for recursing bounding volume union. */
static void kzuObjectNodeGetBoundingVolumeUnion_internal(const struct KzuObjectNode* objectNode, struct KzcVolume* currentVolume,
                                                         const struct KzcMatrix4x4* worldTransformation, struct KzcVolume* out_volume);


kzsError kzuObjectNodeCreate_private(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                     const struct KzuObjectNodeClass* objectNodeClass, kzString name, struct KzuObjectNodeData* objectNodeData, struct KzuObjectNode** out_objectNode)
{
    kzsError result;
    struct KzuObjectNode* objectNode;

    kzsAssert(objectNodeClass != KZ_NULL);
    kzsAssert(objectNodeClass->kzuObjectNodeDeletor != KZ_NULL);
    kzsAssert(objectNodeClass->kzuObjectNodeCopier != KZ_NULL);
    
    /* Allocate memory for object */
    result = kzcMemoryAllocVariable(memoryManager, objectNode, "Object node");
    kzsErrorForward(result);

    objectNode->data = objectNodeData;
    objectNode->data->propertyManager = propertyManager;
    objectNode->data->messageDispatcher = messageDispatcher;
    objectNodeData->selfOwned = KZ_TRUE;

    objectNodeData->objectNodeClass = objectNodeClass;
    if (name != KZ_NULL)
    {
        result = kzcStringCopy(memoryManager, name, &objectNodeData->name);
        kzsErrorForward(result);
    }
    else
    {
        objectNodeData->name = KZ_NULL;
    }

    objectNodeData->parent = KZ_NULL;

    result = kzcDynamicArrayCreate(memoryManager, &objectNodeData->children);
    kzsErrorForward(result);
    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &objectNodeData->childrenFromKzb);
    kzsErrorForward(result);

    /* TODO: Property manager "kzuPropertySetFromKzb(kzuMatrix4x4PropertyToProperty(matrixProperty));" was here. */
    result = kzuObjectNodeSetMatrix4x4Property(objectNode, KZU_PROPERTY_TYPE_TRANSFORMATION, &KZC_MATRIX4X4_IDENTITY);
    kzsErrorForward(result);

    *out_objectNode = objectNode;
    kzsSuccess();
}

kzsError kzuObjectNodeDeleteBase_private(struct KzuObjectNode* objectNode)
{
    kzsError result;
    struct KzuObjectNodeData* objectNodeData;
    kzBool selfOwned;

    kzsAssert(kzuObjectNodeIsValid(objectNode));

    /* Notify the message dispatcher. */
    result = kzuMessageDispatcherNotifyObjectDeleted(kzuObjectNodeGetMessageDispatcher(objectNode), objectNode);
    kzsErrorForward(result);

    objectNodeData = objectNode->data;
    selfOwned = objectNodeData->selfOwned;

    /* Remove all child nodes. */
    result = kzuObjectNodeRemoveAllChildren(objectNode);
    kzsErrorForward(result);

    if (objectNodeData->childrenFromKzb != KZ_NULL)
    {
        /* Clear hash map. */
        struct KzcHashMapIterator it = kzcHashMapGetIterator(objectNodeData->childrenFromKzb);
        while(kzcHashMapIterate(it))
        {
            kzBool* value = (kzBool*)kzcHashMapIteratorGetValue(it);
            result = kzcMemoryFreeVariable(value);
            kzsErrorForward(result);
        }
        result = kzcHashMapDelete(objectNodeData->childrenFromKzb);
        kzsErrorForward(result);
    }
    result = kzcDynamicArrayDelete(objectNodeData->children);
    kzsErrorForward(result);

    result = kzuPropertyManagerRemoveProperties(kzuObjectNodeGetPropertyManager(objectNode), objectNode);
    kzsErrorForward(result);

    if (objectNodeData->name != KZ_NULL)
    {
        result = kzcStringDelete(objectNodeData->name);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreePointer(objectNodeData);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(objectNode);
        kzsErrorForward(result);
    }
    else
    {
        objectNode->data = KZ_NULL;
    }
 
    kzsSuccess();
}

kzsError kzuObjectNodeCopy_private(const struct KzcMemoryManager* memoryManager, const struct KzuObjectNode* sourceObjectNode,
                                   struct KzuObjectNodeData* objectNodeData, struct KzuObjectNode** out_objectNode)
{
    kzsError result;
    struct KzuObjectNode* objectNode;

    kzsAssert(kzuObjectNodeIsValid(sourceObjectNode));

    result = kzuObjectNodeCreate_private(memoryManager, sourceObjectNode->data->propertyManager, sourceObjectNode->data->messageDispatcher,
                                         sourceObjectNode->data->objectNodeClass,
                                         sourceObjectNode->data->name, objectNodeData, &objectNode);
    kzsErrorForward(result);

    result = kzuPropertyManagerCopyProperties(kzuObjectNodeGetPropertyManager(sourceObjectNode), sourceObjectNode, objectNode);
    kzsErrorForward(result);

    *out_objectNode = objectNode;
    kzsSuccess();
}

kzsError kzuObjectNodeDelete(struct KzuObjectNode* objectNode)
{
    kzsError result;

    kzsAssert(kzuObjectNodeIsValid(objectNode));

    result = objectNode->data->objectNodeClass->kzuObjectNodeDeletor(objectNode);
    kzsErrorForward(result);
 
    kzsSuccess();
}

kzsError kzuObjectNodeDeleteRecursive(struct KzuObjectNode *objectNode)
{
    kzsError result;
    KzuObjectType type = kzuObjectNodeGetType(objectNode);
    struct KzuObjectNode* parent = kzuObjectNodeGetParent(objectNode);

    if(type == KZU_OBJECT_TYPE_UI_COMPONENT) /* call uninitialize before delete */
    {
        struct KzuUiComponentNode* uiNode = kzuUiComponentNodeFromObjectNode(objectNode);

        result = kzuUiComponentNodeUninitialize(uiNode);
        kzsErrorForward(result);
    }

    while(kzuObjectNodeGetChildCount(objectNode) > 0)
    {
        struct KzuObjectNode* currentElement = kzuObjectNodeGetChildAtIndex(objectNode, 0);

        kzsAssert(currentElement != NULL);

        result = kzuObjectNodeDeleteRecursive(currentElement);
        kzsErrorForward(result);
    }

    if(parent != KZ_NULL)
    {
        result = kzuObjectNodeRemoveChild(parent, objectNode);
        kzsErrorForward(result);
    }

    result = kzuObjectNodeDelete(objectNode);
    kzsErrorForward(result);

    kzsSuccess();    
}

void kzuObjectNodeTakeOwnership(const struct KzuObjectNode* objectNode)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    objectNode->data->selfOwned = KZ_FALSE;
}

kzBool kzuObjectNodeIsValid(const struct KzuObjectNode* objectNode)
{
    return objectNode != KZ_NULL && objectNode->data != KZ_NULL;
}

void kzuObjectNodeTransferData(struct KzuObjectNode* targetObjectNode, struct KzuObjectNode* sourceObjectNode)
{
    kzsAssert(kzcIsValidPointer(targetObjectNode));
    kzsAssert(targetObjectNode->data == KZ_NULL);
    kzsAssert(kzuObjectNodeIsValid(sourceObjectNode));

    targetObjectNode->data = sourceObjectNode->data;
    sourceObjectNode->data = KZ_NULL;
}

kzsError kzuObjectNodeCopyWithChildren(const struct KzcMemoryManager* memoryManager, const struct KzuObjectNode* sourceObjectNode,
                                        struct KzuObjectNode** out_objectNode)
{
    kzsError result;
    struct KzuObjectNode* targetObjectNode;
    struct KzuObjectNodeData* sourceObjectNodeData;

    kzsAssert(kzuObjectNodeIsValid(sourceObjectNode));

    sourceObjectNodeData = sourceObjectNode->data;

    kzsAssert(sourceObjectNodeData->objectNodeClass->kzuObjectNodeCopier != KZ_NULL);
    result = sourceObjectNodeData->objectNodeClass->kzuObjectNodeCopier(memoryManager, sourceObjectNode, &targetObjectNode);
    kzsErrorForward(result);

    {
        struct KzcDynamicArrayIterator it = kzuObjectNodeGetChildren(sourceObjectNode);
        while (kzcDynamicArrayIterate(it))
        {
            struct KzuObjectNode* currentNode = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(it);
            struct KzuObjectNode* newNode;

            result = kzuObjectNodeCopyWithChildren(memoryManager, currentNode, &newNode);
            kzsErrorForward(result);

            result = kzuObjectNodeAddChild(targetObjectNode, newNode);
            kzsErrorForward(result);

            if (kzuObjectNodeIsChildFromKzb(sourceObjectNode, currentNode))
            {
                result = kzuObjectNodeSetChildFromKzb(targetObjectNode, newNode);
                kzsErrorForward(result);
            }
        }
    }

    *out_objectNode = targetObjectNode;
    kzsSuccess();
}

struct KzuPropertyManager* kzuObjectNodeGetPropertyManager(const struct KzuObjectNode* objectNode)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    return objectNode->data->propertyManager;
}

struct KzuMessageDispatcher* kzuObjectNodeGetMessageDispatcher(const struct KzuObjectNode* objectNode)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    return objectNode->data->messageDispatcher;
}

struct KzuObjectNode* kzuObjectNodeGetParent(const struct KzuObjectNode* objectNode)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    return objectNode->data->parent;
}

kzBool kzuObjectnodeIsParentOf(const struct KzuObjectNode* parent, const struct KzuObjectNode* child)
{
    kzBool returnValue = KZ_FALSE;

    while(child != NULL)
    {
        if(child == parent)
        {
            returnValue = KZ_TRUE;
        }
        child = kzuObjectNodeGetParent(child);
    }

    return returnValue;
}


struct KzuObjectNode* kzuObjectNodeGetRootNode(struct KzuObjectNode* objectNode)
{
    struct KzuObjectNode* tempNode = objectNode;

    kzsAssert(kzuObjectNodeIsValid(tempNode));

    while (tempNode->data->parent != KZ_NULL)
    {
        tempNode = tempNode->data->parent;
    }

    return tempNode;
}


kzsError kzuObjectNodeAddChild(struct KzuObjectNode* parentObjectNode, struct KzuObjectNode* childObjectNode)
{
    kzsError result;

    kzsAssertText(kzuObjectNodeIsValid(parentObjectNode), "kzuObjectNodeAddChildNode: parent object not valid");
    kzsAssertText(kzuObjectNodeIsValid(childObjectNode), "kzuObjectNodeAddChildNode: child node not valid");

    kzsErrorTest(childObjectNode->data->parent == KZ_NULL, KZU_ERROR_INVALID_SCENE_GRAPH, "Cannot add object node as a child if it already has a parent");

    result = kzcDynamicArrayAdd(parentObjectNode->data->children, childObjectNode);
    kzsErrorForward(result);

    childObjectNode->data->parent = parentObjectNode;

    kzsSuccess();
}

kzsError kzuObjectNodeAddChildAtIndex(struct KzuObjectNode* parentObjectNode, kzUint position, struct KzuObjectNode* childObjectNode)
{
    kzsError result;

    kzsAssertText(kzuObjectNodeIsValid(parentObjectNode), "kzuObjectNodeAddChildNodeAtIndex: parent object not valid");
    kzsAssertText(kzuObjectNodeIsValid(childObjectNode), "kzuObjectNodeAddChildNodeAtIndex: child node not valid");

    kzsErrorTest(childObjectNode->data->parent == KZ_NULL, KZU_ERROR_INVALID_SCENE_GRAPH, "Cannot add object node as a child if it already has a parent");

    result = kzcDynamicArrayInsert(parentObjectNode->data->children, position, childObjectNode);
    kzsErrorForward(result);

    childObjectNode->data->parent = parentObjectNode;

    kzsSuccess();
}

kzsError kzuObjectNodeRemoveChild(const struct KzuObjectNode* parentObjectNode, const struct KzuObjectNode *childObjectNode)
{
    kzsError result;
    kzBool* fromKzb;
    kzsAssert(kzuObjectNodeIsValid(parentObjectNode));
    kzsAssert(kzuObjectNodeIsValid(childObjectNode));

    if (kzcHashMapGet(parentObjectNode->data->childrenFromKzb, childObjectNode, (void**)&fromKzb))
    {  
        result = kzcHashMapRemove(parentObjectNode->data->childrenFromKzb, childObjectNode);
        kzsErrorForward(result);
        result = kzcMemoryFreeVariable(fromKzb);
        kzsErrorForward(result);
    }
    result = kzcDynamicArrayRemove(parentObjectNode->data->children, childObjectNode);
    kzsErrorForward(result);

    childObjectNode->data->parent = KZ_NULL;

    kzsSuccess();
}

kzsError kzuObjectNodeRemoveAllChildren(const struct KzuObjectNode* parentObjectNode)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;
    struct KzcHashMapIterator it2;

    kzsAssert(kzuObjectNodeIsValid(parentObjectNode));

    it = kzcDynamicArrayGetIterator(parentObjectNode->data->children);

    while (kzcDynamicArrayIterate(it))
    {
        struct KzuObjectNode* childObjectNode = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(it);

        if (kzuObjectNodeIsValid(childObjectNode))
        {
            childObjectNode->data->parent = KZ_NULL;
        }
    }

    kzcDynamicArrayClear(parentObjectNode->data->children);

    it2 = kzcHashMapGetIterator(parentObjectNode->data->childrenFromKzb);
    while (kzcHashMapIterate(it2))
    {
        kzBool* fromKzb = (kzBool*)kzcHashMapIteratorGetValue(it2);

        result = kzcMemoryFreeVariable(fromKzb);
        kzsErrorForward(result);
    }

    kzcHashMapClear(parentObjectNode->data->childrenFromKzb);

    kzsSuccess();
}

kzBool kzuObjectNodeHasChild(const struct KzuObjectNode* parentObjectNode, const struct KzuObjectNode* childObjectNode)
{
    kzsAssert(kzuObjectNodeIsValid(childObjectNode));

    return childObjectNode->data->parent == parentObjectNode;
}

kzUint kzuObjectNodeGetChildCount(const struct KzuObjectNode* objectNode)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    return kzcDynamicArrayGetSize(objectNode->data->children);    
}

struct KzcDynamicArrayIterator kzuObjectNodeGetChildren(const struct KzuObjectNode* objectNode)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    return kzcDynamicArrayGetIterator(objectNode->data->children);
}

struct KzuObjectNode* kzuObjectNodeGetChildAtIndex(const struct KzuObjectNode* objectNode, kzUint index)
{
    struct KzuObjectNode* childNode;

    kzsAssert(kzuObjectNodeIsValid(objectNode));

    if (index < kzuObjectNodeGetChildCount(objectNode))
    {
        childNode = kzcDynamicArrayGet(objectNode->data->children, index);
    }
    else
    {
        childNode = KZ_NULL;
    }

    return childNode;    
}

struct KzuObjectNode* kzuObjectNodeGetChildByName(const struct KzuObjectNode* objectNode, kzString childName)
{
    struct KzcDynamicArrayIterator it;
    struct KzuObjectNode* returnNode = KZ_NULL;

    kzsAssert(kzuObjectNodeIsValid(objectNode));

    it = kzuObjectNodeGetChildren(objectNode);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuObjectNode* childObjectNode = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(it);
        kzMutableString name = childObjectNode->data->name;
        kzUint index;

        if (kzcStringFindSubstring(name, childName, &index))
        {
            if (kzcStringLength(name) - kzcStringLength(childName) == index)
            {
                returnNode = childObjectNode;
                break;
            }
        }
    }
    return returnNode;
}

void kzuObjectNodeSetParent_private(const struct KzuObjectNode* objectNode, struct KzuObjectNode* parentObjectNode)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    objectNode->data->parent = parentObjectNode;
}

kzsError kzuObjectNodeSetName(const struct KzuObjectNode* objectNode, kzString name)
{
    kzsError result;

    kzsAssert(kzuObjectNodeIsValid(objectNode));

    /* Delete previous name if possible. */
    if (objectNode->data->name != KZ_NULL)
    {
        result = kzcStringDelete(objectNode->data->name);
        kzsErrorForward(result);
    }

    result = kzcStringCopy(kzcMemoryGetManager(objectNode), name, &objectNode->data->name);
    kzsErrorForward(result);

    kzsSuccess();
}

kzString kzuObjectNodeGetName(const struct KzuObjectNode* objectNode)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    return objectNode->data->name;
}

KzuObjectType kzuObjectNodeGetType(const struct KzuObjectNode* objectNode)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    return (KzuObjectType)objectNode->data->objectNodeClass;
}

kzsError kzuObjectNodeSetVisible(const struct KzuObjectNode* objectNode, kzBool visible)
{
    kzsError result;

    result = kzuObjectNodeSetBoolProperty(objectNode, KZU_PROPERTY_TYPE_VISIBLE, visible);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuObjectNodeIsVisible(const struct KzuObjectNode* objectNode)
{
    return kzuObjectNodeGetBoolPropertyDefault(objectNode, KZU_PROPERTY_TYPE_VISIBLE);
}

struct KzuBoundingVolume* kzuObjectNodeGetBoundingVolume(const struct KzuObjectNode* objectNode)
{
    struct KzuBoundingVolume* boundingVolume;

    kzsAssert(kzuObjectNodeIsValid(objectNode));
    
    if (objectNode->data->objectNodeClass->kzuObjectNodeBoundingVolumeGetter != KZ_NULL)
    {
        boundingVolume = objectNode->data->objectNodeClass->kzuObjectNodeBoundingVolumeGetter(objectNode);
    }
    else
    {
        boundingVolume = KZ_NULL;
    }

    return boundingVolume;
}

/* TODO: Pass KzuObjectNode instead of KzuTransformedObjectNode after other refactorings allow. */
kzsError kzuObjectNodeRender(struct KzuRenderer* renderer, const struct KzuTransformedObjectNode* transformedObjectNode)
{
    kzsError result;
    struct KzuObjectNode* objectNode = kzuTransformedObjectNodeGetObjectNode(transformedObjectNode);

    kzsAssert(kzcIsValidPointer(objectNode));

    if (objectNode->data->objectNodeClass->kzuObjectNodeRenderer != KZ_NULL)
    {
        result = objectNode->data->objectNodeClass->kzuObjectNodeRenderer(renderer, transformedObjectNode);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

static void kzuObjectNodeGetBoundingVolumeUnion_internal(const struct KzuObjectNode* objectNode, struct KzcVolume* currentVolume,
                                                         const struct KzcMatrix4x4* worldTransformation, struct KzcVolume* out_volume)
{
    struct KzuBoundingVolume* boundingVolume = kzuObjectNodeGetBoundingVolume(objectNode);
    struct KzcDynamicArrayIterator it;

    if (boundingVolume != KZ_NULL)
    {
        struct KzcVector3 minCorner;
        struct KzcVector3 maxCorner;
        struct KzcVolume vol;
        struct KzcVolume tempVolume;
        struct KzuTransformedBoundingVolume transformedBoundingVolume;

        kzuBoundingVolumeCalculateTransformedBoundingVolume(boundingVolume, worldTransformation, &transformedBoundingVolume);

        kzuTransformedBoundingVolumeGetAABB(&transformedBoundingVolume, &minCorner, &maxCorner);

        vol = kzcVolume(minCorner.data[0], minCorner.data[1], minCorner.data[2],
                        maxCorner.data[0] - minCorner.data[0], maxCorner.data[1] - minCorner.data[1], maxCorner.data[2] - minCorner.data[2]);
        tempVolume = *currentVolume;
        kzcVolumeUnion(&tempVolume, &vol, currentVolume);
    }

    it = kzuObjectNodeGetChildren(objectNode);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuObjectNode* childObjectNode = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(it);
        struct KzcMatrix4x4 newMatrix;
        struct KzcMatrix4x4 childMatrix = kzuObjectNodeGetTransformation(childObjectNode);
        kzcMatrix4x4Multiply(&childMatrix, worldTransformation, &newMatrix);
        kzuObjectNodeGetBoundingVolumeUnion_internal(childObjectNode, currentVolume, &newMatrix, out_volume);
    }
}

void kzuObjectNodeGetBoundingVolumeUnion(const struct KzuObjectNode* objectNode, struct KzcVolume* out_volume)
{
    struct KzcVolume volume = kzcVolume(KZ_FLOAT_MAXIMUM, KZ_FLOAT_MAXIMUM, KZ_FLOAT_MAXIMUM,
                                        -KZ_FLOAT_MAXIMUM, -KZ_FLOAT_MAXIMUM, -KZ_FLOAT_MAXIMUM);
    kzuObjectNodeGetBoundingVolumeUnion_internal(objectNode, &volume, &KZC_MATRIX4X4_IDENTITY, out_volume); 
    *out_volume = volume;
}

kzsError kzuObjectNodeSetIdentityTransformation(const struct KzuObjectNode* objectNode)
{
    kzsError result;
    result = kzuObjectNodeSetTransformation(objectNode, &KZC_MATRIX4X4_IDENTITY);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzcVector3 kzuObjectNodeGetPositionVector(const struct KzuObjectNode* objectNode)
{
    struct KzcVector3 position;
    struct KzcMatrix4x4 matrix = kzuObjectNodeGetTransformation(objectNode);
    kzcMatrix4x4GetPosition(&matrix, &position);
    return position;
}

kzsError kzuObjectNodeSetIdentity3x3Transformation(const struct KzuObjectNode* objectNode)
{
    kzsError result;
    struct KzcMatrix4x4 matrix;
    kzBool found;

    found = kzuObjectNodeGetBaseMatrix4x4Property(objectNode, KZU_PROPERTY_TYPE_TRANSFORMATION, &matrix);
    kzsAssert(found);
    kzcMatrix4x4SetIdentity3x3(&matrix);
    result = kzuObjectNodeSetTransformation(objectNode, &matrix);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuObjectNodeTranslate(const struct KzuObjectNode* objectNode, kzFloat x, kzFloat y, kzFloat z)
{
    kzsError result;
    struct KzcMatrix4x4 matrix = kzuObjectNodeGetBaseTransformation(objectNode);
    kzcMatrix4x4Translate(&matrix, x, y, z);
    result = kzuObjectNodeSetTransformation(objectNode, &matrix);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuObjectNodeSetPosition(const struct KzuObjectNode* objectNode, kzFloat x, kzFloat y, kzFloat z)
{
    kzsError result;
    struct KzcMatrix4x4 matrix = kzuObjectNodeGetBaseTransformation(objectNode);
    matrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_X] = x;
    matrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Y] = y;
    matrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Z] = z;
    result = kzuObjectNodeSetTransformation(objectNode, &matrix);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuObjectNodeRotate(const struct KzuObjectNode* objectNode, kzFloat angle, kzFloat x, kzFloat y, kzFloat z)
{
    kzsError result;
    struct KzcMatrix4x4 matrix = kzuObjectNodeGetBaseTransformation(objectNode);
    kzcMatrix4x4Rotate(&matrix, angle, x, y, z);
    result = kzuObjectNodeSetTransformation(objectNode, &matrix);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuObjectNodeScale(const struct KzuObjectNode* objectNode, kzFloat scale)
{
    kzsError result;
    struct KzcMatrix4x4 matrix = kzuObjectNodeGetBaseTransformation(objectNode);
    kzcMatrix4x4ScaleUniform(&matrix, scale);
    result = kzuObjectNodeSetTransformation(objectNode, &matrix);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuObjectNodeScaleXYZ(const struct KzuObjectNode* objectNode, kzFloat scaleX, kzFloat scaleY, kzFloat scaleZ)
{
    kzsError result;
    struct KzcMatrix4x4 matrix = kzuObjectNodeGetBaseTransformation(objectNode);
    kzcMatrix4x4Scale(&matrix, scaleX, scaleY, scaleZ);
    result = kzuObjectNodeSetTransformation(objectNode, &matrix);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuObjectNodeFindObjectNode(const struct KzuObjectNode* rootObjectNode, const struct KzuObjectNode* objectNodeToFind)
{
    kzBool found = KZ_FALSE;

    kzsAssert(kzuObjectNodeIsValid(rootObjectNode));
    kzsAssert(kzuObjectNodeIsValid(objectNodeToFind));

    /* Object was found from current root node, return it. */
    if (rootObjectNode == objectNodeToFind)
    {
        found = KZ_TRUE;
    }
    else
    {
        struct KzcDynamicArrayIterator it = kzuObjectNodeGetChildren(rootObjectNode);

        while (kzcDynamicArrayIterate(it) && !found)
        {
            struct KzuObjectNode* childObjectNode = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(it);
            found = kzuObjectNodeFindObjectNode(childObjectNode, objectNodeToFind);
        }
    }

    return found;
}

struct KzuObjectNode* kzuObjectNodeFindObjectNodeByName(struct KzuObjectNode* rootObjectNode, kzString objectName)
{
    struct KzuObjectNode* foundObject = KZ_NULL;

    kzsAssert(kzcIsValidPointer(rootObjectNode));

    if (rootObjectNode->data->name != KZ_NULL && kzcStringIsEqual(rootObjectNode->data->name, objectName))
    {
        foundObject = rootObjectNode;
    }
    else
    {
        struct KzcDynamicArrayIterator it = kzuObjectNodeGetChildren(rootObjectNode);

        while (kzcDynamicArrayIterate(it) && foundObject == KZ_NULL)
        {
            struct KzuObjectNode* childObjectNode = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(it);
            foundObject = kzuObjectNodeFindObjectNodeByName(childObjectNode, objectName);
        }
    }

    return foundObject;
}

struct KzuObjectNode* kzuObjectNodeFindObjectNodeContainingPropertyType(struct KzuObjectNode* rootObjectNode, const struct KzuPropertyType* propertyType)
{
    struct KzuObjectNode* foundObject = KZ_NULL;

    kzsAssert(kzcIsValidPointer(rootObjectNode));

    if (kzuPropertyManagerHasProperty(kzuObjectNodeGetPropertyManager(rootObjectNode), rootObjectNode, propertyType))
    {
        foundObject = rootObjectNode;
    }
    else
    {
        struct KzcDynamicArrayIterator it = kzuObjectNodeGetChildren(rootObjectNode);

        while (kzcDynamicArrayIterate(it) && foundObject == KZ_NULL)
        {
            struct KzuObjectNode* childObjectNode = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(it);
            foundObject = kzuObjectNodeFindObjectNodeContainingPropertyType(childObjectNode, propertyType);
        }
    }

    return foundObject;
}

/* TODO: Comment */
static kzBool kzuObjectNodeCalculateWorldTransformation_internal(const struct KzuObjectNode* rootObjectNode,
                                                                 const struct KzuObjectNode* objectNodeToFind,
                                                                 const struct KzcMatrix4x4* currentTransformation,
                                                                 struct KzcMatrix4x4* out_relativeTransformation)
{
    struct KzcMatrix4x4 worldTransformation;
    kzBool found = KZ_FALSE;

    kzsAssert(kzuObjectNodeIsValid(rootObjectNode));
    kzsAssert(kzuObjectNodeIsValid(objectNodeToFind));

    {
        struct KzcMatrix4x4 rootLocalTransformation = kzuObjectNodeGetTransformation(rootObjectNode);
        kzcMatrix4x4Multiply(&rootLocalTransformation, currentTransformation, &worldTransformation);
    }

    /* Object was found from current root node, return it. */
    if (rootObjectNode == objectNodeToFind)
    {
        *out_relativeTransformation = worldTransformation;
        found = KZ_TRUE;
    }
    else
    {
        struct KzcDynamicArrayIterator it = kzuObjectNodeGetChildren(rootObjectNode);

        while (kzcDynamicArrayIterate(it) && !found)
        {
            struct KzuObjectNode* childObjectNode = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(it);
            found = kzuObjectNodeCalculateWorldTransformation_internal(childObjectNode, objectNodeToFind,
                                                                       &worldTransformation, out_relativeTransformation);
        }
    }

    return found;
}

kzBool kzuObjectNodeCalculateRelativeTransformation(const struct KzuObjectNode* rootObjectNode,
                                                    const struct KzuObjectNode* objectNode,
                                                    struct KzcMatrix4x4* out_transformation)
{
    struct KzcMatrix4x4 relativeTransformation = KZC_MATRIX4X4_IDENTITY;
    kzBool objectFound;

    kzsAssert(kzuObjectNodeIsValid(rootObjectNode));

    objectFound = kzuObjectNodeCalculateWorldTransformation_internal(rootObjectNode, objectNode,
                                                                     &KZC_MATRIX4X4_IDENTITY, &relativeTransformation);
    
    *out_transformation = relativeTransformation;
    return objectFound;
}

void kzuObjectNodeCalculateWorldTransformation(const struct KzuObjectNode* objectNode,
                                               struct KzcMatrix4x4* out_transformation)
{
    const struct KzuObjectNode* tempNode = objectNode;
    struct KzcMatrix4x4 worldTransformation = KZC_MATRIX4X4_IDENTITY;

    while (tempNode != KZ_NULL)
    {
        struct KzcMatrix4x4 objectTransformation = kzuObjectNodeGetTransformation(tempNode);
        struct KzcMatrix4x4 newWorldTransformation;
        kzcMatrix4x4Multiply(&worldTransformation, &objectTransformation, &newWorldTransformation);

        worldTransformation = newWorldTransformation;
        tempNode = tempNode->data->parent;
    }

    *out_transformation = worldTransformation;
}

kzsError kzuObjectNodeSetPickable(const struct KzuObjectNode* objectNode, kzBool pickable)
{
    kzsError result;
    
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    result = kzuObjectNodeSetBoolProperty(objectNode, KZU_PROPERTY_TYPE_PICKABLE, pickable);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuObjectNodeIsPickable(const struct KzuObjectNode* objectNode)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    return kzuObjectNodeGetBoolPropertyDefault(objectNode, KZU_PROPERTY_TYPE_PICKABLE);
}

kzBool kzuObjectNodeIsPickable2D(const struct KzuObjectNode* objectNode)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    return kzuObjectNodeGetBoolPropertyDefault(objectNode, KZU_PROPERTY_TYPE_PICKABLE_2D);
}

kzsError kzuObjectNodeSetPickable2D(const struct KzuObjectNode* objectNode, kzBool pickable)
{
    kzsError result;

    kzsAssert(kzuObjectNodeIsValid(objectNode));

    result = kzuObjectNodeSetBoolProperty(objectNode, KZU_PROPERTY_TYPE_PICKABLE_2D, pickable);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuObjectNodeIsChildFromKzb(const struct KzuObjectNode* parent, const struct KzuObjectNode* child)
{
    kzBool found;
    kzBool* value;
    
    kzsAssert(kzuObjectNodeIsValid(parent));
    kzsAssert(kzuObjectNodeIsValid(child));
    
    if (kzcHashMapGet(parent->data->childrenFromKzb, child, (void**)&value))
    {
        found = *value;
    }
    else
    {
        found = KZ_FALSE;
    }

    return found;
}

kzsError kzuObjectNodeSetChildFromKzb(const struct KzuObjectNode* parent, const struct KzuObjectNode* child)
{
    kzsError result;
    kzBool* value;

    kzsAssert(kzuObjectNodeIsValid(parent));
    kzsAssert(kzuObjectNodeIsValid(child));
    kzsAssertText(!kzuObjectNodeIsChildFromKzb(parent, child), "Children fromKzb status already set.");

    result = kzcMemoryAllocVariable(kzcMemoryGetManager(parent), value, "Child from Kzb bool id");
    kzsErrorForward(result);

    *value = KZ_TRUE;

    result = kzcHashMapPut(parent->data->childrenFromKzb, child, value);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzcVector3 kzuObjectNodeGetSize(const struct KzuObjectNode* objectNode)
{
    struct KzcVector3 size = kzcVector3(0.0f, 0.0f, 0.0f);
    struct KzcVector3 minimum;
    struct KzcVector3 maximum;
    struct KzuBoundingVolume* boundingVolume = kzuObjectNodeGetBoundingVolume(objectNode);

    if (boundingVolume != KZ_NULL)
    {
        struct KzuBoundingVolumeAxisAlignedBox* boundingBox = kzuBoundingVolumeGetAxisAlignedBox(boundingVolume);

        minimum = kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner(boundingBox);
        maximum = kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner(boundingBox);
        kzcVector3Subtract(&maximum, &minimum, &size);
    }

    return size;
}

struct KzcVector3 kzuObjectNodeGetMaximumBoundingBoxCoordinates(const struct KzuObjectNode* objectNode)
{
    struct KzcVector3 size = kzcVector3(0.0f, 0.0f, 0.0f);
    struct KzuBoundingVolume* boundingVolume = kzuObjectNodeGetBoundingVolume(objectNode);

    if (boundingVolume != KZ_NULL)
    {
        struct KzuBoundingVolumeAxisAlignedBox* boundingBox = kzuBoundingVolumeGetAxisAlignedBox(boundingVolume);

        size = kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner(boundingBox);
    }

    return size;
}

struct KzcVector3 kzuObjectNodeGetMinimumBoundingBoxCoordinates(const struct KzuObjectNode* objectNode)
{
    struct KzcVector3 size = kzcVector3(0.0f, 0.0f, 0.0f);
    struct KzuBoundingVolume* boundingVolume = kzuObjectNodeGetBoundingVolume(objectNode);

    if (boundingVolume != KZ_NULL)
    {
        struct KzuBoundingVolumeAxisAlignedBox* boundingBox = kzuBoundingVolumeGetAxisAlignedBox(boundingVolume);
        
        size = kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner(boundingBox);
        
    }

    return size;
}

kzBool kzuObjectNodeIsLayout(const struct KzuObjectNode* node)
{
    kzBool layout = KZ_FALSE;
    kzsAssert(kzuObjectNodeIsValid(node));

    if (kzuObjectNodeGetType(node) == KZU_OBJECT_TYPE_UI_COMPONENT)
    {
        struct KzuUiComponentNode* componentNode = kzuUiComponentNodeFromObjectNode(node);
        KzuUiComponentArrangeFunction arrangeFunction = kzuUiComponentNodeGetArrangeFunction(componentNode);
        if (arrangeFunction != KZ_NULL)
        {
            /* Sanity check for layouts specifying only one of the layout callbacks. */
            kzsAssert(kzuUiComponentNodeGetMeasureFunction(componentNode) != KZ_NULL);
            layout = KZ_TRUE;
        }
    }

    return layout;
}

kzBool kzuObjectNodeGetLayoutWidth(const struct KzuObjectNode* objectNode, kzFloat* width)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    return kzuObjectNodeGetFloatProperty(objectNode, KZU_PROPERTY_TYPE_LAYOUT_WIDTH, width);
}

kzBool kzuObjectNodeGetLayoutHeight(const struct KzuObjectNode* objectNode, kzFloat* height)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    return kzuObjectNodeGetFloatProperty(objectNode, KZU_PROPERTY_TYPE_LAYOUT_HEIGHT, height);
}

kzBool kzuObjectNodeGetLayoutDepth(const struct KzuObjectNode* objectNode, kzFloat* depth)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    return kzuObjectNodeGetFloatProperty(objectNode, KZU_PROPERTY_TYPE_LAYOUT_DEPTH, depth);
}

enum KzuObjectNodeHorizontalAlignment kzuObjectNodeGetHorizontalAlignment(const struct KzuObjectNode* objectNode)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    return (enum KzuObjectNodeHorizontalAlignment)kzuObjectNodeGetIntPropertyDefault(objectNode, KZU_PROPERTY_TYPE_LAYOUT_HORIZONTAL_ALIGNMENT);/*lint !e930*/
}

enum KzuObjectNodeVerticalAlignment kzuObjectNodeGetVerticalAlignment(const struct KzuObjectNode* objectNode)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    return (enum KzuObjectNodeVerticalAlignment)kzuObjectNodeGetIntPropertyDefault(objectNode, KZU_PROPERTY_TYPE_LAYOUT_VERTICAL_ALIGNMENT);/*lint !e930*/
}

enum KzuObjectNodeDepthAlignment kzuObjectNodeGetDepthAlignment(const struct KzuObjectNode* objectNode)
{
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    return (enum KzuObjectNodeDepthAlignment)kzuObjectNodeGetIntPropertyDefault(objectNode, KZU_PROPERTY_TYPE_LAYOUT_DEPTH_ALIGNMENT);/*lint !e930*/
}

void kzuObjectNodeCalculateUserRestrictedSize(const struct KzuObjectNode* objectNode,
                                              const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax,
                                              struct KzcVector3* out_aabbMin, struct KzcVector3* out_aabbMax)
{
    struct KzcVector3 newMin = *aabbMin;
    struct KzcVector3 newMax = *aabbMax;
    struct KzcVector3 center;
    /* TODO: User size */ /* TODO: Use 3 separate variables, do not reuse variable for different purposes. */
    kzFloat overrideValue = 0.0f;

    kzsAssert(kzuObjectNodeIsValid(objectNode));

    /* Calculate center. */
    kzcVector3Add(aabbMin, aabbMax, &center);
    kzcVector3Scale(&center, 0.5f, &center);

    if (kzuObjectNodeGetLayoutWidth(objectNode, &overrideValue))
    {
        kzFloat halfDimension = overrideValue * 0.5f;
        kzcVector3SetX(&newMin, kzcVector3GetX(&center) - halfDimension);
        kzcVector3SetX(&newMax, kzcVector3GetX(&center) + halfDimension);
    }

    if (kzuObjectNodeGetLayoutHeight(objectNode, &overrideValue))
    {
        kzFloat halfDimension = overrideValue * 0.5f;
        kzcVector3SetY(&newMin, kzcVector3GetY(&center) - halfDimension);
        kzcVector3SetY(&newMax, kzcVector3GetY(&center) + halfDimension);
    }

    if (kzuObjectNodeGetLayoutDepth(objectNode, &overrideValue))
    {
        kzFloat halfDimension = overrideValue * 0.5f;
        kzcVector3SetZ(&newMin, kzcVector3GetZ(&center) - halfDimension);
        kzcVector3SetZ(&newMax, kzcVector3GetZ(&center) + halfDimension);
    }

    *out_aabbMin = newMin;
    *out_aabbMax = newMax;
}

kzsError kzuEmptyNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                            kzString name, struct KzuObjectNode** out_emptyObjectNode)
{
    kzsError result;
    struct KzuObjectNode* objectNode;
    struct KzuObjectNodeData* objectNodeData;

    result = kzcMemoryAllocVariable(memoryManager, objectNodeData, "Empty object node");
    kzsErrorForward(result);

    result = kzuObjectNodeCreate_private(memoryManager, propertyManager, messageDispatcher, &KZU_EMPTY_NODE_CLASS, name, objectNodeData, &objectNode);
    kzsErrorForward(result);

    *out_emptyObjectNode = objectNode;
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuEmptyNodeDelete_internal(struct KzuObjectNode* objectNode)
{
    kzsError result;

    result = kzuObjectNodeDeleteBase_private(objectNode);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuEmptyNodeCopyImplementation_internal(const struct KzcMemoryManager* memoryManager,
                                                                    const struct KzuObjectNode* sourceObjectNode,
                                                                    struct KzuObjectNode** out_objectNode)
{
    kzsError result;
    struct KzuObjectNodeData* objectNodeData;
    struct KzuObjectNode* objectNode;

    result = kzcMemoryAllocVariable(memoryManager, objectNodeData, "Empty object node");
    kzsErrorForward(result);

    result = kzuObjectNodeCopy_private(memoryManager, sourceObjectNode, objectNodeData, &objectNode);
    kzsErrorForward(result);

    *out_objectNode = objectNode;
    kzsSuccess();
}

const struct KzuObjectNodeClass KZU_EMPTY_NODE_CLASS =
{
    kzuEmptyNodeDelete_internal,
    kzuEmptyNodeCopyImplementation_internal,
    KZ_NULL,
    KZ_NULL
};
