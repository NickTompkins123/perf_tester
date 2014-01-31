/**
* \file
* Project loader for font files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_font.h"

#include "kzu_project_loader.h"
#include "kzu_project_loader_general.h"
#include "kzu_project_loader_image.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/binary/kzu_binary_util.h>
#include <user/kzu_error_codes.h>

#include <core/debug/kzc_log.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/util/image/kzc_image.h>
#include <core/util/font/kzc_font.h>
#include <core/util/font/kzc_bitmap_font.h>
#include <core/util/font/kzc_truetype.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/string/kzc_string.h>
#include <core/memory/kzc_memory_manager.h>


kzsException kzuProjectLoaderLoadBitmapFont(struct KzuProject* project, kzString path, struct KzcBitmapFont** out_bitmapFont)
{
    kzsException result;
    struct KzcBitmapFont* bitmapFont;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load font with null path");

    bitmapFont = (struct KzcBitmapFont*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_FONT, path);

    if (!kzcFontIsValid(kzcBitmapFontToFont(bitmapFont)))
    {
        struct KzcMemoryManager* memoryManager;
        struct KzuBinaryFileInfo* file;
        kzMutableString text;
        struct KzcBitmapFontSystem* fontSystem;

        memoryManager = kzcMemoryGetManager(project);

        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_BITMAP_FONT, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load font file.");

        result = kzuProjectLoaderReadTextFile(memoryManager, file, &text);
        kzsErrorForward(result);

        /* Create font. */
        result = kzuProjectGetBitmapFontSystem(project, &fontSystem);
        kzsErrorForward(result);

        result = kzcBitmapFontCreateFromMemory(memoryManager, fontSystem, kzuProjectGetResourceManager(project), text, &bitmapFont);
        kzsErrorForward(result);

        {
            kzUint slot = 0;
            struct KzcDynamicArrayIterator it = kzcBitmapFontGetFontTextureNames(bitmapFont);
            while(kzcDynamicArrayIterate(it))
            {
                kzMutableString name = (kzMutableString)kzcDynamicArrayIteratorGetValue(it);
                kzString fontDirectoryPath = "Resource Files/Fonts/";
                kzMutableString filePath;
                struct KzcImage* image;
                struct KzcTexture* texture;
                kzBool deleteImage = KZ_TRUE;

                result = kzcStringConcatenate(memoryManager, fontDirectoryPath, name, &filePath);
                kzsErrorForward(result);
                /* If this image was found from project previously (by application developer), it can not be released for texture. */
                if(kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, filePath) != KZ_NULL)
                {
                    deleteImage = KZ_FALSE;
                }
                result = kzuProjectLoaderLoadImage(project, filePath, &image);
                kzsErrorForward(result);
                result = kzcTextureCreateFromImage(kzuProjectGetResourceManager(project), KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM, 
                    KZC_TEXTURE_FILTER_BILINEAR, KZC_TEXTURE_WRAP_CLAMP, 
                    KZC_TEXTURE_COMPRESSION_NONE, image, &texture);
                kzsErrorForward(result);

                if(deleteImage)
                {
                    result = kzuProjectDeleteObject(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, filePath);
                    kzsErrorForward(result);
                }

                result = kzcBitmapFontSetTexture(bitmapFont, texture, slot);
                kzsErrorForward(result);

                result = kzcStringDelete(filePath);
                kzsErrorForward(result);

                slot++;
            }
        }

        result = kzcStringDelete(text);
        kzsErrorForward(result);

        result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_FONT, bitmapFont, kzuBinaryFileInfoGetPath(file),
            (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&bitmapFont);
        kzsErrorForward(result);
    }

    *out_bitmapFont = bitmapFont;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadTruetypeFont(struct KzuProject* project, kzString path, struct KzcFont** out_font)
{
    kzsException result;
    struct KzcFont* font;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load font with null path");

    font = (struct KzcFont*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_FONT, path);

    if (!kzcFontIsValid(font))
    {
        struct KzcMemoryManager* memoryManager;
        struct KzuBinaryFileInfo* file;
        struct KzcTruetypeSystem* fontSystem;
        kzByte* data;
        kzUint size;

        memoryManager = kzcMemoryGetManager(project);

        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_TRUETYPE_FONT, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load font file.");

        size = kzuBinaryFileInfoGetSize(file);

        result = kzuBinaryDirectoryReadFile(memoryManager, file, &data);
        kzsErrorForward(result);

        result = kzuProjectGetTruetypeSystem(project, &fontSystem);
        kzsErrorForward(result);

        result = kzcTruetypeSystemCreateFontFromMemory(memoryManager, fontSystem, data, size, &font);
        kzsErrorForward(result);

        result = kzcMemoryFreeArray(data);
        kzsErrorForward(result);

        result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_FONT, font, kzuBinaryFileInfoGetPath(file),
            (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&font);
        kzsErrorForward(result);
    }

    *out_font = font;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadFont(struct KzuProject* project, kzString path, struct KzcFont** out_font)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcFont* font;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load font with null path");

    /* Check if the image is already loaded. */
    font = (struct KzcFont*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_FONT, path);

    if (!kzcFontIsValid(font))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        /*lint -e{788} Suppress warning about not listing all possible enum values. */
        switch (kzuBinaryFileInfoGetType(file))
        {
            case KZU_BINARY_FILE_TYPE_BITMAP_FONT:
            {
                struct KzcBitmapFont* bitmapFont;

                result = kzuProjectLoaderLoadBitmapFont(project, kzuBinaryFileInfoGetPath(file), &bitmapFont);
                kzsErrorForward(result);

                font = kzcBitmapFontToFont(bitmapFont);
                break;
            }

            case KZU_BINARY_FILE_TYPE_TRUETYPE_FONT:
            {
                result = kzuProjectLoaderLoadTruetypeFont(project, kzuBinaryFileInfoGetPath(file), &font);
                kzsErrorForward(result);
                break;
            }

            default:
            {
                kzsErrorThrow(KZU_ERROR_WRONG_BINARY_FILE_TYPE, "Wrong file type encountered while trying to load font file.");
            }
        }
    }

    *out_font = font;
    kzsSuccess();
}

kzsException kzuProjectLoaderUnloadFont(struct KzuProject* project, const struct KzcFont* font)
{
    kzsError result;
    kzString objectPath = kzuProjectGetObjectPath(project, font);

    if(objectPath != KZ_NULL)
    {
        result = kzuProjectLoaderUnloadObject(project, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
