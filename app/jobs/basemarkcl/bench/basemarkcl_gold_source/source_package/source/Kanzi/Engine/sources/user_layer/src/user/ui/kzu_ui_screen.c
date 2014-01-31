/**
* \file
* Graphical user interface screen functionality.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_screen.h"

#include <user/ui/kzu_ui_component.h>
#include <user/ui/kzu_ui_event.h>
#include <user/util/kzu_raypick.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_object.h>
#include <user/properties/kzu_bool_property.h>
#include <user/filter/kzu_object_source.h>
#include <user/scene_graph/kzu_transformed_scene.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_quick.h>
#include <core/util/math/kzc_ray.h>
#include <core/debug/kzc_log.h>


#define KZU_UI_EVENT_POINTER_PRESSED "PointerPressedEvent"
#define KZU_UI_EVENT_POINTER_RELEASED "PointerReleasedEvent"


struct KzuUiScreen
{
    struct KzuUiEvent* uiEvent;              /**< Event structure for ui screen. */
    struct KzuObjectSource* objectSource;    /**< Object source which can be used to identify the objects used for picking.
                                                  If KZ_NULL the engine root object source is used. */

    struct KzuUiComponentNode* focusedComponentNode; /**< Focused component of the screen. When KZ_NULL no focused is set. */
    struct KzuCameraNode* pickingCamera;     /**< Picking camera, NULL when picking is done with default scene camera. */
    struct KzuCameraNode* pickingCamera2D;   /**< Picking camera 2D, NULL if no 2D picking is wanted. */
    kzBool pickingEnabled;                   /**< Is picking enabled. */
};


kzsError kzuUiScreenCreate(const struct KzcMemoryManager* memoryManager, struct KzuUiScreen** out_screen)
{
    kzsError result;
    struct KzuUiScreen* screen;
 
    result = kzcMemoryAllocVariable(memoryManager, screen, "UI Screen");
    kzsErrorForward(result);

    result = kzuUiEventCreate(memoryManager, &screen->uiEvent);
    kzsErrorForward(result);

    screen->focusedComponentNode = KZ_NULL;
    screen->pickingCamera = KZ_NULL;
    screen->pickingCamera2D = KZ_NULL;
    screen->pickingEnabled = KZ_TRUE;
    screen->objectSource = KZ_NULL;

    *out_screen = screen;
    kzsSuccess();
}

kzsError kzuUiScreenDelete(struct KzuUiScreen* screen)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(screen));

    result = kzuUiEventDelete(screen->uiEvent);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(screen);
    kzsErrorForward(result);   

    kzsSuccess();
}

void kzuUiScreenSetObjectSource(struct KzuUiScreen* screen, struct KzuObjectSource* objectSource)
{
    /* Parameter objectSource can be KZ_NULL. */
    kzsAssert(kzcIsValidPointer(screen));
    screen->objectSource = objectSource;
}

void kzuUiScreenSetPickingCamera(struct KzuUiScreen* screen, struct KzuCameraNode* pickingCamera)
{
    kzsAssert(kzcIsValidPointer(screen));
    kzsAssert(pickingCamera == KZ_NULL || kzcIsValidPointer(pickingCamera));
    screen->pickingCamera = pickingCamera;
}

void kzuUiScreenSet2DPickingCamera(struct KzuUiScreen* screen, struct KzuCameraNode* pickingCamera)
{
    kzsAssert(kzcIsValidPointer(screen));
    kzsAssert(pickingCamera == KZ_NULL || kzcIsValidPointer(pickingCamera));
    screen->pickingCamera2D = pickingCamera;
}

void kzuUiScreenEnablePicking(struct KzuUiScreen* screen, kzBool value)
{
    kzsAssert(kzcIsValidPointer(screen));
    screen->pickingEnabled = value;
}

kzBool kzuUiScreenGetPickingEnabled(const struct KzuUiScreen* screen)
{
    kzsAssert(kzcIsValidPointer(screen));
    return screen->pickingEnabled;
}

