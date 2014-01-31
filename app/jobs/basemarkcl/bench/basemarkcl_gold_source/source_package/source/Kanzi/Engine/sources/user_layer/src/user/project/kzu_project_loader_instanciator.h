/**
* \file
* Project loader for level of detail object.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_INSTANCIATOR_H
#define KZU_PROJECT_LOADER_INSTANCIATOR_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzuInstanciatorNode;


/** Loads instanciator node from binary. */
kzsException kzuProjectLoaderLoadInstanciatorNode(struct KzuProject* project, kzString path, struct KzuInstanciatorNode** out_instanciatorNode);


#endif
