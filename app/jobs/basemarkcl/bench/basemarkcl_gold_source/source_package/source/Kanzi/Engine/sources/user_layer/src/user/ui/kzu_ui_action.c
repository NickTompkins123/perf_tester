/**
* \file
* User interface action.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_action.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>


struct KzuUiAction
{
    KzuUiActionCallback callback; /**< Action callback. */
    kzBool requiresMessageQueue; /**< Is this action engine internal action. */
    void* userData;              /**< UserData passed along this action. */
};


kzsError kzuUiActionCreate(const struct KzcMemoryManager* memoryManager, KzuUiActionCallback actionCallback, 
                           kzBool requiresMessageQueue, void* userData, struct KzuUiAction** out_action)
{
    kzsError result;
    struct KzuUiAction* action;

    result = kzcMemoryAllocVariable(memoryManager, action, "UI Action (callback)");
    kzsErrorForward(result);

    action->callback = actionCallback;
    action->requiresMessageQueue = requiresMessageQueue;
    action->userData = userData;

    *out_action = action;
    kzsSuccess();
}

kzsError kzuUiActionDelete(struct KzuUiAction* action)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(action));

    result = kzcMemoryFreeVariable(action);
    kzsErrorForward(result);
    
    kzsSuccess();
}

kzsError kzuUiActionCopy(const struct KzcMemoryManager* memoryManager, const struct KzuUiAction* sourceAction,
                         struct KzuUiAction** out_action)
{
    kzsError result;
    struct KzuUiAction* action;

    result = kzuUiActionCreate(memoryManager, sourceAction->callback, sourceAction->requiresMessageQueue,
                               sourceAction->userData, &action);
    kzsErrorForward(result);

    *out_action = action;
    kzsSuccess();
}

kzBool kzuUiActionGetRequiresMessageQueue(const struct KzuUiAction* action)
{
    kzsAssert(kzcIsValidPointer(action));
    return action->requiresMessageQueue;
}

kzsError kzuUiActionCall(const struct KzuUiAction* action, const struct KzuUiComponentNode* component, void* userData, 
                         kzString eventType, struct KzuUiEvent* event, struct KzcHashMap* customParameters)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(action));

    /* KZ_NULL callback must be allowed here. It is to support user defined callbacks even in default preview. */
    if(action->callback != KZ_NULL) 
    {
        result = action->callback(component, userData, event, eventType, customParameters); 
        kzsErrorForward(result);
    }

    kzsSuccess();
}

void* kzuUiActionGetUserData(const struct KzuUiAction* action)
{
    kzsAssert(kzcIsValidPointer(action));
    return action->userData;
}
