/**
* \file 
* Settings reader helper functions.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef SETTINGS_H
#define SETTINGS_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcSettingContainer;
struct KzcMemoryManager;
struct KzcSettingContainer;
struct KzcSettingNode;


#define CL_ERROR_INVALID_CONFIGURATION (kzsError)50001 /**< Settings contain invalid data. */


/** Creates settings container. */
kzsError settingsCreate(const struct KzcMemoryManager* memoryManager, kzString pathToSettingsFile, struct KzcSettingContainer** out_settings);
/** Releases settings container. */
kzsError settingsDelete(struct KzcSettingContainer* settings);

/** Gets string value for given setting string from settings data. */
kzsError settingGetString(const struct KzcSettingContainer* settings, kzString settingName, kzString* settingValue);
/** Gets integer value for given setting string from settings data. */
kzsError settingGetInt(const struct KzcSettingContainer* settings, kzString settingName, kzInt* settingValue);
/** Gets float value for given setting string from settings data. */
kzsError settingGetFloat(const struct KzcSettingContainer* settings, kzString settingName, kzFloat* settingValue);
/** Gets node for given setting string from settings data. */
kzsError settingGetNode(const struct KzcSettingContainer* settings, kzString settingName, struct KzcSettingNode** settingValue);

/** Gets string value for given setting string from given setting node. */
kzsError settingGetStringFromNode(const struct KzcSettingNode* settingNode, kzString settingName, kzString* settingValue);
/** Gets integer value for given setting string from given setting node. */
kzsError settingGetIntFromNode(const struct KzcSettingNode* settingNode, kzString settingName, kzInt* settingValue);
/** Gets float value for given setting string from given setting node. */
kzsError settingGetFloatFromNode(const struct KzcSettingNode* settingNode, kzString settingName, kzFloat* settingValue);
/** Gets node for given setting string from given setting node. */
kzsError settingGetNodeFromNode(const struct KzcSettingNode* settingNode, kzString settingName, struct KzcSettingNode** settingValue);

/** Gets float value for given setting string from settings node data. */
kzsError settingNodeGetFloat(const struct KzcSettingNode* settingNode, kzString settingName, kzFloat* settingValue);


#endif
