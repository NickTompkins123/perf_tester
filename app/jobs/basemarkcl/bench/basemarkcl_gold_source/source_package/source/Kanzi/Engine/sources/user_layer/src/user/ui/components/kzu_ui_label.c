/**
* \file
* User interface label.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_label.h"

#include <user/ui/kzu_ui_component.h>
#include <user/ui/kzu_ui_component_type.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_text_object.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_void_property.h>
#include <user/properties/kzu_string_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/bounding_volume/kzu_bounding_volume.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>

#include <system/debug/kzs_log.h>


const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_LABEL = (void*)&KZU_COMPONENT_TYPE_LABEL;


KZ_CALLBACK static kzsError kzuUiLabelMeasure_internal(struct KzuTransformedObjectNode* transformedObjectNode)
{
    kzsError result;
    struct KzuObjectNode* objectNode = kzuTransformedObjectNodeGetObjectNode(transformedObjectNode);    
    struct KzuTextObjectNode* textObjectNode;

    textObjectNode = kzuObjectNodeGetVoidPropertyDefault(objectNode, KZU_PROPERTY_TYPE_TEXT_LABEL_OBJECT);

    result = kzuTextObjectNodeValidate(textObjectNode, transformedObjectNode);
    kzsErrorForward(result);

    {
        struct KzcVector3 contentMin, contentMax;
        struct KzcVector3 desiredMin, desiredMax;
        struct KzcVector3 aabbMin;
        struct KzcVector3 aabbMax;

        struct KzuBoundingVolume* volume = kzuObjectNodeGetBoundingVolume(kzuTextObjectNodeToObjectNode(textObjectNode));
        struct KzuBoundingVolumeAxisAlignedBox* boundingBox = kzuBoundingVolumeGetAxisAlignedBox(volume);
        aabbMin = kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner(boundingBox);
        aabbMax = kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner(boundingBox);

        kzuObjectNodeCalculateUserRestrictedSize(objectNode, &aabbMin, &aabbMax, &contentMin, &contentMax);
        kzuTransformedObjectNodeSetContentSize(transformedObjectNode, &contentMin, &contentMax);

        kzuTransformedObjectNodeCalculateDesiredSize(transformedObjectNode, &contentMin, &contentMax, &desiredMin, &desiredMax);
        kzuTransformedObjectNodeSetDesiredSize(transformedObjectNode, &desiredMin, &desiredMax);

        kzuTransformedObjectNodeSetContentSize(kzuTransformedObjectNodeGetChildAtIndex(transformedObjectNode, 0), &contentMin, &contentMax);
        kzuTransformedObjectNodeSetDesiredSize(kzuTransformedObjectNodeGetChildAtIndex(transformedObjectNode, 0), &desiredMin, &desiredMax);
    }

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuUiLabelArrange_internal(const struct KzuScene* scene, struct KzuTransformedObjectNode* transformedNode,
                                                       struct KzuTransformedObjectNode* transformedParentNode)
{
    kzsError result;
    struct KzcDynamicArrayIterator it = kzuTransformedObjectNodeGetChildren(transformedNode);
    kzuTransformedObjectNodeDefaultArrange(transformedNode);
     
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuTransformedObjectNode* transformedChildNode = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);                

        result = kzuSceneArrangeNode(scene, transformedChildNode, transformedNode);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuUiLabelSpecificCopy_internal(const struct KzuObjectNode* sourceNode, struct KzuObjectNode* targetNode)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(targetNode);
    struct KzuTextObjectNode* textObjectNode;

    textObjectNode = kzuObjectNodeGetVoidPropertyDefault(sourceNode, KZU_PROPERTY_TYPE_TEXT_LABEL_OBJECT);
    if(kzcIsValidPointer(textObjectNode))
    {
        struct KzuObjectNode* targetTextObjectNode;

        result = kzuObjectNodeCopyWithChildren(memoryManager, kzuTextObjectNodeToObjectNode(textObjectNode), &targetTextObjectNode);
        kzsErrorForward(result);

        result = kzuObjectNodeSetVoidProperty(targetNode, KZU_PROPERTY_TYPE_TEXT_LABEL_OBJECT, targetTextObjectNode);
        kzsErrorForward(result);

        result = kzuObjectNodeAddChild(targetNode, targetTextObjectNode);
        kzsErrorForward(result);

        result = kzuObjectNodeSetChildFromKzb(targetNode, targetTextObjectNode);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiLabelInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode)
{
    kzsError result;
    struct KzuObjectNode* objectNode = kzuUiComponentNodeToObjectNode(componentNode);

    kzuUiComponentTypeSetTypeIdentifier(kzuUiComponentNodeGetType(componentNode), KZU_COMPONENT_TYPE_LABEL);

    /* Create text object. */
    {
        kzMutableString textObjectName;
        struct KzuTextObjectNode* textObjectNode;
        struct KzuObjectNode* childNode;

        result = kzcStringConcatenate(memoryManager, kzuObjectNodeGetName(objectNode), "_TextObject", &textObjectName);
        kzsErrorForward(result);

        result = kzuTextObjectNodeCreate(memoryManager, kzuObjectNodeGetPropertyManager(objectNode), kzuObjectNodeGetMessageDispatcher(objectNode), textObjectName, &textObjectNode);
        kzsErrorForward(result);

        result = kzuObjectNodeSetVoidProperty(objectNode, KZU_PROPERTY_TYPE_TEXT_LABEL_OBJECT, textObjectNode);
        kzsErrorForward(result);

        result = kzcStringDelete(textObjectName);
        kzsErrorForward(result);

        childNode = kzuTextObjectNodeToObjectNode(textObjectNode);

        result = kzuObjectNodeAddChild(objectNode, childNode);
        kzsErrorForward(result);

        result = kzuObjectNodeSetChildFromKzb(objectNode, childNode);
        kzsErrorForward(result);
    }

    kzuUiComponentNodeSetMeasureFunction(componentNode, kzuUiLabelMeasure_internal);
    kzuUiComponentNodeSetArrangeFunction(componentNode, kzuUiLabelArrange_internal);
    kzuUiComponentNodeSetSpecificCopyFunction(componentNode, kzuUiLabelSpecificCopy_internal);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiLabelUninitialize(struct KzuUiComponentNode* componentNode)
{
    kzsError result;
    struct KzuObjectNode* objectNode = kzuUiComponentNodeToObjectNode(componentNode);
    struct KzuTextObjectNode* textObjectNode = kzuObjectNodeGetVoidPropertyDefault(objectNode, KZU_PROPERTY_TYPE_TEXT_LABEL_OBJECT);

    if (textObjectNode != KZ_NULL)
    {
        result = kzuObjectNodeRemoveChild(objectNode, kzuTextObjectNodeToObjectNode(textObjectNode));
        kzsErrorForward(result);

        result = kzuObjectNodeDelete(kzuTextObjectNodeToObjectNode(textObjectNode));
        kzsErrorForward(result);
    }

    kzsSuccess();
}
