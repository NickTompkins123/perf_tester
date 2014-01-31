/**
* \file
* Message dispatcher.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_message_dispatcher.h"
#include "kzu_message.h"
#include "kzu_message_private.h"
#include "kzu_general_messages.h"

#include <user/scene_graph/kzu_object.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_quick.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/collection/kzc_dynamic_array.h>


/** Defines how much memory is used by each of two alternating quick memory managers inside a message dispatcher. */
#define KZU_MESSAGE_DISPATCHER_QUICK_MEMORY_SIZE    (4 * 1024)


/** Describes a subscription. */
struct KzuMessageSubscription
{
    kzBool valid;
    KzuMessageHandlerFunction callback;
    struct KzuObjectNode* messageSource;
    enum KzuMessageRouting routing;
    void* userData;
};

/** Describes a subscription site. */
struct KzuMessageSubscriptionSite
{
    kzBool valid;                           /**< When dispatcher is notified of object node destruction this is set to false to signal that all processing should immediately return. */
    kzUint referenceCount;                  /**< The number of references to this object. When it becomes zero the KzuMessageSubscriptionSite is destroyed. */
    kzUint dispatchDepth;                   /**< The number of users of subscriptions list. The last user cleans up the list from invalidated entries. */
    struct KzuObjectNode* site;             /**< The object node with which the site is associated. */
    struct KzcDynamicArray* subscriptions;  /**< The subscriptions list. The entries are freely added, but never removed directly. */
};

/** Holds subscriptions to particular message type. */
struct KzuMessageSubscriptionRegistry
{
    struct KzcHashMap* siteMap;     /**< Map of <KzuObjectNode, SubscriptionSite>. */
};

/** Message dispatcher. */
struct KzuMessageDispatcher
{
    struct KzcHashMap* messageTypeMap;                          /**< Map of <MessageType, SubscriptionRegistry>. */
    struct KzcMemoryManager* currentQuickMemoryManager;
    struct KzcMemoryManager* otherQuickMemoryManager;
    struct KzcDynamicArray* currentMessageQueue;
    struct KzcDynamicArray* otherMessageQueue;
    /*struct KzcDynamicArray* currentDispatches;*/
};

/** Processing of standard messages. */
static kzsError kzuMessageDispatcherProcessStandardMessages_internal(const struct KzuMessageDispatcher* messageDispatcher, const struct KzuMessage* message);

static struct KzuMessageSubscriptionRegistry* kzuMessageDispatcherFindSubscriptionRegistry_internal(const struct KzuMessageDispatcher* messageDispatcher, const struct KzuMessageType* messageType)
{
    kzBool found;
    struct KzuMessageSubscriptionRegistry* registry = KZ_NULL;

    kzsAssert(kzcIsValidPointer(messageDispatcher));
    kzsAssert(kzcIsValidPointer(messageType));

    found = kzcHashMapGet(messageDispatcher->messageTypeMap, messageType, (void**)&registry);
    KZ_UNUSED_RETURN_VALUE(found);

    return registry;
}

static kzsError kzuMessageDispatcherAcquireSubscriptionRegistry_internal(const struct KzuMessageDispatcher* messageDispatcher, const struct KzuMessageType* messageType,
                                                                         struct KzuMessageSubscriptionRegistry** out_registry)
{
    kzsError result;
    struct KzuMessageSubscriptionRegistry* registry = kzuMessageDispatcherFindSubscriptionRegistry_internal(messageDispatcher, messageType);

    if (registry == KZ_NULL)
    {
        struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(messageDispatcher);

        result = kzcMemoryAllocVariable(memoryManager, registry, "Message subscription registry");
        kzsErrorForward(result);

        result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &registry->siteMap);
        kzsErrorForward(result);

        result = kzcHashMapPut(messageDispatcher->messageTypeMap, messageType, registry);
        kzsErrorForward(result);
    }

    *out_registry = registry;
    kzsSuccess();
}

static struct KzuMessageSubscriptionSite* kzuMessageDispatcherReferenceSubscriptionSite_internal(const struct KzuMessageSubscriptionRegistry* registry, const struct KzuObjectNode* node)
{
    struct KzuMessageSubscriptionSite* subscriptionSite = KZ_NULL;

