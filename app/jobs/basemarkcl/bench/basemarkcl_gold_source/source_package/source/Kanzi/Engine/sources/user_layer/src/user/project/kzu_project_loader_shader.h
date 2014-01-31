/**
* \file
* Project loader for shader.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_SHADER_H
#define KZU_PROJECT_LOADER_SHADER_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;


/** Loads shader from binary. */
kzsException kzuProjectLoaderLoadShaderSource(const struct KzuProject* project, kzString path, kzMutableString* out_shaderSource);


#endif
