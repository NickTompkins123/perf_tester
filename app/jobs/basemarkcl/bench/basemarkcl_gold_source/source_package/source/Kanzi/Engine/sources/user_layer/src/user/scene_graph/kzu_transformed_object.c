/**
* \file
* Transformed object node. Created from original scene graph object node (KzuObjectNode).
* Contains all characteristics as object node, as transformed object node owns instance to one.
* Matrix in calculated world coordinates after extracted in pipeline (kzuSceneExtract / kzuEngineExtractScene called).
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_transformed_object.h"

#include "kzu_object.h"
#include "bounding_volume/kzu_bounding_volume.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_vector2_property.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/ui/kzu_ui_component.h>

#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector2.h>
#include <core/util/math/kzc_volume.h>
#include <core/memory/kzc_memory_manager.h>


/* TODO: Comment struct members. Check if some of these can be dropped and explain the use. */
struct KzuTransformedObjectNodeUiData
{
    struct KzcVector3 contentMin, contentMax; /** Node content size. */ /* TODO: Explain these values. */
    struct KzcVector3 desiredMin, desiredMax; /** Node desired size. */
    struct KzcVector3 layoutMin, layoutMax; /** Node layout size. */ /* TODO: Rename to allocated. */
    struct KzcVector3 actualMin, actualMax; /** Node actual size. */
    struct KzcVector3 clipMin, clipMax; /** Clipping box. */
    struct KzcVector3 clipOffset;
    kzBool clipping;
    kzBool clipped;
    kzBool simpleLayout;
};

struct KzuTransformedObjectNode
{
    struct KzuObjectNode*   object;                /**< Reference to original data */
    struct KzcDynamicArray* children;              /**< Direct children of the object. */
    struct KzuTransformedObjectNode* parent;       /**< Parent object. */

    struct KzcMatrix4x4        transformation;     /**< World matrix for transformed object node */
    
    struct KzuTransformedObjectNodeUiData* uiData; /**< UI data for the object. */
};


kzsError kzuTransformedObjectNodeCreateFromObjectNode(const struct KzcMemoryManager* memoryManager, struct KzuObjectNode* objectNode, 
    struct KzuTransformedObjectNode** out_transformedObjectNode)
{
    kzsError result;
    struct KzuTransformedObjectNode* transformedObject;

    kzsAssert(kzcIsValidPointer(objectNode));

    /* Allocate memory for transformed object node*/
    result = kzcMemoryAllocVariable(memoryManager, transformedObject, "TransformedObjectNode");
    kzsErrorForward(result);

    /* Copy original scene graph transformation */
    transformedObject->transformation = kzuObjectNodeGetTransformation(objectNode);

    /* Set address to original node in scene graph */
    transformedObject->object = objectNode;

    /* Create dynamic array for children. */
    result = kzcDynamicArrayCreate(memoryManager, &transformedObject->children);
    kzsErrorForward(result);

    transformedObject->uiData = KZ_NULL;
    transformedObject->parent = KZ_NULL;

    *out_transformedObjectNode = transformedObject;

    kzsSuccess();
}

struct KzuTransformedObjectNode* kzuTransformedObjectNodeGetParent(const struct KzuTransformedObjectNode* transformedObjectNode)
{
    return transformedObjectNode->parent;
}

kzsError kzuTransformedObjectNodeAddChildNode(struct KzuTransformedObjectNode* parentObject, struct KzuTransformedObjectNode* childNode)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(childNode));
    kzsAssert(kzcIsValidPointer(parentObject));

    /* Add new object to dynamic array. */
    result = kzcDynamicArrayAdd(parentObject->children, childNode);
    kzsErrorForward(result);

    childNode->parent = parentObject;

    kzsSuccess();
}

kzsError kzuTransformedObjectNodeRemove(struct KzuTransformedObjectNode* parentObject, const struct KzuTransformedObjectNode* destObject)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;

    kzsAssert(kzcIsValidPointer(parentObject));
    kzsAssert(kzcIsValidPointer(destObject));

    /* Remove the object from parent's children. We don't need to care about memory release since quick memory is used 
        with transformed object nodes. */
    result = kzcDynamicArrayRemove(parentObject->children, destObject);
    kzsErrorForward(result);

    /** Add the removed object node's children to its parent's children */
    it = kzcDynamicArrayGetIterator(destObject->children);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuTransformedObjectNode* childNode = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);
        result = kzuTransformedObjectNodeAddChildNode(parentObject, childNode);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuTransformedObjectNodeCopyWithoutChildren(const struct KzcMemoryManager* memoryManager, 
                                                     const struct KzuTransformedObjectNode* sourceObject, 
                                                     struct KzuTransformedObjectNode** out_object)
{
    kzsError result;
    struct KzuTransformedObjectNode* transformedObject;

