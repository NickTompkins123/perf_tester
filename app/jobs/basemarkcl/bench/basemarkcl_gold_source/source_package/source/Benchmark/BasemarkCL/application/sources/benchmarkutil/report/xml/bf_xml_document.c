/**
* \file
* OpenCL benchmark xml document.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#include "bf_xml_document.h"

#include <benchmarkutil/report/bf_info.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_output_stream.h>
#include <core/util/string/kzc_string.h>
#include <core/debug/kzc_log.h>

#include "bf_xml_node.h"
#include "bf_xml_attribute.h"

#ifdef WIN32
#include <windows.h>
#include <stdlib.h>
#include <direct.h>
#define GetCurrentDir _getcwd
#endif


struct XMLDocument
{
    struct XMLNode* root; /**< Document root node. */
};


kzsError XMLDocumentCreate(const struct KzcMemoryManager* memoryManager, struct XMLDocument** out_xmlDocument)
{
    kzsError result;
    struct XMLDocument* document;

    result = kzcMemoryAllocVariable(memoryManager, document, "report");
    kzsErrorForward(result);

    result = XMLNodeCreateContainer(memoryManager, "report", &document->root);
    kzsErrorForward(result);
    {
        struct XMLAttribute* attribute;
        result = XMLAttributeCreateString(memoryManager, "name", "Basemark CL Report", &attribute);
        kzsErrorForward(result);
        result = XMLNodeAddAttribute(document->root, attribute);
        kzsErrorForward(result);
    }

    *out_xmlDocument = document;
    kzsSuccess();
}

kzsError XMLDocumentDelete(struct XMLDocument* xmlDocument)
{
    kzsError result;
    
    result = XMLNodeDelete(xmlDocument->root);
    kzsErrorForward(result);
    result = kzcMemoryFreeVariable(xmlDocument);
    kzsErrorForward(result);

    kzsSuccess();
}

struct XMLNode* XMLDocumentGetRoot(const struct XMLDocument *xmlDocument)
{
    kzsAssert(kzcIsValidPointer(xmlDocument));
    return xmlDocument->root;
}

kzsError XMLDocumentSaveToFile(const struct XMLDocument* xmlDocument, kzString filePath)
{
    kzsError result;
    struct KzcOutputStream* outputStream;

    result = kzcOutputStreamCreateToFile(kzcMemoryGetManager(xmlDocument), filePath, KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED, &outputStream);
    kzsErrorForward(result);

    {
        kzString versionString = "<?xml version=\"1.0\" ?>\n";
        kzString styleSheetString = "<?xml-stylesheet type=\"text/xsl\" href=\"reportdata/report.xsl\" ?>\n";

        result = kzcOutputStreamWriteBytes(outputStream, kzcStringLength(versionString), (kzByte*)versionString);
        kzsErrorForward(result);
        result = kzcOutputStreamWriteBytes(outputStream, kzcStringLength(styleSheetString), (kzByte*)styleSheetString);
        kzsErrorForward(result);
    }

    /* Update overall score. */
    {
        struct XMLNode* scoreNode;
        struct XMLNode* testsNode;
        result = XMLDocumentGetNode(xmlDocument, "xml/benchmark/overallScore", &scoreNode);
        kzsErrorForward(result);
        result = XMLDocumentGetNode(xmlDocument, "xml/benchmark/tests", &testsNode);
        kzsErrorForward(result);
        result = bfInfoUpdateOverallScore(xmlDocument, scoreNode, testsNode);
        kzsErrorForward(result);
    }

    {
        kzUint intentLevel = 0;
        result = XMLNodeSave(xmlDocument->root, outputStream, &intentLevel);
        kzsErrorForward(result);
    }

    result = kzcOutputStreamDelete(outputStream);
    kzsErrorForward(result);

    kzsLog(KZS_LOG_LEVEL_INFO, "");
    result = kzcLog(kzcMemoryGetManager(xmlDocument), KZS_LOG_LEVEL_INFO, "Report created to '%s'", filePath);
    kzsErrorForward(result);

#ifdef WIN32
    /*
    Opening of the completed results in IE on windows.
    TODO: Make implementations for other platforms and wrap this away nicely. Thankfully other platforms tend to have easy way of opening arbitray url on the default browser.
    */
    {
    struct XMLNode* validNode;
    kzInt isValidNode;
    result = XMLDocumentGetNode(xmlDocument, "xml/validTestSuite", &validNode);
    isValidNode = XMLNodeGetInteger(validNode);
    /* TestSuite is valid when all tests have been run -> the report should be launched */
    if(isValidNode)
     {
        char fpath[1024];
        char curdir[512];
        strcpy(fpath, "start iexplore ");
        strcat(fpath, "file://");
        GetCurrentDir(curdir,512);
        strcat(fpath, curdir);
        strcat(fpath,"\\");
        strcat(fpath, filePath);      
        system(fpath);
     }
    }

#endif
    kzsSuccess();
}

kzsError XMLDocumentGetNode(const struct XMLDocument* xmlDocument, kzString path, struct XMLNode** out_node)
{
    kzsError result;
    struct XMLNode* node;
    kzMutableString* strings;
    kzUint stringCount;
    kzUint currentString = 1;
    kzsAssert(kzcIsValidPointer(xmlDocument));

    result = kzcStringSplit(kzcMemoryGetManager(xmlDocument), path, "/", &stringCount, &strings);
    kzsErrorForward(result); 

    result = XMLNodeGetNode(XMLDocumentGetRoot(xmlDocument), &currentString, strings, stringCount, &node);
    kzsErrorForward(result);

    {
        kzUint i;
        kzUint maxIndex = stringCount;
        for (i = 0; i < maxIndex; ++i)
        {
            result = kzcStringDelete(strings[i]);
            kzsErrorForward(result);
        }
    }
    result = kzcMemoryFreeArray(strings);
    kzsErrorForward(result);

    *out_node = node;
    kzsSuccess();
}
