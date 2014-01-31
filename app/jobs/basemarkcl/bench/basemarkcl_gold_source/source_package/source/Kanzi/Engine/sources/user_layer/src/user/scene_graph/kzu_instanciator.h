/**
* \file
* Instanciator node, inherited from KzuObjectNode.
* Instanciator allows objects to be rendered in more than one place in the scene.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_INSTANCIATOR_H
#define KZU_INSTANCIATOR_H


#include <user/scene_graph/kzu_object_base.h>

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations */
struct KzuObjectNode;
struct KzcMemoryManager;
struct KzuPropertyManager;
struct KzuMessageDispatcher;

/**
* \struct KzuInstanciatorNode
* Structure for instanciator node, inherited from KzuObjectNode.
*/
struct KzuInstanciatorNode;


/** Object type identifier for Instanciator selector objects. */
extern const KzuObjectType KZU_OBJECT_TYPE_INSTANCIATOR;


/** Creates a new instanciator node. */
kzsError kzuInstanciatorNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                   kzString name, struct KzuInstanciatorNode** out_instanciatorNode);

/** Deletes a instanciator node. */
kzsError kzuInstanciatorNodeDelete(struct KzuInstanciatorNode* instanciatorNode);

/** Assigns distance Instanciator data for Instanciator selector levels. */
kzsError kzuInstanciatorNodeInitialize(const struct KzuInstanciatorNode* instanciatorNode, struct KzuObjectNode* targetObjectNode);


/** Returns Instanciator selector node from object node. */
struct KzuInstanciatorNode* kzuInstanciatorNodeFromObjectNode(const struct KzuObjectNode* objectNode);
/** Returns an object node from Instanciator selector node. */
struct KzuObjectNode* kzuInstanciatorNodeToObjectNode(struct KzuInstanciatorNode* instanciatorNode);


/** Gets Instanciator level object with given constraint (distance, perspective size). */
struct KzuObjectNode* kzuInstanciatorNodeGetTarget(const struct KzuInstanciatorNode* instanciatorNode);


#endif
