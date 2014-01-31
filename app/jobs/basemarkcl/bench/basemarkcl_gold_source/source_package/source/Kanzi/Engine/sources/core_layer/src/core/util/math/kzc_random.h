/**
* \file
* Pseudo-random number generators.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_RANDOM_H
#define KZC_RANDOM_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;


/**
 * \struct KzcRandom
 * Structure for pseudo-random number generator.
 */
struct KzcRandom
{
    kzS32 seed_private; /**< Seed of the pseudo random number generator. This is modified with every call to random functions. */
};


/** Creates a new pseudo random number generator with the given seed. */
kzsError kzcRandomCreate(const struct KzcMemoryManager* memoryManager, kzS32 seed, struct KzcRandom** out_random);

/** Creates a new pseudo random number generator with a seed based on current system time. */
kzsError kzcRandomCreateDefault(const struct KzcMemoryManager* memoryManager, struct KzcRandom** out_random);

/** Frees the memory allocated for the given pseudo random number generator. */
kzsError kzcRandomDelete(struct KzcRandom* random);

/** Returns a pseudo random number generator without allocating memory for it. Uses the given seed. */
struct KzcRandom kzcRandomInline(kzS32 seed);

/** Returns a pseudo random number generator without allocating memory for it. Uses a seed based on current system time. */
struct KzcRandom kzcRandomInlineDefault(void);

/** Sets the random seed */
void kzcRandomSetSeed(struct KzcRandom* random, kzS32 seed);

/** Returns a random float in the range [0,1[. */
kzFloat kzcRandomFloat(struct KzcRandom* random);

/** Returns a random integer in the range [min,max[. This function provides only 16 bit accuracy */
kzInt kzcRandomInteger(struct KzcRandom* random, kzInt min, kzInt max);


#endif
