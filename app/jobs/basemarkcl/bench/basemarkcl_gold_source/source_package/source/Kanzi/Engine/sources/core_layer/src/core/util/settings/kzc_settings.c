/**
* \file
* Setting interface
*
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_settings.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/string/kzc_string.h>

#include <system/debug/kzs_error.h>
#include <system/debug/kzs_log.h>


struct KzcSettingContainer
{
    struct KzcSettingNode* root; /**< root node. */
};

struct KzcSettingNode
{
    struct KzcHashMap* settings; /**< <kzString, KzcSetting>. */
};

struct KzcSetting
{
    enum KzcSettingType type; /**< Setting type. */
    kzMutableString name;     /**< Pointer to name. */
    union                     /**< Contained value. */
    {
        struct KzcSettingNode* node;
        kzInt                  integerValue;
        kzFloat                floatValue;
        kzMutableString        string;
    } data;                   /**< Setting data*/
};


/* Forward declaration */
static kzsError kzcSettingNodeDelete_internal(struct KzcSettingNode* node);


static kzsError kzcSettingNodeCreate_internal(const struct KzcMemoryManager* memoryManager, struct KzcSettingNode** out_node)
{
    kzsError result;
    struct KzcSettingNode* node;

    result = kzcMemoryAllocVariable(memoryManager, node, "SettingNode");
    kzsErrorForward(result);

    result = kzcHashMapCreateOrdered(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &node->settings);
    kzsErrorForward(result);

    *out_node = node;

    kzsSuccess();
}

static kzsError finalizeSettingData_internal(const struct KzcSetting* setting)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(setting));
    switch(setting->type)
    {
        case KZC_SETTING_INTEGER:
        case KZC_SETTING_FLOAT:
        {
            break;
        }
        case KZC_SETTING_STRING:
        {
            result = kzcStringDelete(setting->data.string);
            kzsErrorForward(result);
            break;
        }
        case KZC_SETTING_SUBNODE:
        {
            result = kzcSettingNodeDelete_internal(setting->data.node);
            kzsErrorForward(result);
            break;
        }
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Enum out of range");
        }
    }
    kzsSuccess();
}

static kzsError kzcSettingCreate_internal(const struct KzcMemoryManager* memoryManager, kzString name, struct KzcSetting** out_setting)
{
    kzsError result;
    kzMutableString containerKey;
    struct KzcSetting* setting;

    result = kzcMemoryAllocVariable(memoryManager, setting, "KzcSetting");
    kzsErrorForward(result);

    /* Allocate setting key string. */
    result = kzcStringCopy(memoryManager, name, &containerKey);
    kzsErrorForward(result);
    
    setting->name = containerKey;

    *out_setting = setting;
    kzsSuccess();
}

