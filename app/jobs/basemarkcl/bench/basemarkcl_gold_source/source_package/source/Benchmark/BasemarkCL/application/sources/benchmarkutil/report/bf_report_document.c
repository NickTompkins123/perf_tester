/**
* \file
* Benchmark report document utilities.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#include "bf_report_document.h"

#include <benchmarkutil/report/xml/bf_xml_document.h>
#include <benchmarkutil/report/xml/bf_xml_node.h>
#include <benchmarkutil/report/xml/bf_xml_attribute.h>
#include <benchmarkutil/bf_version.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/debug/kzc_log.h>
#include <core/util/string/kzc_string.h>

#include <system/time/kzs_tick.h>
#include <system/time/kzs_time.h>
#include <system/debug/kzs_log.h>


struct BfReportDocument
{
    struct XMLDocument* reportDocument; /**< XML document containing the report to be saved. */
    struct KzsTime time;
};


static kzsError bfReportDocumentAddStringNode(const struct KzcMemoryManager* memoryManager, kzString name, kzString value, const struct XMLNode* parent)
{
    kzsError result;
    struct XMLNode* child;
    result = XMLNodeCreateString(memoryManager, name, value, &child);
    kzsErrorForward(result);
    result = XMLNodeAddChild(parent, child);
    kzsErrorForward(result);
    kzsSuccess();
}

