/**
* \file
* User interface stack layout component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_stack_layout.h"
#include "kzu_ui_component_common.h"

#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/kzu_ui_component.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_mesh.h>
#include <user/scene_graph/bounding_volume/kzu_bounding_volume.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector3.h>


const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_STACK_LAYOUT = (void*)&KZU_COMPONENT_TYPE_STACK_LAYOUT;


enum KzuStackLayoutDirection kzuUiStackLayoutGetDirection(const struct KzuObjectNode* objectNode)
{   
    return (enum KzuStackLayoutDirection)kzuObjectNodeGetIntPropertyDefault(objectNode, KZU_PROPERTY_TYPE_STACK_LAYOUT_DIRECTION);/*lint !e930*/
}

kzBool kzuUiStackLayoutIsReversed(const struct KzuObjectNode* objectNode)
{
    return kzuObjectNodeGetBoolPropertyDefault(objectNode, KZU_PROPERTY_TYPE_STACK_LAYOUT_REVERSED);
}

KZ_CALLBACK static kzsError kzuUiStackLayoutMeasure(struct KzuTransformedObjectNode* transformedNode)
{
    struct KzcVector3 accumulatedSize = KZC_VECTOR3_ZERO;
    struct KzcVector3 maxSize = KZC_VECTOR3_ZERO;
    struct KzcVector3 contentSize;
    struct KzuObjectNode* objectNode = kzuTransformedObjectNodeGetObjectNode(transformedNode);
    enum KzuStackLayoutDirection direction = kzuUiStackLayoutGetDirection(objectNode);
    struct KzcDynamicArrayIterator it = kzuTransformedObjectNodeGetChildren(transformedNode);

    /* Calculate max extents and accumulated extents. */
    while (kzcDynamicArrayIterate(it))
    {
        struct KzcVector3 childMin, childMax;
        struct KzcVector3 childSize;
        struct KzcVector3 newAccumulatedSize;
        struct KzuTransformedObjectNode* transformedChildNode = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);
        
        kzuTransformedObjectNodeGetDesiredSize(transformedChildNode, &childMin, &childMax);
        kzcVector3Subtract(&childMax, &childMin, &childSize);
        
        /* TODO: No need to use temporary variable for result. */
        kzcVector3Add(&accumulatedSize, &childSize, &newAccumulatedSize);
        accumulatedSize = newAccumulatedSize;

        maxSize = kzcVector3ComponentMax(&maxSize, &childSize);
    }

    contentSize = maxSize;

    if (direction == KZU_STACK_LAYOUT_DIRECTION_X)
    {
        /* TODO: Do not make function calls inside function calls. */
        kzcVector3SetX(&contentSize, kzcVector3GetX(&accumulatedSize));
    }
    else if (direction == KZU_STACK_LAYOUT_DIRECTION_Y)
    {
        kzcVector3SetY(&contentSize, kzcVector3GetY(&accumulatedSize));
    }
    else if (direction == KZU_STACK_LAYOUT_DIRECTION_Z)
    {
        kzcVector3SetZ(&contentSize, kzcVector3GetZ(&accumulatedSize));
    }

    /* Calculate bounding boxes. */
    {
        struct KzcVector3 halfSize;
        struct KzcVector3 aabbMin, aabbMax;
        struct KzcVector3 contentMin, contentMax;
        struct KzcVector3 desiredMin, desiredMax;

        kzcVector3Scale(&contentSize, 0.5f, &halfSize);
        kzcVector3Add(&KZC_VECTOR3_ZERO, &halfSize, &aabbMax);
        kzcVector3Subtract(&KZC_VECTOR3_ZERO, &halfSize, &aabbMin);

        /* Final content size is content size restricted by user defined dimensions. */
        kzuObjectNodeCalculateUserRestrictedSize(objectNode, &aabbMin, &aabbMax, &contentMin, &contentMax);
        kzuTransformedObjectNodeSetContentSize(transformedNode, &contentMin, &contentMax);

        /* Calculate desired size. */
        kzuTransformedObjectNodeCalculateDesiredSize(transformedNode, &contentMin, &contentMax, &desiredMin, &desiredMax);
        kzuTransformedObjectNodeSetDesiredSize(transformedNode, &desiredMin, &desiredMax);
    }

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuUiStackLayoutArrange(const struct KzuScene* scene, struct KzuTransformedObjectNode* transformedNode,
                                                    struct KzuTransformedObjectNode* transformedParentNode)
{
    kzsError result;
    struct KzcVector3 actualMin, actualMax;
    struct KzcVector3 offset;
    enum KzuStackLayoutDirection direction = kzuUiStackLayoutGetDirection(kzuTransformedObjectNodeGetObjectNode(transformedNode));
    kzBool reversed = kzuUiStackLayoutIsReversed(kzuTransformedObjectNodeGetObjectNode(transformedNode));
    /* TODO: move init where it is used. */
    struct KzcDynamicArrayIterator it = kzuTransformedObjectNodeGetChildren(transformedNode);