    kzsAssert(kzcIsValidPointer(sourceObject));

    /* Allocate memory for transformed object node*/
    result = kzcMemoryAllocVariable(memoryManager, transformedObject, "TransformedObjectNodeCopy");
    kzsErrorForward(result);

    /* Copy object node data, but naturally no children (keep references) */
    kzuTransformedObjectNodeSetMatrix(transformedObject, &sourceObject->transformation);
    transformedObject->object = sourceObject->object;

    /* Create dynamic array for children. */
    result = kzcDynamicArrayCreate(memoryManager, &transformedObject->children);
    kzsErrorForward(result);
    
    *out_object = transformedObject;

    kzsSuccess();
}

static kzsError kzuTransformedObjectNodeCreateDynamicArray_private(struct KzuTransformedObjectNode* rootNode, 
                                                                   struct KzcDynamicArray* dynamicArray)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;

    result = kzcDynamicArrayAdd(dynamicArray, rootNode);
    kzsErrorForward(result);

    it = kzuTransformedObjectNodeGetChildren(rootNode);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuTransformedObjectNode* node = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);
        result = kzuTransformedObjectNodeCreateDynamicArray_private(node, dynamicArray);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuTransformedObjectTreeToDynamicArray(const struct KzcMemoryManager* memoryManager, struct KzuTransformedObjectNode* rootNode, 
                                                    struct KzcDynamicArray** out_dynamicArray)
{
    kzsError result;
    struct KzcDynamicArray* dynamicArray;

    /* Allocate memory (usually quick alloc) for the dynamic array. */
    result = kzcDynamicArrayCreate(memoryManager, &dynamicArray);
    kzsErrorForward(result);

    /* Iterate the child nodes. */
    result = kzuTransformedObjectNodeCreateDynamicArray_private(rootNode, dynamicArray);
    kzsErrorForward(result);

    *out_dynamicArray = dynamicArray;
    kzsSuccess();
}

kzUint kzuTransformedObjectNodeGetChildCount(const struct KzuTransformedObjectNode* transformedObjectNode)
{
    kzsAssert(kzcIsValidPointer(transformedObjectNode));
    return kzcDynamicArrayGetSize(transformedObjectNode->children);
}

struct KzcDynamicArrayIterator kzuTransformedObjectNodeGetChildren(const struct KzuTransformedObjectNode* transformedObjectNode)
{
    kzsAssert(kzcIsValidPointer(transformedObjectNode));
    return kzcDynamicArrayGetIterator(transformedObjectNode->children);
}

struct KzuTransformedObjectNode* kzuTransformedObjectNodeGetChildAtIndex(const struct KzuTransformedObjectNode* transformedObjectNode,
                                                                         kzUint index)
{
    struct KzuTransformedObjectNode* node;
    kzsAssert(kzcIsValidPointer(transformedObjectNode));
    node = kzcDynamicArrayGet(transformedObjectNode->children, index);
    return node;    
}

struct KzuObjectNode* kzuTransformedObjectNodeGetObjectNode(const struct KzuTransformedObjectNode* transformedObjectNode)
{
    kzsAssert(kzcIsValidPointer(transformedObjectNode));
    kzsAssert(kzcIsValidPointer(transformedObjectNode->object));
    return transformedObjectNode->object;
}

struct KzuTransformedObjectNode* kzuTransformedObjectNodeFindChildObjectNode(const struct KzuTransformedObjectNode* transformedObjectNode,
                                                                             const struct KzuObjectNode* objectToFind)
{
    struct KzuTransformedObjectNode* foundObject = KZ_NULL;
    struct KzcDynamicArrayIterator childIterator = kzuTransformedObjectNodeGetChildren(transformedObjectNode);

    while(kzcDynamicArrayIterate(childIterator))
    {
        struct KzuTransformedObjectNode* childObject = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(childIterator);
        /* Object was found, return it. */
        if(kzuTransformedObjectNodeGetObjectNode(childObject) == objectToFind)
        {
            foundObject = childObject;
            break;
        }
    }

    return foundObject;
}

struct KzuTransformedObjectNode* kzuTransformedObjectNodeFindChildObjectNodeFromDescendants(const struct KzuTransformedObjectNode* transformedObjectNode,
    const struct KzuObjectNode* objectToFind)
{
    struct KzuTransformedObjectNode* foundObject = KZ_NULL;
    struct KzcDynamicArrayIterator childIterator = kzuTransformedObjectNodeGetChildren(transformedObjectNode);

    while(kzcDynamicArrayIterate(childIterator))
    {
        struct KzuTransformedObjectNode* childObject = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(childIterator);
        if(kzuTransformedObjectNodeGetObjectNode(childObject) == objectToFind)
        {
            foundObject = childObject;
            break;
        }
        foundObject = kzuTransformedObjectNodeFindChildObjectNodeFromDescendants(childObject, objectToFind);
        if(foundObject != KZ_NULL)
        {
            break;
        }
    }

    return foundObject;
}

