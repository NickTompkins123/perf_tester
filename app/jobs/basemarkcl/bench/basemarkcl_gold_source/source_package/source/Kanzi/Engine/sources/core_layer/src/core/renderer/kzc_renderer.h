/**
* \file
* Core renderer. Contains the graphics state machine.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_RENDERER_H
#define KZC_RENDERER_H


#include <system/wrappers/kzs_opengl_base.h>


/* Include correct renderer implementation depending on the OpenGL version. */
#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
#include "kzc_renderer_es1.h"
#elif KZ_OPENGL_VERSION == KZ_OPENGL_ES_2_0
#include "kzc_renderer_es2.h"
#elif KZ_OPENGL_VERSION == KZ_OPENGL_2_1
#include "kzc_renderer_gl21.h"
#elif KZ_OPENGL_VERSION == KZ_OPENGL_NONE
#include "kzc_renderer_es1.h"
#else
#error KZ_OPENGL_VERSION undefined or unknown
#endif


#endif
