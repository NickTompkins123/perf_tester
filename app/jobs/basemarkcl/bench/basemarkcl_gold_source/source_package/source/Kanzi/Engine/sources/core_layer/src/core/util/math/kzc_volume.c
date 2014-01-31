/**
* \file
* 3-dimensional volume.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_volume.h"


const struct KzcVolume KZC_VOLUME_EMPTY = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
const struct KzcVolume KZC_VOLUME_INFINITE = { -KZ_FLOAT_MAXIMUM, -KZ_FLOAT_MAXIMUM, -KZ_FLOAT_MAXIMUM,
                                               KZ_FLOAT_MAXIMUM, KZ_FLOAT_MAXIMUM, KZ_FLOAT_MAXIMUM };

/* TODO: Rename to box */
