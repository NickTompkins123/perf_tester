/**
* \file
* OpenCL benchmark xml document node.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "bf_xml_node.h"

#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/string/kzc_string.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_output_stream.h>
#include <core/util/string/kzc_string.h>

#include <core/debug/kzc_log.h>

#include "bf_xml_attribute.h"

#include "stdio.h"


struct XMLNode
{
    kzMutableString name; /**< Name of the node. */

    enum XMLNodeType type; /**< Type of the XML node. */

    union
    {
        kzInt integer; /**< Integer. */
        kzMutableString string; /**< String. */
        kzFloat real; /**< Real number. */
        struct KzcDynamicArray* children; /**< XML Objects <struct XMLNode>. */
    } value;

    struct KzcDynamicArray* attributes; /**< Node attributes <struct XMLAttribute>. */
};


kzsError XMLNodeCreate_internal(const struct KzcMemoryManager* memoryManager, kzString name, enum XMLNodeType type, struct XMLNode** out_node)
{
    kzsError result;
    struct XMLNode* node;

    kzsErrorTest(kzcStringLength(name) > 0, KZS_ERROR_ILLEGAL_ARGUMENT, "Name must not be null");

    result = kzcMemoryAllocVariable(memoryManager, node, "XML Node");
    kzsErrorForward(result);

    result = kzcStringCopy(memoryManager, name, &node->name);
    kzsErrorForward(result);
    result = kzcDynamicArrayCreate(memoryManager, &node->attributes);
    kzsErrorForward(result);
    node->type = type;

    *out_node = node;
    kzsSuccess();
}

kzsError XMLNodeCreateContainer(const struct KzcMemoryManager* memoryManager, kzString name, struct XMLNode** out_node)
{
    kzsError result;
    struct XMLNode* node;

    result = XMLNodeCreate_internal(memoryManager, name, XML_NODE_TYPE_CONTAINER, &node);
    kzsErrorForward(result);
    result = kzcDynamicArrayCreate(memoryManager, &node->value.children);
    kzsErrorForward(result);

    *out_node = node;
    kzsSuccess();
}

kzsError XMLNodeCreateInteger(const struct KzcMemoryManager* memoryManager, kzString name, kzInt value, struct XMLNode** out_node)
{
    kzsError result;
    struct XMLNode* node;

    result = XMLNodeCreate_internal(memoryManager, name, XML_NODE_TYPE_INTEGER, &node);
    kzsErrorForward(result);

    node->value.integer = value;

    *out_node = node;
    kzsSuccess();
}

kzsError XMLNodeCreateString(const struct KzcMemoryManager* memoryManager, kzString name, kzString value, struct XMLNode** out_node)
{
    kzsError result;
    struct XMLNode* node;

    result = XMLNodeCreate_internal(memoryManager, name, XML_NODE_TYPE_STRING, &node);
    kzsErrorForward(result);

    result = kzcStringCopy(memoryManager, value, &node->value.string);
    kzsErrorForward(result);

    *out_node = node;
    kzsSuccess();
}

kzsError XMLNodeCreateFloat(const struct KzcMemoryManager* memoryManager, kzString name, kzFloat value, struct XMLNode** out_node)
{
    kzsError result;
    struct XMLNode* node;

    result = XMLNodeCreate_internal(memoryManager, name, XML_NODE_TYPE_REAL, &node);
    kzsErrorForward(result);

    node->value.real = value;

    *out_node = node;
    kzsSuccess();
}

kzsError XMLNodeDelete(struct XMLNode* node)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(node));

    {
        struct KzcDynamicArrayMutableIterator it = kzcDynamicArrayGetMutableIterator(node->attributes);
        while(kzcDynamicArrayMutableIterate(it))
        {
            struct XMLAttribute* attribute = kzcDynamicArrayMutableIteratorGetValue(it);
            result = XMLAttributeDelete(attribute);
            kzsErrorForward(result);
        }
    }
    result = kzcDynamicArrayDelete(node->attributes);
    kzsErrorForward(result);
    
    switch(node->type)
    {
        case XML_NODE_TYPE_CONTAINER:
        {
            struct KzcDynamicArrayMutableIterator it = kzcDynamicArrayGetMutableIterator(node->value.children);
            while(kzcDynamicArrayMutableIterate(it))
            {
                struct XMLNode* childNode = kzcDynamicArrayMutableIteratorGetValue(it);
                result = XMLNodeDelete(childNode);
                kzsErrorForward(result);
            }            
            result = kzcDynamicArrayDelete(node->value.children);
            kzsErrorForward(result);
            break;
        }
        case XML_NODE_TYPE_STRING:
        {
            result = kzcStringDelete(node->value.string);
            kzsErrorForward(result);
            break;
        }
        case XML_NODE_TYPE_REAL:
        case XML_NODE_TYPE_INTEGER:
        default:
        {
            break;
        }
    }

    result = kzcStringDelete(node->name);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(node);
    kzsErrorForward(result);
    
    kzsSuccess();
}

