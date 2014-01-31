/**
* \file
* OpenCL benchmark menu.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef CL_MENU_H
#define CL_MENU_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */ 
struct BenchmarkFramework;
struct KzuUiComponentNode;
struct KzuUiEvent;
struct KzcMemoryManager;
struct KzcHashMap;
struct KzsKeyDeviceInputData;
struct KzsPointingDeviceInputData;
struct ApplicationData;

/**
 * \struct ClMenu
 * Test menu struct.
 */
struct ClMenu;


/** Create ClMenu structure. */
kzsError clMenuCreate(const struct BenchmarkFramework* framework, struct ClMenu** out_sceneData);
/** Delete ClMenu structure. */
kzsError clMenuDelete(const struct BenchmarkFramework* framework, struct ClMenu* clMenu);

/** Initialize clMenu. */
kzsError clMenuInitialize(const struct BenchmarkFramework* framework, struct ClMenu* clMenu);
/* Uninitialize clMenu. */
kzsError clMenuUninitialize(const struct BenchmarkFramework* framework, struct ClMenu* clMenu);

/** Updates menu. */
kzsError clMenuUpdate(struct ApplicationData* applicationData, struct BenchmarkFramework* framework, struct ClMenu* clMenu, kzUint deltaTime);
/** Menu render. */
kzsError clMenuRender(const struct BenchmarkFramework* framework, struct ClMenu* clMenu);

/** KeyDevice. */
kzsError clMenuKeyDeviceHandler(struct BenchmarkFramework* framework, struct ClMenu* clMenu, const struct KzsKeyDeviceInputData* inputData);
/** PointingDevice. */
kzsError clMenuPointingDeviceHandler(struct BenchmarkFramework* framework, struct ClMenu* clMenu, const struct KzsPointingDeviceInputData* inputData);

/** Get index of menu item. */
kzInt clMenuGetIndex(const struct KzuUiComponentNode* node);

/** Hover menu item Kanzi event callback. */
KZ_CALLBACK kzsError clMenuHoverHandler(const struct KzuUiComponentNode* node, void* userData, struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters);
/** Dehover menu item event callback. */
KZ_CALLBACK kzsError clMenuDehoverHandler(const struct KzuUiComponentNode* node, void* userData, struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters);

/** Create context with default settings. Should be used only for development and debugging. */
kzsError clMenuCreateContextAndSkipPlatformMenu(struct ClMenu* clMenu, struct BenchmarkFramework* framework, kzInt platform, kzInt device, kzBool* out_success);

/** Select Kanzi event callback. */
KZ_CALLBACK kzsError clMenuSelectHandler(const struct KzuUiComponentNode* node, void* userData, struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters);
/** Back Kanzi event callback. */
KZ_CALLBACK kzsError clMenuBackHandler(const struct KzuUiComponentNode* node, void* userData, struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters);

/** Escape pressed. */
kzBool clMenuEscapePressed(struct BenchmarkFramework* framework, struct ClMenu* clMenu);

/** Get index for test name. */
kzsError clMenuGetTestIndexForName(struct KzcMemoryManager* memoryManager, kzString benchmarkTestName, kzInt* out_value);


#endif