static kzsError bfReportDocumentAddIntegerNode(const struct KzcMemoryManager* memoryManager, kzString name, kzInt value, const struct XMLNode* parent)
{
    kzsError result;
    struct XMLNode* child;
    result = XMLNodeCreateInteger(memoryManager, name, value, &child);
    kzsErrorForward(result);
    result = XMLNodeAddChild(parent, child);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError bfReportDocumentCreate(const struct KzcMemoryManager* memoryManager, struct BfReportDocument** out_document)
{
    kzsError result;
    struct BfReportDocument* document;

    result = kzcMemoryAllocVariable(memoryManager, document, "Report Document");
    kzsErrorForward(result);

    document->time = kzsTimeGetTime();

    {
        result = XMLDocumentCreate(memoryManager, &document->reportDocument);
        kzsErrorForward(result);

        {
            struct XMLNode* root = XMLDocumentGetRoot(document->reportDocument);

            /* Benchmark Version. */
            {
                struct XMLNode* node;
                result = XMLNodeCreateString(memoryManager, "benchmarkVersion", bfGetVersionString(), &node);
                kzsErrorForward(result);
                result = XMLNodeAddChild(root, node);
                kzsErrorForward(result);
            }

            /* Date. */
            {
                struct XMLNode* node;
                kzMutableString date;
                struct KzsTime time = kzsTimeGetTime();
                result = kzcStringFormat(memoryManager, "%d-%02d-%02d", &date, time.year, time.month, time.day);
                kzsErrorForward(result);
                result = XMLNodeCreateString(memoryManager, "date", date, &node);
                kzsErrorForward(result);
                result = kzcStringDelete(date);
                kzsErrorForward(result);
                result = XMLNodeAddChild(root, node);
                kzsErrorForward(result);
            }

            /* Time. */
            {
                struct XMLNode* node;
                kzMutableString timeString;
                struct KzsTime time = kzsTimeGetTime();
                result = kzcStringFormat(memoryManager, "%02d:%02d:%02d", &timeString, time.hours, time.minutes, time.seconds);
                kzsErrorForward(result);
                result = XMLNodeCreateString(memoryManager, "time", timeString, &node);
                kzsErrorForward(result);
                result = kzcStringDelete(timeString);
                kzsErrorForward(result);
                result = XMLNodeAddChild(root, node);
                kzsErrorForward(result);
            }

            /* Used CL profile. */
            {
                struct XMLNode* node;
                result = XMLNodeCreateString(memoryManager, "clProfile", "", &node);
                kzsErrorForward(result);
                result = XMLNodeAddChild(root, node);
                kzsErrorForward(result);
            }

            /* Is all data valid. */
            {
                struct XMLNode* node;
                result = XMLNodeCreateInteger(memoryManager, "validData", 0, &node);
                kzsErrorForward(result);
                result = XMLNodeAddChild(root, node);
                kzsErrorForward(result);
            }

            /* Binary kernel usage. */
            {
                struct XMLNode* node;
                result = XMLNodeCreateInteger(memoryManager, "binaryKernels", 0, &node);
                kzsErrorForward(result);
                result = XMLNodeAddChild(root, node);
                kzsErrorForward(result);
            }
            
            /* All tests run once. */
            {
                struct XMLNode* node;
                result = XMLNodeCreateInteger(memoryManager, "validTestSuite", 0, &node);
                kzsErrorForward(result);
                result = XMLNodeAddChild(root, node);
                kzsErrorForward(result);
            }

            /* Command line profile. */
            {
                struct XMLNode* node;
                result = XMLNodeCreateString(memoryManager, "commandLineParameters", "", &node);
                kzsErrorForward(result);
                result = XMLNodeAddChild(root, node);
                kzsErrorForward(result);
            }

            /* Device. */
            {
                struct XMLNode* deviceNode;
                result = XMLNodeCreateContainer(memoryManager, "device", &deviceNode);
                kzsErrorForward(result);
                result = XMLNodeAddChild(root, deviceNode);
                kzsErrorForward(result);
                {
                    result = bfReportDocumentAddStringNode(memoryManager, "name", "", deviceNode);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "platform", "", deviceNode);
                    kzsErrorForward(result);
                }
                {
                    struct XMLNode* os;
                    result = XMLNodeCreateContainer(memoryManager, "os", &os);
                    kzsErrorForward(result);
                    result = XMLNodeAddChild(deviceNode, os);
                    kzsErrorForward(result);
                    {
                        result = bfReportDocumentAddStringNode(memoryManager, "name", "", os);
                        kzsErrorForward(result);
                        result = bfReportDocumentAddStringNode(memoryManager, "version", "", os);
                        kzsErrorForward(result);
                    }
                }
                {
                    struct XMLNode* screen;
                    result = XMLNodeCreateContainer(memoryManager, "screen", &screen);
                    kzsErrorForward(result);
                    result = XMLNodeAddChild(deviceNode, screen);
                    kzsErrorForward(result);
                    {
                        result = bfReportDocumentAddIntegerNode(memoryManager, "width", 0, screen);
                        kzsErrorForward(result);
                        result = bfReportDocumentAddIntegerNode(memoryManager, "height", 0, screen);
                        kzsErrorForward(result);
                        result = bfReportDocumentAddIntegerNode(memoryManager, "colorBits", 0, screen);
                        kzsErrorForward(result);
                        result = bfReportDocumentAddIntegerNode(memoryManager, "colorBits", 0, screen);
                        kzsErrorForward(result);
                        result = bfReportDocumentAddIntegerNode(memoryManager, "colors", 0, screen);
                        kzsErrorForward(result);
                    }
                }
                {
                    struct XMLNode* cpu;
                    result = XMLNodeCreateContainer(memoryManager, "cpu", &cpu);
                    kzsErrorForward(result);
                    result = XMLNodeAddChild(deviceNode, cpu);
                    kzsErrorForward(result);
                
                    result = bfReportDocumentAddStringNode(memoryManager, "vendor", "", cpu);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "name", "", cpu);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "core", "", cpu);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "instructionSet", "", cpu);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "instructionSetID", "", cpu);
                    kzsErrorForward(result);                    
                }
            }

            /* Surface. */
            {
                struct XMLNode* node;
                result = XMLNodeCreateContainer(memoryManager, "surface", &node);
                kzsErrorForward(result);
                result = XMLNodeAddChild(root, node);
                kzsErrorForward(result);
            }

            /* OPENGL. */
            {
                struct XMLNode* node;
                result = XMLNodeCreateContainer(memoryManager, "opengl", &node);
                kzsErrorForward(result);
                result = XMLNodeAddChild(root, node);
                kzsErrorForward(result);
            }


            {
                struct XMLNode* node;
                result = XMLNodeCreateContainer(memoryManager, "opencl", &node);
                kzsErrorForward(result);
                result = XMLNodeAddChild(root, node);
                kzsErrorForward(result);
            }

            {
                struct XMLNode* android;
                result = XMLNodeCreateContainer(memoryManager, "android", &android);
                kzsErrorForward(result);
                result = XMLNodeAddChild(root, android);
                kzsErrorForward(result);

                {
                    result = bfReportDocumentAddStringNode(memoryManager, "board", "", android);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "brand", "", android);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "device", "", android);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "fingerprint", "", android);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "model", "", android);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "product", "", android);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "tags", "", android);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "time", "", android);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "type", "", android);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "user", "", android);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "versionIncremental", "", android);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "versionRelease", "", android);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "versionSDK", "", android);
                    kzsErrorForward(result);
                    result = bfReportDocumentAddStringNode(memoryManager, "display", "", android);
                    kzsErrorForward(result);
                }
            }

            {
                struct XMLNode* benchmark;
                result = XMLNodeCreateContainer(memoryManager, "benchmark", &benchmark);
                kzsErrorForward(result);
                result = XMLNodeAddChild(root, benchmark);
                kzsErrorForward(result);
                {
                    struct XMLNode* node;
                    result = XMLNodeCreateContainer(memoryManager, "settings", &node);
                    kzsErrorForward(result);
                    result = XMLNodeAddChild(benchmark, node);
                    kzsErrorForward(result);
                }
                {
                    struct XMLNode* node;
                    result = XMLNodeCreateContainer(memoryManager, "tests", &node);
                    kzsErrorForward(result);
                    result = XMLNodeAddChild(benchmark, node);
                    kzsErrorForward(result);
                }
                {
                    struct XMLNode* node;
                    result = XMLNodeCreateContainer(memoryManager, "overallScore", &node);
                    kzsErrorForward(result);
                    result = XMLNodeAddChild(benchmark, node);
                    kzsErrorForward(result);
                }
            }
        }

        {
            struct XMLNode* node;
            result = bfReportDocumentGetNode(document, "xml/device/cpu/vendor", &node);
            kzsErrorForward(result);
        }
    }

    *out_document = document;
    kzsSuccess();
}