kzsError XMLNodeSetName(struct XMLNode* node, kzString name)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(node));
    kzsErrorTest(kzcStringLength(name) > 0, KZS_ERROR_ILLEGAL_ARGUMENT, "Name must not be null");

    result = kzcStringDelete(node->value.string);
    kzsErrorForward(result);
    result = kzcStringCopy(kzcMemoryGetManager(node), name, &node->value.string);
    kzsErrorForward(result);

    kzsSuccess();
}
kzString XMLNodeGetName(const struct XMLNode* node)
{
    kzsAssert(kzcIsValidPointer(node));
    return node->name;
}

enum XMLNodeType XMLNodeGetType(const struct XMLNode* node)
{
    kzsAssert(kzcIsValidPointer(node));
    return node->type;
}

kzInt XMLNodeGetInteger(const struct XMLNode* node)
{
    kzsAssert(node->type == XML_NODE_TYPE_INTEGER);
    kzsAssert(kzcIsValidPointer(node));
    return node->value.integer;
}

void XMLNodeSetInteger(struct XMLNode* node, kzInt value)
{
    kzsAssert(node->type == XML_NODE_TYPE_INTEGER);
    kzsAssert(kzcIsValidPointer(node));
    node->value.integer = value;
}

kzString XMLNodeGetString(const struct XMLNode* node)
{
    kzsAssert(node->type == XML_NODE_TYPE_STRING);
    kzsAssert(kzcIsValidPointer(node));
    return node->value.string;
}

kzsError XMLNodeSetString(struct XMLNode* node, kzString value)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(node));

    result = kzcStringDelete(node->value.string);
    kzsErrorForward(result);
    result = kzcStringCopy(kzcMemoryGetManager(node), value, &node->value.string);
    kzsErrorForward(result);

    kzsSuccess();
}

kzFloat XMLNodeGetFloat(const struct XMLNode* node)
{
    kzsAssert(node->type == XML_NODE_TYPE_REAL);
    kzsAssert(kzcIsValidPointer(node));
    return node->value.real;
}

void XMLNodeSetFloat(struct XMLNode* node, kzFloat value)
{
    kzsAssert(node->type == XML_NODE_TYPE_REAL);
    kzsAssert(kzcIsValidPointer(node));
    node->value.real = value;
}