static kzsError kzcSettingDelete_internal(struct KzcSetting* setting)
{
    kzsError result;

    /* First finalize contained data. */
    result = finalizeSettingData_internal(setting);
    kzsErrorForward(result);

    /*delete setting*/
    result = kzcStringDelete(setting->name);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(setting);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzcSettingNodeDelete_internal(struct KzcSettingNode* node)
{
    kzsError result;
    struct KzcHashMapIterator it;

    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(kzcIsValidPointer(node->settings));

    /* Delete settings and its contents. */
    it = kzcHashMapGetIterator(node->settings);
    while(kzcHashMapIterate(it))
    {
        struct KzcSetting* setting = (struct KzcSetting*) kzcHashMapIteratorGetValue(it);
        /* Delete setting. */
        result = kzcSettingDelete_internal(setting);
        kzsErrorForward(result);
    }

    result = kzcHashMapDelete(node->settings);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(node);
    kzsErrorForward(result);

    kzsSuccess();
}

/** 
 * Allocate a new setting, it's key and store them in the node's settings.
 * The actual data inside the setting is initialized in the calling function.
 */
static kzsError kzcSettingNodeAddSetting_internal(const struct KzcSettingNode* node, kzString name,
                                                 struct KzcSetting** out_setting)
{
    kzsError result;
    struct KzcSetting* setting;
    struct KzcMemoryManager* containerManager = kzcMemoryGetManager(node);

    /* Check if name already exists. */
    if(kzcHashMapGet(node->settings, name, (void**)&setting))
    {
        /* Clear the memory allocated for the stored value. */
        result = finalizeSettingData_internal(setting);
        kzsErrorForward(result);
    }
    else /* Allocate a new setting as old does not exist */
    {
        /* Allocate a new setting to settings. */
        result = kzcSettingCreate_internal(containerManager, name, &setting);
        kzsErrorForward(result);

        /* Push to settings. */
        result = kzcHashMapPut(node->settings, setting->name, setting);
        kzsErrorForward(result);
    }

    *out_setting = setting;
    kzsSuccess();
}

kzsError kzcSettingContainerCreate(const struct KzcMemoryManager* memoryManager, struct KzcSettingContainer** out_settings)
{
    kzsError result;
    struct KzcSettingContainer* settings;

    result = kzcMemoryAllocVariable(memoryManager, settings, "SettingContainer");
    kzsErrorForward(result);

    result = kzcSettingNodeCreate_internal(memoryManager, &settings->root);
    kzsErrorForward(result);

    *out_settings = settings;

    kzsSuccess();
}

kzsError kzcSettingContainerDelete(struct KzcSettingContainer* settings)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(settings));

    result = kzcSettingNodeDelete_internal(settings->root);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(settings);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzcSettingNode* kzcSettingContainerGetRoot(const struct KzcSettingContainer* settings)
{
    kzsAssert(kzcIsValidPointer(settings));
    kzsAssert(kzcIsValidPointer(settings->root));
    return settings->root;
}

kzsError kzcSettingContainerGetNode(const struct KzcSettingContainer* settings, kzString path, struct KzcSettingNode** out_node)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    struct KzcSettingNode* node;
    kzMutableString* paths;
    kzUint pathCount;
    kzUint i;

    kzsAssert(kzcIsValidPointer(settings));
    kzsAssert(path != KZ_NULL);

    memoryManager = kzcMemoryGetManager(settings);

    result = kzcStringSplit(memoryManager, path, "/", &pathCount, &paths);
    kzsErrorForward(result);

    /* Find the node specified by the path. */

    node = kzcSettingContainerGetRoot(settings);

    for(i = 0; i < pathCount; ++i)
    {
        if(node != KZ_NULL)
        {
            if(! kzcSettingNodeGetSubNode(node, paths[i], &node))
            {
                node = KZ_NULL;
                break;
            }
        }
    }

    /* Free used resources. */
    for(i = 0; i < pathCount; i++)
    {
        result = kzcStringDelete(paths[i]);
        kzsErrorForward(result);
    }
    result = kzcMemoryFreeArray(paths);
    kzsErrorForward(result);


    *out_node = node;

    kzsSuccess();
}

kzsError kzcSettingNodeAddSubNode(const struct KzcSettingNode* node, kzString name, struct KzcSettingNode** out_subNode)
{
    kzsError result;
    struct KzcSetting* setting;
    struct KzcSettingNode* subNode;
    struct KzcMemoryManager* memoryManager= kzcMemoryGetManager(node);

    kzsAssert(kzcIsValidPointer(node));

    /* Add setting. */
    result = kzcSettingNodeAddSetting_internal(node, name, &setting);
    kzsErrorForward(result);

     /* Initialize node. */
    result = kzcSettingNodeCreate_internal(memoryManager, &subNode);
    kzsErrorForward(result);

    setting->type = KZC_SETTING_SUBNODE;
    setting->data.node = subNode;

    *out_subNode = subNode;
    kzsSuccess();
}