struct KzuTransformedObjectNode* kzuTransformedObjectNodeFindChildObjectTransformedNodeFromDescendants(const struct KzuTransformedObjectNode* transformedObjectNode,
                                                                                                       const struct KzuTransformedObjectNode* transformedObjectNodeToFind)
{
    struct KzuTransformedObjectNode* foundObject = KZ_NULL;
    struct KzcDynamicArrayIterator childIterator = kzuTransformedObjectNodeGetChildren(transformedObjectNode);

    while(kzcDynamicArrayIterate(childIterator))
    {
        struct KzuTransformedObjectNode* childObject = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(childIterator);
        if(childObject == transformedObjectNodeToFind)
        {
            foundObject = childObject;
            break;
        }
        foundObject = kzuTransformedObjectNodeFindChildObjectTransformedNodeFromDescendants(childObject, transformedObjectNodeToFind);
        if(foundObject != KZ_NULL)
        {
            break;
        }
    }

    return foundObject;
}

void kzuTransformedObjectNodeSetMatrix(struct KzuTransformedObjectNode* transformedObjectNode, const struct KzcMatrix4x4* matrix)
{
    kzsAssert(kzcIsValidPointer(transformedObjectNode));
    transformedObjectNode->transformation = *matrix;
}

struct KzcMatrix4x4 kzuTransformedObjectNodeGetMatrix(const struct KzuTransformedObjectNode* transformedObjectNode)
{
    kzsAssert(kzcIsValidPointer(transformedObjectNode));
    return transformedObjectNode->transformation;
}

void kzuTransformedObjectNodeGetPosition(const struct KzuTransformedObjectNode* transformedObjectNode, struct KzcVector3* out_position)
{
    struct KzcMatrix4x4 objectNodeMatrix;
    kzsAssert(kzcIsValidPointer(transformedObjectNode));
    objectNodeMatrix = kzuTransformedObjectNodeGetMatrix(transformedObjectNode);
    kzcMatrix4x4GetPosition(&objectNodeMatrix, out_position);
}

struct KzuTransformedObjectNode* kzuTransformedObjectListFindObject(const struct KzcDynamicArray* objectList,
                                                                    const struct KzuObjectNode* objectToFind)
{
    struct KzcDynamicArrayIterator it;
    struct KzuTransformedObjectNode* foundObject = KZ_NULL;

    kzsAssert(kzcIsValidPointer(objectList));
    kzsAssert(objectToFind == KZ_NULL || kzcIsValidPointer(objectToFind));

    it = kzcDynamicArrayGetIterator(objectList);

    while (kzcDynamicArrayIterate(it))
    {
        struct KzuTransformedObjectNode* currentObjectNode = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);
        if(currentObjectNode->object == objectToFind)
        {
            foundObject = currentObjectNode;
            break;
        }
    }
    return foundObject;
}

kzsError kzuTransformedObjectNodeAllocateUiData(const struct KzcMemoryManager* memoryManager, struct KzuTransformedObjectNode* node)
{
    kzsError result;
    struct KzcVector3 invalidMinVector = kzcVector3(KZ_FLOAT_MAXIMUM, KZ_FLOAT_MAXIMUM, KZ_FLOAT_MAXIMUM);
    struct KzcVector3 invalidMaxVector = kzcVector3(-KZ_FLOAT_MAXIMUM, -KZ_FLOAT_MAXIMUM, -KZ_FLOAT_MAXIMUM);

    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(node->uiData == KZ_NULL);

    result = kzcMemoryAllocVariable(memoryManager, node->uiData, "TransformedObjectNodeUiData");
    kzsErrorForward(result);

    kzuTransformedObjectNodeSetContentSize(node, &invalidMinVector, &invalidMaxVector);
    kzuTransformedObjectNodeSetDesiredSize(node, &invalidMinVector, &invalidMaxVector);
    kzuTransformedObjectNodeSetLayoutSize(node, &invalidMinVector, &invalidMaxVector);
    kzuTransformedObjectNodeSetActualSize(node, &invalidMinVector, &invalidMaxVector);
    kzuTransformedObjectNodeSetClipBox(node, &invalidMinVector, &invalidMaxVector);
    node->uiData->clipOffset = KZC_VECTOR3_ZERO;

    node->uiData->clipping = KZ_FALSE;
    node->uiData->clipped = KZ_FALSE;
    node->uiData->simpleLayout = KZ_FALSE;

    kzsSuccess();
}

kzBool kzuTransformedObjectNodeHasUiData(const struct KzuTransformedObjectNode* node)
{
    kzsAssert(kzcIsValidPointer(node));

    return node->uiData != KZ_NULL;
}

