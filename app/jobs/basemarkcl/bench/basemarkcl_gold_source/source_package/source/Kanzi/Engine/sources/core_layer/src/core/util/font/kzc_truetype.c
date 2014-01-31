/**
* \file
* TrueType font interface.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_truetype.h"
#include "kzc_font_private.h"


kzsError kzcTruetypeSystemInitialize_private(struct KzcTruetypeSystem* truetypeSystem, KzcTruetypeSystemDeletor systemDeletor,
                                             KzcTruetypeFontFileCreator fontFileCreator,
                                             KzcTruetypeFontMemoryCreator fontMemoryCreator)
{
    truetypeSystem->systemDeletor = systemDeletor;
    truetypeSystem->fontFileCreator = fontFileCreator;
    truetypeSystem->fontMemoryCreator = fontMemoryCreator;

    kzsSuccess();
}

kzsError kzcTruetypeSystemDelete(struct KzcTruetypeSystem* truetypeSystem)
{
    kzsError result;

    result = truetypeSystem->systemDeletor(truetypeSystem);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcTruetypeSystemCreateFontFromFile(const struct KzcMemoryManager* memoryManager,
                                             struct KzcTruetypeSystem* truetypeSystem,
                                             kzString filePath, struct KzcFont** out_font)
{
    kzsError result;
    struct KzcFont* font;

    result = truetypeSystem->fontFileCreator(memoryManager, truetypeSystem, filePath, &font);
    kzsErrorForward(result);

    *out_font = font;
    kzsSuccess();
}

kzsError kzcTruetypeSystemCreateFontFromMemory(const struct KzcMemoryManager* memoryManager,
                                               struct KzcTruetypeSystem* truetypeSystem,
                                               const kzByte* data, kzUint size, struct KzcFont** out_font)
{
    kzsError result;
    struct KzcFont* font;

    result = truetypeSystem->fontMemoryCreator(memoryManager, truetypeSystem, data, size, &font);
    kzsErrorForward(result);

    *out_font = font;
    kzsSuccess();
}