    kzsAssert(kzcIsValidPointer(registry));
    kzsAssert(kzuObjectNodeIsValid(node));

    if (kzcHashMapGet(registry->siteMap, node, (void**)&subscriptionSite))
    {
        kzsAssert(subscriptionSite->valid);
        kzsAssert(subscriptionSite->referenceCount > 0);

        ++subscriptionSite->referenceCount;
    }

    return subscriptionSite;
}

static kzsError kzuMessageDispatcherDereferenceSubscriptionSite_internal(struct KzuMessageSubscriptionSite* subscriptionSite)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(subscriptionSite));
    kzsAssert(subscriptionSite->referenceCount > 0);

    --subscriptionSite->referenceCount;
    if (subscriptionSite->referenceCount == 0)
    {
        result = kzcDynamicArrayDelete(subscriptionSite->subscriptions);
        kzsErrorForward(result);

        result = kzcMemoryFreeVariable(subscriptionSite);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

static void kzuMessageDispatcherEnterDispatch_internal(struct KzuMessageSubscriptionSite* subscriptionSite)
{
    kzsAssert(kzcIsValidPointer(subscriptionSite));
    kzsAssert(subscriptionSite->referenceCount > 0);
    kzsAssert(subscriptionSite->valid);

    ++subscriptionSite->dispatchDepth;
}

static kzsError kzuMessageDispatcherLeaveDispatch_internal(struct KzuMessageSubscriptionSite* subscriptionSite)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(subscriptionSite));
    kzsAssert(subscriptionSite->referenceCount > 0);
    kzsAssert(subscriptionSite->dispatchDepth > 0);

    --subscriptionSite->dispatchDepth;
    if (subscriptionSite->dispatchDepth == 0)
    {
        struct KzcDynamicArrayMutableIterator it = kzcDynamicArrayGetMutableIterator(subscriptionSite->subscriptions);
        while (kzcDynamicArrayMutableIterate(it))
        {
            struct KzuMessageSubscription* subscription = kzcDynamicArrayMutableIteratorGetValue(it);
            kzsAssert(kzcIsValidPointer(subscription));

            if (!subscription->valid)
            {
                result = kzcMemoryFreeVariable(subscription);
                kzsErrorForward(result);

                result = kzcDynamicArrayMutableIteratorRemove(it);
                kzsErrorForward(result);
            }
        }
    }

    kzsSuccess();
}

static kzsError kzuMessageDispatcherCreateSubscriptionSite_internal(const struct KzuMessageSubscriptionRegistry* registry, struct KzuObjectNode* node,
                                                                    struct KzuMessageSubscriptionSite** out_subscriptionSite)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(registry);
    struct KzuMessageSubscriptionSite* subscriptionSite;

    result = kzcMemoryAllocVariable(memoryManager, subscriptionSite, "Message subscription site");
    kzsErrorForward(result);

    subscriptionSite->valid = KZ_TRUE;
    subscriptionSite->referenceCount = 1;   /* The registry holds 1 reference. */
    subscriptionSite->dispatchDepth = 0;
    subscriptionSite->site = node;

    result = kzcDynamicArrayCreate(memoryManager, &subscriptionSite->subscriptions);
    kzsErrorForward(result);

    kzsAssert(kzcHashMapContains(registry->siteMap, node) == KZ_FALSE);

    result = kzcHashMapPut(registry->siteMap, node, subscriptionSite);
    kzsErrorForward(result);
    
    *out_subscriptionSite = subscriptionSite;
    kzsSuccess();
}

static struct KzuMessageSubscription* kzuMessageDispatcherFindSubscription_internal(const struct KzuMessageSubscriptionSite* subscriptionSite,
                                                                                    KzuMessageHandlerFunction handlerFunction, const void* userData)
{
    struct KzuMessageSubscription* foundSubscription = KZ_NULL;
    struct KzcDynamicArrayIterator it;
    kzsAssert(kzcIsValidPointer(subscriptionSite));

    /* Iterator used here because no calls can happen in parallel. */
    it = kzcDynamicArrayGetIterator(subscriptionSite->subscriptions);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuMessageSubscription* subscription = kzcDynamicArrayIteratorGetValue(it);
        kzsAssert(kzcIsValidPointer(subscription));

        /* Process only subscriptions that are valid. */
        if (subscription->valid)
        {
            if (subscription->callback == handlerFunction && subscription->userData == userData)
            {
                /* The subscription is considered identical. */
                foundSubscription = subscription;
                break;
            }
        }
    }

    return foundSubscription;
}

