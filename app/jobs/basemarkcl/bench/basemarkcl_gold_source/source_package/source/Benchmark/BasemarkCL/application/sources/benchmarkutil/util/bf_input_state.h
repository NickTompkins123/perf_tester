/**
* \file
* Benchmark framework input state handler.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef BF_INPUT_STATE_H
#define BF_INPUT_STATE_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>
#include <system/input/kzs_input.h>


/** Forward declarations. */
struct KzcMemoryManager;


/** Key state. */
enum BfKeyState
{
    BF_KEY_NO_EVENT, /**< No key event. */
    BF_KEY_DOWN, /**< Key down. */
    BF_KEY_UP, /**< Key up. */
    BF_KEY_REPEAT /**< Key repeat. */
};


/** 
* \struct BfInputState. 
* Wrapper for input events. 
*/
struct BfInputState;


/** Create input state store. */
kzsError bfInputStateCreate(const struct KzcMemoryManager* manager, struct BfInputState** out_inputState);
/** Delete input state store. */
kzsError bfInputStateDelete(struct BfInputState* inputState);
/** Reset input state store. */
void bfInputStateReset(const struct BfInputState* inputState);

/** Register input state events. */
kzsError bfInputStateRegisterEvents(const struct BfInputState* inputState, enum KzsInputKey button, enum KzsInputKeyDeviceState state);
/** Register input state events. */
enum BfKeyState bfInputStateGetState(const struct BfInputState* inputState, enum KzsInputKey button);


#endif

