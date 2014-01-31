/**
* \file
* Default application. This application implements the default pipeline for rendering a simple scene.
* In addition this application accepts binary data from the tool and is able to render an arbitrary scene.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include <application/kza_application_interface.h>
#include <application/kza_application.h>

#include <user/project/kzu_project.h>
#include <user/ui/kzu_ui_manager.h>
#include <user/ui/kzu_ui_event.h>
#include <user/ui/kzu_ui_component.h>
#include <user/scene_graph/kzu_object.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/debug/kzc_log.h>


/** Application specific data. */
struct ApplicationData
{
    kzBool isControlDown; /**< Flag to indicate if the control key is pressed down or not. */
};


/** Set the internal states depending if the control key is pressed or not. */
void setControlDown_internal(struct KzaApplication* application, kzBool isControlDown)
{
    struct ApplicationData* applicationData = (struct ApplicationData*)kzaApplicationGetUserData(application);

    kzsAssert(kzcIsValidPointer(applicationData));

    applicationData->isControlDown = isControlDown;
    kzaApplicationSetDefaultKeyHandlingEnabled(application, !isControlDown);
}

KZ_CALLBACK static kzsError keyDeviceHandler(struct KzaApplication* application, const struct KzsKeyDeviceInputData* inputData)
{
    kzsError result;
    enum KzsInputKey button = kzsInputGetKeyDeviceButton(inputData);
    enum KzsInputKeyDeviceState state = kzsInputGetKeyDeviceState(inputData);
    struct ApplicationData* applicationData = (struct ApplicationData*)kzaApplicationGetUserData(application);

    kzsAssert(kzcIsValidPointer(applicationData));

    if (!applicationData->isControlDown)
    {
        if (state == KZS_KEY_DEVICE_STATE_DOWN)
        {
            /* Show renderer info. */
            if (button == KZS_KEY_R)
            {
                result = kzaApplicationPrintRendererInfo(application);
                kzsErrorForward(result);
            }
            /* Show FPS info. */
            if (button == KZS_KEY_F)
            {
                kzBool enabled = kzaApplicationIsFpsInfoEnabled(application);
                kzaApplicationSetFpsInfoEnabled(application, !enabled);
            }
            /* Show Log contents. */
            if (button == KZS_KEY_L)
            {
                kzBool enabled = kzaApplicationIsLogVisualizationEnabled(application);
                kzaApplicationSetLogVisualizationEnabled(application, !enabled);
            }
            /* Toggle bounding box visualization. */
            if (button == KZS_KEY_0)
            {
                kzBool enabled = kzaApplicationIsBoundingBoxVisualizationEnabled(application);
                kzaApplicationSetBoundingBoxVisualizationEnabled(application, !enabled);
            }

            /* Quit application */
            if (button == KZS_KEY_ESC)
            {
                kzaApplicationQuit(application);
            }
        }
    }

    if (button == KZS_KEY_MODIFIER2)
    {
        setControlDown_internal(application, state == KZS_KEY_DEVICE_STATE_DOWN);
    }

    kzsSuccess();
}

KZ_CALLBACK kzsError applicationWindowEventHandler(struct KzaApplication* application, enum KzsWindowEventType eventType,
                                                   struct KzsWindow* window)
{
    if (eventType == KZS_WINDOW_EVENT_FOCUS_LOST)
    {
        setControlDown_internal(application, KZ_FALSE);
    }

    kzsSuccess();
}

KZ_CALLBACK static kzsError applicationUiEventHandler(const struct KzuUiComponentNode* node, void* userData,
                                                      struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(userData);
    kzInt x = kzuUiEventGetPointerX(event);
    kzInt y = kzuUiEventGetPointerY(event);
    kzString nodeName = kzuObjectNodeGetName(kzuUiComponentNodeToObjectNode(node));

    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "%s, %s, node address: (%p) pointer: (%d, %d)", eventType, nodeName, node, x, y);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK void kzApplicationConfigure(const struct KzaSystemProperties* systemProperties, struct KzaApplicationProperties* configuration)
{
    configuration->memoryPoolSize = 100 * 1024 * 1024;
    configuration->mainHandler = applicationUiEventHandler;

    configuration->pickingEnabled = KZ_TRUE;
    configuration->clearBackgroundEnabled = KZ_TRUE;
    configuration->transformGizmoEnabled = KZ_TRUE;
}

KZ_CALLBACK kzsError kzApplicationInitialize(struct KzaApplication* application)
{
    kzsError result;
    struct ApplicationData* applicationData;

    kzaApplicationSetKeyDeviceHandler(application, keyDeviceHandler);
    kzaApplicationSetWindowEventHandler(application, applicationWindowEventHandler);

    result = kzcMemoryAllocVariable(kzaApplicationGetApplicationMemoryManager(application), applicationData, "Application data");
    kzsErrorForward(result);

    applicationData->isControlDown = KZ_FALSE;

    kzaApplicationSetUserData(application, applicationData);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzApplicationUninitialize(struct KzaApplication* application)
{
    kzsError result;

    struct ApplicationData* applicationData = (struct ApplicationData*)kzaApplicationGetUserData(application);

    result = kzcMemoryFreeVariable(applicationData);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzApplicationUpdate(struct KzaApplication* application, kzUint deltaTime)
{
    kzsSuccess();
}

KZ_CALLBACK kzsError kzApplicationRender(const struct KzaApplication* application)
{
    kzsSuccess();
}