kzsError bfReportDocumentDelete(struct BfReportDocument* document)
{
    kzsError result;

    result = XMLDocumentDelete(document->reportDocument);
    kzsErrorForward(result);
    result = kzcMemoryFreeVariable(document);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError bfReportDocumentGetNode(const struct BfReportDocument* document, kzString path, struct XMLNode** out_node)
{
    kzsError result;
    struct XMLNode* node;
    kzsAssert(kzcIsValidPointer(document));

    result = XMLDocumentGetNode(document->reportDocument, path, &node);
    kzsErrorForward(result);

    *out_node = node;
    kzsSuccess();
}

kzsError bfReportDocumentSaveXML(const struct BfReportDocument* document)
{
    kzsError result;
    kzMutableString fileName;
    struct KzsTime time;
    kzsAssert(kzcIsValidPointer(document));

    time = document->time;

    result = kzcStringFormat(kzcMemoryGetManager(document), "reports/report_%d-%02d-%02d_%02d-%02d-%02d.xml", &fileName, 
        time.year, time.month, time.day, time.hours, time.minutes, time.seconds);
    kzsErrorForward(result);

    result = XMLDocumentSaveToFile(document->reportDocument, fileName);
    kzsErrorForward(result);
    
    result = kzcStringDelete(fileName);
    kzsErrorForward(result);

    kzsSuccess();
}