kzsError kzcSettingNodeAddInteger(const struct KzcSettingNode* node, kzString name, kzInt value)
{
    kzsError result;
    struct KzcSetting* setting;

    kzsAssert(kzcIsValidPointer(node));

    /* Add setting. */
    result = kzcSettingNodeAddSetting_internal(node, name, &setting);
    kzsErrorForward(result);

    setting->type = KZC_SETTING_INTEGER;
    setting->data.integerValue = value;

    kzsSuccess();
}

kzsError kzcSettingNodeAddFloat(const struct KzcSettingNode* node, kzString name, kzFloat value)
{
    kzsError result;
    struct KzcSetting* setting;

    kzsAssert(kzcIsValidPointer(node));

    /* Add setting. */
    result = kzcSettingNodeAddSetting_internal(node, name, &setting);
    kzsErrorForward(result);

    setting->type = KZC_SETTING_FLOAT;
    setting->data.floatValue = value;

    kzsSuccess();
}

kzsError kzcSettingNodeAddString(const struct KzcSettingNode* node, kzString name, kzString value)
{
    kzsError result;
    struct KzcSetting* setting;
    struct KzcMemoryManager* memoryManager= kzcMemoryGetManager(node);

    kzsAssert(kzcIsValidPointer(node));

    /* Add setting. */
    result = kzcSettingNodeAddSetting_internal(node, name, &setting);
    kzsErrorForward(result);

    setting->type = KZC_SETTING_STRING;

    result = kzcStringCopy(memoryManager, value, &(setting->data.string));
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzcSettingNodeGetSubNode(const struct KzcSettingNode* node, kzString name, struct KzcSettingNode** out_subNode)
{
    kzBool result = KZ_FALSE;
    struct KzcSetting* setting;
    struct KzcSettingNode* subNode = KZ_NULL;

    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(kzcIsValidPointer(node->settings));

    if(kzcHashMapGet(node->settings, name, (void**)&setting))
    {
        if(setting->type == KZC_SETTING_SUBNODE)
        {
            subNode = setting->data.node;
            result = KZ_TRUE;
        }
        else
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "Trying to get subnode from setting which is not node.");
        }
    }

    if(result)
    {
        *out_subNode = subNode;
    }
    return result;
}

kzBool kzcSettingNodeGetInteger(const struct KzcSettingNode* node, kzString name, kzInt* out_value)
{
    kzBool result = KZ_FALSE;
    struct KzcSetting* setting;
    kzInt integerValue = 0;

    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(kzcIsValidPointer(node->settings));

    if(kzcHashMapGet(node->settings, name, (void**)&setting))
    {
        if(setting->type == KZC_SETTING_INTEGER)
        {
            integerValue = setting->data.integerValue;
            result = KZ_TRUE;
        }
        else if(setting->type == KZC_SETTING_FLOAT)
        {
            integerValue = (kzInt) setting->data.floatValue;
            result = KZ_TRUE;
        }
        else
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "Trying to get integer value from setting which is not a number.");
            kzsAssert(KZ_FALSE);
        }
    }

    if(result)
    {
        *out_value = integerValue;
    }
    return result;
}

kzBool kzcSettingNodeGetFloat(const struct KzcSettingNode* node, kzString name, kzFloat* out_value)
{
    kzBool result = KZ_FALSE;
    struct KzcSetting* setting;
    kzFloat floatValue = 0.0f;

    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(kzcIsValidPointer(node->settings));

    if(kzcHashMapGet(node->settings, name, (void**)&setting))
    {
        if(setting->type == KZC_SETTING_FLOAT)
        {
            floatValue = setting->data.floatValue;
            result = KZ_TRUE;
        }
        else if(setting->type == KZC_SETTING_INTEGER)
        {
            floatValue = (kzFloat) setting->data.integerValue;
            result = KZ_TRUE;
        }
        else
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "Trying to get float value from setting which is not number.");
            kzsAssert(KZ_FALSE);
        }
    }

    if(result)
    {
        *out_value = floatValue;
    }
    return result;
}

