/**
* \file
* User interface viewbox component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_viewbox.h"
#include "kzu_ui_component_common.h"

#include <user/scene_graph/kzu_transformed_object.h>
#include <user/ui/kzu_ui_component_base.h>
#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/kzu_ui_component.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_scene.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>
#include <core/util/math/kzc_vector3.h>


const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_VIEWBOX = (void*)&KZU_COMPONENT_TYPE_VIEWBOX;


KZ_CALLBACK static kzsError kzuUiViewBoxMeasure(struct KzuTransformedObjectNode* transformedNode)
{   
    struct KzuObjectNode* objectNode = kzuTransformedObjectNodeGetObjectNode(transformedNode);
    
    /* Calculate bounding boxes. */
    {
        struct KzcVector3 contentMin, contentMax;
        struct KzcVector3 desiredMin, desiredMax;
        struct KzcVector3 aabbMin = KZC_VECTOR3_ZERO;
        struct KzcVector3 aabbMax = KZC_VECTOR3_ZERO;

        /* Final content size is content size restricted by user defined dimensions. */
        kzuObjectNodeCalculateUserRestrictedSize(objectNode, &aabbMin, &aabbMax, &contentMin, &contentMax);
        kzuTransformedObjectNodeSetContentSize(transformedNode, &contentMin, &contentMax);

        /* Calculate desired size. */
        kzuTransformedObjectNodeCalculateDesiredSize(transformedNode, &contentMin, &contentMax, &desiredMin, &desiredMax);
        kzuTransformedObjectNodeSetDesiredSize(transformedNode, &desiredMin, &desiredMax);
    }

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuUiViewBoxArrange(const struct KzuScene* scene, struct KzuTransformedObjectNode* transformedNode, struct KzuTransformedObjectNode* transformedParentNode)
{
    kzsError result;
    struct KzcDynamicArrayIterator it = kzuTransformedObjectNodeGetChildren(transformedNode);

    kzuTransformedObjectNodeDefaultArrange(transformedNode);

    while (kzcDynamicArrayIterate(it))
    {
        struct KzuTransformedObjectNode* transformedChildNode = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);                
        struct KzcVector3 childDesiredMin, childDesiredMax;
        struct KzcVector3 offset;
        struct KzuObjectNode* childObjectNode = kzuTransformedObjectNodeGetObjectNode(transformedChildNode);
        struct KzcMatrix4x4 childOriginalTransform = kzuObjectNodeGetTransformation(childObjectNode);

        /* Viewbox behaves like a canvas layout. Allocate space according to the translation of the object inside the viewbox. */
        kzuTransformedObjectNodeGetDesiredSize(transformedChildNode, &childDesiredMin, &childDesiredMax);
        kzcMatrix4x4GetPosition(&childOriginalTransform, &offset);
        kzcVector3Add(&childDesiredMin, &offset, &childDesiredMin);
        kzcVector3Add(&childDesiredMax, &offset, &childDesiredMax);
        kzuTransformedObjectNodeSetLayoutSize(transformedChildNode, &childDesiredMin, &childDesiredMax);

        result = kzuSceneArrangeNode(scene, transformedChildNode, transformedNode);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiViewBoxInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode)
{
    kzuUiComponentTypeSetTypeIdentifier(kzuUiComponentNodeGetType(componentNode), KZU_COMPONENT_TYPE_VIEWBOX);

    kzuUiComponentNodeSetMeasureFunction(componentNode, kzuUiViewBoxMeasure);
    kzuUiComponentNodeSetArrangeFunction(componentNode, kzuUiViewBoxArrange);
    
    kzsSuccess();
}
