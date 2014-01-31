/**
* \file
* OpenCL benchmark xml node attribute.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef BF_XML_ATTRIBUTE_H
#define BF_XML_ATTRIBUTE_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/** XML Attribute type. */
enum XMLAttributeType
{
    XML_ATTRIBUTE_TYPE_INTEGER = 0, /**< Attribute contains integer value. */
    XML_ATTRIBUTE_TYPE_STRING, /**< Attribute contains string. */
    XML_ATTRIBUTE_TYPE_REAL /**< Attribute contains real number. */
};


/* Forward declarations. */
struct KzcMemoryManager;


/** 
 * \struct XMLAttribute
 * XML attribute structure. 
 */
struct XMLAttribute;


/** Create XML attribute for integer data. */
kzsError XMLAttributeCreateInteger(const struct KzcMemoryManager* memoryManager, kzString name, kzInt value, struct XMLAttribute** out_attribute);
/** Create XML attribute for strings data. */
kzsError XMLAttributeCreateString(const struct KzcMemoryManager* memoryManager, kzString name, kzString value, struct XMLAttribute** out_attribute);
/** Create XML attribute for float data. */
kzsError XMLAttributeCreateFloat(const struct KzcMemoryManager* memoryManager, kzString name, kzFloat value, struct XMLAttribute** out_attribute);

/** Deletes XML attributes. */
kzsError XMLAttributeDelete(struct XMLAttribute* attribute);

/** Set XML attribute name. */
kzsError XMLAttributeSetName(struct XMLAttribute* attribute, kzString name);
/** Get XML attribute name. */
kzString XMLAttributeGetName(const struct XMLAttribute* attribute);

/** Get XML attribute type. */
enum XMLAttributeType XMLAttributeGetType(const struct XMLAttribute* attribute);

/** Sets integer value from XML attribute. */
kzInt XMLAttributeGetInteger(const struct XMLAttribute* attribute);
/** Sets integer value to XML attribute. */
void XMLAttributeSetInteger(struct XMLAttribute* attribute, kzInt value);
/** Gets string value from XML attribute. */
kzString XMLAttributeGetString(const struct XMLAttribute* attribute);
/** Sets string value to XML attribute. */
kzsError XMLAttributeSetString(struct XMLAttribute* attribute, kzString value);
/** Gets float value from XML attribute. */
kzFloat XMLAttributeGetFloat(const struct XMLAttribute* attribute);
/** Sets float value to XML attribute. */
void XMLAttributeSetFloat(struct XMLAttribute* attribute, kzFloat value);


#endif
