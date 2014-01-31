/**
* \file
* OpenGL wrapper.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_OPENGL_H
#define KZS_OPENGL_H


#include "kzs_opengl_base.h"


#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
#include "kzs_opengles11.h"
#elif KZ_OPENGL_VERSION == KZ_OPENGL_ES_2_0
#include "kzs_opengles20.h"
#elif KZ_OPENGL_VERSION == KZ_OPENGL_2_1
#include "kzs_opengl21.h"
#elif KZ_OPENGL_VERSION == KZ_OPENGL_NONE
#include "kzs_opengl_none.h"
#else
#error KZ_OPENGL_VERSION undefined or unknown
#endif


#endif
