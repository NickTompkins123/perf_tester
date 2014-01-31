/**
* \file
* User interface slider.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_slider.h"
#include "kzu_ui_component_common.h"

#include <user/ui/kzu_ui_component_base.h>
#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/kzu_ui_component.h>
#include <user/ui/kzu_ui_event.h>
#include <user/ui/kzu_ui_manager.h>
#include <user/scene_graph/kzu_object.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/engine/kzu_engine_message_queue.h>
#include <user/scene_graph/kzu_mesh.h>
#include <user/scene_graph/bounding_volume/kzu_bounding_volume.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_ray.h>
#include <core/util/math/kzc_plane.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/debug/kzc_log.h>


/** Index of rail child. */
#define SLIDER_RAIL_CHILD_INDEX 0
/** Index of knob child. */
#define SLIDER_KNOB_CHILD_INDEX 1


const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_SLIDER = (void*)&KZU_COMPONENT_TYPE_SLIDER;


const kzString KZU_SLIDER_VALUE_CHANGED = "SliderValueChanged";
const kzString KZU_SLIDER_PRESSED = "SliderPressedEvent";
const kzString KZU_SLIDER_RELEASED = "SliderReleasedEvent";


KZ_CALLBACK kzsError kzuUiSliderInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode)
{
    kzuUiComponentTypeSetTypeIdentifier(kzuUiComponentNodeGetType(componentNode), KZU_COMPONENT_TYPE_SLIDER);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiSliderLogicUpdate(const struct KzuUiComponentNode* componentNode, kzFloat deltaTime, struct KzuEngineMessageQueue* queue)
{
    kzsError result;
    struct KzuObjectNode* node = kzuUiComponentNodeToObjectNode(componentNode);

    /* TODO: If the value of slider has changed trigger value changed action. */

    if(kzuObjectNodeGetChildCount(node) > 1)
    {
        struct KzuObjectNode* knobParent = kzuObjectNodeGetChildAtIndex(node, SLIDER_KNOB_CHILD_INDEX);
        struct KzuObjectNode* railParent = kzuObjectNodeGetChildAtIndex(node, SLIDER_RAIL_CHILD_INDEX);
        struct KzuObjectNode* railNode = railParent == KZ_NULL ? KZ_NULL : kzuObjectNodeGetChildAtIndex(railParent, 0);
        struct KzuObjectNode* knobNode = knobParent == KZ_NULL ? KZ_NULL : kzuObjectNodeGetChildAtIndex(knobParent, 0);
        
        if(railNode != KZ_NULL && knobNode != KZ_NULL)
        {
            kzUint i;
            kzInt direction = kzuObjectNodeGetIntPropertyDefault(node, KZU_PROPERTY_TYPE_SLIDER_DIRECTION);
            struct KzcVector3 size = kzuObjectNodeGetSize(railNode);
            struct KzcMatrix4x4 railTransformation = kzuObjectNodeGetTransformation(railNode);
            struct KzcVector3 minimumBound = kzuObjectNodeGetMinimumBoundingBoxCoordinates(railNode);
            struct KzcVector3 maximumBound = kzuObjectNodeGetMaximumBoundingBoxCoordinates(railNode);
            kzFloat minimumValue = kzuObjectNodeGetFloatPropertyDefault(node, KZU_PROPERTY_TYPE_SLIDER_MINIMUM_VALUE);
            kzFloat maximumValue = kzuObjectNodeGetFloatPropertyDefault(node, KZU_PROPERTY_TYPE_SLIDER_MAXIMUM_VALUE);
            kzBool invertDirection = kzuObjectNodeGetBoolPropertyDefault(node, KZU_PROPERTY_TYPE_SLIDER_INVERT_DIRECTION);

            if(maximumValue < minimumValue)
            {
                kzFloat temp = maximumValue;
                maximumValue = minimumValue;
                minimumValue = temp;
            }

            {
                kzFloat value = kzuObjectNodeGetFloatPropertyDefault(node, KZU_PROPERTY_TYPE_SLIDER_VALUE);
                kzFloat delta = maximumValue - minimumValue;
                kzFloat interpolationValue = (value - minimumValue) / delta;
                kzInt side = kzsClampi(direction, 0, 2);
                kzFloat longSide = size.data[side];
                kzFloat minimum = minimumBound.data[side];
                kzFloat maximum = maximumBound.data[side];

                for(i = 0; i < 3; ++i)
                {
                    size.data[i] = 0.0f;
                }

                size.data[side] = longSide;

                if(invertDirection)
                {
                    railTransformation.data[KZC_MATRIX4X4_INDEX_TRANSLATION_X + side] = railTransformation.data[KZC_MATRIX4X4_INDEX_TRANSLATION_X] + maximum - longSide * interpolationValue;
                }
                else
                {
                    railTransformation.data[KZC_MATRIX4X4_INDEX_TRANSLATION_X + side] = railTransformation.data[KZC_MATRIX4X4_INDEX_TRANSLATION_X] + minimum + longSide * interpolationValue;
                }

                result = kzuObjectNodeSetTransformation(knobNode, &railTransformation);
                kzsErrorForward(result); 
            }
        }
    }

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackIncreaseSliderValue(const struct KzuUiComponentNode* componentNode, void* userData,
                                                      struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_INCREASE_SLIDER_VALUE, parameters);
    kzsErrorForward(result); 

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackDecreaseSliderValue(const struct KzuUiComponentNode* componentNode, void* userData,
                                                      struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_DECREASE_SLIDER_VALUE, parameters);
    kzsErrorForward(result); 

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackSetSliderValue(const struct KzuUiComponentNode* componentNode, void* userData,
                                                 struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_SET_SLIDER_VALUE, parameters);
    kzsErrorForward(result); 

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiSliderSetValueAction(const struct KzuUiComponentNode* componentNode, kzFloat value, void* userData,
                                   struct KzuUiEvent* event, struct KzuEngineMessageQueue* queue)
{
    kzsError result;
    struct KzuObjectNode* node = kzuUiComponentNodeToObjectNode(componentNode);
    kzFloat minimumValue = kzuObjectNodeGetFloatPropertyDefault(node, KZU_PROPERTY_TYPE_SLIDER_MINIMUM_VALUE);
    kzFloat maximumValue = kzuObjectNodeGetFloatPropertyDefault(node, KZU_PROPERTY_TYPE_SLIDER_MAXIMUM_VALUE);
    kzBool invertDirection = kzuObjectNodeGetBoolPropertyDefault(node, KZU_PROPERTY_TYPE_SLIDER_INVERT_DIRECTION);

    if(maximumValue < minimumValue)
    {
        kzFloat temp = maximumValue;
        maximumValue = minimumValue;
        minimumValue = temp;
    }
    
    {
        kzFloat delta = maximumValue - minimumValue;
        kzFloat newValue = kzsClampf(minimumValue + value * delta, minimumValue, maximumValue);
        if(invertDirection)
        {
            newValue = kzsClampf(maximumValue - value * delta, minimumValue, maximumValue);
        }

        result = kzuObjectNodeSetFloatProperty(node, KZU_PROPERTY_TYPE_SLIDER_VALUE, newValue);
        kzsErrorForward(result);
        result = kzuUiSliderLogicUpdate(componentNode, 0.0f, queue);
        kzsErrorForward(result);

        {
            struct KzuFloatProperty* newItemProperty;

            result = kzuFloatPropertyCreate(kzcMemoryGetManager(event), kzuFloatPropertyTypeFromPropertyType((struct KzuPropertyType*)KZU_PROPERTY_TYPE_FLOAT_VALUE), newValue, &newItemProperty);
            kzsErrorForward(result);
            result = kzuUiEventAddProperty(event, kzuFloatPropertyToProperty(newItemProperty));
            kzsErrorForward(result);
        }
    }

    result = kzuUiComponentNodeHandleEvents(componentNode, event, KZU_SLIDER_VALUE_CHANGED, queue);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiSliderHandleEvents(const struct KzuUiComponentNode* componentNode, kzString eventType, 
                                             struct KzuUiEvent* event, struct KzcHashMap* customParameters,
                                             struct KzuEngineMessageQueue* queue)
{  
     kzsError result;
     struct KzuObjectNode* objectNode = kzuUiComponentNodeToObjectNode(componentNode);
     struct KzuObjectNode* railParent = kzuObjectNodeGetChildAtIndex(objectNode, SLIDER_RAIL_CHILD_INDEX);
     struct KzuObjectNode* railNode = railParent == KZ_NULL ? KZ_NULL : kzuObjectNodeGetChildAtIndex(railParent, 0);
     kzBool buttonHit = ((kzuUiEventGetPickedObjectNode(event) == railNode) ||
                         (kzuUiEventGetDragStartPickedObjectNode(event) == railNode)) && railNode!= KZ_NULL; 

     if(kzuObjectNodeGetChildCount(objectNode) > 1)
     {
         kzBool pressed = kzuObjectNodeGetBoolPropertyDefault(objectNode, KZU_PROPERTY_TYPE_COMPONENT_PRESSED);

         if (kzuUiEventGetPickedObjectNode(event) == railNode)
         {
             if (kzcStringIsEqual(eventType, "PointerPressedEvent") && kzuUiEventIsPointerPressed(event))
             {
                 if(!pressed)
                 {
                     result = kzuObjectNodeSetBoolProperty(objectNode, KZU_PROPERTY_TYPE_COMPONENT_PRESSED, KZ_TRUE);
                     kzsErrorForward(result);
                     result = kzuUiComponentNodeHandleEvents(componentNode, event, KZU_SLIDER_PRESSED, queue);
                     kzsErrorForward(result);
                     pressed = KZ_TRUE;
                 }
             }
         }

         if (pressed && ((buttonHit && kzuUiEventIsPointerReleased(event) && kzcStringIsEqual(eventType, "PointerReleasedEvent")) || !buttonHit))
         {
             result = kzuObjectNodeSetBoolProperty(objectNode, KZU_PROPERTY_TYPE_COMPONENT_PRESSED, KZ_FALSE);
             kzsErrorForward(result);
             result = kzuUiComponentNodeHandleEvents(componentNode, event, KZU_SLIDER_RELEASED, queue);
             kzsErrorForward(result);
         }

         if (railNode != KZ_NULL && kzcStringIsEqual(eventType, "PointerPressedEvent") && 
             kzuUiEventGetDragStartPickedObjectNode(event) == railNode)
         {
             struct KzcRay ray = kzuUiEventGetStoredPickingRay(event);
             struct KzcVector3 expandDirections = KZC_VECTOR3_ZERO;
             struct KzcVector3 sliderOffsetHelper = KZC_VECTOR3_ZERO;
             struct KzcVector3 point = kzcVector3(0.0f, 0.0f, 0.0f);
             kzInt direction = kzuObjectNodeGetIntPropertyDefault(objectNode, KZU_PROPERTY_TYPE_SLIDER_DIRECTION);
             struct KzcMatrix4x4 railWorldTransformation;
             kzFloat distance;
             kzBool boundingBoxHit;
             struct KzcVector3 objectPosition;
             struct KzuMeshNode* meshNode = kzuMeshNodeFromObjectNode(railNode);
             struct KzuMesh* mesh = kzuMeshNodeGetMesh(meshNode);
             struct KzuBoundingVolume* volume = kzuMeshGetBoundingVolume(mesh);

             kzuObjectNodeCalculateWorldTransformation(railNode, &railWorldTransformation);

             kzcMatrix4x4GetPosition(&railWorldTransformation, &objectPosition);

             switch(direction)
             {
                case KZU_UI_SLIDER_DIRECTION_X:
                {
                    expandDirections = kzcVector3(1.0f, 0.0f, 0.0f);
                    break;
                }
                case KZU_UI_SLIDER_DIRECTION_Y:
                {
                    expandDirections = kzcVector3(0.0f, 1.0f, 0.0f);
                    break;
                }
                case KZU_UI_SLIDER_DIRECTION_Z:
                {
                    expandDirections = kzcVector3(0.0f, 0.0f, 1.0f);
                    break;
                }
                default:
                {
                    kzsAssertText(KZ_FALSE, "Invalid slider direction!");
                }
             }
             
             /* Get direction vector. */
             kzcMatrix4x4MultiplyVector3By3x3(&railWorldTransformation, &expandDirections, &sliderOffsetHelper);

             {
                kzInt expandDirection = kzuObjectNodeGetIntPropertyDefault(objectNode, KZU_PROPERTY_TYPE_SLIDER_EXPAND_DIRECTION);
                switch(expandDirection)
                {
                    case KZU_UI_SLIDER_EXPAND_DIRECTION_X:
                    {
                        expandDirections.data[0] = 1.0f;
                        break;
                    }
                    case KZU_UI_SLIDER_EXPAND_DIRECTION_Y:
                    {
                        expandDirections.data[1] = 1.0f;
                        break;
                    }
                    case KZU_UI_SLIDER_EXPAND_DIRECTION_Z:
                    {
                        expandDirections.data[2] = 1.0f;
                        break;
                    }
                    case KZU_UI_SLIDER_EXPAND_DIRECTION_ONLY_SLIDER_DIRECTION:
                    {
                        break;
                    }
                    case KZU_UI_SLIDER_EXPAND_DIRECTION_NONE:
                    default:
                    {
                        expandDirections = kzcVector3(0.0f, 0.0f, 0.0f);
                        break;
                    }
                }
             }
             
             {
                 kzUint i;
                 struct KzuBoundingVolumeAxisAlignedBox* obb = kzuBoundingVolumeGetAxisAlignedBox(volume);
                 struct KzcVector3 originalMinimum = kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner(obb);
                 struct KzcVector3 originalMaximum = kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner(obb);
                 
                 for(i = 0; i < 3; ++i)
                 {
                     if(expandDirections.data[i] > 0.1f)
                     {
                         originalMinimum.data[i] = -KZ_FLOAT_MAXIMUM;
                         originalMaximum.data[i] = KZ_FLOAT_MAXIMUM;
                     }
                 }

                 boundingBoxHit = kzcRayBoundingBoxIntersection(&ray, &originalMinimum, &originalMaximum, 
                     &railWorldTransformation, &distance);

                 if (kzsFloatIsEqual(distance, 0.0f))
                 {
                     kzsLog(KZS_LOG_LEVEL_WARNING, "Camera is inside slider. (Check slider expand direction settings)");
                 }

                 if (boundingBoxHit)
                 {
                     struct KzcVector3 hitPosition = kzcVector3(kzcVector3GetX(&ray.origin) + kzcVector3GetX(&ray.direction) * distance, 
                         kzcVector3GetY(&ray.origin) + kzcVector3GetY(&ray.direction) * distance, 
                         kzcVector3GetZ(&ray.origin) + kzcVector3GetZ(&ray.direction) * distance);
                     kzcVector3Subtract(&hitPosition, &objectPosition, &point);
                 }
             }

             if(boundingBoxHit)
             {
                 kzFloat length;
                 kzFloat newValue;
                 kzFloat oldValue;

                 struct KzcVector3 size = kzuObjectNodeGetSize(railNode);
                 struct KzcVector3 minBound = kzuObjectNodeGetMinimumBoundingBoxCoordinates(railNode);

                 {
                     kzFloat scale;
                     kzFloat offset;
                     kzInt side;
                     kzcVector3Normalize(&sliderOffsetHelper);
                     offset = kzcVector3Dot(&point, &sliderOffsetHelper);
                     
                     side = kzsClampi(direction, 0, 2);

                     /* Adjust bounding box length by scale factor. */
                     switch(side)
                     {
                         case 0:
                         {
                             scale = kzcMatrix4x4GetScaleX(&railWorldTransformation);
                             break;
                         }
                         case 1:
                         {
                             scale = kzcMatrix4x4GetScaleY(&railWorldTransformation); 
                             break;
                         }
                         case 2:
                         default:
                         {
                             scale = kzcMatrix4x4GetScaleZ(&railWorldTransformation); 
                             break;
                         }
                     }

                     length = size.data[side] * scale;
                     offset -= minBound.data[side] * scale;
                     newValue = offset / length;
                     oldValue = kzuObjectNodeGetFloatPropertyDefault(objectNode, KZU_PROPERTY_TYPE_SLIDER_VALUE);

                     if(kzsFabsf(newValue - oldValue) > KZ_FLOAT_EPSILON)
                     {
                         result = kzuUiSliderSetValueAction(componentNode, newValue, KZ_NULL, event, queue);
                         kzsErrorForward(result);
                     }
                 }
             }
         }
     }
     kzsSuccess();
}

struct KzuObjectNode* kzuUiSliderGetKnob(const struct KzuUiComponentNode* componentNode)
{
     struct KzuObjectNode* node;
     struct KzuObjectNode* knobNode;

     kzsAssert(kzcIsValidPointer(componentNode));

     node = kzuUiComponentNodeToObjectNode(componentNode);
     knobNode = kzuObjectNodeGetChildAtIndex(node, SLIDER_KNOB_CHILD_INDEX);
     return knobNode;
}

struct KzuObjectNode* kzuUiSliderGetBar(const struct KzuUiComponentNode* componentNode)
{
     struct KzuObjectNode* node;
     struct KzuObjectNode* railNode;

     kzsAssert(kzcIsValidPointer(componentNode));

     node = kzuUiComponentNodeToObjectNode(componentNode);
     railNode = kzuObjectNodeGetChildAtIndex(node, SLIDER_RAIL_CHILD_INDEX);
     return railNode;
}

kzFloat kzuUiSliderGetValue(const struct KzuUiComponentNode* componentNode)
{
    struct KzuObjectNode* componentObjectNode = kzuUiComponentNodeToObjectNode(componentNode);

    return kzuObjectNodeGetFloatPropertyDefault(componentObjectNode, KZU_PROPERTY_TYPE_SLIDER_VALUE);
}

kzsError kzuUiSliderSetValue(const struct KzuUiComponentNode* componentNode, kzFloat value)
{
    kzsError result;
    struct KzuObjectNode* componentObjectNode = kzuUiComponentNodeToObjectNode(componentNode);

    result = kzuObjectNodeSetFloatProperty(componentObjectNode, KZU_PROPERTY_TYPE_SLIDER_VALUE, value);
    kzsErrorForward(result);

    kzsSuccess();
}

enum KzuUiSliderExpandDirection kzuUiSliderGetExpandOnDragDirection(const struct KzuUiComponentNode* componentNode)
{
    struct KzuObjectNode* componentObjectNode = kzuUiComponentNodeToObjectNode(componentNode);

    /*lint -e{930}*/
    return (enum KzuUiSliderExpandDirection)kzuObjectNodeGetIntPropertyDefault(componentObjectNode, KZU_PROPERTY_TYPE_SLIDER_EXPAND_DIRECTION);
}

kzsError kzuUiSliderSetExpandOnDragDirection(const struct KzuUiComponentNode* componentNode, enum KzuUiSliderExpandDirection value)
{
    kzsError result;
    struct KzuObjectNode* componentObjectNode = kzuUiComponentNodeToObjectNode(componentNode);

    /*lint -e{930}*/
    result = kzuObjectNodeSetIntProperty(componentObjectNode, KZU_PROPERTY_TYPE_SLIDER_EXPAND_DIRECTION, (kzInt)value);
    kzsErrorForward(result);

    kzsSuccess();
}

enum KzuUiSliderDirection kzuUiSliderGetDirection(const struct KzuUiComponentNode* componentNode)
{
    struct KzuObjectNode* componentObjectNode = kzuUiComponentNodeToObjectNode(componentNode);

    /*lint -e{930}*/
    return (enum KzuUiSliderDirection)kzuObjectNodeGetIntPropertyDefault(componentObjectNode, KZU_PROPERTY_TYPE_SLIDER_DIRECTION);
}

kzsError kzuUiSliderSetDirection(const struct KzuUiComponentNode* componentNode, enum KzuUiSliderDirection direction)
{
    kzsError result;
    struct KzuObjectNode* componentObjectNode = kzuUiComponentNodeToObjectNode(componentNode);

    /*lint -e{930}*/
    result = kzuObjectNodeSetIntProperty(componentObjectNode, KZU_PROPERTY_TYPE_SLIDER_DIRECTION, (kzInt)direction);
    kzsErrorForward(result);

    kzsSuccess();
}