void kzuTransformedObjectNodeSetClipBox(const struct KzuTransformedObjectNode* node, const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax)
{
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(node->uiData != KZ_NULL);

    node->uiData->clipMin = *aabbMin;
    node->uiData->clipMax = *aabbMax;
}

void kzuTransformedObjectNodeGetClipBox(const struct KzuTransformedObjectNode* node, struct KzcVector3* aabbMin, struct KzcVector3* aabbMax)
{
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(node->uiData != KZ_NULL);

    *aabbMin = node->uiData->clipMin;
    *aabbMax = node->uiData->clipMax;
}

kzBool kzuTransformedObjectNodeIsClippingEnabled(const struct KzuTransformedObjectNode* node)
{
    kzBool clipping = KZ_FALSE;
    kzsAssert(kzcIsValidPointer(node));
    
    if (kzuTransformedObjectNodeHasUiData(node))
    {
        clipping = node->uiData->clipping;
    }

    return clipping;
}

void kzuTransformedObjectNodeSetClippingEnabled(const struct KzuTransformedObjectNode* node, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(node->uiData != KZ_NULL);
    
    node->uiData->clipping = enabled;
}

kzBool kzuTransformedObjectNodeIsClipped(const struct KzuTransformedObjectNode* node)
{
    kzBool clipped = KZ_FALSE;

    kzsAssert(kzcIsValidPointer(node));

    if (kzuTransformedObjectNodeHasUiData(node))
    {
        clipped = node->uiData->clipped;
    }

    return clipped;
}

void kzuTransformedObjectNodeSetClipped(const struct KzuTransformedObjectNode* node, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(node->uiData != KZ_NULL);

    node->uiData->clipped = enabled;
}

kzBool kzuTransformedObjectNodeIsLayout(const struct KzuTransformedObjectNode* node)
{
    kzsAssert(kzcIsValidPointer(node));
    return kzuObjectNodeIsLayout(kzuTransformedObjectNodeGetObjectNode(node));
}

kzBool kzuTransformedObjectNodeIsClipChildrenEnabled(const struct KzuTransformedObjectNode* node)
{
    struct KzuObjectNode* objectNode = kzuTransformedObjectNodeGetObjectNode(node);

    return kzuObjectNodeGetBoolPropertyDefault(objectNode, KZU_PROPERTY_TYPE_CLIP_CHILDREN);
}

/* 
* TODO: Add to comment or name:
* This is bounding box where user restrictions are applied. It does not contain transformations.
*/
void kzuTransformedObjectNodeSetContentSize(const struct KzuTransformedObjectNode* node, const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax)
{
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(node->uiData != KZ_NULL);

    node->uiData->contentMin = *aabbMin;
    node->uiData->contentMax = *aabbMax;
}

void kzuTransformedObjectNodeGetContentSize(const struct KzuTransformedObjectNode* node, struct KzcVector3* aabbMin, struct KzcVector3* aabbMax)
{
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(node->uiData != KZ_NULL);

    *aabbMin = node->uiData->contentMin;
    *aabbMax = node->uiData->contentMax;
}

/* 
* TODO: Add to comment or name:
* This is AABB bounding volume for all the object and it's children after transformations. May be smaller than the space required for content, if the user has specified smaller area.
* Contains also the margins for the object. Margins are not for axes, not transformed!
*/
void kzuTransformedObjectNodeSetDesiredSize(const struct KzuTransformedObjectNode* node, const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax)
{
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(node->uiData != KZ_NULL);

    node->uiData->desiredMin = *aabbMin;
    node->uiData->desiredMax = *aabbMax;
}

void kzuTransformedObjectNodeGetDesiredSize(const struct KzuTransformedObjectNode* node, struct KzcVector3* aabbMin, struct KzcVector3* aabbMax)
{
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(node->uiData != KZ_NULL);

    *aabbMin = node->uiData->desiredMin;
    *aabbMax = node->uiData->desiredMax;
}

void kzuTransformedObjectNodeSetLayoutSize(const struct KzuTransformedObjectNode* node, const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax)
{
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(node->uiData != KZ_NULL);

    node->uiData->layoutMin = *aabbMin;
    node->uiData->layoutMax = *aabbMax;
}

void kzuTransformedObjectNodeGetLayoutSize(const struct KzuTransformedObjectNode* node, struct KzcVector3* aabbMin, struct KzcVector3* aabbMax)
{
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(node->uiData != KZ_NULL);

    *aabbMin = node->uiData->layoutMin;
    *aabbMax = node->uiData->layoutMax;
}

void kzuTransformedObjectNodeSetActualSize(const struct KzuTransformedObjectNode* node, const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax)
{
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(node->uiData != KZ_NULL);

    node->uiData->actualMin = *aabbMin;
    node->uiData->actualMax = *aabbMax;
}

