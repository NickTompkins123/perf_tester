/**
* \file 
* Settings reader helper functions.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "bf_settings.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/settings/kzc_settings.h>
#include <core/util/settings/kzc_settings_parser.h>
#include <core/debug/kzc_log.h>
#include <core/util/string/kzc_string.h>
#include <core/util/io/kzc_file.h>


/** Error code for failing to read application configuration file. */
#define KZU_ERROR_APPLICATION_CONFIGURATION_READ_FAILED (kzsError)100001


/** Internal helper function for loading a text file from given path. */
static kzsError loadTextFile_internal(const struct KzcMemoryManager* memoryManager, kzString filePath, kzMutableString* out_string);


kzsError settingsCreate(const struct KzcMemoryManager* memoryManager, kzString pathToSettingsFile, struct KzcSettingContainer** out_settings)
{
    kzsError result;
    struct KzcSettingContainer* settings;
    kzMutableString configContents;

    result = kzcSettingContainerCreate(memoryManager, &settings);
    kzsErrorForward(result);
    result = loadTextFile_internal(memoryManager, pathToSettingsFile, &configContents);
    kzsErrorForward(result);
    result = kzcSettingsParseString(settings, configContents);
    kzsErrorForward(result);
    result = kzcStringDelete(configContents);
    kzsErrorForward(result);

    *out_settings = settings;
    kzsSuccess();
}

kzsError settingsDelete(struct KzcSettingContainer* settings)
{
    kzsError result;
    result = kzcSettingContainerDelete(settings);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError settingGetString(const struct KzcSettingContainer* settings, kzString settingName, kzString* settingValue)
{
    kzsError result;
    struct KzcSettingNode* settingRootNode = kzcSettingContainerGetRoot(settings);

    if(!kzcSettingNodeGetString(settingRootNode, settingName, settingValue))
    {
        struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(settings);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_ERROR, "String value '%s' not found in settings file.", settingName);
        kzsErrorForward(result);
        kzsErrorThrow(KZU_ERROR_APPLICATION_CONFIGURATION_READ_FAILED, "Failed to read setting from configuration file");
    }

    kzsSuccess();
}

kzsError settingGetInt(const struct KzcSettingContainer* settings, kzString settingName, kzInt* settingValue)
{
    kzsError result;
    struct KzcSettingNode* settingRootNode = kzcSettingContainerGetRoot(settings);

    result = settingGetIntFromNode(settingRootNode, settingName, settingValue);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError settingGetFloat(const struct KzcSettingContainer* settings, kzString settingName, kzFloat* settingValue)
{
    kzsError result;
    struct KzcSettingNode* settingRootNode = kzcSettingContainerGetRoot(settings);

    result = settingGetFloatFromNode(settingRootNode, settingName, settingValue);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError settingGetNode(const struct KzcSettingContainer* settings, kzString settingName, struct KzcSettingNode** settingValue)
{
    kzsError result;
    struct KzcSettingNode* settingRootNode = kzcSettingContainerGetRoot(settings);

    result = settingGetNodeFromNode(settingRootNode, settingName, settingValue);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError settingGetStringFromNode(const struct KzcSettingNode* settingNode, kzString settingName, kzString* settingValue)
{
    kzsError result;

    if(!kzcSettingNodeGetString(settingNode, settingName, settingValue))
    {
        struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(settingNode);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_ERROR, "String value '%s' not found in settings file.", settingName);
        kzsErrorForward(result);
        kzsErrorThrow(KZU_ERROR_APPLICATION_CONFIGURATION_READ_FAILED, "Failed to read setting from configuration file");
    }

    kzsSuccess();
}

kzsError settingGetIntFromNode(const struct KzcSettingNode* settingNode, kzString settingName, kzInt* settingValue)
{
    kzsError result;

    if(!kzcSettingNodeGetInteger(settingNode, settingName, settingValue))
    {
        struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(settingNode);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_ERROR, "Integer value '%s' not found in settings file.", settingName);
        kzsErrorForward(result);
        kzsErrorThrow(KZU_ERROR_APPLICATION_CONFIGURATION_READ_FAILED, "Failed to read setting from configuration file");
    }

    kzsSuccess();
}

kzsError settingGetFloatFromNode(const struct KzcSettingNode* settingNode, kzString settingName, kzFloat* settingValue)
{
    kzsError result;
    if(!kzcSettingNodeGetFloat(settingNode, settingName, settingValue))
    {
        struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(settingNode);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_ERROR, "Float value '%s' not found in settings file.", settingName);
        kzsErrorForward(result);
        kzsErrorThrow(KZU_ERROR_APPLICATION_CONFIGURATION_READ_FAILED, "Failed to read setting from configuration file");
    }

    kzsSuccess();
}

kzsError settingGetNodeFromNode(const struct KzcSettingNode* settingNode, kzString settingName, struct KzcSettingNode** settingValue)
{
    kzsError result;

    if(!kzcSettingNodeGetSubNode(settingNode, settingName, settingValue))
    {
        struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(settingNode);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_ERROR, "Node '%s' not found in settings file.", settingName);
        kzsErrorForward(result);
        kzsErrorThrow(KZU_ERROR_APPLICATION_CONFIGURATION_READ_FAILED, "Failed to read setting from configuration file");
    }

    kzsSuccess();
}

static kzsError loadTextFile_internal(const struct KzcMemoryManager* memoryManager, kzString filePath, kzMutableString* out_string)
{
    kzsError result;
    kzMutableString* lines;
    kzMutableString string;
    kzMutableString resultString;

    result = kzcFileReadTextResourceLines(memoryManager, filePath, &lines);
    kzsErrorForward(result);
    result = kzcStringJoin(memoryManager, kzcArrayLength(lines), lines, "\n", &string);
    kzsErrorForward(result);
    result = kzcStringCopy(memoryManager, string, &resultString);
    kzsErrorForward(result);
    result = kzcStringDelete(string);
    kzsErrorForward(result);

    {
        kzUint index;
        kzUint arrayLength = kzcArrayLength(lines);
        for(index=0; index<arrayLength; ++index)
        {
            result = kzcStringDelete(lines[index]);
            kzsErrorForward(result);
        }
        result = kzcMemoryFreeArray(lines);
        kzsErrorForward(result);
    }

    *out_string = resultString;
    kzsSuccess();
}

kzsError settingNodeGetFloat(const struct KzcSettingNode* settingNode, kzString settingName, kzFloat* settingValue)
{
    kzsError result;

    if(!kzcSettingNodeGetFloat(settingNode, settingName, settingValue))
    {
        struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(settingNode);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_ERROR, "Float value '%s' not found in settings file.", settingName);
        kzsErrorForward(result);
        kzsErrorThrow(KZU_ERROR_APPLICATION_CONFIGURATION_READ_FAILED, "Failed to read setting from configuration file");
    }

    kzsSuccess();
}
