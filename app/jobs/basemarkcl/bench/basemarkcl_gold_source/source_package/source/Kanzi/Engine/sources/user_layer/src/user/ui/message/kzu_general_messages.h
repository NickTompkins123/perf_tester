/**
* \file
* Standard message types.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_GENERAL_MESSAGES_H
#define KZU_GENERAL_MESSAGES_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzuObjectNode;
struct KzuMessageType;
struct KzcDynamicArray;
struct KzcMemoryManager;


/** Destroy object node message. */
extern const struct KzuMessageType* KZU_MESSAGE_DELETE_OBJECT;                         /*lint !e956*/
/** Pointer down message. */
extern const struct KzuMessageType* KZU_MESSAGE_POINTER_DOWN;                          /*lint !e956*/
/** Pointer up message. */
extern const struct KzuMessageType* KZU_MESSAGE_POINTER_UP;                            /*lint !e956*/


/** Helper function to post a message to delete an object node. */
kzsError kzuGeneralMessagesPostDeleteObject(struct KzuObjectNode* object);

/** Create standard message types. */
kzsError kzuMessageCreateGeneralTypes(const struct KzcMemoryManager* memoryManager, struct KzcDynamicArray** out_messageArray);
/** Destroy standard message types. */
kzsError kzuMessageDeleteGeneralTypes(struct KzcDynamicArray* messageArray);


#endif
