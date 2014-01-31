/**
* \file
* Project loader for script files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_SCRIPT_H
#define KZU_PROJECT_LOADER_SCRIPT_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzuScript;


/** Loads script data structure from binary. */
kzsException kzuProjectLoaderLoadScript(const struct KzuProject* project, kzString path, struct KzuScript** out_script);


#endif
