/**
* \file
* Standard file I/O functions
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_file.h"

#include <system/wrappers/kzs_wrapper.h>
#include <system/wrappers/kzs_math.h>
#include <system/debug/kzs_log.h>

#include <stdio.h>


static const kzInt KZS_FILE_SEEK_TABLE[] = {SEEK_SET, SEEK_CUR, SEEK_END};


kzsFile* kzsFopen(kzString fileName, kzString mode)
{
    void* file = fopen(fileName, mode);
    return (kzsFile*)KZS_WRAP_NULL(file);
}

kzInt kzsFclose(kzsFile* file)
{
    return (kzInt)fclose((FILE*)file);
}

kzBool kzsFeof(kzsFile* file)
{
    return feof((FILE*)file) != 0;
}

kzInt kzsFerror(kzsFile* file)
{
    return (kzInt)ferror((FILE*)file);
}

kzInt kzsFseek(kzsFile* file, kzUint offset, enum KzsFileOrigin origin)
{
    return (kzInt)fseek((FILE*)file, (kzInt)offset, KZS_FILE_SEEK_TABLE[origin]);
}

kzInt kzsFtell(kzsFile* file)
{
    return (kzInt)ftell((FILE*)file);
}

kzInt kzsFflush(kzsFile* file)
{
    return (kzInt)fflush((FILE*)file);
}

kzInt kzsFgetc(kzsFile* file)
{
    return (kzInt)fgetc((FILE*)file);
}

kzInt kzsFputc(kzInt character, kzsFile* file)
{
    return (kzInt)fputc(character, (FILE*)file);
}

kzUint kzsFread(void* buffer, kzUint elementSize, kzUint elementCount, kzsFile* file)
{
    return (kzUint)fread(buffer, elementSize, elementCount, (FILE*)file);
}

kzUint kzsFwrite(const void* buffer, kzUint elementSize, kzUint elementCount, kzsFile* file)
{
    return (kzUint)fwrite(buffer, elementSize, elementCount, (FILE*)file);
}

kzInt kzsFileGetSize(kzString fileName)
{
    FILE* file = fopen(fileName, KZS_FILE_READONLY);
    kzInt result = -1;
    if (file != NULL)
    {
        kzInt seekResult = (kzInt)fseek(file, 0, SEEK_END);
        if (seekResult == 0)
        {
            result = (kzInt)ftell(file);
        }
    }
    
    return result;
}

kzInt kzsFileReadSkip(kzsFile* file, kzUint byteCount)
{
    kzInt result;

    if (feof((FILE*)file))
    {
        result = 0;
    }
    else
    {
        kzInt currentPosition = (kzInt)ftell((FILE*)file);
        kzInt length;
        kzUint skipAmount;
        kzInt seekResult;
        if (currentPosition == -1)
        {
            result = KZS_FILE_ERROR;
        }
        else
        {
            seekResult = (kzInt)fseek((FILE*)file, 0, SEEK_END);
            if (seekResult != 0)
            {
                result = KZS_FILE_ERROR;
            }
            else
            {
                length = (kzInt)ftell((FILE*)file);
                if (length == -1)
                {
                    result = KZS_FILE_ERROR;
                }
                else
                {
                    skipAmount = kzsMinU(byteCount, (kzUint)(length - currentPosition));
                    seekResult = (kzInt)fseek((FILE*)file, currentPosition + (kzInt)skipAmount, SEEK_SET);

                    if (seekResult != 0)
                    {
                        result = KZS_FILE_ERROR;
                    }
                    else
                    {
                        result = (kzInt)skipAmount;
                    }
                }
            }
        }
    }

    return result;
}
