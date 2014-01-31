/**
* \file
* Text object.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_TEXT_OBJECT_H
#define KZU_TEXT_OBJECT_H


#include <user/scene_graph/kzu_object_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuObjectNode;
struct KzuTransformedObjectNode;
struct KzcMemoryManager;
struct KzcFont;
struct KzcTextLayouter;
struct KzuPropertyManager;
struct KzuMessageDispatcher;


/**
* \struct KzuTextObject
* Text object in scene graph, inherited from KzuObjectNode.
*/
struct KzuTextObjectNode;


/** Creates new text object node. */
kzsError kzuTextObjectNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                 kzString name, struct KzuTextObjectNode** out_textObjectNode);

/** Deletes a text object node. */
kzsError kzuTextObjectNodeDelete(struct KzuTextObjectNode* textObjectNode);


/** Casts text object node to object node. */
struct KzuObjectNode* kzuTextObjectNodeToObjectNode(struct KzuTextObjectNode* textObjectNode);
/** Casts object node to text object node. */
struct KzuTextObjectNode* kzuTextObjectNodeFromObjectNode(const struct KzuObjectNode* objectNode);


/** Recreates the text layout if necessary. */
kzsError kzuTextObjectNodeValidate(const struct KzuTextObjectNode* textObjectNode, const struct KzuTransformedObjectNode* transformedObjectNode);


#endif
