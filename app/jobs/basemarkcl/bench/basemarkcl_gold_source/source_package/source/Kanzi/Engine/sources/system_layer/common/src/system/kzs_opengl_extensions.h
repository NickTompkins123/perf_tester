/**
* \file
* OpenGL extension wrapper
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/

#ifndef KZS_OPENGL_EXTENSIONS_H_
#define KZS_OPENGL_EXTENSIONS_H_


#include <system/kzs_types.h>


/** Gets the funtion pointer address for given string. */
void (*kzsGLGetProcAddress(kzString functionName))(void);


#endif
