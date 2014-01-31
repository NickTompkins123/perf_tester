/**
* \file
* TrueType font interface.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_TRUETYPE_FONT_H
#define KZC_TRUETYPE_FONT_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations. */
struct KzcFont;
struct KzcMemoryManager;


/**
 * \struct KzcTruetypeSystem
 * Font system base for truetype font implementations.
 */
struct KzcTruetypeSystem;


/** Deletes a TrueType font system. */
kzsError kzcTruetypeSystemDelete(struct KzcTruetypeSystem* truetypeSystem);

/** Creates a TrueType font from file using the given TrueType font system. */
kzsError kzcTruetypeSystemCreateFontFromFile(const struct KzcMemoryManager* memoryManager,
                                             struct KzcTruetypeSystem* truetypeSystem,
                                             kzString filePath, struct KzcFont** out_font);

/** Creates a TrueType font from memory using the given TrueType font system. */
kzsError kzcTruetypeSystemCreateFontFromMemory(const struct KzcMemoryManager* memoryManager,
                                               struct KzcTruetypeSystem* truetypeSystem,
                                               const kzByte* data, kzUint size, struct KzcFont** out_font);


#endif