static kzsError kzuMessageDispatcherAcquireSubscription_internal(const struct KzuMessageSubscriptionSite* subscriptionSite,
                                                                 struct KzuObjectNode* messageSource, enum KzuMessageRouting routing,
                                                                 KzuMessageHandlerFunction handlerFunction, void* userData, struct KzuMessageSubscription** out_subscription)
{
    kzsError result;
    struct KzuMessageSubscription* subscription = kzuMessageDispatcherFindSubscription_internal(subscriptionSite, handlerFunction, userData);

    kzsAssert(kzcIsValidPointer(subscriptionSite));

    if (subscription == KZ_NULL)
    {
        struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(subscriptionSite);
        result = kzcMemoryAllocVariable(memoryManager, subscription, "Message subscription");
        kzsErrorForward(result);

        subscription->valid = KZ_TRUE;

        /* Add to the list. */
        result = kzcDynamicArrayAdd(subscriptionSite->subscriptions, subscription);
        kzsErrorForward(result);
    }

    /* Here we initialize or overwrite existing stuff. */
    subscription->messageSource = messageSource;
    subscription->routing = routing;
    subscription->callback = handlerFunction;
    subscription->userData = userData;

    *out_subscription = subscription;
    kzsSuccess();
}

kzsError kzuMessageDispatcherCreate(const struct KzcMemoryManager* memoryManager, struct KzuMessageDispatcher** out_messageDispatcher)
{
    kzsError result;
    struct KzuMessageDispatcher* messageDispatcher;

    result = kzcMemoryAllocVariable(memoryManager, messageDispatcher, "Message Dispatcher");
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &messageDispatcher->messageTypeMap);
    kzsErrorForward(result);

    result = kzcMemoryManagerCreateQuickManager(memoryManager, KZU_MESSAGE_DISPATCHER_QUICK_MEMORY_SIZE, &messageDispatcher->currentQuickMemoryManager);
    kzsErrorForward(result);

    result = kzcMemoryManagerCreateQuickManager(memoryManager, KZU_MESSAGE_DISPATCHER_QUICK_MEMORY_SIZE, &messageDispatcher->otherQuickMemoryManager);
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(messageDispatcher->currentQuickMemoryManager, &messageDispatcher->currentMessageQueue);
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(messageDispatcher->otherQuickMemoryManager, &messageDispatcher->otherMessageQueue);
    kzsErrorForward(result);

    *out_messageDispatcher = messageDispatcher;
    kzsSuccess();
}

