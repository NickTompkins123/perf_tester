/**
* \file
* Benchmark report document utilities.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef BF_REPORT_DOCUMENT_H
#define BF_REPORT_DOCUMENT_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct XMLNode;

/**
* \struct BfReportDocument
* Contains the utilities creating xml document where report contents are stored.
*/
struct BfReportDocument;


/** Creates report document struct. */
kzsError bfReportDocumentCreate(const struct KzcMemoryManager* memoryManager, struct BfReportDocument** out_document);

/** Deletes report document struct. */
kzsError bfReportDocumentDelete(struct BfReportDocument* document);

/** Get node that corresponds to given xml path. */
kzsError bfReportDocumentGetNode(const struct BfReportDocument* document, kzString path, struct XMLNode** out_node);

/** Save report document to xml. */
kzsError bfReportDocumentSaveXML(const struct BfReportDocument* document);


#endif
