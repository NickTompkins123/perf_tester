/**
* \file
* Settings parser interface. This file provides functionality for loading settings hierarchies from files.
*
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_SETTINGS_PARSER_H
#define KZC_SETTINGS_PARSER_H


#include "kzc_settings.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/**
 * Parses a setting hierarchy from given source string.
 *
 * Setting string syntax:
 *
 *  name = value
 *
 * Names cannot contain whitespace or any of the following characters {}()[]=
 * Strings are delimited by "". Any other strings are parsed as numbers. A number is converted to either a float, or an integer.
 * How ever, a number may be retrieved in either format. Nodes can have child nodes inside {}.
 *
 * Lines can be quoted with #
 *
 * For example:
 * ----------------------------------------------
 *
 * # This is an example configuration
 * myFloat = 1.0
 * myInteger = 1
 * myString = "Hello, string!"
 * myNode = {
 *     name = "Inner node"
 *     weight = 65.3 # in kilograms
 * }
 *
 * ----------------------------------------------
 */
kzsError kzcSettingsParseString(const struct KzcSettingContainer* settings, kzString source);


#endif