kzsError kzuMessageDispatcherDelete(struct KzuMessageDispatcher* messageDispatcher)
{
    kzsError result;
    struct KzcHashMapIterator typeIterator;

    kzsAssert(kzcIsValidPointer(messageDispatcher));

    /* Destroy all message type registries. */
    typeIterator = kzcHashMapGetIterator(messageDispatcher->messageTypeMap);
    while (kzcHashMapIterate(typeIterator))
    {
        struct KzuMessageSubscriptionRegistry* registry = kzcHashMapIteratorGetValue(typeIterator);
        kzsAssert(kzcIsValidPointer(registry));

        result = kzcHashMapDelete(registry->siteMap);
        kzsErrorForward(result);

        result = kzcMemoryFreeVariable(registry);
        kzsErrorForward(result);
    }

    result = kzcMemoryManagerDelete(messageDispatcher->currentQuickMemoryManager);
    kzsErrorForward(result);

    result = kzcMemoryManagerDelete(messageDispatcher->otherQuickMemoryManager);
    kzsErrorForward(result);

    result = kzcHashMapDelete(messageDispatcher->messageTypeMap);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(messageDispatcher);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuMessageDispatcherCreateMessage(struct KzuMessageDispatcher* messageDispatcher, const struct KzuMessageType* messageType, struct KzuObjectNode* source,
                                           struct KzuMessage** out_Message)
{
    kzsError result;
    struct KzuMessage* message;

    kzsAssert(kzcIsValidPointer(messageDispatcher));

    result = kzuMessageCreate_private(messageDispatcher->currentQuickMemoryManager, messageDispatcher, messageType, source, &message);
    kzsErrorForward(result);

    *out_Message = message;
    kzsSuccess();
}

kzsError kzuMessageDispatcherAddHandler(const struct KzuMessageDispatcher* messageDispatcher, struct KzuObjectNode* subscriptionSiteNode,
                                        const struct KzuMessageType* messageType, struct KzuObjectNode* messageSource, enum KzuMessageRouting routing,
                                        KzuMessageHandlerFunction handlerFunction, void* userData)
{
    kzsError result;
    struct KzuMessageSubscriptionRegistry* registry;
    struct KzuMessageSubscriptionSite* subscriptionSite;
    struct KzuMessageSubscription* subscription;

    kzsAssert(kzcIsValidPointer(messageDispatcher));
    kzsAssert(kzuObjectNodeIsValid(subscriptionSiteNode));

    result = kzuMessageDispatcherAcquireSubscriptionRegistry_internal(messageDispatcher, messageType, &registry);
    kzsErrorForward(result);

    subscriptionSite = kzuMessageDispatcherReferenceSubscriptionSite_internal(registry, subscriptionSiteNode);
    if (subscriptionSite == KZ_NULL)
    {
        result = kzuMessageDispatcherCreateSubscriptionSite_internal(registry, subscriptionSiteNode, &subscriptionSite);
        kzsErrorForward(result);

        /* Increase reference count manually to avoid calling kzuMessageDispatcherReferenceSubscriptionSite_internal again. */
        ++subscriptionSite->referenceCount;
    }

    /* Sanity check. */
    kzsAssert(subscriptionSite->valid);

    result = kzuMessageDispatcherAcquireSubscription_internal(subscriptionSite, messageSource, routing, handlerFunction, userData, &subscription);
    kzsErrorForward(result);

    result = kzuMessageDispatcherDereferenceSubscriptionSite_internal(subscriptionSite);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuMessageDispatcherRemoveHandler(const struct KzuMessageDispatcher* messageDispatcher, const struct KzuObjectNode* subscriptionSiteNode,
                                           const struct KzuMessageType* messageType, KzuMessageHandlerFunction handlerFunction, const void* userData)
{
    kzsError result;
    struct KzuMessageSubscriptionRegistry* registry = kzuMessageDispatcherFindSubscriptionRegistry_internal(messageDispatcher, messageType);
    if (registry != KZ_NULL)
    {
        struct KzuMessageSubscriptionSite* subscriptionSite = kzuMessageDispatcherReferenceSubscriptionSite_internal(registry, subscriptionSiteNode);
        if (subscriptionSite != KZ_NULL)
        {
            struct KzuMessageSubscription* subscription = kzuMessageDispatcherFindSubscription_internal(subscriptionSite, handlerFunction, userData);
            if (subscription != KZ_NULL)
            {
                /* Sanity check. */
                kzsAssert(subscriptionSite->valid);

                /* If a matching subscription record is found it is marked as invalid. Enter and leave dispatch to trigger cleanup now or when the last dispatch returns. */
                kzuMessageDispatcherEnterDispatch_internal(subscriptionSite);
                subscription->valid = KZ_FALSE;
                result = kzuMessageDispatcherLeaveDispatch_internal(subscriptionSite);
                kzsErrorForward(result);
            }

            result = kzuMessageDispatcherDereferenceSubscriptionSite_internal(subscriptionSite);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

static kzsError kzuMessageDispatcherInvoke_internal(const struct KzuMessageSubscriptionRegistry* registry, const struct KzuObjectNode* subscriptionSiteNode, struct KzuMessage* message,
                                                    enum KzuMessageRouting routing)
{
    kzsError result;
    struct KzuMessageSubscriptionSite* subscriptionSite = kzuMessageDispatcherReferenceSubscriptionSite_internal(registry, subscriptionSiteNode);
    if (subscriptionSite != KZ_NULL)
    {
        kzuMessageDispatcherEnterDispatch_internal(subscriptionSite);
        {
            kzUint i;
            /* Indexes are used because entries can be added from nested calls. The size is queried every iteration. */
            for (i = 0; i != kzcDynamicArrayGetSize(subscriptionSite->subscriptions); ++i)
            {
                struct KzuMessageSubscription* subscription;

                if (kzuMessageIsHandled(message))
                {
                    /* Abort all processing. */
                    break;
                }

                if (!subscriptionSite->valid)
                {
                    /* Abort all processing. */
                    break;
                }

                subscription = kzcDynamicArrayGet(subscriptionSite->subscriptions, i);
                kzsAssert(kzcIsValidPointer(subscription));

                if (!subscription->valid)
                {
                    /* Skip this subscription. */
                    continue;
                }

                if (routing == KZU_MESSAGE_ROUTING_TUNNELLING && (subscription->routing != KZU_MESSAGE_ROUTING_TUNNELLING_BUBBLING &&
                                                                  subscription->routing != KZU_MESSAGE_ROUTING_TUNNELLING))
                {
                    /* Subscription doesnt match. */
                    continue;
                }

                if (routing == KZU_MESSAGE_ROUTING_BUBBLING && (subscription->routing != KZU_MESSAGE_ROUTING_TUNNELLING_BUBBLING &&
                                                                subscription->routing != KZU_MESSAGE_ROUTING_BUBBLING))
                {
                    /* Subscription doesnt match. */
                    continue;
                }

                if (subscription->messageSource != KZ_NULL && subscription->messageSource != kzuMessageGetSource(message))
                {
                    /* Subscription doesnt match. */
                    continue;
                }

                result = subscription->callback(message, subscription->userData);
                kzsErrorForward(result);
            }
        }
        /* subscriptionSite must not be accessed after this point. */ 
        result = kzuMessageDispatcherLeaveDispatch_internal(subscriptionSite);
        kzsErrorForward(result);
        result = kzuMessageDispatcherDereferenceSubscriptionSite_internal(subscriptionSite);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuMessageDispatcherPostMessage(const struct KzuMessageDispatcher* messageDispatcher, struct KzuMessage* message)
{
    kzsError result;

    result = kzcDynamicArrayAdd(messageDispatcher->currentMessageQueue, message);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuMessageDispatcherSendMessage(const struct KzuMessageDispatcher* messageDispatcher, struct KzuMessage* message)
{
    kzsError result;
    const struct KzuMessageType* messageType = kzuMessageGetType(message);
    struct KzuMessageSubscriptionRegistry* registry = kzuMessageDispatcherFindSubscriptionRegistry_internal(messageDispatcher, messageType);

    result = kzuMessageDispatcherProcessStandardMessages_internal(messageDispatcher, message);
    kzsErrorForward(result);

    if (registry != KZ_NULL && kzcHashMapGetSize(registry->siteMap) != 0 && !kzuMessageIsHandled(message))
    {
        enum KzuMessageRouting routing = kzuMessageTypeGetRouting(messageType);

        /* Perform tunnelling phase. */
        if (routing == KZU_MESSAGE_ROUTING_TUNNELLING_BUBBLING || routing == KZU_MESSAGE_ROUTING_TUNNELLING)
        {
            struct KzcDynamicArray* tunnellingRoute;
            kzUint i, end;

            /* Build the tunnelling route. */
            {
                struct KzuObjectNode* currentNode = kzuMessageGetSource(message);
                result = kzcDynamicArrayCreate(messageDispatcher->currentQuickMemoryManager, &tunnellingRoute);
                kzsErrorForward(result);

                while (currentNode != KZ_NULL)
                {
                    result = kzcDynamicArrayAdd(tunnellingRoute, currentNode);
                    kzsErrorForward(result);

                    currentNode = kzuObjectNodeGetParent(currentNode);
                }
            }

            /* Process in reverse order. */
            for (i = kzcDynamicArrayGetSize(tunnellingRoute), end = 0; i != end; --i)
            {
                struct KzuObjectNode* currentNode = kzcDynamicArrayGet(tunnellingRoute, i - 1);
                kzsAssert(kzuObjectNodeIsValid(currentNode));

                if (kzuMessageIsHandled(message))
                {
                    /* Abort processing. */
                    break;
                }

                result = kzuMessageDispatcherInvoke_internal(registry, currentNode, message, KZU_MESSAGE_ROUTING_TUNNELLING);
                kzsErrorForward(result);
            }
        }

        /* Perform bubbling phase. */
        if (routing == KZU_MESSAGE_ROUTING_TUNNELLING_BUBBLING || routing == KZU_MESSAGE_ROUTING_BUBBLING)
        {
            struct KzuObjectNode* currentNode = kzuMessageGetSource(message);
            while (currentNode != KZ_NULL)
            {
                if (kzuMessageIsHandled(message))
                {
                    /* Abort processing. */
                    break;
                }

                result = kzuMessageDispatcherInvoke_internal(registry, currentNode, message, KZU_MESSAGE_ROUTING_BUBBLING);
                kzsErrorForward(result);

                /* TODO: this is unprotected. */
                currentNode = kzuObjectNodeGetParent(currentNode);
            }
        }
    }

    kzsSuccess();
}

kzsError kzuMessageDispatcherDispatch(struct KzuMessageDispatcher* messageDispatcher)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;

    kzsAssert(kzcIsValidPointer(messageDispatcher));

    /* Swap the memory managers and queues. */
    {
        struct KzcMemoryManager* tempManager = messageDispatcher->currentQuickMemoryManager;
        struct KzcDynamicArray* tempQueue = messageDispatcher->currentMessageQueue;
        
        messageDispatcher->currentQuickMemoryManager = messageDispatcher->otherQuickMemoryManager;
        messageDispatcher->otherQuickMemoryManager = tempManager;

        messageDispatcher->currentMessageQueue = messageDispatcher->otherMessageQueue;
        messageDispatcher->otherMessageQueue = tempQueue;
    }

    /* Dispatch all the messages from old queue. */
    it = kzcDynamicArrayGetIterator(messageDispatcher->otherMessageQueue);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuMessage* message = kzcDynamicArrayIteratorGetValue(it);

        if (kzuMessageIsValid(message))
        {
            result = kzuMessageDispatcherSendMessage(messageDispatcher, message);
            kzsErrorForward(result);
        }
    }

    /* Reset old memory manager and queue. */
    result = kzcMemoryManagerResetQuickManager(messageDispatcher->otherQuickMemoryManager);
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(messageDispatcher->otherQuickMemoryManager, &messageDispatcher->otherMessageQueue);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuMessageDispatcherNotifyObjectDeleted(const struct KzuMessageDispatcher* messageDispatcher, const struct KzuObjectNode* objectNode)
{
    kzsError result;
    struct KzcHashMapIterator it;

    kzsAssert(kzcIsValidPointer(messageDispatcher));
    kzsAssert(kzuObjectNodeIsValid(objectNode));

    it = kzcHashMapGetIterator(messageDispatcher->messageTypeMap);
    while (kzcHashMapIterate(it))
    {
        struct KzuMessageSubscriptionRegistry* registry = kzcHashMapIteratorGetValue(it);
        struct KzuMessageSubscriptionSite* subscriptionSite;

        kzsAssert(kzcIsValidPointer(registry));

        if (kzcHashMapGet(registry->siteMap, objectNode, (void**)&subscriptionSite))
        {
            /* Unlink. */
            result = kzcHashMapRemove(registry->siteMap, objectNode);
            kzsErrorForward(result);

            result = kzuMessageDispatcherDereferenceSubscriptionSite_internal(subscriptionSite);
            kzsErrorForward(result);
        }
    }

    /* Cancel all messages where the object is the source, also in both queues. */

    kzsSuccess();
}

static kzsError kzuMessageDispatcherProcessStandardMessages_internal(const struct KzuMessageDispatcher* messageDispatcher, const struct KzuMessage* message)
{
    kzsError result;
    const struct KzuMessageType* messageType = kzuMessageGetType(message);

    KZ_UNUSED_PARAMETER(messageDispatcher);

    if (messageType == KZU_MESSAGE_DELETE_OBJECT)
    {
        struct KzuObjectNode* objectNode = kzuMessageGetVoidArgumentDefault(message, KZU_PROPERTY_TYPE_MESSAGE_ARGUMENT_TARGET_OBJECT);
        if (objectNode != KZ_NULL)
        {
            result = kzuObjectNodeDelete(objectNode);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}