void kzuTransformedObjectNodeGetActualSize(const struct KzuTransformedObjectNode* node, struct KzcVector3* aabbMin, struct KzcVector3* aabbMax)
{
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(node->uiData != KZ_NULL);

    *aabbMin = node->uiData->actualMin;
    *aabbMax = node->uiData->actualMax;
}

struct KzcVector2 kzuTransformedObjectNodeGetHorizontalMargin(const struct KzuTransformedObjectNode* transformedObjectNode)
{
    return kzuObjectNodeGetVector2PropertyDefault(kzuTransformedObjectNodeGetObjectNode(transformedObjectNode), KZU_PROPERTY_TYPE_LAYOUT_HORIZONTAL_MARGIN);
}

struct KzcVector2 kzuTransformedObjectNodeGetVerticalMargin(const struct KzuTransformedObjectNode* transformedObjectNode)
{
    return kzuObjectNodeGetVector2PropertyDefault(kzuTransformedObjectNodeGetObjectNode(transformedObjectNode), KZU_PROPERTY_TYPE_LAYOUT_VERTICAL_MARGIN);
}

struct KzcVector2 kzuTransformedObjectNodeGetDepthMargin(const struct KzuTransformedObjectNode* transformedObjectNode)
{
    return kzuObjectNodeGetVector2PropertyDefault(kzuTransformedObjectNodeGetObjectNode(transformedObjectNode), KZU_PROPERTY_TYPE_LAYOUT_DEPTH_MARGIN);
}

static void kzuTransformedObjectSubtractMargins_internal(const struct KzuTransformedObjectNode* node,
                                                         const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax,
                                                         struct KzcVector3* out_aabbMin, struct KzcVector3* out_aabbMax)
{
    struct KzcVector3 newMin, newMax;
    struct KzcVector2 horizontalMargin;
    struct KzcVector2 verticalMargin;
    struct KzcVector2 depthMargin;
    struct KzcVector3 marginCenter;

    kzsAssert(kzcIsValidPointer(node));    

    /* Get margins. */
    horizontalMargin = kzuTransformedObjectNodeGetHorizontalMargin(node);
    verticalMargin = kzuTransformedObjectNodeGetVerticalMargin(node);
    depthMargin = kzuTransformedObjectNodeGetDepthMargin(node);

    newMin = *aabbMin;
    newMax = *aabbMax;

    /* Subtract margins. */
    newMin.data[0] += horizontalMargin.data[0];
    newMax.data[0] -= horizontalMargin.data[1];

    newMin.data[1] += verticalMargin.data[0];
    newMax.data[1] -= verticalMargin.data[1];

    newMin.data[2] += depthMargin.data[0];
    newMax.data[2] -= depthMargin.data[1];

    /* Verify the margins don't generate degenerate bboxes. */
    kzcVector3Add(&newMin, &newMax, &marginCenter);
    kzcVector3Scale(&marginCenter, 0.5f, &marginCenter);
    newMin = kzcVector3ComponentMin(&newMin, &marginCenter);
    newMax = kzcVector3ComponentMax(&newMax, &marginCenter);

    *out_aabbMin = newMin;
    *out_aabbMax = newMax;
}

