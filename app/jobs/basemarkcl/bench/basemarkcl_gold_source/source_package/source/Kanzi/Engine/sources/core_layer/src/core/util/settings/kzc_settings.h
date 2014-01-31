/**
* \file
* Setting interface. This file provides functionality for storing integer, float and string data
* in arbitrary tree structure.
*
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_SETTINGS_H
#define KZC_SETTINGS_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/** Enumeration of the possible node types */
enum KzcSettingType
{
    KZC_SETTING_INTEGER, /**< Integer value type. */
    KZC_SETTING_FLOAT, /**< Floating point value type. */
    KZC_SETTING_STRING, /**< String value type. */
    KZC_SETTING_SUBNODE /**< Sub-node value type. */
};


/* Forward declarations */
struct KzcMemoryManager;
struct KzcDynamicArray;
struct KzcHashMap;


/**
 * \struct KzcSettingContainer
 * Container for a property tree.
 */
struct KzcSettingContainer;

/**
 * \struct KzcSettingNode
 * A single setting node.
 */
struct KzcSettingNode;

/**
 * \struct KzcSetting
 * Polymorphic container for properties.
 */
struct KzcSetting;


/** Allocate settings structure. */
kzsError kzcSettingContainerCreate(const struct KzcMemoryManager* manager, struct KzcSettingContainer** out_settings);
/** Delete settings structure. */
kzsError kzcSettingContainerDelete(struct KzcSettingContainer* settings);

/** Get root node from container. */
struct KzcSettingNode* kzcSettingContainerGetRoot(const struct KzcSettingContainer* settings);

/**
 * Get node from path. The syntax used for path is "root/some/path". If node is not found out_node
 * will be set to null but no error is raised. Error is raised only if memory operations during path parsing fail. 
 * Do not start the path string with '/'.
 */
kzsError kzcSettingContainerGetNode(const struct KzcSettingContainer* settings, kzString path, struct KzcSettingNode** out_node);

/** 
 * Add node as a child to node. This function returns the new node created, unlike the other add functions that
 * take the value of the new setting in as the parameter.
 */
kzsError kzcSettingNodeAddSubNode(const struct KzcSettingNode* node, kzString name, struct KzcSettingNode** out_subNode);
/** Add an integer property as a child to node */
kzsError kzcSettingNodeAddInteger(const struct KzcSettingNode* node, kzString name, kzInt value);
/** Add a float property as a child to node */
kzsError kzcSettingNodeAddFloat(const struct KzcSettingNode* node, kzString name, kzFloat value);
/** Add a string property as a child to node */
kzsError kzcSettingNodeAddString(const struct KzcSettingNode* node, kzString name, kzString value);

/** Try to get child node of 'node' with the key 'name' */
kzBool kzcSettingNodeGetSubNode(const struct KzcSettingNode* node, kzString name, struct KzcSettingNode** out_subNode);
/** Try to get child integer value of 'node' with the key 'name' */
kzBool kzcSettingNodeGetInteger(const struct KzcSettingNode* node, kzString name, kzInt* out_value);
/** Try to get child float value of 'node' with the key 'name' */
kzBool kzcSettingNodeGetFloat(const struct KzcSettingNode* node, kzString name, kzFloat* out_value);
/** Try to get child string value of 'node' with the key 'name' */
kzBool kzcSettingNodeGetString(const struct KzcSettingNode* node, kzString name, kzString* out_value);

/** Try to get child integer value of 'node' with the key 'name'. If not found, return default value. */
kzInt kzcSettingNodeGetIntegerDefault(const struct KzcSettingNode* node, kzString name, kzInt defaultValue);
/** Try to get child float value of 'node' with the key 'name'. If not found, return default value. */
kzFloat kzcSettingNodeGetFloatDefault(const struct KzcSettingNode* node, kzString name, kzFloat defaultValue);
/** Try to get child string value of 'node' with the key 'name'. If not found, return default value. */
kzString kzcSettingNodeGetStringDefault(const struct KzcSettingNode* node, kzString name, kzString defaultValue);

/** Try to remove setting under node. */
kzsError kzcSettingNodeRemoveSetting(const struct KzcSettingNode* node, kzString name);

/* Direct accessors. Preferably use the typed accessors above. */

/** Accessor to node dictionary */
struct KzcHashMap* kzcSettingNodeGetDictionary(const struct KzcSettingNode* node);
/** Accessor to setting data */
void* kzcSettingGetData(struct KzcSetting* setting, enum KzcSettingType* out_type);

/** Get type of node. */
enum KzcSettingType kzcSettingNodeGetType(const struct KzcSettingNode* node, kzString name);


#endif
