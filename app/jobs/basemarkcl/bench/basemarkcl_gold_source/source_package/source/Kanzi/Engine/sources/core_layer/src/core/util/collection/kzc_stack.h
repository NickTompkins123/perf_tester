/**
 * \file
 * Stack (Last-In-First-Out).
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_STACK_H
#define KZC_STACK_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;


/**
 * \struct KzcStack
 * Structure of a stack.
 */
struct KzcStack;


/** Creates a new initially empty stack. */
kzsError kzcStackCreate(const struct KzcMemoryManager* memoryManager, struct KzcStack** out_stack);

/** Frees the memory allocated for the stack. */
kzsError kzcStackDelete(struct KzcStack* stack);


/** Pushes the specified element to the stack. */
kzsError kzcStackPush(struct KzcStack* stack, void* element);

/** Pops the last element from the stack. Returns error if the stack is empty. */
kzsError kzcStackPop(struct KzcStack* stack, void** out_element);

/** Gets the last element from the stack without removing it. Returns error if the stack is empty. */
kzsError kzcStackPeek(const struct KzcStack* stack, void** out_element);


/** Returns the number of elements in the stack. */
kzUint kzcStackGetSize(const struct KzcStack* stack);

/** Checks if the stack is empty or not. */
kzBool kzcStackIsEmpty(const struct KzcStack* stack);


/** Clears the stack by removing all items from it. */
kzsError kzcStackClear(struct KzcStack* stack);


#endif