void kzuTransformedObjectNodeDefaultArrange(struct KzuTransformedObjectNode* node)
{
    struct KzuObjectNode* objectNode;
    enum KzuObjectNodeHorizontalAlignment horizontalAlignment;
    enum KzuObjectNodeVerticalAlignment verticalAlignment;
    enum KzuObjectNodeDepthAlignment depthAlignment;
    struct KzcVector3 allocatedMin, allocatedMax;           /* Allocated box. */
    struct KzcVector3 allocatedCoreMin, allocatedCoreMax;   /* Allocated box without margins. */
    struct KzcVector3 desiredMin, desiredMax;               /* Desired box. */
    struct KzcVector3 coreMin, coreMax;                     /* Desired box without margins. */
    struct KzcVector3 actualMin, actualMax;                 /* Actual box. */
    struct KzcVector3 offsetPosition;                       /* Offset position generated for the object. */
    struct KzcVector3 coreSize, coreCenter, halfCoreSize;
    struct KzcVector3 allocatedCoreSize, allocatedCoreCenter, halfAllocatedCoreSize;
    struct KzcVector3 clipOffset = KZC_VECTOR3_ZERO;
    struct KzuTransformedObjectNode* transformedParentNode = kzuTransformedObjectNodeGetParent(node);
    struct KzcMatrix4x4 transform = kzuTransformedObjectNodeGetMatrix(node);
    kzBool simpleLayout = kzcMatrix4x4IsSimpleLayout(&transform, 0.001f);

    node->uiData->simpleLayout = simpleLayout;

    /* Remove translation from original transform if parent is a layout. */
    if (transformedParentNode != KZ_NULL && kzuTransformedObjectNodeIsLayout(transformedParentNode))
    {
        kzcMatrix4x4ResetTranslation(&transform);
    }

    objectNode = kzuTransformedObjectNodeGetObjectNode(node);
    horizontalAlignment = kzuObjectNodeGetHorizontalAlignment(objectNode);
    verticalAlignment = kzuObjectNodeGetVerticalAlignment(objectNode);
    depthAlignment = kzuObjectNodeGetDepthAlignment(objectNode);

    if (!simpleLayout)
    {
        /* Stretch alignment not valid in this case. */
        if (horizontalAlignment == KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_STRETCH) horizontalAlignment = KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_CENTER;
        if (verticalAlignment == KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_STRETCH) verticalAlignment = KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_CENTER;
        if (depthAlignment == KZU_OBJECT_NODE_DEPTH_ALIGNMENT_STRETCH) depthAlignment = KZU_OBJECT_NODE_DEPTH_ALIGNMENT_CENTER;
    }

    /* Reduce the allocated space by margins. */
    kzuTransformedObjectNodeGetLayoutSize(node, &allocatedMin, &allocatedMax);
    kzuTransformedObjectSubtractMargins_internal(node, &allocatedMin, &allocatedMax, &allocatedCoreMin, &allocatedCoreMax);

    /* Reduce the desired size by margins. */
    kzuTransformedObjectNodeGetDesiredSize(node, &desiredMin, &desiredMax);
    kzuTransformedObjectSubtractMargins_internal(node, &desiredMin, &desiredMax, &coreMin, &coreMax);

    /* Calculate the sizes of the boxes. */
    kzcVector3Subtract(&coreMax, &coreMin, &coreSize);
    kzcVector3Scale(&coreSize, 0.5f, &halfCoreSize);
    kzcVector3Subtract(&allocatedCoreMax, &allocatedCoreMin, &allocatedCoreSize);
    kzcVector3Scale(&allocatedCoreSize, 0.5f, &halfAllocatedCoreSize);

    /* Calculate centers of the boxes. */
    kzcVector3Add(&coreMin, &coreMax, &coreCenter);
    kzcVector3Scale(&coreCenter, 0.5f, &coreCenter);
    kzcVector3Add(&allocatedCoreMin, &allocatedCoreMax, &allocatedCoreCenter);
    kzcVector3Scale(&allocatedCoreCenter, 0.5f, &allocatedCoreCenter);

    /* Start calculating the actual box. Default values are for center alignment. */
    offsetPosition = allocatedCoreCenter;
    actualMin = coreMin;
    actualMax = coreMax;

    /* Apply horizontal alignment. */
    if (horizontalAlignment == KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_LEFT)
    {
        kzcVector3SetX(&offsetPosition, kzcVector3GetX(&allocatedCoreMin) + kzcVector3GetX(&halfCoreSize));
    }
    else if (horizontalAlignment == KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_RIGHT)
    {
        kzcVector3SetX(&offsetPosition, kzcVector3GetX(&allocatedCoreMax) - kzcVector3GetX(&halfCoreSize));
    }
    else if (horizontalAlignment == KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_STRETCH)
    {
        kzcVector3SetX(&actualMin, kzcVector3GetX(&coreCenter) - kzcVector3GetX(&halfAllocatedCoreSize));
        kzcVector3SetX(&actualMax, kzcVector3GetX(&coreCenter) + kzcVector3GetX(&halfAllocatedCoreSize));
    }

    /* Apply vertical alignment. */
    if (verticalAlignment == KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_BOTTOM)
    {
        kzcVector3SetY(&offsetPosition, kzcVector3GetY(&allocatedCoreMin) + kzcVector3GetY(&halfCoreSize));
    }
    else if (verticalAlignment == KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_TOP)
    {
        kzcVector3SetY(&offsetPosition, kzcVector3GetY(&allocatedCoreMax) - kzcVector3GetY(&halfCoreSize));
    }
    else if (verticalAlignment == KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_STRETCH)
    {
        kzcVector3SetY(&actualMin, kzcVector3GetY(&coreCenter) - kzcVector3GetY(&halfAllocatedCoreSize));
        kzcVector3SetY(&actualMax, kzcVector3GetY(&coreCenter) + kzcVector3GetY(&halfAllocatedCoreSize));
    }

    /* Apply depth alignment. */
    if (depthAlignment == KZU_OBJECT_NODE_DEPTH_ALIGNMENT_BACK)
    {
        kzcVector3SetZ(&offsetPosition, kzcVector3GetZ(&allocatedCoreMin) + kzcVector3GetZ(&halfCoreSize));
    }
    else if (depthAlignment == KZU_OBJECT_NODE_DEPTH_ALIGNMENT_FRONT)
    {
        kzcVector3SetZ(&offsetPosition, kzcVector3GetZ(&allocatedCoreMax) - kzcVector3GetZ(&halfCoreSize));
    }
    else if (depthAlignment == KZU_OBJECT_NODE_DEPTH_ALIGNMENT_STRETCH)
    {
        kzcVector3SetZ(&actualMin, kzcVector3GetZ(&coreCenter) - kzcVector3GetZ(&halfAllocatedCoreSize));
        kzcVector3SetZ(&actualMax, kzcVector3GetZ(&coreCenter) + kzcVector3GetZ(&halfAllocatedCoreSize));
    }

    if (transformedParentNode != KZ_NULL)
    {
        struct KzcMatrix4x4 parentTransform = kzuTransformedObjectNodeGetMatrix(transformedParentNode);
        struct KzcMatrix4x4 offsetTransform, newTransform, finalTransform;        

        kzcMatrix4x4SetIdentity(&offsetTransform);

        /* TODO: Use some other check that is probably related to having the bounding box functiom */
        if (kzuObjectTypeIsRenderable(kzuObjectNodeGetType(objectNode)))
        {
            /* Compensate pivot offset. */
            struct KzcVector3 contentMin, contentMax;
            struct KzcVector3 pivotOffset;

            kzuTransformedObjectNodeGetContentSize(node, &contentMin, &contentMax);
            kzcVector3Add(&contentMin, &contentMax, &pivotOffset);
            kzcVector3Scale(&pivotOffset, 0.5f, &pivotOffset);

            kzcVector3Subtract(&offsetPosition, &pivotOffset, &offsetPosition);
        }

        kzcMatrix4x4TranslateVector3(&offsetTransform, &offsetPosition);

        if (kzuObjectNodeGetType(objectNode) != KZU_OBJECT_TYPE_UI_COMPONENT)
        {
            if (simpleLayout)
            {
                kzBool stretchApplied = KZ_FALSE;
                kzFloat scaleX = 1.0f;
                kzFloat scaleY = 1.0f;
                kzFloat scaleZ = 1.0f;

                /* Apply stretch scale. */
                if (horizontalAlignment == KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_STRETCH)
                {
                    scaleX = kzcVector3GetX(&allocatedCoreSize) / kzcVector3GetX(&coreSize);
                    stretchApplied = KZ_TRUE;
                }
                if (verticalAlignment == KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_STRETCH)
                {
                    scaleY = kzcVector3GetY(&allocatedCoreSize) / kzcVector3GetY(&coreSize);
                    stretchApplied = KZ_TRUE;
                }
                if (depthAlignment == KZU_OBJECT_NODE_DEPTH_ALIGNMENT_STRETCH)
                {
                    scaleZ = kzcVector3GetZ(&allocatedCoreSize) / kzcVector3GetZ(&coreSize);
                    stretchApplied = KZ_TRUE;
                }

                if (stretchApplied)
                {
                    struct KzcMatrix4x4 scaleTransform;
                    struct KzcMatrix4x4 scaleOffsetTransform;
                    kzcMatrix4x4SetIdentity(&scaleTransform);
                    kzcMatrix4x4Scale(&scaleTransform, scaleX, scaleY, scaleZ);
                    kzcMatrix4x4MultiplyAffine(&scaleTransform, &offsetTransform, &scaleOffsetTransform);
                    offsetTransform = scaleOffsetTransform;
                }
            }
        }        
        
        kzcMatrix4x4MultiplyAffine(&transform, &offsetTransform, &newTransform);
        kzcMatrix4x4MultiplyAffine(&newTransform, &parentTransform, &finalTransform);
        kzuTransformedObjectNodeSetMatrix(node, &finalTransform);

        /* TODO: Clip offset has to be recalculated, because the viewbox allows translations for its children. Should be fixed. */
        if (kzuTransformedObjectNodeIsLayout(transformedParentNode))
        {
            kzcMatrix4x4GetPosition(&offsetTransform, &clipOffset);
        }        
    }

    if (simpleLayout)
    {
        /* Backtransform the box. */
        /* Should back transform by rotation only, preserving scale!!! */
        /*kzcMatrix4x4Inverse(&transform, &backTransform);
        kzuAxisAlignedBoundingBoxFromTransformedAABB(&allocatedCoreMin, &allocatedCoreMax, &backTransform, &objectAllocatedMin, &objectAllocatedMax);*/
    }
    else
    {
        /* In the case of non-right angle rotation the layout can not be simplified.
           The object receives its desired core box as actual box and is transformed. */
        actualMin = coreMin;
        actualMax = coreMax;
    }

    kzuTransformedObjectNodeSetActualSize(node, &actualMin, &actualMax);
    kzuTransformedObjectNodeClip(node, &clipOffset);
}