static kzsError kzuUiScreenForwardHandleEvents_internal(const struct KzuUiScreen* screen, const struct KzuObjectNode* node, 
                                                        struct KzuUiEvent* event, kzString eventType, struct KzuEngineMessageQueue* queue)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(screen));
    kzsAssert(kzcIsValidPointer(node));
    {
        struct KzcDynamicArrayIterator it = kzuObjectNodeGetChildren(node);
        while(kzcDynamicArrayIterate(it))
        {
            struct KzuObjectNode* childNode = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(it);
            if(kzuObjectNodeGetType(childNode) == KZU_OBJECT_TYPE_UI_COMPONENT)
            {
                struct KzuUiComponentNode* componentNode = kzuUiComponentNodeFromObjectNode(childNode);

                result = kzuUiComponentNodeHandleEvents(componentNode, event, eventType, queue);
                kzsErrorForward(result);
            }
            result = kzuUiScreenForwardHandleEvents_internal(screen, childNode, event, eventType, queue);
            kzsErrorForward(result);
        }
    }
    kzsSuccess();
}

kzsError kzuUiScreenHandleEvents(struct KzuUiScreen* screen, const struct KzuObjectNode* root,
                                 const struct KzuTransformedScene* transformedScene,
                                 const struct KzuRenderer* renderer, const struct KzsWindow* window,
                                 struct KzuEngineMessageQueue* queue)
{
    kzsError result;
    struct KzuUiEvent* event;

    kzsAssert(kzcIsValidPointer(screen));

    event = screen->uiEvent;
    
    if(kzuUiEventIsTabPressed(event))
    {
        if(kzuUiEventIsShiftPressed(event))
        {
            result = kzuUiScreenFocusPreviousControl(screen);
            kzsErrorForward(result);
        }
        else
        {
            result = kzuUiScreenFocusNextControl(screen);
            kzsErrorForward(result);
        }
    }

    if(screen->pickingEnabled && (kzuUiEventIsPointerPressed(event) || kzuUiEventIsPointerReleased(event)))
    {
        kzFloat distance;
        struct KzcVector3 vectorToOrigin = kzcVector3(0.0f, 0.0f, 0.0f);
        struct KzcRay pickingRay;
        struct KzuObjectNode* foundObjectNode = KZ_NULL;
        struct KzuObjectSource* objectSource = screen->objectSource;

        pickingRay.origin = kzcVector3(0.0f, 0.0f, 0.0f);
        pickingRay.direction = kzcVector3(0.0f, 0.0f, 0.0f);

        if (!kzuObjectSourceIsValid(objectSource))
        {
            objectSource = kzuTransformedSceneGetRootObjectSource(transformedScene);
        }

        /* Pick 2D items. */
        if (objectSource != KZ_NULL && screen->pickingCamera2D != KZ_NULL)
        {
            result = kzuRayPick(renderer, window, objectSource, transformedScene, screen->pickingCamera2D,
                                kzuUiEventGetPointerX(event), kzuUiEventGetPointerY(event), KZ_TRUE,
                                &distance, &foundObjectNode, KZ_NULL, &vectorToOrigin, &pickingRay);
            kzsErrorForward(result);
        }

        
        /* Pick 3D items. */
        if (objectSource != KZ_NULL && foundObjectNode == KZ_NULL)
        {
            struct KzuCameraNode* cameraNode;

            if (screen->pickingCamera == KZ_NULL)
            {
                 cameraNode = kzuSceneGetViewCamera(kzuTransformedSceneGetScene(transformedScene));
                 kzsAssert(kzcIsValidPointer(cameraNode));
            }
            else
            {
                cameraNode = screen->pickingCamera;
            }

            result = kzuRayPick(renderer, window, objectSource,
                                transformedScene, cameraNode, kzuUiEventGetPointerX(event), kzuUiEventGetPointerY(event), 
                                KZ_FALSE, &distance, &foundObjectNode, KZ_NULL, &vectorToOrigin, &pickingRay);
            kzsErrorForward(result);
        }

        {
            /* TODO: Don't use "string typing" for event types. */
            kzString eventType = (kzuUiEventIsPointerPressed(event) ? KZU_UI_EVENT_POINTER_PRESSED : KZU_UI_EVENT_POINTER_RELEASED);
                         
            kzuUiEventSetPickedObjectNodeOffset(event, &vectorToOrigin);
            kzuUiEventSetPickedObjectNode(event, foundObjectNode);
            kzuUiEventSetStoredPickingRay(event, &pickingRay);

            result = kzuUiScreenForwardHandleEvents_internal(screen, root, event, eventType, queue);
            kzsErrorForward(result);
        }
    }

