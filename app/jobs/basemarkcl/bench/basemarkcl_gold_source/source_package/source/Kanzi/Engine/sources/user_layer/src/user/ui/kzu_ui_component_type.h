/**
* \file
* User interface component type.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_COMPONENT_TYPE_H
#define KZU_UI_COMPONENT_TYPE_H

#include <user/ui/kzu_ui_component_base.h>

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations. */
struct KzuPropertyTypeCollection;
struct KzuPropertyType;
struct KzuEngineMessageQueue;
struct KzuUiComponentNode;
struct KzuUiEvent;
struct KzuUiAction;
struct KzcMemoryManager;
struct KzcHashMap;


/**
* \struct KzuUiComponentType
* Structure for component type. Component type is used to provide a set of properties and bindings to logic.
*/
struct KzuUiComponentType;


/* Function pointers for attaching custom code to components handles. */
typedef kzsError (*KzuUiComponentInitializeCallback)(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode);
typedef kzsError (*KzuUiComponentUninitializeCallback)(struct KzuUiComponentNode* componentNode);
typedef kzsError (*KzuUiComponentUpdateCallback)(const struct KzuUiComponentNode* componentNode, kzFloat deltaTime, struct KzuEngineMessageQueue* queue);
typedef kzsError (*KzuUiComponentHandleEventsCallback)(const struct KzuUiComponentNode* componentNode, kzString eventType, struct KzuUiEvent* event, struct KzcHashMap* customParameters, struct KzuEngineMessageQueue* queue);


/** User interface component callback type. */
enum KzuUiComponentCallbackType
{
    KZU_UI_COMPONENT_CALLBACK_TYPE_CALLBACK,
    KZU_UI_COMPONENT_CALLBACK_TYPE_SCRIPT,
    KZU_UI_COMPONENT_CALLBACK_TYPE_INHERITED
};

/** Type of the callback. */
enum KzuUiComponentCallback
{
    KZU_UI_COMPONENT_CALLBACK_INITIALIZE,
    KZU_UI_COMPONENT_CALLBACK_UNINITIALIZE,
    KZU_UI_COMPONENT_CALLBACK_LOGIC_UPDATE,
    KZU_UI_COMPONENT_CALLBACK_HANDLE_EVENTS
};


/**
* \struct KzuUiComponentType
* Structure for component type.
*/
struct KzuUiComponentType;


/** Creates new user interface component type. */
kzsError kzuUiComponentTypeCreate(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentType** out_componentType);
/** Deletes user interface component type. */
kzsError kzuUiComponentTypeDelete(struct KzuUiComponentType* componentType);

/** Sets the parent component type. If callbacks of a child component type are not set, the ones from parent are used. */
void kzuUiComponentTypeSetParent(const struct KzuUiComponentType* componentType, struct KzuUiComponentType* parent);

/** Adds new action to the component type. */
kzsError kzuUiComponentTypeAddAction(const struct KzuUiComponentType* componentType, kzString name, struct KzuUiAction* action);


/**
* Takes ownership of the component type container deletion. After this, when the object type is deleted,
* only the data is actually deleted and the container remains alive.
*/
void kzuUiComponentTypeTakeOwnership(const struct KzuUiComponentType* componentType);
/** Checks if the component type is valid (pointer and the data are non-null). */
kzBool kzuUiComponentTypeIsValid(const struct KzuUiComponentType* componentType);
/** Moves the data from source component type to the target component type. */
void kzuUiComponentTypeTransferData(struct KzuUiComponentType* targetComponentType, struct KzuUiComponentType* sourceComponentType);


/** Assigns property types to component type. */
kzsError kzuUiComponentTypeAddPropertyType(const struct KzuUiComponentType* componentType, struct KzuPropertyType* propertyType);

/** Returns property type collection from component type. */
struct KzuPropertyTypeCollection* kzuUiComponentTypeGetPropertyTypeCollection(const struct KzuUiComponentType* componentType);

/** Returns property types of component type. */
struct KzcDynamicArray* kzuUiComponentTypeGetPropertyTypes(const struct KzuUiComponentType* componentType);

/** Returns the inherited callback from the given component type and its parents. */ /* TODO: Why does this return component type instead of callback */
struct KzuUiComponentType* kzuUiComponentTypeGetInheritedCallback(struct KzuUiComponentType* componentType,
                                                                  enum KzuUiComponentCallback callback);

/** Get component type identifier from component type. */
KzuComponentTypeIdentifier kzuUiComponentTypeGetTypeIdentifier(const struct KzuUiComponentType* componentType);
/** Sets component type identifier for component type. This should not be called by user. */
void kzuUiComponentTypeSetTypeIdentifier(const struct KzuUiComponentType* componentType, KzuComponentTypeIdentifier type);


/** 
 * Binds component types initialize to given logic.
 * \param function The callback function to attach.
 */
void kzuUiComponentTypeSetInitializeCallback(const struct KzuUiComponentType* componentType,
                                             KzuUiComponentInitializeCallback function);

/** 
 * Binds component types uninitialize to given logic.
 * \param function The callback function to attach.
 */
void kzuUiComponentTypeSetUninitializeCallback(const struct KzuUiComponentType* componentType,
                                               KzuUiComponentUninitializeCallback function);

/** 
 * Binds component types callback to given logic.
 * \param function The callback function to attach.
 */
void kzuUiComponentTypeSetLogicUpdateCallback(const struct KzuUiComponentType* componentType,
                                              KzuUiComponentUpdateCallback function);

/** 
 * Binds component types handle events callback to given logic.
 * \param function The callback function to attach.
 */
void kzuUiComponentTypeSetHandleEventsCallback(const struct KzuUiComponentType* componentType,
                                               KzuUiComponentHandleEventsCallback function);

/** Binds given script to the component. */
kzsError kzuUiComponentTypeSetCallbackScript(const struct KzcMemoryManager* memoryManager, 
                                             const struct KzuUiComponentType* componentType, 
                                             enum KzuUiComponentCallback callback,
                                             kzString scriptPath);

/** Sets given logic to be inherited on the component. This is the default behaviour. */
kzsError kzuUiComponentTypeSetCallbackInherited(const struct KzuUiComponentType* componentType, 
                                                enum KzuUiComponentCallback callback);

/** Calls the initialize callback function linked to this component type. */
kzsError kzuUiComponentTypeCallCallbackInitialize(const struct KzuUiComponentType* componentType,
                                                  struct KzuUiComponentNode* componentNode, 
                                                  const struct KzcMemoryManager* memoryManager);
/** Calls the uninitialize callback function linked to this component type. */
kzsError kzuUiComponentTypeCallCallbackUninitialize(const struct KzuUiComponentType* componentType, 
                                                    struct KzuUiComponentNode* componentNode);
/** Calls the logic update callback function linked to this component type. */
kzsError kzuUiComponentTypeCallCallbackLogicUpdate(const struct KzuUiComponentType* componentType, 
                                                   const struct KzuUiComponentNode* componentNode, kzFloat deltaTime,
                                                   struct KzuEngineMessageQueue* queue);
/** Calls the handle events callback function linked to this component type. */
kzsError kzuUiComponentTypeCallCallbackHandleEvents(const struct KzuUiComponentType* componentType, 
                                                    const struct KzuUiComponentNode* componentNode, 
                                                    kzString eventType, struct KzuUiEvent* event,
                                                    struct KzuEngineMessageQueue* queue);


#endif