void kzuTransformedObjectNodeCalculateDesiredSize(const struct KzuTransformedObjectNode* node,
                                                  const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax,
                                                  struct KzcVector3* out_aabbMin, struct KzcVector3* out_aabbMax)
{
    struct KzcVector3 minCorner, maxCorner;
    struct KzcVector3 newMin, newMax;
    struct KzcMatrix4x4 transform;
    struct KzcVector2 horizontalMargin;
    struct KzcVector2 verticalMargin;
    struct KzcVector2 depthMargin;
    struct KzuObjectNode* objectNode;

    kzsAssert(kzcIsValidPointer(node));

    objectNode = kzuTransformedObjectNodeGetObjectNode(node);
    transform = kzuObjectNodeGetTransformation(objectNode);

    /* Translation doesnt affect the calculations. */
    kzcMatrix4x4ResetTranslation(&transform);

    /* Transform the bounding box and calculate new enclosing aabb from that. */
    kzuAxisAlignedBoundingBoxFromTransformedAABB(aabbMin, aabbMax, &transform, &minCorner, &maxCorner);

    /* Get margins. We don't care if they were specified, default values are fine. */
    horizontalMargin = kzuTransformedObjectNodeGetHorizontalMargin(node);
    verticalMargin = kzuTransformedObjectNodeGetVerticalMargin(node);
    depthMargin = kzuTransformedObjectNodeGetDepthMargin(node);

