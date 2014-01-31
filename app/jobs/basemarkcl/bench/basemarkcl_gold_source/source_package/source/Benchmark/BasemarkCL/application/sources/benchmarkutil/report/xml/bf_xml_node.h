/**
* \file
* OpenCL benchmark xml document node.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef BF_XML_NODE_H
#define BF_XML_NODE_H

#include <core/util/collection/kzc_dynamic_array.h>

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/** XML Node type. */
enum XMLNodeType
{
    XML_NODE_TYPE_CONTAINER = 0, /**< Node container, no value assigned. */
    XML_NODE_TYPE_INTEGER, /**< Node contains integer values. */
    XML_NODE_TYPE_STRING, /**< Node contains string values. */
    XML_NODE_TYPE_REAL /**< Node contains real numbers. */
};


/* Forward declarations. */
struct KzcOutputStream;
struct KzcMemoryManager;
struct XMLAttribute;
struct KzcDynamicArrayIterator;
struct KzcDynamicArrayMutableIterator;

/**
 * \struct XMLNode
 * XML documents node structure
 */
struct XMLNode;


/** Create empty XML node. */
kzsError XMLNodeCreateContainer(const struct KzcMemoryManager* memoryManager, kzString name, struct XMLNode** out_node);
/** Create XML node for integer data. */
kzsError XMLNodeCreateInteger(const struct KzcMemoryManager* memoryManager, kzString name, kzInt value, struct XMLNode** out_node);
/** Create XML node for strings data. */
kzsError XMLNodeCreateString(const struct KzcMemoryManager* memoryManager, kzString name, kzString value, struct XMLNode** out_node);
/** Create XML node for float data. */
kzsError XMLNodeCreateFloat(const struct KzcMemoryManager* memoryManager, kzString name, kzFloat value, struct XMLNode** out_node);

/** Deletes the node. */
kzsError XMLNodeDelete(struct XMLNode* node);


/** Set XML node name. */
kzsError XMLNodeSetName(struct XMLNode* node, kzString name);
/** Get XML node name. */
kzString XMLNodeGetName(const struct XMLNode* node);

/** Get XML node type. */
enum XMLNodeType XMLNodeGetType(const struct XMLNode* node);

/** Sets integer value from XML node. */
kzInt XMLNodeGetInteger(const struct XMLNode* node);
/** Sets integer value to XML node. */
void XMLNodeSetInteger(struct XMLNode* node, kzInt value);
/** Gets string value from XML node. */
kzString XMLNodeGetString(const struct XMLNode* node);
/** Sets string value to XML node. */
kzsError XMLNodeSetString(struct XMLNode* node, kzString value);
/** Gets float value from XML node. */
kzFloat XMLNodeGetFloat(const struct XMLNode* node);
/** Sets float value to XML node. */
void XMLNodeSetFloat(struct XMLNode* node, kzFloat value);

/** Add new child to XML node. Can be only used with container nodes. */
kzsError XMLNodeAddChild(const struct XMLNode* parent, struct XMLNode* child);
/** Remove child from XML node. Can be only used with container nodes. */
kzsError XMLNodeRemoveChild(const struct XMLNode* parent, const struct XMLNode* child);

/** Add new attribute to XML node. */
kzsError XMLNodeAddAttribute(const struct XMLNode* node, struct XMLAttribute* attribute);
/** Add new attribute to XML node. */
kzsError XMLNodeRemoveAttribute(const struct XMLNode* node, const struct XMLAttribute* attribute);
/** Get attribute by name. */
kzsError XMLNodeGetAttributeByName(const struct XMLNode* node, kzString name, struct XMLAttribute** out_attribute);

/** Get child node with given name from node. */
kzsError XMLNodeGetChildNode(const struct XMLNode* node, kzString childName, struct XMLNode** out_node);

/** Get child count from given node. */
kzsError XMLNodeGetChildCount(const struct XMLNode* node, kzUint* out_count);
/** Get child iterator for given node. */
kzsError XMLNodeGetChildIterator(const struct XMLNode* node, struct KzcDynamicArrayIterator* out_it);
/** Get mutable child iterator for given node. */
kzsError XMLNodeGetMutableChildIterator(const struct XMLNode* node, struct KzcDynamicArrayMutableIterator* out_it);

/** Get node which corresponds to given path. */
kzsError XMLNodeGetNode(const struct XMLNode* node, kzUint* currentIndex, kzMutableString* strings, kzUint stringCount, struct XMLNode** out_node);
/** Saves XML node. Used when saving the document. */
kzsError XMLNodeSave(const struct XMLNode* node, struct KzcOutputStream* outputStream, kzUint* intentLevel);


#endif