    result = kzuUiEventResetFrame(screen->uiEvent);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzuUiScreenForwardLogicUpdate_internal(const struct KzuUiScreen* screen, const struct KzuObjectNode* node, kzFloat deltaTime, struct KzuEngineMessageQueue* queue)
{
    kzsError result;
    {
        struct KzcDynamicArrayIterator it = kzuObjectNodeGetChildren(node);
        while(kzcDynamicArrayIterate(it))
        {
            struct KzuObjectNode* childNode = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(it);
            if(kzuObjectNodeGetType(childNode) == KZU_OBJECT_TYPE_UI_COMPONENT)
            {
                struct KzuUiComponentNode* componentNode = kzuUiComponentNodeFromObjectNode(childNode);
                result = kzuUiComponentNodeLogicUpdate(componentNode, deltaTime, queue);
                kzsErrorForward(result);
            }
            result = kzuUiScreenForwardLogicUpdate_internal(screen, childNode, deltaTime, queue);
            kzsErrorForward(result);
        }
    }
    kzsSuccess();
}

kzsError kzuUiScreenLogicUpdate(const struct KzuUiScreen* screen, const struct KzuObjectNode* root, kzFloat deltaTime, struct KzuEngineMessageQueue* queue)
{
    kzsError result;

    result = kzuUiScreenForwardLogicUpdate_internal(screen, root, deltaTime, queue);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzuUiScreenSetFocusedControl(struct KzuUiScreen* screen, struct KzuUiComponentNode* componentNode)
{
    kzsAssert(kzcIsValidPointer(screen));
    screen->focusedComponentNode = componentNode;
}

struct KzuUiComponentNode* kzuUiScreenGetFocusedControl(const struct KzuUiScreen* screen)
{
    kzsAssert(kzcIsValidPointer(screen));
    return screen->focusedComponentNode;
}

kzsError kzuUiScreenFocusNextControl(struct KzuUiScreen* screen)
{
    KZ_UNUSED_PARAMETER(screen);
    /* TODO: Implement focus control. */
    kzsSuccess();
    /*
    kzsError result;
    struct KzcDynamicArray* array;
    struct KzcDynamicArrayIterator it;
    kzBool oldControlFound = KZ_FALSE;
    struct KzuUiComponent* newControl = KZ_NULL;
    struct KzcMemoryManager* quickMemoryManager;

    kzsAssert(kzcIsValidPointer(screen));

    result = kzcMemoryManagerCreateQuickManager(kzcMemoryGetManager(screen), 500000, &quickMemoryManager);
    kzsErrorForward(result);

    result = kzuUiScreenGetControlsAsArray_internal(quickMemoryManager, screen, &array);
    kzsErrorForward(result);

    if(kzcDynamicArrayGetSize(array) > 0)
    {

        it = kzcDynamicArrayGetIterator(array);
        while(kzcDynamicArrayIterate(it))
        {
            struct KzuUiComponent* component = (struct KzuUiComponent*)kzcDynamicArrayIteratorGetValue(it);
            if(oldControlFound)
            {
                newControl = component;
                break;
            }
            if(screen->focusedComponent == component)
            {
                oldControlFound = KZ_TRUE;
            }
        }

        if(newControl == KZ_NULL)
        {
            newControl = kzcDynamicArrayGet(array, 0);
        }

        if(screen->focusedComponent != KZ_NULL)
        {
            result = kzuUiComponentHandleEvents(screen->focusedComponent, screen->uiEvent, KZU_UI_EVENT_FOCUS_LOST);
            kzsErrorForward(result);
        }
        result = kzuUiComponentHandleEvents(screen->focusedComponent, screen->uiEvent, KZU_UI_EVENT_FOCUS_GAINED);
        kzsErrorForward(result);
    }

    result = kzcMemoryManagerDelete(quickMemoryManager);
    kzsErrorForward(result);

    screen->focusedComponent = newControl;
    kzsSuccess();
    */
}

kzsError kzuUiScreenFocusPreviousControl(struct KzuUiScreen* screen)
{
    KZ_UNUSED_PARAMETER(screen);
    /* TODO: Implement focus control.*/
    kzsSuccess();
    /*
    kzsError result;
    struct KzcDynamicArray* array;
    struct KzcDynamicArrayIterator it;
    struct KzuUiComponent* newControl = KZ_NULL;
    struct KzcMemoryManager* quickMemoryManager;
    
    kzsAssert(kzcIsValidPointer(screen));
    result = kzcMemoryManagerCreateQuickManager(kzcMemoryGetManager(screen), 500000, &quickMemoryManager);
    kzsErrorForward(result);

    result = kzuUiScreenGetControlsAsArray_internal(quickMemoryManager, screen, &array);
    kzsErrorForward(result);

    if(kzcDynamicArrayGetSize(array) > 0)
    {
        it = kzcDynamicArrayGetIterator(array);
        while(kzcDynamicArrayIterate(it))
        {
            struct KzuUiComponent* component = (struct KzuUiComponent*)kzcDynamicArrayIteratorGetValue(it);
            if(screen->focusedComponent == component)
            {
                break;
            }
            newControl = component;
        }

        if(newControl == KZ_NULL)
        {
            newControl = kzcDynamicArrayGet(array, kzcDynamicArrayGetSize(array) - 1);
        }

        if(screen->focusedComponent != KZ_NULL)
        {
            result = kzuUiComponentHandleEvents(screen->focusedComponent, screen->userData, screen->uiEvent, KZU_UI_EVENT_FOCUS_LOST);
            kzsErrorForward(result);
        }
        result = kzuUiComponentHandleEvents(screen->focusedComponent, screen->userData, screen->uiEvent, KZU_UI_EVENT_FOCUS_GAINED);
        kzsErrorForward(result);
    }
   
    result = kzcMemoryManagerDelete(quickMemoryManager);
    kzsErrorForward(result);

    screen->focusedComponent = newControl;
    kzsSuccess();
    */
}

void kzuUiScreenHandleKeyEvents(const struct KzuUiScreen* screen, enum KzsInputKey keyCode, enum KzsInputKeyDeviceState keyState)
{
    kzsAssert(kzcIsValidPointer(screen));

    switch(keyState)
    {
        case KZS_KEY_DEVICE_STATE_DOWN:
        {
            kzuUiEventKeyPressed(screen->uiEvent, keyCode);
            break;
        }
        case KZS_KEY_DEVICE_STATE_UP:
        {
            kzuUiEventKeyReleased(screen->uiEvent, keyCode);
            break;
        }
        case KZS_KEY_DEVICE_STATE_REPEAT:
        default:
        {
            break;
        }
    }
}

void kzuUiScreenHandlePointerEvents(const struct KzuUiScreen* screen, kzU32 buttons, enum KzsInputPointingDeviceState state, kzInt mouseX, kzInt mouseY)
{
    KZ_UNUSED_PARAMETER(buttons);

    /* TODO: Implement multiple pointer device support. */
    kzsAssert(kzcIsValidPointer(screen));

    switch(state)
    {
        case KZS_POINTING_DEVICE_STATE_DOWN:
        {
            kzuUiEventPointerPressed(screen->uiEvent, KZU_UI_EVENT_BUTTON_1, mouseX, mouseY);
            break;
        }
        case KZS_POINTING_DEVICE_STATE_UP:
        {
            kzuUiEventPointerReleased(screen->uiEvent, KZU_UI_EVENT_BUTTON_1, mouseX, mouseY);
            break;
        }
        case KZS_POINTING_DEVICE_STATE_DRAG:
        {
            kzuUiEventPointerDragged(screen->uiEvent, KZU_UI_EVENT_BUTTON_1, mouseX, mouseY);
            break;
        }
        case KZS_POINTING_DEVICE_STATE_MOVE:
        {
            kzuUiEventPointerMoved(screen->uiEvent, mouseX, mouseY);
            break;
        }
    }
}

kzsError kzuUiScreenReset(const struct KzuUiScreen* screen)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(screen));

    result = kzuUiEventResetEvent(screen->uiEvent);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuUiEvent* kzuUiScreenGetEvent(const struct KzuUiScreen* screen)
{
    kzsAssert(kzcIsValidPointer(screen));
    return screen->uiEvent;
}
