/**
* \file
* Color buffer based picking utility.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PICK_H
#define KZU_PICK_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations */
struct KzuEngine;
struct KzuObjectSource;
struct KzuObjectNode;
struct KzuCameraNode;
struct KzcMemoryManager;
struct KzuScene;
struct KzuTransformedScene;
struct KzuMaterial;
struct KzuPropertyManager;


/**
* \struct KzuPick
* Structure to hold picking context.
*/
struct KzuPick;


/** Create a new picking context. */
kzsError kzuPickCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, const struct KzuEngine* engine, struct KzuPick** out_pick);
/** Delete a picking context. */
kzsError kzuPickDelete(struct KzuPick* pick);
/* TODO: Explain what is double sided  picking. It seems to be picking with no backface culling. */
/** Enables or disables double-sided picking. */
void kzuPickEnableDoubleSided(struct KzuPick* pick, kzBool enable);
/** Returns KZ_TRUE if double-sided picking is enabled, KZ_FALSE otherwise. */
kzBool kzuPickIsDoubleSided(const struct KzuPick* pick);
/** Perform object picking using a color id buffer. */
kzsError kzuPick(struct KzuPick* pick, const struct KzuEngine* engine, struct KzuObjectSource* pickObjectSource,
                 const struct KzuTransformedScene* transformedScene, struct KzuCameraNode* camera,
                 kzUint positionX, kzUint positionY, struct KzuObjectNode** out_node, kzUint* out_meshCluster);
/** Perform material picking using a material id buffer. */
kzsError kzuPickMaterial(struct KzuPick* pick, const struct KzuEngine* engine, struct KzuObjectSource* pickObjectSource,
                 const struct KzuTransformedScene* transformedScene, struct KzuCameraNode* camera,
                 kzUint positionX, kzUint positionY, struct KzuMaterial** out_material);



#endif