kzsError XMLNodeAddChild(const struct XMLNode* parent, struct XMLNode* child)
{
    kzsError result;
    kzsAssert(parent->type == XML_NODE_TYPE_CONTAINER);
    kzsAssert(kzcIsValidPointer(parent));
    kzsAssert(kzcIsValidPointer(child));
    kzsAssert(parent != child);

    result = kzcDynamicArrayAdd(parent->value.children, child);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError XMLNodeRemoveChild(const struct XMLNode* parent, const struct XMLNode* child)
{
    kzsError result;
    kzsAssert(parent->type == XML_NODE_TYPE_CONTAINER);
    kzsAssert(kzcIsValidPointer(parent));
    kzsAssert(kzcIsValidPointer(child));

    result = kzcDynamicArrayRemove(parent->value.children, child);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError XMLNodeAddAttribute(const struct XMLNode* node, struct XMLAttribute* attribute)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(kzcIsValidPointer(attribute));

    result = kzcDynamicArrayAdd(node->attributes, attribute);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError XMLNodeRemoveAttribute(const struct XMLNode* node, const struct XMLAttribute* attribute)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(kzcIsValidPointer(attribute));
    
    result = kzcDynamicArrayRemove(node->attributes, attribute);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError XMLNodeGetAttributeByName(const struct XMLNode* node, kzString name, struct XMLAttribute** out_attribute)
{
    struct XMLAttribute* returnAttribute = KZ_NULL;
    kzsAssert(kzcIsValidPointer(node));

    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(node->attributes);
        while(kzcDynamicArrayIterate(it))
        {
            struct XMLAttribute* attribute = kzcDynamicArrayIteratorGetValue(it);
            kzString attributeName = XMLAttributeGetName(attribute);
            if(kzcStringCompare(name, attributeName) == 0)
            {
                returnAttribute = attribute;
            }
        }
    }
    *out_attribute = returnAttribute;
    kzsSuccess();
}

static kzsError XMLNodeSaveAttributes_internal(const struct XMLNode* node, struct KzcOutputStream* outputStream)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(kzcIsValidPointer(outputStream));

    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(node->attributes);
        while(kzcDynamicArrayIterate(it))
        {
            struct XMLAttribute* attribute = kzcDynamicArrayIteratorGetValue(it);
            enum XMLAttributeType type = XMLAttributeGetType(attribute);
            kzString name = XMLAttributeGetName(attribute);
            kzChar buf[1024];

            switch(type)
            {
                case XML_ATTRIBUTE_TYPE_INTEGER:
                {
                    kzInt value = XMLAttributeGetInteger(attribute);
                    /* TODO: Use kanzi functions. */
                    sprintf(buf, " %s=\"%d\"", name, value);
                    break;
                }
                case XML_ATTRIBUTE_TYPE_REAL:
                {
                    kzFloat value = XMLAttributeGetFloat(attribute);
                    /* TODO: Use kanzi functions. */
                    sprintf(buf, " %s=\"%f\"", name, value);
                    break;
                }
                case XML_ATTRIBUTE_TYPE_STRING:
                {
                    kzString value = XMLAttributeGetString(attribute);
                    /* TODO: Use kanzi functions. */
                    sprintf(buf, " %s=\"%s\"", name, value);
                    break;
                }
                default:
                {
                    kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Unknown attribute type");
                }
            }

            result = kzcOutputStreamWriteBytes(outputStream, kzcStringLength(buf), (kzByte*)buf);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

static kzsError XMLNodeSaveValue_internal(const struct XMLNode* node, struct KzcOutputStream* outputStream, kzUint* intentLevel)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(kzcIsValidPointer(outputStream));

    switch(node->type)
    {
        case XML_NODE_TYPE_CONTAINER:
        {
            if(kzcDynamicArrayGetSize(node->value.children) > 0)
            {
                kzChar buf[] = "\n";
                (*intentLevel)++;
                result = kzcOutputStreamWriteBytes(outputStream, kzcStringLength(buf), (kzByte*)buf);
                kzsErrorForward(result);
                {
                    struct KzcDynamicArrayMutableIterator it = kzcDynamicArrayGetMutableIterator(node->value.children);
                    while(kzcDynamicArrayMutableIterate(it))
                    {
                        struct XMLNode* childNode = kzcDynamicArrayMutableIteratorGetValue(it);
                        result = XMLNodeSave(childNode, outputStream, intentLevel);
                        kzsErrorForward(result);
                    }
                }
            }
            break;
        }
        case XML_NODE_TYPE_INTEGER:
        {
            kzChar buf[255];
            /* TODO: Use kanzi functions. */
            sprintf(buf, "%d", node->value.integer);

            result = kzcOutputStreamWriteBytes(outputStream, kzcStringLength(buf), (kzByte*)buf);
            kzsErrorForward(result);
            break;
        }
        case XML_NODE_TYPE_REAL:
        {
            kzChar buf[255];
            /* TODO: Use kanzi functions. */
            sprintf(buf, "%f", node->value.real);

            result = kzcOutputStreamWriteBytes(outputStream, kzcStringLength(buf), (kzByte*)buf);
            kzsErrorForward(result);
            break;
        }
        case XML_NODE_TYPE_STRING:
        {
            result = kzcOutputStreamWriteBytes(outputStream, kzcStringLength(node->value.string), (kzByte*)node->value.string);
            kzsErrorForward(result);
            break;
        }
    }
    kzsSuccess();
}

static kzsError XMLNodeApplyIntent(struct KzcOutputStream* outputStream, kzUint intentLevel)
{
    kzsError result;
    {
        /* TODO: This implementation could be faster. */
        kzString padd = "   ";
        kzUint i;
        for(i = 0; i < intentLevel; i++)
        {
            result = kzcOutputStreamWriteBytes(outputStream, kzcStringLength(padd), (kzByte*)padd);
            kzsErrorForward(result);
        }
    }
    kzsSuccess();
}

kzsError XMLNodeGetChildNode(const struct XMLNode* node, kzString childName, struct XMLNode** out_node)
{
    struct XMLNode* returnNode = KZ_NULL;
    kzsAssert(kzcIsValidPointer(node));

    if(node->type == XML_NODE_TYPE_CONTAINER)
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(node->value.children);
        while(kzcDynamicArrayIterate(it))
        {
            struct XMLNode* childNode = kzcDynamicArrayIteratorGetValue(it);
            if(kzcStringCompare(childNode->name, childName) == 0)            
            {
                returnNode = childNode;         
                break;
            }
        }
    }

    *out_node = returnNode;
    kzsSuccess();
}

