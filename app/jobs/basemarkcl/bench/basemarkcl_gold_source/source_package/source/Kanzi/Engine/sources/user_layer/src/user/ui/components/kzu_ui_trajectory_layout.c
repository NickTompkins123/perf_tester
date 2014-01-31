/**
* \file
* User interface trajectory layout component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_trajectory_layout.h"
#include "kzu_ui_component_common.h"

#include <user/ui/kzu_ui_component.h>
#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/kzu_ui_component_base.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/bounding_volume/kzu_bounding_volume.h>
#include <user/properties/kzu_void_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/curve/kzc_trajectory.h>
#include <core/util/string/kzc_string.h>
#include <core/debug/kzc_log.h>


const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_TRAJECTORY_LAYOUT = (void*)&KZU_COMPONENT_TYPE_TRAJECTORY_LAYOUT;


static kzsError kzuUiTrajectoryLayoutGetObjectPosition_internal(const struct KzuObjectNode* parentNode, kzUint childIndex, 
                                                                kzUint childCount, struct KzcVector3* out_position)
{
    kzsError result;
    struct KzcVector3 position = kzcVector3(0.0f, 0.0f, 0.0f);
    struct KzcTrajectory* trajectory = kzuObjectNodeGetVoidPropertyDefault(parentNode, KZU_PROPERTY_TYPE_TRAJECTORY);

    if (trajectory != KZ_NULL)
    {
        kzFloat offset = kzuObjectNodeGetFloatPropertyDefault(parentNode, KZU_PROPERTY_TYPE_TRAJECTORY_LAYOUT_OFFSET);
        kzFloat startOffset = kzuObjectNodeGetFloatPropertyDefault(parentNode, KZU_PROPERTY_TYPE_TRAJECTORY_LAYOUT_START_VALUE);
        kzFloat endOffset = kzuObjectNodeGetFloatPropertyDefault(parentNode, KZU_PROPERTY_TYPE_TRAJECTORY_LAYOUT_END_VALUE);
        
        startOffset = kzsClampf(startOffset, 0.0f, 1.0f);
        endOffset = kzsMaxf(startOffset, endOffset);
        endOffset = kzsClampf(endOffset, 0.0f, 1.0f);

        if(kzcTrajectoryIsComplete(trajectory))
        {
            kzFloat offsetPosition = offset + childIndex / (kzFloat)childCount;
            kzFloat integerPart;
            offsetPosition = kzsModf(offsetPosition, &integerPart);
            offsetPosition = startOffset + offsetPosition * (endOffset - startOffset);
            result = kzcTrajectoryGetPoint(trajectory, offsetPosition, &position);
            kzsErrorForward(result);
        }
    }

    *out_position = position;
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuUiTrajectoryLayoutArrange(const struct KzuScene* scene, struct KzuTransformedObjectNode* transformedNode,
                                                         struct KzuTransformedObjectNode* transformedParentNode)
{
    kzsError result;
    struct KzcVector3 contentMin, contentMax;
    struct KzuObjectNode* node;

    kzsAssert(kzcIsValidPointer(transformedNode));

    node = kzuTransformedObjectNodeGetObjectNode(transformedNode);

    kzuTransformedObjectNodeDefaultArrange(transformedNode);
    /* Content size restricted by user defined properties is used to line up objects. */
    kzuTransformedObjectNodeGetContentSize(transformedNode, &contentMin, &contentMax);

    {
        struct KzcTrajectory* trajectory = kzuObjectNodeGetVoidPropertyDefault(node, KZU_PROPERTY_TYPE_TRAJECTORY);
        if(trajectory != KZ_NULL)
        {
            if(kzcTrajectoryIsComplete(trajectory))
            {
                kzUint i = 0;
                kzUint childCount = kzuTransformedObjectNodeGetChildCount(transformedNode);
                struct KzcDynamicArrayIterator it = kzuTransformedObjectNodeGetChildren(transformedNode);
                while (kzcDynamicArrayIterate(it))
                {
                    struct KzuTransformedObjectNode* transformedChildNode = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);
                    struct KzcVector3 position;

                    result = kzuUiTrajectoryLayoutGetObjectPosition_internal(node, i, childCount, &position);
                    kzsErrorForward(result);

                    {
                        struct KzcVector3 childDesiredMin, childDesiredMax;
                        kzuTransformedObjectNodeGetDesiredSize(transformedChildNode, &childDesiredMin, &childDesiredMax);
                        kzcVector3Add(&childDesiredMin, &position, &childDesiredMin);
                        kzcVector3Add(&childDesiredMax, &position, &childDesiredMax);
                        kzuTransformedObjectNodeSetLayoutSize(transformedChildNode, &childDesiredMin, &childDesiredMax);
                    }
                    
                    result = kzuSceneArrangeNode(scene, transformedChildNode, KZ_NULL);
                    kzsErrorForward(result);
                    ++i;
                }
            }
        }
    }
    

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuUiTrajectoryLayoutMeasure(struct KzuTransformedObjectNode* transformedNode)
{
    kzsError result;
    kzUint i = 0;
    struct KzcVector3 maxSize = kzcVector3(0.0f, 0.0f, 0.0f);
    struct KzcVector3 minSize = kzcVector3(0.0f, 0.0f, 0.0f);
    struct KzcVector3 contentSize;
    struct KzuObjectNode* objectNode = kzuTransformedObjectNodeGetObjectNode(transformedNode);
    struct KzcDynamicArrayIterator it = kzuTransformedObjectNodeGetChildren(transformedNode);
    kzUint childCount = kzuTransformedObjectNodeGetChildCount(transformedNode);

    if(childCount > 0)
    {
        minSize = KZC_VECTOR3_MAXIMUM;
        maxSize = KZC_VECTOR3_MINIMUM;

        /* Calculate max extents and accumulated extents. */
        while (kzcDynamicArrayIterate(it))
        {
            struct KzcVector3 childExtendsMin, childExtendsMax;
            struct KzcVector3 childMin, childMax;
            struct KzcVector3 position;
            struct KzuTransformedObjectNode* transformedChildNode = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);

            kzuTransformedObjectNodeGetDesiredSize(transformedChildNode, &childExtendsMin, &childExtendsMax);

            result = kzuUiTrajectoryLayoutGetObjectPosition_internal(objectNode, i, childCount, &position);
            kzsErrorForward(result);

            kzcVector3Add(&position, &childExtendsMin, &childMin);
            kzcVector3Add(&position, &childExtendsMax, &childMax);

            maxSize = kzcVector3ComponentMax(&maxSize, &childMax);
            minSize = kzcVector3ComponentMin(&minSize, &childMin);

            ++i;
        }
    }

    kzcVector3Subtract(&maxSize, &minSize, &contentSize);

    /* Calculate bounding boxes. */
    {
        struct KzcVector3 halfSize;
        struct KzcVector3 aabbMin, aabbMax;
        struct KzcVector3 contentMin, contentMax;
        struct KzcVector3 desiredMin, desiredMax;

        kzcVector3Scale(&contentSize, 0.5f, &halfSize);
        kzcVector3Add(&KZC_VECTOR3_ZERO, &halfSize, &aabbMax);
        kzcVector3Subtract(&KZC_VECTOR3_ZERO, &halfSize, &aabbMin);

        kzuObjectNodeCalculateUserRestrictedSize(objectNode, &aabbMin, &aabbMax, &contentMin, &contentMax);
        kzuTransformedObjectNodeSetContentSize(transformedNode, &contentMin, &contentMax);

        kzuTransformedObjectNodeCalculateDesiredSize(transformedNode, &contentMin, &contentMax, &desiredMin, &desiredMax);
        kzuTransformedObjectNodeSetDesiredSize(transformedNode, &desiredMin, &desiredMax);
    }

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiTrajectoryLayoutInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode)
{
    kzuUiComponentTypeSetTypeIdentifier(kzuUiComponentNodeGetType(componentNode), KZU_COMPONENT_TYPE_TRAJECTORY_LAYOUT);
    
    kzuUiComponentNodeSetMeasureFunction(componentNode, kzuUiTrajectoryLayoutMeasure);
    kzuUiComponentNodeSetArrangeFunction(componentNode, kzuUiTrajectoryLayoutArrange);

    kzsSuccess();
}

kzsError kzuUiTrajectoryLayoutSetOffset(const struct KzuUiComponentNode* componentNode, kzFloat offset)
{
    kzsError result;
    struct KzuObjectNode* node = kzuUiComponentNodeToObjectNode(componentNode);

    result = kzuObjectNodeSetFloatProperty(node, KZU_PROPERTY_TYPE_TRAJECTORY_LAYOUT_OFFSET, offset);
    kzsErrorForward(result);

    kzsSuccess();
}

kzFloat kzuUiTrajectoryLayoutGetOffset(const struct KzuUiComponentNode* componentNode)
{
    struct KzuObjectNode* node = kzuUiComponentNodeToObjectNode(componentNode);

    return kzuObjectNodeGetFloatPropertyDefault(node, KZU_PROPERTY_TYPE_TRAJECTORY_LAYOUT_OFFSET);
}

struct KzcTrajectory* kzuUiTrajectoryGetTrajectory(const struct KzuUiComponentNode* componentNode)
{
    struct KzuObjectNode* node = kzuUiComponentNodeToObjectNode(componentNode);

    return kzuObjectNodeGetVoidPropertyDefault(node, KZU_PROPERTY_TYPE_TRAJECTORY);
}
