/**
 * \file
 * User module.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzu_user.h"

#include <user/common/kzu_globals.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/ui/message/kzu_general_messages.h>

#include <core/memory/kzc_memory_manager.h>


kzsError kzuUserInitialize(const struct KzcMemoryManager* memoryManager)
{
    kzsError result;

    result = kzuGlobalsInitialize(memoryManager);
    kzsErrorForward(result);

    result = kzuFixedPropertiesCreate(memoryManager);
    kzsErrorForward(result);

    {
        struct KzcDynamicArray* messages;

        result = kzuMessageCreateGeneralTypes(memoryManager, &messages);
        kzsErrorForward(result);
        result = kzuGlobalsPut(KZU_GLOBALS_STANDARD_MESSAGE_TYPES, messages);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuUserUninitialize(void)
{
    kzsError result;

    result = kzuFixedPropertiesDelete();
    kzsErrorForward(result);

    {
        struct KzcDynamicArray* messages = kzuGlobalsGet(KZU_GLOBALS_STANDARD_MESSAGE_TYPES);

        kzsAssert(kzcIsValidPointer(messages));

        result = kzuMessageDeleteGeneralTypes(messages);
        kzsErrorForward(result);
    }

    result = kzuGlobalsUninitialize();
    kzsErrorForward(result);

    kzsSuccess();
}
