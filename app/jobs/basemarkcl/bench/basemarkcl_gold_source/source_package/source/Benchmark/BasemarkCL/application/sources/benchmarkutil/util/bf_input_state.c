/**
* \file
* Benchmark framework input state handler.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/

#include "bf_input_state.h"

#include <core/memory/kzc_memory_manager.h>

#include <system/input/kzs_input.h>


struct BfInputState
{
    enum BfKeyState* inputTable;
    kzUint inputTableSize;
};

kzsError bfInputStateCreate(const struct KzcMemoryManager* manager, struct BfInputState** out_inputState)
{
    kzsError result;
    kzUint i;
    kzUint inputKeyCount = (kzUint)KZS_KEY_COUNT;
    struct BfInputState* inputState;

    result = kzcMemoryAllocVariable(manager, inputState, "bfInputState");
    kzsErrorForward(result);

    inputState->inputTableSize = inputKeyCount;

    result = kzcMemoryAllocArray(manager, inputState->inputTable, inputState->inputTableSize, "BfInputState->inputTable");
    kzsErrorForward(result);

    for(i = 0; i < inputState->inputTableSize; ++i)
    {
        inputState->inputTable[i] = BF_KEY_NO_EVENT;
    }

    *out_inputState = inputState;
    kzsSuccess();
}

kzsError bfInputStateDelete(struct BfInputState* inputState)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(inputState));

    result = kzcMemoryFreeArray(inputState->inputTable);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(inputState);
    kzsErrorForward(result);

    kzsSuccess();
}

void bfInputStateReset(const struct BfInputState* inputState)
{
    kzUint i;
    for(i = 0; i < inputState->inputTableSize; ++i)
    {
        inputState->inputTable[i] = BF_KEY_NO_EVENT;
    }
    kzsAssert(kzcIsValidPointer(inputState));
}

kzsError bfInputStateRegisterEvents(const struct BfInputState* inputState, enum KzsInputKey button, enum KzsInputKeyDeviceState state)
{
    kzUint index = (kzUint) button;

    kzsAssert(kzcIsValidPointer(inputState));

    kzsErrorTest(index < inputState->inputTableSize, KZS_ERROR_ARRAY_OUT_OF_BOUNDS, "struct BfInputState: Malformed input state table");

    switch(state)
    {
        case KZS_KEY_DEVICE_STATE_DOWN:
        {
            inputState->inputTable[index] = BF_KEY_DOWN;
            break;
        }
        case KZS_KEY_DEVICE_STATE_UP:
        {
            inputState->inputTable[index] = BF_KEY_UP;
            break;
        }
        case KZS_KEY_DEVICE_STATE_REPEAT:
        {
            inputState->inputTable[index] = BF_KEY_REPEAT;
            break;
        }
        default:
        {
            kzsAssertText(KZ_FALSE, "Unknown keyboard event");
        }
    }
    kzsSuccess();
}

enum BfKeyState bfInputStateGetState( const struct BfInputState* inputState, enum KzsInputKey button )
{
    kzUint index = (kzUint) button;
    kzsAssert(kzcIsValidPointer(inputState));

    return inputState->inputTable[index];
}