    kzuTransformedObjectNodeDefaultArrange(transformedNode);
    
    kzuTransformedObjectNodeGetContentSize(transformedNode, &actualMin, &actualMax);

    if (reversed == KZ_FALSE)
    {
        offset = actualMin;
    }
    else
    {
        offset = actualMax;
    }    

    while (kzcDynamicArrayIterate(it))
    {
        struct KzuTransformedObjectNode* transformedChildNode = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);                
        struct KzcVector3 childLayoutMin, childLayoutMax;
        struct KzcVector3 childDesiredMin, childDesiredMax;
        struct KzcVector3 childDesiredSize;
        
        kzuTransformedObjectNodeGetDesiredSize(transformedChildNode, &childDesiredMin, &childDesiredMax);
        kzcVector3Subtract(&childDesiredMax, &childDesiredMin, &childDesiredSize);

        /* Arrange objects along the direction axis, but preserve restrictions in other directions. */
        childLayoutMin = offset;
        childLayoutMax = offset;
        if (direction == KZU_STACK_LAYOUT_DIRECTION_X)
        {
            kzFloat step = kzcVector3GetX(&childDesiredSize);
            
            if (reversed == KZ_FALSE)
            {
                /* TODO: x + step as its own variable. */                
                kzcVector3SetX(&offset, kzcVector3GetX(&offset) + step);
                childLayoutMax = kzcVector3(kzcVector3GetX(&offset), kzcVector3GetY(&actualMax), kzcVector3GetZ(&actualMax));
            }
            else
            {
                kzcVector3SetX(&offset, kzcVector3GetX(&offset) - step);
                childLayoutMin = kzcVector3(kzcVector3GetX(&offset), kzcVector3GetY(&actualMin), kzcVector3GetZ(&actualMin));
            }
        }
        else if (direction == KZU_STACK_LAYOUT_DIRECTION_Y)
        {
            kzFloat step = kzcVector3GetY(&childDesiredSize);

            if (reversed == KZ_FALSE)
            {
                kzcVector3SetY(&offset, kzcVector3GetY(&offset) + step);
                childLayoutMax = kzcVector3(kzcVector3GetX(&actualMax), kzcVector3GetY(&offset), kzcVector3GetZ(&actualMax));
            }
            else
            {
                kzcVector3SetY(&offset, kzcVector3GetY(&offset) - step);
                childLayoutMin = kzcVector3(kzcVector3GetX(&actualMin), kzcVector3GetY(&offset), kzcVector3GetZ(&actualMin));
            }
        }
        else if (direction == KZU_STACK_LAYOUT_DIRECTION_Z)
        {
            kzFloat step = kzcVector3GetZ(&childDesiredSize);

            if (reversed == KZ_FALSE)
            {
                kzcVector3SetZ(&offset, kzcVector3GetZ(&offset) + step);
                childLayoutMax = kzcVector3(kzcVector3GetX(&actualMax), kzcVector3GetY(&actualMax), kzcVector3GetZ(&offset));
            }
            else
            {
                kzcVector3SetZ(&offset, kzcVector3GetZ(&offset) - step);
                childLayoutMin = kzcVector3(kzcVector3GetX(&actualMin), kzcVector3GetY(&actualMin), kzcVector3GetZ(&offset));
            }
        }

        kzuTransformedObjectNodeSetLayoutSize(transformedChildNode, &childLayoutMin, &childLayoutMax);

        result = kzuSceneArrangeNode(scene, transformedChildNode, transformedNode);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiStackLayoutInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode)
{
    kzuUiComponentTypeSetTypeIdentifier(kzuUiComponentNodeGetType(componentNode), KZU_COMPONENT_TYPE_STACK_LAYOUT);
    kzuUiComponentNodeSetMeasureFunction(componentNode, kzuUiStackLayoutMeasure);
    kzuUiComponentNodeSetArrangeFunction(componentNode, kzuUiStackLayoutArrange);

    kzsSuccess();
}

