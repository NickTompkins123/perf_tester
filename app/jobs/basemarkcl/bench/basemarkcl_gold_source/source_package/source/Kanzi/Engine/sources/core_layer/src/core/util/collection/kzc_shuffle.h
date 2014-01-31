/**
 * \file
 * Generic shiffle functions.
 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_SHUFFLE_H
#define KZC_SHUFFLE_H


#include <system/kzs_types.h>


/* Forward declarations. */
struct KzcRandom;


/** Shuffles the given buffer of elementCount elements. Size of the elements is determined automatically. */
#define kzcShuffleArray(buffer_param, length_param) \
        kzcShuffle(buffer_param, length_param, sizeof(*buffer_param))

/** Shuffles the given buffer of elementCount elements with given random number generator. Size of the elements is determined automatically. */
#define kzcShuffleArrayWithRandom(random_param, buffer_param, length_param) \
        kzcShuffleWithRandom(random_param, buffer_param, length_param, sizeof(*buffer_param))


/** Shuffles the given buffer of elementCount elements, each of size elementSize. */
void kzcShuffle(void* buffer, kzUint elementCount, kzUint elementSize);

/** Shuffles the given buffer of elementCount elements, each of size elementSize. Uses the given random number generator. */
void kzcShuffleWithRandom(struct KzcRandom* random, void* buffer, kzUint elementCount, kzUint elementSize);


#endif