    newMin = minCorner;
    newMax = maxCorner;

    /* Add margins. */
    newMin.data[0] -= horizontalMargin.data[0];
    newMax.data[0] += horizontalMargin.data[1];

    newMin.data[1] -= verticalMargin.data[0];
    newMax.data[1] += verticalMargin.data[1];

    newMin.data[2] -= depthMargin.data[0];
    newMax.data[2] += depthMargin.data[1];

    *out_aabbMin = newMin;
    *out_aabbMax = newMax;
}

void kzuTransformedObjectNodeClip(const struct KzuTransformedObjectNode* transformedNode, const struct KzcVector3* offset)
{
    kzBool clipped = KZ_FALSE;
    struct KzuTransformedObjectNode* transformedParentNode = kzuTransformedObjectNodeGetParent(transformedNode);

    if (transformedParentNode != KZ_NULL)
    {
        if (kzuTransformedObjectNodeIsClipped(transformedParentNode))
        {
            /* If the parent is clipped, then the child is clipped as well. */
            clipped = KZ_TRUE;
        }
        else if (kzuTransformedObjectNodeIsClippingEnabled(transformedParentNode))
        {
            struct KzcVector3 clipMin, clipMax;
            struct KzcVector3 childMin, childMax;                
            struct KzcVolume clipVolume, childVolume, intersectionVolume;
            /* TODO: Implement when message forwarding with arbitrary properties is implemented. */
            /*struct KzuProperty* amountProperty = kzuObjectNodeGetPropertyByName(objectNode, "AmountVisibleInParent");*/

            kzuTransformedObjectNodeGetClipBox(transformedParentNode, &clipMin, &clipMax);
            kzuTransformedObjectNodeGetLayoutSize(transformedNode, &childMin, &childMax);

            clipVolume = kzcVolumeFromVectors(&clipMin, &clipMax);
            childVolume = kzcVolumeFromVectors(&childMin, &childMax);
            kzcVolumeIntersection(&clipVolume, &childVolume, &intersectionVolume);

            if (kzcVolumeIsEmpty(&intersectionVolume))
            {
                clipped = KZ_TRUE;
            }

            /* Update the visible amount property if present. */
            /*if (amountProperty != KZ_NULL)
            {
                kzFloat amount = kzcVolumeGetVolume(&intersectionVolume) / kzcVolumeGetVolume(&childVolume);
                struct KzuFloatProperty* floatProperty = kzuFloatPropertyFromProperty(amountProperty);
                kzuFloatPropertySetValue(floatProperty, amount);
            }*/

            /* Copy the clip box. */
            kzcVector3Subtract(&clipMin, offset, &clipMin);
            kzcVector3Subtract(&clipMax, offset, &clipMax);
            kzuTransformedObjectNodeSetClipBox(transformedNode, &clipMin, &clipMax);
            kzuTransformedObjectNodeSetClippingEnabled(transformedNode, KZ_TRUE);
        }
    }

    if (!clipped && kzuTransformedObjectNodeIsClipChildrenEnabled(transformedNode))
    {
        struct KzcVector3 clipMin, clipMax;
        kzuTransformedObjectNodeGetLayoutSize(transformedNode, &clipMin, &clipMax);
        /* TODO: could be combined with previous if there is no rotation. */
        kzuTransformedObjectNodeSetClippingEnabled(transformedNode, KZ_TRUE);
        kzuTransformedObjectNodeSetClipBox(transformedNode, &clipMin, &clipMax);
    }
    
    kzuTransformedObjectNodeSetClipped(transformedNode, clipped);
}
