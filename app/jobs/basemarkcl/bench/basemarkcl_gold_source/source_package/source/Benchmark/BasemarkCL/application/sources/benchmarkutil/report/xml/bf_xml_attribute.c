/**
* \file
* OpenCL benchmark xml document.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#include "bf_xml_attribute.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_output_stream.h>
#include <core/util/string/kzc_string.h>


struct XMLAttribute
{
    kzMutableString name; /**< Name of the attribute. */
    enum XMLAttributeType type; /**< Type of the attribute. */
    union
    {
        kzInt integer; /**< Integer. */
        kzMutableString string; /**< String. */
        kzFloat real; /**< Real number. */
    } value;
};


kzsError XMLAttributeCreate_internal(const struct KzcMemoryManager* memoryManager, kzString name, enum XMLAttributeType type, struct XMLAttribute** out_attribute)
{
    kzsError result;
    struct XMLAttribute* attribute;

    kzsErrorTest(kzcStringLength(name) > 0, KZS_ERROR_ILLEGAL_ARGUMENT, "Name must not be null");

    result = kzcMemoryAllocVariable(memoryManager, attribute, "XML Node Attribute");
    kzsErrorForward(result);

    result = kzcStringCopy(memoryManager, name, &attribute->name);
    kzsErrorForward(result);
    attribute->type = type;

    *out_attribute = attribute;
    kzsSuccess();
}
kzsError XMLAttributeCreateInteger(const struct KzcMemoryManager* memoryManager, kzString name, kzInt value, struct XMLAttribute** out_attribute)
{
    kzsError result;
    struct XMLAttribute* attribute;

    result = XMLAttributeCreate_internal(memoryManager, name, XML_ATTRIBUTE_TYPE_INTEGER, &attribute);
    kzsErrorForward(result);

    attribute->value.integer = value;

    *out_attribute = attribute;
    kzsSuccess();
}

kzsError XMLAttributeCreateString(const struct KzcMemoryManager* memoryManager, kzString name, kzString value, struct XMLAttribute** out_attribute)
{
    kzsError result;
    struct XMLAttribute* attribute;

    result = XMLAttributeCreate_internal(memoryManager, name, XML_ATTRIBUTE_TYPE_STRING, &attribute);
    kzsErrorForward(result);

    result = kzcStringCopy(memoryManager, value, &attribute->value.string);
    kzsErrorForward(result);

    *out_attribute = attribute;
    kzsSuccess();
}

kzsError XMLAttributeCreateFloat(const struct KzcMemoryManager* memoryManager, kzString name, kzFloat value, struct XMLAttribute** out_attribute)
{
    kzsError result;
    struct XMLAttribute* attribute;

    result = XMLAttributeCreate_internal(memoryManager, name, XML_ATTRIBUTE_TYPE_REAL, &attribute);
    kzsErrorForward(result);

    attribute->value.real = value;

    *out_attribute = attribute;
    kzsSuccess();
}

kzsError XMLAttributeDelete(struct XMLAttribute* attribute)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(attribute));
    
    switch(attribute->type)
    {
        case XML_ATTRIBUTE_TYPE_STRING:
        {
            result = kzcStringDelete(attribute->value.string);
            kzsErrorForward(result);
            break;
        }
        case XML_ATTRIBUTE_TYPE_REAL:
        case XML_ATTRIBUTE_TYPE_INTEGER:
        default:
        {
            break;
        }
    }

    result = kzcStringDelete(attribute->name);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(attribute);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError XMLAttributeSetName(struct XMLAttribute* attribute, kzString name)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(attribute));
    kzsErrorTest(kzcStringLength(name) > 0, KZS_ERROR_ILLEGAL_ARGUMENT, "Name must not be null");

    result = kzcStringDelete(attribute->value.string);
    kzsErrorForward(result);
    result = kzcStringCopy(kzcMemoryGetManager(attribute), name, &attribute->value.string);
    kzsErrorForward(result);

    kzsSuccess();
}

kzString XMLAttributeGetName(const struct XMLAttribute* attribute)
{
    kzsAssert(kzcIsValidPointer(attribute));
    return attribute->name;
}

enum XMLAttributeType XMLAttributeGetType(const struct XMLAttribute* attribute)
{
    kzsAssert(kzcIsValidPointer(attribute));
    return attribute->type;
}


kzInt XMLAttributeGetInteger(const struct XMLAttribute* attribute)
{
    kzsAssert(attribute->type == XML_ATTRIBUTE_TYPE_INTEGER);
    kzsAssert(kzcIsValidPointer(attribute));
    return attribute->value.integer;
}

void XMLAttributeSetInteger(struct XMLAttribute* attribute, kzInt value)
{
    kzsAssert(attribute->type == XML_ATTRIBUTE_TYPE_INTEGER);
    kzsAssert(kzcIsValidPointer(attribute));
    attribute->value.integer = value;
}

kzString XMLAttributeGetString(const struct XMLAttribute* attribute)
{
    kzsAssert(attribute->type == XML_ATTRIBUTE_TYPE_STRING);
    kzsAssert(kzcIsValidPointer(attribute));
    return attribute->value.string;
}

kzsError XMLAttributeSetString(struct XMLAttribute* attribute, kzString value)
{
    kzsError result;
    kzsAssert(attribute->type == XML_ATTRIBUTE_TYPE_STRING);
    kzsAssert(kzcIsValidPointer(attribute));

    result = kzcStringDelete(attribute->value.string);
    kzsErrorForward(result);
    result = kzcStringCopy(kzcMemoryGetManager(attribute), value, &attribute->value.string);
    kzsErrorForward(result);

    kzsSuccess();
}

kzFloat XMLAttributeGetFloat(const struct XMLAttribute* attribute)
{
    kzsAssert(attribute->type == XML_ATTRIBUTE_TYPE_REAL);
    kzsAssert(kzcIsValidPointer(attribute));
    return attribute->value.real;
}

void XMLAttributeSetFloat(struct XMLAttribute* attribute, kzFloat value)
{
    kzsAssert(kzcIsValidPointer(attribute));
    attribute->value.real = value;
}
