/**
* \file
* User interface component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_COMPONENT_H
#define KZU_UI_COMPONENT_H

#include <user/ui/kzu_ui_component_base.h>
#include <user/scene_graph/kzu_object_base.h>

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations. */
struct KzuProperty;
struct KzuPropertyType;
struct KzuPropertyManager;
struct KzuMessageDispatcher;
struct KzuEngineMessageQueue;
struct KzuUiComponentType;
struct KzuUiAction;
struct KzuUiEvent;
struct KzuUiEventListener;
struct KzuTransformedObjectNode;
struct KzuObjectNode;
struct KzuScene;
struct KzcMemoryManager;
struct KzcVector3;
struct KzcDynamicArray;


/**
* \struct KzuUiComponentNode
* Structure for graphical user interface component node, inherited from KzuObjectNode.
*/
struct KzuUiComponentNode;


/** Component type identifier for empty components. */
extern const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_EMPTY;

/** Object type identifier for component objects. */
extern const KzuObjectType KZU_OBJECT_TYPE_UI_COMPONENT;



/** Measure function prototype. */
typedef kzsError (*KzuUiComponentMeasureFunction)(struct KzuTransformedObjectNode* node);
/** Arrange function prototype. */
typedef kzsError (*KzuUiComponentArrangeFunction)(const struct KzuScene* scene, struct KzuTransformedObjectNode* node, struct KzuTransformedObjectNode* transformedParentNode);
/** Layout copy function prototype. */
typedef kzsError (*KzuUiComponentSpecificCopyFunction)(const struct KzuObjectNode* sourceNode, struct KzuObjectNode* targetNode);


/** Creates UI component node for scene graph. */
kzsError kzuUiComponentNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                  kzString path, struct KzuUiComponentType* componentType, struct KzuUiComponentNode** out_componentNode);

/** Deletes a UI component node. */
kzsError kzuUiComponentNodeDelete(struct KzuUiComponentNode* uiComponentNode);


/** Sets the component type of the UI component. */
void kzuUiComponentNodeSetComponentType(const struct KzuUiComponentNode* componentNode, struct KzuUiComponentType* componentType);


/** Initialize user interface component. */
kzsError kzuUiComponentNodeInitialize(struct KzuUiComponentNode* componentNode, const struct KzcMemoryManager* memoryManager);
/** Uninitialize user interface component. */
kzsError kzuUiComponentNodeUninitialize(struct KzuUiComponentNode* componentNode);


/** Updates UI component logic. */
kzsError kzuUiComponentNodeLogicUpdate(const struct KzuUiComponentNode* componentNode, kzFloat deltaTime,
                                   struct KzuEngineMessageQueue* queue);
/** Propagates events to components in this components hierarchy. */
kzsError kzuUiComponentNodeHandleEvents(const struct KzuUiComponentNode* componentNode, struct KzuUiEvent* event, 
                                    kzString eventType, struct KzuEngineMessageQueue* queue);


/** Binds event listener to the event. */
kzsError kzuUiComponentNodeAddEventListener(const struct KzuUiComponentNode* componentNode, kzString event,
                                            struct KzuUiEventListener* eventListener);
/** Removes event listener. */
kzsError kzuUiComponentNodeRemoveEventListener(const struct KzuUiComponentNode* componentNode, kzString event,
                                            const struct KzuUiEventListener* eventListener);
/** Gets event listeners which are targeting actions in components as dynamic array from component. */
kzsError kzuUiComponentNodeGetEventListeners(const struct KzcMemoryManager* memoryManager,
                                             const struct KzuUiComponentNode* componentNode,
                                             struct KzcDynamicArray** out_array);


/** Casts UI component node from object node. */
struct KzuUiComponentNode* kzuUiComponentNodeFromObjectNode(const struct KzuObjectNode* objectNode);
/** Casts UI component node to object node. */
struct KzuObjectNode* kzuUiComponentNodeToObjectNode(const struct KzuUiComponentNode* componentNode);

/** Gets UI component type from component node. */
struct KzuUiComponentType* kzuUiComponentNodeGetType(const struct KzuUiComponentNode* componentNode);

/** Set a measure function for an object. */
void kzuUiComponentNodeSetMeasureFunction(const struct KzuUiComponentNode* componentNode, KzuUiComponentMeasureFunction function);
/** Set an arrange function for an object. */
void kzuUiComponentNodeSetArrangeFunction(const struct KzuUiComponentNode* componentNode, KzuUiComponentArrangeFunction function);
/** Set a measure function for an object. */
void kzuUiComponentNodeSetSpecificCopyFunction(const struct KzuUiComponentNode* componentNode, KzuUiComponentSpecificCopyFunction function);
/** Get the measure function of object. */
KzuUiComponentMeasureFunction kzuUiComponentNodeGetMeasureFunction(const struct KzuUiComponentNode* componentNode);
/** Get the arrange function of object. */
KzuUiComponentArrangeFunction kzuUiComponentNodeGetArrangeFunction(const struct KzuUiComponentNode* componentNode);
/** Get the arrange function of object. */
KzuUiComponentSpecificCopyFunction kzuUiComponentNodeGetSpecificCopyFunction(const struct KzuUiComponentNode* componentNode);

/** Checks if given object can be found from parentObjects descendants and is above another UI Component. */
kzBool kzuUiFindChildNode(const struct KzuObjectNode* parentObject, const struct KzuObjectNode* objectToFind);


#endif
