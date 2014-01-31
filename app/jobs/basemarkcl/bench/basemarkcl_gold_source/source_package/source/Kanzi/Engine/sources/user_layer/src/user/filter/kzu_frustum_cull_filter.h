/**
* \file
* Specifies frustum cull filter.
* Frustum cull filter accepts objects whose bounding volume is at least partially inside current view frustum.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_FRUSTUM_CULL_FILTER_H
#define KZU_FRUSTUM_CULL_FILTER_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;
struct KzuObjectSource;


/** Creates frustum cull filter. */
kzsError kzuFrustumCullFilterCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* input,
                                    struct KzuObjectSource** out_objectSource);


#endif
