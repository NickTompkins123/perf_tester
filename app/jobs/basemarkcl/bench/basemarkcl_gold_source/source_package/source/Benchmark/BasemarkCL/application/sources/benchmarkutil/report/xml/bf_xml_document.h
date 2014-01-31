/**
* \file
* OpenCL benchmark xml document.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef BF_XML_DOCUMENT_H
#define BF_XML_DOCUMENT_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations. */ 
struct XMLNode;
struct KzcMemoryManager;

/**
 * \struct XMLDocument
 * XML Document structure
 */
struct XMLDocument;


/** Creates the XML document. */
kzsError XMLDocumentCreate(const struct KzcMemoryManager* memoryManager, struct XMLDocument** out_xmlDocument);
/** Deletes XML document and all nodes contained in it. */
kzsError XMLDocumentDelete(struct XMLDocument* xmlDocument);

/** Gets the root node of XML document. */
struct XMLNode* XMLDocumentGetRoot(const struct XMLDocument* xmlDocument);

/** Saves the document to file. */
kzsError XMLDocumentSaveToFile(const struct XMLDocument* xmlDocument, kzString filePath);

/** Gets node which corresponds to given path. */
kzsError XMLDocumentGetNode(const struct XMLDocument* xmlDocument, kzString path, struct XMLNode** out_node);


#endif
