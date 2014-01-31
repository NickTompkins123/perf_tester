/**
* \file
* Object node base definitions.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_SCENE_GRAPH_OBJECT_BASE_H
#define KZU_SCENE_GRAPH_OBJECT_BASE_H


#include <system/debug/kzs_error.h>


/** Each object type is unique. Implemented with pointers referencing the pointers themselves. */
typedef const void* KzuObjectType;

/** Callback function type for deleting the object data of an object node. */
typedef kzsError (*KzuObjectDataDestructor)(void* objectData);


#endif
