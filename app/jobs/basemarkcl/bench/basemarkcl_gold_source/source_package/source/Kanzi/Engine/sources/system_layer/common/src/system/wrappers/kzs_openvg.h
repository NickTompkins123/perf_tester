/**
* \file
* OpenVG wrapper
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZ_OPENVG_H
#define KZ_OPENVG_H


#include "kzs_openvg_base.h"


#ifdef KZ_OPENVG_VERSION
#if KZ_OPENVG_VERSION == KZ_OPENGV_1_1
#include "kzs_openvg11.h"
#else
#error Unsupported OpenVG version
#endif
#endif

#endif