kzBool kzcSettingNodeGetString(const struct KzcSettingNode* node, kzString name, kzString* out_value)
{
    kzBool result = KZ_FALSE;
    struct KzcSetting* setting;
    kzString stringValue = KZ_NULL;

    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(kzcIsValidPointer(node->settings));

    if(kzcHashMapGet(node->settings, name, (void**)&setting))
    {
        if(setting->type == KZC_SETTING_STRING)
        {
            stringValue = setting->data.string;
            result = KZ_TRUE;
        }
        else
        {
            /* TODO: Print string name which failed. */
            kzsLog(KZS_LOG_LEVEL_WARNING, "Trying to get string value from setting which is not string.");
        }
    }

    if(result)
    {
        *out_value = stringValue;
    }
    return result;
}

kzInt kzcSettingNodeGetIntegerDefault(const struct KzcSettingNode* node, kzString name, kzInt defaultValue)
{
    kzInt outValue;
    if(!kzcSettingNodeGetInteger(node, name, &outValue))
    {
        outValue = defaultValue;
    }
    return outValue;
}

kzFloat kzcSettingNodeGetFloatDefault(const struct KzcSettingNode* node, kzString name, kzFloat defaultValue)
{
    kzFloat outValue;
    if(!kzcSettingNodeGetFloat(node, name, &outValue))
    {
        outValue = defaultValue;
    }
    return outValue;
}

kzString kzcSettingNodeGetStringDefault(const struct KzcSettingNode* node, kzString name, kzString defaultValue)
{
    kzString outValue;
    if(!kzcSettingNodeGetString(node, name, &outValue))
    {
        outValue = defaultValue;
    }
    return outValue;
}

kzsError kzcSettingNodeRemoveSetting(const struct KzcSettingNode* node, kzString name)
{
    kzsError result;
    struct KzcSetting* setting;

    /* Check if name exists. */
    if(kzcHashMapGet(node->settings, name, (void**)&setting))
    {
        /* Clear the memory allocated for the stored value. */
        kzsAssert(kzcIsValidPointer(setting));

        result = kzcHashMapRemove(node->settings, name);
        kzsErrorForward(result);

        result = kzcSettingDelete_internal(setting);
        kzsErrorForward(result);
    }
    else
    {
        /* It's illegal to delete something that does not exist */
        kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Setting to remove not found");
    }

    kzsSuccess();
}

/* TODO: Replace this function with one that lists all keys and types of a node. This function allows too dangerous access to the internals of settings. */
struct KzcHashMap* kzcSettingNodeGetDictionary(const struct KzcSettingNode* node)
{
    kzsAssert(kzcIsValidPointer(node));
    return node->settings;
}

void* kzcSettingGetData(struct KzcSetting* setting, enum KzcSettingType* out_type)
{
    void* returnValue = KZ_NULL;

    kzsAssert(kzcIsValidPointer(setting));
    *out_type = setting->type;

    switch(*out_type)
    {
        case KZC_SETTING_INTEGER:
        case KZC_SETTING_FLOAT:
        {
            returnValue = &setting->data;
            break;
        }
        case KZC_SETTING_STRING:
        {
            returnValue = setting->data.string;
            break;
        }
        case KZC_SETTING_SUBNODE:
        {
            returnValue = setting->data.node;
            break;
        }
    }
    return returnValue;
}

enum KzcSettingType kzcSettingNodeGetType(const struct KzcSettingNode* node, kzString name)
{
    struct KzcSetting* setting;
    enum KzcSettingType type = KZ_NULL;

    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(kzcIsValidPointer(node->settings));

    if(kzcHashMapGet(node->settings, name, (void**)&setting))
    {
        type = setting->type;
    }

    return type;
}

/* TODO: consider implementing iterator. */
