/**
* \file
* OpenCL benchmark reporting info. 
* Collects information about the platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef BF_INFO_H
#define BF_INFO_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations. */
struct XMLNode;
struct XMLDocument;
struct CluInfo;
struct KzcSettingContainer;
struct BfReportLogger;
struct KzsSurface;


/** Add OpenGL info to given XML node. */
kzsError bfInfoUpdateGL(struct XMLNode* node);

/** Add OpenCL info to given XML node. */
kzsError bfInfoUpdateCL(struct XMLNode* node, struct CluInfo* cluInfo, struct XMLNode* profileNode);

/** Add surface info to given XML node. */
kzsError bfInfoUpdateSurface(const struct XMLNode* node, const struct KzsSurface* surface);

/** Add benchmark config data under given XML node. */
kzsError bfInfoUpdateConfiguration(struct XMLNode* node, struct KzcSettingContainer* settingContainer);

/** Add test results for a scene to given XML node. */
kzsError bfInfoUpdateSceneResults(struct XMLNode* node, struct BfReportLogger* logger, kzString sceneName, kzString sceneCategory, kzBool validData, kzBool binaryKernel, 
                                  kzFloat scoreWeightFactor);

/** Update overall score XML node. */
kzsError bfInfoUpdateOverallScore(const struct XMLDocument* xmlDocument, struct XMLNode* scoreNode, struct XMLNode* testsParentNode);


#endif
