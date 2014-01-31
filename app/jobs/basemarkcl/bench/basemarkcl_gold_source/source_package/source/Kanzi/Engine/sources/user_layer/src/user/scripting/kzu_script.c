/**
* \file
* Script container.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_script.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>
#include <core/util/io/kzc_file.h>
#include <core/debug/kzc_log.h>

#include <system/debug/kzs_log.h>
#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_math.h>


struct KzuScript
{
    struct KzuScriptData* data;
};

struct KzuScriptData
{
    kzBool selfOwned;
    kzMutableString name; /**< Name of the script file. */
    enum KzuScriptingLanguage language; /**< The language script is using. */
    enum KzuScriptDataType dataType;    /**< Type of the script */
    union
    {
        kzMutableString text;   /**< Script source. */
        struct
        {
            kzUint size;               /**< Size of the binary code. */
            kzChar* data;              /**< Binary code for script. */
        } binary;                      /**< Binary separated data. */
    } data;                     /**< Union to data. */
};



kzsError kzuScriptCreateBinary(const struct KzcMemoryManager* memoryManager, kzString name, enum KzuScriptingLanguage language, const void* data, kzUint dataSize, 
                               struct KzuScript** out_script)
{
    kzsError result;
    struct KzuScript* script;

    result = kzcMemoryAllocVariable(memoryManager, script, "Script");
    kzsErrorForward(result);
    result = kzcMemoryAllocVariable(memoryManager, script->data, "Script data");
    kzsErrorForward(result);
    
    script->data->selfOwned = KZ_TRUE;
    script->data->language = language;
    script->data->dataType = KZU_SCRIPT_DATA_TYPE_BINARY;
    script->data->data.binary.size = dataSize;

    result = kzcMemoryAllocArray(memoryManager, script->data->data.binary.data, dataSize, "Binary shader data");
    kzsErrorForward(result);
    kzsMemcpy(script->data->data.binary.data, data, dataSize);

    result = kzcStringCopy(memoryManager, name, &script->data->name);
    kzsErrorForward(result);

    *out_script = script;
    kzsSuccess();
}

kzsError kzuScriptCreateSource(const struct KzcMemoryManager* memoryManager, kzString name, enum KzuScriptingLanguage language, kzString text, struct KzuScript** out_script)
{
    kzsError result;
    struct KzuScript* script;

    result = kzcMemoryAllocVariable(memoryManager, script, "Script");
    kzsErrorForward(result);
    result = kzcMemoryAllocVariable(memoryManager, script->data, "Script data");
    kzsErrorForward(result);

    script->data->selfOwned = KZ_TRUE;
    script->data->language = language;
    script->data->dataType = KZU_SCRIPT_DATA_TYPE_SOURCE;

    result = kzcStringCopy(memoryManager, text, &script->data->data.text);
    kzsErrorForward(result);

    result = kzcStringCopy(memoryManager, name, &script->data->name);
    kzsErrorForward(result);

    *out_script = script;
    kzsSuccess();
}

kzsError kzuScriptDelete(struct KzuScript* script)
{
    kzsError result;
    struct KzuScriptData* scriptData;
    kzBool selfOwned;
    
    kzsAssert(kzcIsValidPointer(script));
    kzsAssert(kzcIsValidPointer(script->data));
    
    scriptData = script->data;
    selfOwned = scriptData->selfOwned;

    switch(script->data->dataType)
    {
        case KZU_SCRIPT_DATA_TYPE_BINARY:
        {
            result = kzcMemoryFreeArray(script->data->data.binary.data);
            kzsErrorForward(result);
            break;
        }
        case KZU_SCRIPT_DATA_TYPE_SOURCE:
        {
            result = kzcStringDelete(script->data->data.text);
            kzsErrorForward(result);
            break;
        }
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Script data type unknown.");
        }
    }

    if(scriptData->name != KZ_NULL)
    {
        result = kzcStringDelete(scriptData->name);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(scriptData);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(script);
        kzsErrorForward(result);  
    }
    else
    {
        script->data = KZ_NULL;
    }

    kzsSuccess();
}

enum KzuScriptDataType kzuScriptGetDataType(const struct KzuScript* script)
{
    kzsAssert(kzcIsValidPointer(script));
    return script->data->dataType;
}

enum KzuScriptingLanguage kzuScriptGetLanguage(const struct KzuScript* script)
{
    kzsAssert(kzcIsValidPointer(script));
    return script->data->language;
}

kzsError kzuScriptGetSource(const struct KzuScript* script, kzString* out_source)
{
    kzsAssert(kzcIsValidPointer(script));
    kzsErrorTest(script->data->dataType == KZU_SCRIPT_DATA_TYPE_SOURCE, KZS_ERROR_ILLEGAL_OPERATION, "Script dataType must be source.");
    *out_source = script->data->data.text;
    kzsSuccess();
}

kzsError kzuScriptGetBinary(const struct KzuScript* script, void** out_binary, kzUint* out_size)
{
    kzsAssert(kzcIsValidPointer(script));
    kzsErrorTest(script->data->dataType == KZU_SCRIPT_DATA_TYPE_BINARY, KZS_ERROR_ILLEGAL_OPERATION, "Script dataType must be binary.");
    *out_binary = script->data->data.binary.data;
    *out_size = script->data->data.binary.size;
    kzsSuccess();
}


/**
 * Takes ownership of the script container deletion. After this, when a script is deleted,
 * only the data is actually deleted and the container remains alive.
 */
void kzuScriptTakeOwnership(const struct KzuScript* script)
{
    kzsAssert(kzuScriptIsValid(script));

    script->data->selfOwned = KZ_FALSE;
}

kzBool kzuScriptIsValid(const struct KzuScript* script)
{
    return script != KZ_NULL && script->data != KZ_NULL;
}

void kzuScriptTransferData(struct KzuScript* targetScript, struct KzuScript* sourceScript)
{
    kzsAssert(kzcIsValidPointer(targetScript));
    kzsAssert(targetScript->data == KZ_NULL);
    kzsAssert(kzuScriptIsValid(sourceScript));

    targetScript->data = sourceScript->data;
    sourceScript->data = KZ_NULL;
}