kzsError XMLNodeGetChildCount(const struct XMLNode* node, kzUint* out_count)
{
    kzUint count;
    if(node->type == XML_NODE_TYPE_CONTAINER)
    {
        count = kzcDynamicArrayGetSize(node->value.children);
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Children count requested from data node.");
    }
    *out_count = count;
    kzsSuccess();
}

kzsError XMLNodeGetChildIterator(const struct XMLNode* node, struct KzcDynamicArrayIterator* out_it)
{
    struct KzcDynamicArrayIterator it;
    if(node->type == XML_NODE_TYPE_CONTAINER)
    {
        it = kzcDynamicArrayGetIterator(node->value.children);
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Children requested from data node.");
    }
    *out_it = it;
    kzsSuccess();
}

kzsError XMLNodeGetMutableChildIterator(const struct XMLNode* node, struct KzcDynamicArrayMutableIterator* out_it)
{
    struct KzcDynamicArrayMutableIterator it;
    if(node->type == XML_NODE_TYPE_CONTAINER)
    {
        it = kzcDynamicArrayGetMutableIterator(node->value.children);
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Children requested from data node.");
    }
    *out_it = it;
    kzsSuccess();
}

kzsError XMLNodeGetNode(const struct XMLNode* node, kzUint* currentIndex, kzMutableString* strings, kzUint stringCount, struct XMLNode** out_node)
{
    kzsError result;
    struct XMLNode* returnNode = KZ_NULL;
    kzString currentChildName;
    kzsAssert(kzcIsValidPointer(node));

    currentChildName = strings[*currentIndex];
    if(node->type == XML_NODE_TYPE_CONTAINER)
    {
        kzBool found = KZ_FALSE;
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(node->value.children);
        while(kzcDynamicArrayIterate(it))
        {
            struct XMLNode* childNode = kzcDynamicArrayIteratorGetValue(it);
            if(kzcStringCompare(childNode->name, currentChildName) == 0)            
            {
                if(*currentIndex == stringCount - 1)
                {
                    returnNode = childNode;
                }
                else
                {
                    (*currentIndex)++;
                    result = XMLNodeGetNode(childNode, currentIndex, strings, stringCount, &returnNode);
                    kzsErrorForward(result);
                }
                found = KZ_TRUE;
                break;
            }
        }
        if(!found)  kzsAssert(KZ_FALSE);
    }
    else
    {
        returnNode = KZ_NULL;
    }

    *out_node = returnNode;
    kzsSuccess();
}

kzsError XMLNodeSave(const struct XMLNode* node, struct KzcOutputStream* outputStream, kzUint* intentLevel)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(node));

    {
        kzString prefix = "<";
        kzString postfix = ">";
        result = XMLNodeApplyIntent(outputStream, *intentLevel);
        kzsErrorForward(result);
        result = kzcOutputStreamWriteBytes(outputStream, kzcStringLength(prefix), (kzByte*)prefix);
        kzsErrorForward(result);
        result = kzcOutputStreamWriteBytes(outputStream, kzcStringLength(node->name), (kzByte*)node->name);
        kzsErrorForward(result);
        result = XMLNodeSaveAttributes_internal(node, outputStream);
        kzsErrorForward(result);
        result = kzcOutputStreamWriteBytes(outputStream, kzcStringLength(postfix), (kzByte*)postfix);
        kzsErrorForward(result);
    }

    result = XMLNodeSaveValue_internal(node, outputStream, intentLevel);
    kzsErrorForward(result);

    {
        kzString prefix = "</";
        kzString postfix = ">\n";
        if(node->type == XML_NODE_TYPE_CONTAINER)
        {
            if(kzcDynamicArrayGetSize(node->value.children) > 0)
            {
                (*intentLevel)--;
                result = XMLNodeApplyIntent(outputStream, *intentLevel);
                kzsErrorForward(result);
            }
        }
        result = kzcOutputStreamWriteBytes(outputStream, kzcStringLength(prefix), (kzByte*)prefix);
        kzsErrorForward(result);
        result = kzcOutputStreamWriteBytes(outputStream, kzcStringLength(node->name), (kzByte*)node->name);
        kzsErrorForward(result);
        result = kzcOutputStreamWriteBytes(outputStream, kzcStringLength(postfix), (kzByte*)postfix);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
