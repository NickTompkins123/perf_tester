/**
* \file
* Dynamic mesh for Kanzi Engine.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_DYNAMIC_MESH
#define KZU_DYNAMIC_MESH


#include <user/scene_graph/kzu_object_base.h>
#include <user/scene_graph/kzu_object_private.h>
#include <user/renderer/kzu_renderer.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzuObjectNode;
struct KzuRenderer;
struct KzuTransformedObjectNode;
struct KzcMemoryManager;


/** Dynamic mesh node for scene graph. */
struct DynamicMeshNode;


/** User callback for rendering the object. */
typedef kzsError (*DynamicMeshRenderCallback)(struct KzuRenderer* renderer, const struct KzuTransformedObjectNode* transformedObjectNode, void* userData);

/** Create dynamic mesh. */
kzsError dynamicMeshNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                               struct DynamicMeshNode** out_dynamicMeshNode);

/** Delete dynamic mesh. */
kzsError dynamicMeshNodeDelete(struct DynamicMeshNode* dynamicMeshNode);

/** Render the dynamic mesh. */
kzsError dynamicMeshRender(struct KzuRenderer* renderer, const struct KzuTransformedObjectNode* transformedObjectNode);

/** Set time for updating dynamic mesh. */
void dynamicMeshSetTime(const struct DynamicMeshNode* dynamicMeshNode, const kzFloat time);

/** Set user callback and user data for mesh. */
void dynamicMeshSetCallback(const struct DynamicMeshNode* dynamicMeshNode, DynamicMeshRenderCallback callback, void* userData);


#endif
