/**
* \file
* OpenCL benchmark menu.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#include "cl_menu.h"
#include "../application_interface.h"

#include <benchmarkutil/bf_benchmark_framework.h>
#include <benchmarkutil/settings/bf_settings.h>

#include <application/kza_application.h>

#include <user/ui/kzu_ui_component.h>
#include <user/renderer/kzu_renderer.h>
#include <user/engine/kzu_engine.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_mesh.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/ui/kzu_ui_component.h>
#include <user/ui/kzu_ui_screen.h>
#include <user/ui/components/kzu_ui_button.h>
#include <user/material/kzu_material.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_property_manager_color.h>
#include <user/properties/kzu_color_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/animation/kzu_animation_player.h>
#include <user/project/kzu_project_loader.h>
#include <user/project/kzu_project_loader_font.h>

#include <core/util/font/kzc_font.h>
#include <core/util/font/kzc_freetype_font.h>
#include <core/util/font/kzc_text_layout.h>
#include <core/util/font/kzc_text_layouter.h>
#include <core/util/string/kzc_string.h>
#include <core/util/settings/kzc_settings.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/renderer/kzc_renderer.h>
#include <core/debug/kzc_log.h>

#include <system/time/kzs_tick.h>
#include <system/wrappers/kzs_string.h>

#include <clutil/clu_platform.h>
#include <clutil/clu_opencl_base.h>


/*** How long splash is run. */
#define SPLASH_GOAL_TIME 5400

#define MAX_WIDTH_PLATFORM 350

#define OBJECT_NAME_COUNT 60
/** Names of the UI components. Used to find the navigation order. */
kzString testObjectNames[OBJECT_NAME_COUNT] = 
{
    /* Physics tests. */
    "TestSoftBodySimulation",
    "TestFluidSimulation",
    "TestSphFluid",
    "TestWaveSimulation",
    "",
    "",
    "",
    "",
    "",
    "",

    /* Image tests. */
    "TestImageSmoothing",
    "TestImageColorCorrection",
    "TestImageNoiseReduction",
    "TestImageSurfaceAwareSmoothing",
    "TestImageSharpening",
    "",
    "",
    "",
    "",
    "",

    /* Video tests. */
    "TestVideoSmoothing",
    "TestVideoColorCorrection",
    "TestVideoNoiseReduction",
    "TestVideoSurfaceSmoothing",
    "TestVideoSharpening",
    "",
    "",
    "",
    "",
    "",

    /* Feature tests. */
    "TestMandelbulb",
    "TestJuliaFractal",
    "TestOnlineCompiler",
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    /* Run all tests. */
    "RunAllTestsButton",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    /* Main menu. */
    "TestsPhysicsSimulation",
    "TestsImageManipulation",
    "TestsVideoManipulation",
    "TestsFeature",
    "",
    "",
    "BackButton",
    "",
    "ExitButton",
    ""
};


#define INITIALLY_SELECTED_MENU_ITEM 50
#define LAST_MAIN_MENU_SELECTION_INDEX 53
#define BACK_BUTTON_ID 56
#define EXIT_BUTTON_ID 58
#define RUN_ALL_BUTTON_ID 40


kzBool testAvailable[OBJECT_NAME_COUNT] = 
{
    /* Physics tests. */
    KZ_TRUE,
    KZ_TRUE,
    KZ_TRUE,
    KZ_TRUE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,

    /* Image tests. */
    KZ_TRUE,
    KZ_TRUE,
    KZ_TRUE,
    KZ_TRUE,
    KZ_TRUE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,

    /* Video tests. */
    KZ_TRUE,
    KZ_TRUE,
    KZ_TRUE,
    KZ_TRUE,
    KZ_TRUE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,

    /* Feature tests. */
    KZ_TRUE,
    KZ_TRUE,
    KZ_TRUE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,

    /* Test all menu. */
    KZ_TRUE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_FALSE,
    
    /* Main menu. */
    KZ_TRUE,
    KZ_TRUE,
    KZ_TRUE,
    KZ_TRUE,
    KZ_FALSE,
    KZ_FALSE,
    KZ_TRUE,
    KZ_FALSE,
    KZ_TRUE,
    KZ_FALSE,
};


/** CL Menu state. */
enum ClMenuState
{
    CL_MENU_STATE_SELECT_PLATFORM = KZ_FALSE, /**< Select platform state. */
    CL_MENU_STATE_SELECT_DEVICE, /**< Select device state. */
    CL_MENU_STATE_SPLASH, /**< Menu splash. */
    CL_MENU_STATE_MENU /**< Menu state. */
};


struct ClMenu
{
    enum ClMenuState state; /**< Current state of the menu. */
    struct KzuScene* scene; /**< Menu scene file. */

    kzInt selectedPlatform; /**< Currently selected platform. */
    kzInt selectedDevice; /**< Currently selected platform.. */

    struct KzcFont* font; /**< Menu font. */
    struct KzcTextLayouter* textLayouter; /**< Text layouter. */

    kzBool upPressed; /**< Up key pressed. */
    kzBool downPressed; /**< Up key pressed. */
    kzBool leftPressed; /**< Left key pressed. */
    kzBool rightPressed; /**< Right key pressed. */
    kzBool selectPressed; /**< Select pressed. */
    kzBool backPressed; /**< Back key pressed. */

    kzBool lastEventWasPointer; /**< Was last event pointer or keyevent. Changed visualization state. */
    kzInt pointerX, pointerY; /**< Pointer position. */
    kzBool pointerDown; /**< Is pointer pressed. */
    kzBool pointerUp; /**< Pointer was released. */

    kzInt selectedMenuItem; /**< Selected test item. */
    kzInt menuCategory; /**< Category for menu. */

    kzInt splashTime; /**< Time elapsed in splash screen. */

    kzInt requestedPlatform; /**< Platform requested from configuration file */
    kzInt requestedDevice; /**< Device requested*/
};


/** Internal helper for drawing text. */
static kzsError clMenuDrawText_internal(struct KzcFont* font, const struct KzcTextLayouter* textLayouter, const struct KzcMemoryManager* memoryManager,
    struct KzcRenderer* renderer, kzString text, kzFloat x, kzFloat y, kzFloat* out_xSpace, kzFloat* out_ySpace);
/** Update hover colors. */
kzsError clMenuUpdateColors(struct BenchmarkFramework* framework, struct ClMenu* clMenu, kzBool resetToInactive);
/** Returns true if menu is in the submenu. */
kzBool clMenuIsInSubmenu(struct ClMenu* clMenu);


kzsError clMenuCreate(const struct BenchmarkFramework* framework, struct ClMenu** out_menu)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    struct ClMenu* menu;

    kzsAssert(kzcIsValidPointer(framework));

    memoryManager = bfGetMemoryManager(framework);

    result = kzcMemoryAllocVariable(memoryManager, menu, "Benchmark menu");
    kzsErrorForward(result);

    menu->state = CL_MENU_STATE_SELECT_PLATFORM;
    menu->scene = KZ_NULL;
    menu->font = KZ_NULL;
    menu->upPressed = KZ_FALSE;
    menu->downPressed = KZ_FALSE;
    menu->leftPressed = KZ_FALSE;
    menu->rightPressed = KZ_FALSE;
    menu->selectPressed = KZ_FALSE;
    menu->backPressed = KZ_FALSE;
    menu->selectedPlatform = 0;
    menu->selectedDevice = 0;
    menu->textLayouter = KZ_NULL;
    menu->pointerDown = KZ_FALSE;
    menu->pointerUp = KZ_FALSE;
    menu->pointerX = 0;
    menu->pointerY = 0;
    menu->selectedMenuItem = INITIALLY_SELECTED_MENU_ITEM;
    menu->menuCategory = 0;
    menu->lastEventWasPointer = KZ_FALSE;
    menu->splashTime = 0;
    menu->requestedDevice = -1;
    menu->requestedPlatform = -1;

    *out_menu = menu;
    kzsSuccess();
}

kzsError clMenuDelete(const struct BenchmarkFramework* framework, struct ClMenu* clMenu)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(clMenu));

    result = clMenuUninitialize(framework, clMenu);
    kzsErrorForward(result);

    result = kzcMemoryFreePointer(clMenu);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError clMenuInitialize(const struct BenchmarkFramework* framework, struct ClMenu* clMenu)
{
    kzsError result;

    result = kzaApplicationSetScenePath(bfGetApplication(framework), "Scenes/main_menu_scene");
    kzsErrorForward(result);

    {
        struct KzuProjectObjectIterator it = kzuProjectGetObjectsOfType(bfGetProject(framework), KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE);
        while(kzuProjectObjectIterate(it))
        {
            struct KzuObjectNode* node = (struct KzuObjectNode*)kzuProjectObjectIteratorGetValue(it);

            if(kzuObjectNodeGetType(node) == KZU_OBJECT_TYPE_UI_COMPONENT)
            {
                if(!testAvailable[clMenuGetIndex(kzuUiComponentNodeFromObjectNode(node))])
                {
                    struct KzuObjectNode* labelNode = kzuObjectNodeGetChildAtIndex(node, 0);
                    if(kzuObjectNodeGetType(labelNode) == KZU_OBJECT_TYPE_MESH)
                    {
                        struct KzuMeshNode* meshNode = kzuMeshNodeFromObjectNode(labelNode);
                        struct KzuMaterial* material = kzuMeshGetMaterial(kzuMeshNodeGetMesh(meshNode));
                        struct KzuPropertyManager* propertyManager = kzuMaterialGetPropertyManager(material);
                        const struct KzuPropertyType* ambientPropertyType = kzuPropertyManagerFindPropertyType(propertyManager, "userColor");
                        const kzFloat color = 1.f;
                        struct KzcColorRGBA colorRGBA = kzcColorRGBA(color, 0.f, color*0.5f, 0.25f);
                        result = kzuPropertyManagerSetColor(propertyManager, material, ambientPropertyType, colorRGBA);
                        kzsErrorForward(result);
                    }
                }
            }
        }
    }

    result = kzuProjectLoaderLoadFont(bfGetProject(framework), "Resource Files/Fonts/gautami.ttf", &clMenu->font);
    kzsErrorForward(result);

    result = kzcTextLayouterCreate(bfGetMemoryManager(framework), clMenu->font, &clMenu->textLayouter);
    kzsErrorForward(result);

    clMenu->scene = kzaApplicationGetScene(bfGetApplication(framework));

    if(clMenu->state != CL_MENU_STATE_MENU && clMenu->state != CL_MENU_STATE_SPLASH)
    {
        kzuAnimationPlayerSetPaused(kzuSceneGetAnimationPlayer(clMenu->scene), KZ_TRUE);
    }

    result = kzuProjectLoaderLoadPropertyGroups(bfGetProject(framework));
    kzsErrorForward(result);

    clMenu->selectedMenuItem = INITIALLY_SELECTED_MENU_ITEM;

    kzsSuccess();
}

kzsError clMenuUninitialize(const struct BenchmarkFramework* framework, struct ClMenu* clMenu)
{
    kzsError result;

    KZ_UNUSED_PARAMETER(framework);

    if(clMenu->textLayouter != KZ_NULL)
    {
        result = kzcTextLayouterDelete(clMenu->textLayouter);
        kzsErrorForward(result);
        clMenu->textLayouter = KZ_NULL;
    }

    kzsSuccess();
}

kzsError clMenuUpdate(struct ApplicationData* applicationData, struct BenchmarkFramework* framework, struct ClMenu* clMenu, kzUint deltaTime)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(clMenu));

    kzuAnimationPlayerSetPaused(kzuSceneGetAnimationPlayer(clMenu->scene), (clMenu->state != CL_MENU_STATE_MENU && clMenu->state != CL_MENU_STATE_SPLASH));

    switch(clMenu->state)
    {
        case CL_MENU_STATE_SPLASH:
        {
            if(clMenu->selectPressed || clMenu->pointerDown)
            {
                clMenu->pointerDown = KZ_FALSE;
                clMenu->selectPressed = KZ_FALSE;
                clMenu->splashTime = SPLASH_GOAL_TIME;
            }

            clMenu->splashTime += (kzInt)deltaTime;
            if(clMenu->splashTime >= SPLASH_GOAL_TIME)
            {
                result = kzaApplicationSetScenePath(bfGetApplication(framework), "Scenes/main_menu_scene");
                kzsErrorForward(result);
                clMenu->scene = kzaApplicationGetScene(bfGetApplication(framework));
                kzuAnimationPlayerSetTime(kzuSceneGetAnimationPlayer(clMenu->scene), 0.0f);
                result = kzuAnimationPlayerUpdate(kzuSceneGetAnimationPlayer(clMenu->scene), 0.0f);
                kzsErrorForward(result);
                clMenu->state = CL_MENU_STATE_MENU;
            }
            break;
        }
        case CL_MENU_STATE_MENU:
        {
            if(clMenu->upPressed)
            {
                clMenu->selectedMenuItem--;
                clMenu->selectedMenuItem = kzsClampi(clMenu->selectedMenuItem, 0, OBJECT_NAME_COUNT - 1);
                if(!testAvailable[clMenu->selectedMenuItem])
                {
                    clMenu->selectedMenuItem++;
 
                    if(clMenuIsInSubmenu(clMenu))
                    {
                        if(clMenu->selectedMenuItem != clMenu->menuCategory)
                        {
                            kzInt id = clMenu->menuCategory;
                            while(testAvailable[++id]) 
                            {
                            }
                            clMenu->selectedMenuItem = id - 1;
                        }
                        else
                        {
                            clMenu->selectedMenuItem = BACK_BUTTON_ID;
                        }
                    }
                    else
                    {
                        if(clMenu->selectedMenuItem == EXIT_BUTTON_ID)
                        {
                            clMenu->selectedMenuItem = LAST_MAIN_MENU_SELECTION_INDEX;
                        }
                        else if(clMenu->selectedMenuItem == RUN_ALL_BUTTON_ID)
                        {
                            clMenu->selectedMenuItem = EXIT_BUTTON_ID;                            
                        }
                        else if(clMenu->selectedMenuItem == INITIALLY_SELECTED_MENU_ITEM)
                        {
                            clMenu->selectedMenuItem = RUN_ALL_BUTTON_ID;
                        }
                    }
                }
                clMenu->upPressed = KZ_FALSE;
            }
            if(clMenu->downPressed)
            {
                clMenu->selectedMenuItem++;
                clMenu->selectedMenuItem = kzsClampi(clMenu->selectedMenuItem, 0, OBJECT_NAME_COUNT - 1);
                if(!testAvailable[clMenu->selectedMenuItem])
                {
                    clMenu->selectedMenuItem--;

                    if(clMenuIsInSubmenu(clMenu))
                    {
                        if(clMenu->selectedMenuItem == BACK_BUTTON_ID)
                        {
                            clMenu->selectedMenuItem = clMenu->menuCategory;

                        }
                        else
                        {
                            clMenu->selectedMenuItem = BACK_BUTTON_ID;
                        }
                    }
                    else
                    {
                        if(clMenu->selectedMenuItem == EXIT_BUTTON_ID) 
                        {
                            clMenu->selectedMenuItem = RUN_ALL_BUTTON_ID;
                        }
                        else if(clMenu->selectedMenuItem == LAST_MAIN_MENU_SELECTION_INDEX)
                        {
                            clMenu->selectedMenuItem = EXIT_BUTTON_ID;                            
                        }
                        else if(clMenu->selectedMenuItem == RUN_ALL_BUTTON_ID)
                        {
                            clMenu->selectedMenuItem = INITIALLY_SELECTED_MENU_ITEM;
                        }
                    }
                }
                clMenu->downPressed = KZ_FALSE;
            }
            if(!clMenu->lastEventWasPointer)
            {
                result = clMenuUpdateColors(framework, clMenu, KZ_FALSE);
                kzsErrorForward(result);
            }

            break;
        }
        case CL_MENU_STATE_SELECT_PLATFORM:
        {
            struct CluInfo* cluInfo = bfGetCluInfo(framework);
            kzUint platformCount;
            kzsAssert(kzcIsValidPointer(cluInfo));
            platformCount = kzcDynamicArrayGetSize(cluInfo->platforms);
            if(clMenu->requestedPlatform >= 0)
            {
                clMenu->selectedPlatform = clMenu->requestedPlatform;
                clMenu->state = CL_MENU_STATE_SELECT_DEVICE;
                break;
            }
            if(clMenu->leftPressed)
            {
                clMenu->selectedPlatform--;
                clMenu->selectedPlatform = kzsClampi(clMenu->selectedPlatform, 0, platformCount - 1);
                clMenu->leftPressed = KZ_FALSE;
            }
            if(clMenu->rightPressed)
            {
                clMenu->selectedPlatform++;
                clMenu->selectedPlatform = kzsClampi(clMenu->selectedPlatform, 0, platformCount - 1);
                clMenu->rightPressed = KZ_FALSE;
            }
            if(clMenu->selectPressed && platformCount > 0)
            {
                clMenu->state = CL_MENU_STATE_SELECT_DEVICE;
                clMenu->selectPressed = KZ_FALSE;

                /*
                Autoselection for GPU was chosen so we must select the device already here in order to avoid 0.5s flashing of the device menu.
                TODO: Refactor this whole section to make it cleaner and most of all get rid of that goto.
                */
                if(clMenu->requestedDevice == -2)
                {
                    goto device_selection;
                }
            }
            break;
        }
        case CL_MENU_STATE_SELECT_DEVICE:
        {
            
            struct CluInfo* cluInfo;
            struct CluPlatformInfo* platformInfo;
            kzUint deviceCount;
            device_selection:
            cluInfo = bfGetCluInfo(framework);
            kzsAssert(kzcIsValidPointer(cluInfo));
            platformInfo = (struct CluPlatformInfo*)kzcDynamicArrayGet(cluInfo->platforms, clMenu->selectedPlatform);
            kzsAssert(kzcIsValidPointer(platformInfo));
            deviceCount = kzcDynamicArrayGetSize(platformInfo->devices);

            if(clMenu->leftPressed)
            {
                clMenu->selectedDevice--;
                clMenu->selectedDevice = kzsClampi(clMenu->selectedDevice, 0, deviceCount - 1);
                clMenu->leftPressed = KZ_FALSE;
            }
            if(clMenu->rightPressed)
            {
                clMenu->selectedDevice++;
                clMenu->selectedDevice = kzsClampi(clMenu->selectedDevice, 0, deviceCount - 1);
                clMenu->rightPressed = KZ_FALSE;
            }

            if(clMenu->requestedDevice == -2 || (clMenu->selectPressed && deviceCount > 0))
            {
                
                cl_context context;
                struct KzsSurfaceNative *nativeSurf = KZ_NULL;
                
                kzsAssert(cluInfo != KZ_NULL);
                if(clMenu->requestedDevice == -2)
                {
                   clMenu->selectedDevice = clMenu->requestedDevice;
                }


#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
                nativeSurf = kzsSurfaceGetNativeSurface(kzsWindowGetSurface(bfGetWindow(framework)));
#endif
                result = cluCreateGPUcontext(bfGetMemoryManager(framework), nativeSurf, 
                    clMenu->selectedPlatform, &clMenu->selectedDevice, &context); 
                kzsErrorForward(result);

                cluInfo->usedPlatform = clMenu->selectedPlatform;
                cluInfo->usedDevice = clMenu->selectedDevice;
                result = bfReportCreate(framework);
                kzsErrorForward(result);

                bfSetClContext(framework, context);
                {
                    kzBool autoRun;

                    result = clMenuProcessAutoStart(applicationData, framework, clMenu, &autoRun);
                    kzsErrorForward(result);

                    if(!autoRun)
                    {
                        kzInt skipSplash;
                        result = settingGetInt(bfGetSettings(framework), "SkipSplashScreen", &skipSplash);
                        kzsErrorForward(result);
                        if(skipSplash == 0)
                        {
                            clMenu->state = CL_MENU_STATE_SPLASH;
                            result = kzaApplicationSetScenePath(bfGetApplication(framework), "Scenes/splash_scene");
                            kzsErrorForward(result);
                        }
                        else
                        {
                            clMenu->state = CL_MENU_STATE_MENU;
                            result = kzaApplicationSetScenePath(bfGetApplication(framework), "Scenes/main_menu_scene");
                            kzsErrorForward(result);
                        }
                    }
                    else
                    {
                        clMenu->state = CL_MENU_STATE_MENU;
                    }
                }
                clMenu->scene = kzaApplicationGetScene(bfGetApplication(framework));
                kzuAnimationPlayerSetTime(kzuSceneGetAnimationPlayer(clMenu->scene), 0.0f);
                result = kzuAnimationPlayerUpdate(kzuSceneGetAnimationPlayer(clMenu->scene), 0.0f);
                kzsErrorForward(result);
                clMenu->selectPressed = KZ_FALSE;
            }
            if(clMenu->backPressed)
            {
                clMenu->state = CL_MENU_STATE_SELECT_PLATFORM;
                clMenu->backPressed = KZ_FALSE;
            }
            break;
        }
        default:
        {
            kzsAssertText(KZ_FALSE, "Invalid state.");
            break;
        }
    }

    kzsSuccess();
}

kzsError clMenuRender(const struct BenchmarkFramework* framework, struct ClMenu* clMenu)
{
    kzsError result;
    kzFloat x, y, width, height;
    struct KzcRenderer* coreRenderer;
    struct KzuRenderer* renderer;
    struct KzsWindow* window;
    kzBool selectActive = KZ_FALSE;
    kzBool leftActive = KZ_FALSE;
    kzBool rightActive = KZ_FALSE;
    struct CluInfo* cluInfo = bfGetCluInfo(framework);

    kzsAssert(kzcIsValidPointer(cluInfo));
    kzsAssert(kzcIsValidPointer(clMenu));

    renderer = kzuEngineGetRenderer(bfGetEngine(framework));
    coreRenderer = kzuRendererGetCoreRenderer(renderer);
    window = bfGetWindow(framework);
    kzuRendererGetAdjustedViewport(renderer, window, &x, &y, &width, &height);

    /* Touch screen controls for platform and device screens. */
    if(clMenu->state == CL_MENU_STATE_SELECT_PLATFORM || clMenu->state == CL_MENU_STATE_SELECT_DEVICE)
    {
        kzInt windowWidth;
        kzInt windowHeight;
        kzInt areaWidth;
        kzInt areaHeight;
        kzInt px;
        kzInt py;
        kzsAssert(window != KZ_NULL);
        windowWidth = kzsWindowGetWidth(window);
        windowHeight = kzsWindowGetHeight(window);
        areaWidth = MAX_WIDTH_PLATFORM >> 1;
        areaHeight = windowHeight / 5;
        px = clMenu->pointerX;
        py = clMenu->pointerY;
        if(py > areaHeight && py < windowHeight - areaHeight)
        {
            if(px > (windowWidth / 2) - areaWidth && px < (windowWidth / 2) + areaWidth)
            {
                if(clMenu->pointerDown)
                {
                    selectActive = KZ_TRUE;
                }
                if(clMenu->pointerUp)
                {
                    clMenu->selectPressed = KZ_TRUE;
                }
            }
            if(px < (windowWidth / 2) - areaWidth)
            {
                if(clMenu->pointerDown)
                {
                    leftActive = KZ_TRUE;
                }
                if(clMenu->pointerUp)
                {
                    clMenu->leftPressed = KZ_TRUE;
                }
            }
            if(px > (windowWidth / 2) + areaWidth)
            {
                if(clMenu->pointerDown)
                {
                    rightActive = KZ_TRUE;
                }
                if(clMenu->pointerUp)
                {
                    clMenu->rightPressed = KZ_TRUE;
                }
            }
        }
    }
    clMenu->pointerUp = KZ_FALSE;


    switch(clMenu->state)
    {
        case CL_MENU_STATE_SPLASH:
        {
            break;
        }
        case CL_MENU_STATE_MENU:
        {
            break;
        }
        case CL_MENU_STATE_SELECT_PLATFORM:
        {
            kzUint platformCount;
            kzFloat lineHeight;
            kzFloat yOffset;

            kzsAssert(kzcIsValidPointer(cluInfo));
            platformCount = kzcDynamicArrayGetSize(cluInfo->platforms);
            kzsAssert(window != KZ_NULL);

            result = kzcFontSetColor(clMenu->font, KZC_COLOR_BLACK);
            kzsErrorForward(result);

            kzcTextLayouterSetVerticalAlignment(clMenu->textLayouter, KZC_TEXT_VERTICAL_ALIGNMENT_BASELINE);

            result = kzcFreetypeFontSetSize(kzcFreetypeFontFromFont(clMenu->font), 30.f);
            kzsErrorForward(result);

            result = kzcFontGetLineHeight(clMenu->font, &lineHeight);
            kzsErrorForward(result);

            if(platformCount > 0)
            {
                kzInt i = 0;
                struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(cluInfo->platforms);

                kzcTextLayouterSetHorizontalAlignment(clMenu->textLayouter, KZC_TEXT_HORIZONTAL_ALIGNMENT_CENTER);

                result = clMenuDrawText_internal(clMenu->font, clMenu->textLayouter, bfGetMemoryManager(framework), coreRenderer, "Choose CL platform", width * 0.5f, height - lineHeight, KZ_NULL, &yOffset);
                kzsErrorForward(result);

                kzcTextLayouterSetVerticalAlignment(clMenu->textLayouter, KZC_TEXT_VERTICAL_ALIGNMENT_CENTER);

                result = kzcFreetypeFontSetSize(kzcFreetypeFontFromFont(clMenu->font), 18.f);
                kzsErrorForward(result);

                {
                    kzMutableString countString;
                    result = kzcStringFormat(bfGetQuickManager(framework), "%d / %d", &countString, (1 + clMenu->selectedPlatform), platformCount);
                    kzsErrorForward(result);
                    result = clMenuDrawText_internal(clMenu->font, clMenu->textLayouter, bfGetMemoryManager(framework), coreRenderer, countString, width * 0.5f, lineHeight, KZ_NULL, KZ_NULL);
                    kzsErrorForward(result);
                    result = kzcStringDelete(countString);
                    kzsErrorForward(result);
                }

                result = kzcFontGetLineHeight(clMenu->font, &lineHeight);
                kzsErrorForward(result);

                while (kzcDynamicArrayIterate(it))
                {
                    struct CluPlatformInfo* info = (struct CluPlatformInfo*)kzcDynamicArrayIteratorGetValue(it);
                    kzFloat yMod;
                    kzMutableString str;
                    kzFloat xPosition = width * 0.5f + (kzFloat)(i - clMenu->selectedPlatform) * MAX_WIDTH_PLATFORM;

                    if(i == clMenu->selectedPlatform)
                    {
                        result = kzcFontSetColor(clMenu->font, KZC_COLOR_RED);
                        kzsErrorForward(result);
                    }

                    result = kzcStringConcatenateMultiple(kzcMemoryGetManager(clMenu), 7, 
                        &str, 
                        info->name, "\n",
                        info->profile, "\n",
                        info->version, "\n",
                        info->vendor);
                    kzsErrorForward(result);

                    result = clMenuDrawText_internal(clMenu->font, clMenu->textLayouter, bfGetMemoryManager(framework), coreRenderer, str, xPosition, height * 0.5f, KZ_NULL,  &yMod);
                    kzsErrorForward(result);

                    if(selectActive && i == clMenu->selectedPlatform)
                    {
                        result = clMenuDrawText_internal(clMenu->font, clMenu->textLayouter, bfGetMemoryManager(framework), coreRenderer, str, xPosition + 1, height * 0.5f + 1, KZ_NULL,  &yMod);
                        kzsErrorForward(result);
                    }
                    if(leftActive && i < clMenu->selectedPlatform)
                    {
                        result = clMenuDrawText_internal(clMenu->font, clMenu->textLayouter, bfGetMemoryManager(framework), coreRenderer, str, xPosition + 1, height * 0.5f + 1, KZ_NULL,  &yMod);
                        kzsErrorForward(result);
                    }
                    if(rightActive && i >clMenu->selectedPlatform)
                    {
                        result = clMenuDrawText_internal(clMenu->font, clMenu->textLayouter, bfGetMemoryManager(framework), coreRenderer, str, xPosition + 1, height * 0.5f + 1, KZ_NULL,  &yMod);
                        kzsErrorForward(result);
                    }

                    result = kzcStringDelete(str);
                    kzsErrorForward(result);

                    if(i == clMenu->selectedPlatform)
                    {
                        result = kzcFontSetColor(clMenu->font, KZC_COLOR_BLACK);
                        kzsErrorForward(result);
                    }
                    i++;
                }
            }
            else
            {
                kzString errorMessage1 = "No OpenCL platform available.";
                kzString errorMessage2 = "Update your drivers or hardware.";

                kzcTextLayouterSetHorizontalAlignment(clMenu->textLayouter, KZC_TEXT_HORIZONTAL_ALIGNMENT_CENTER);
                kzcTextLayouterSetVerticalAlignment(clMenu->textLayouter, KZC_TEXT_VERTICAL_ALIGNMENT_BASELINE);

                result = clMenuDrawText_internal(clMenu->font, clMenu->textLayouter, bfGetMemoryManager(framework), coreRenderer, errorMessage1, width * 0.5f, (height + lineHeight) * 0.5f, KZ_NULL, KZ_NULL);
                kzsErrorForward(result);
                result = clMenuDrawText_internal(clMenu->font, clMenu->textLayouter, bfGetMemoryManager(framework), coreRenderer, errorMessage2, width * 0.5f, (height - lineHeight) * 0.5f, KZ_NULL, KZ_NULL);
                kzsErrorForward(result);
            }
            break;
        }
        case CL_MENU_STATE_SELECT_DEVICE:
        {
            struct CluPlatformInfo* platformInfo = (struct CluPlatformInfo*)kzcDynamicArrayGet(cluInfo->platforms, clMenu->selectedPlatform);
            kzUint deviceCount;
            kzUint maxWidthDevice = 350;
            kzFloat lineHeight;
            kzFloat yOffset;

            kzsAssert(kzcIsValidPointer(platformInfo));

            deviceCount = kzcDynamicArrayGetSize(platformInfo->devices);

            result = kzcFontSetColor(clMenu->font, KZC_COLOR_BLACK);
            kzsErrorForward(result);

            kzcTextLayouterSetVerticalAlignment(clMenu->textLayouter, KZC_TEXT_VERTICAL_ALIGNMENT_BASELINE);

            result = kzcFreetypeFontSetSize(kzcFreetypeFontFromFont(clMenu->font), 30.f);
            kzsErrorForward(result);

            result = kzcFontGetLineHeight(clMenu->font, &lineHeight);
            kzsErrorForward(result);

            if(deviceCount > 0)
            {
                kzInt i = 0;
                struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(platformInfo->devices);

                kzcTextLayouterSetHorizontalAlignment(clMenu->textLayouter, KZC_TEXT_HORIZONTAL_ALIGNMENT_CENTER);

                result = clMenuDrawText_internal(clMenu->font, clMenu->textLayouter, bfGetMemoryManager(framework), coreRenderer, "Choose CL device for platform", width * 0.5f, height - lineHeight, KZ_NULL, &yOffset);
                kzsErrorForward(result);

                kzcTextLayouterSetVerticalAlignment(clMenu->textLayouter, KZC_TEXT_VERTICAL_ALIGNMENT_CENTER);

                result = kzcFreetypeFontSetSize(kzcFreetypeFontFromFont(clMenu->font), 18.f);
                kzsErrorForward(result);

                {
                    kzMutableString countString;
                    result = kzcStringFormat(bfGetQuickManager(framework), "%d / %d", &countString, (1 + clMenu->selectedDevice), deviceCount);
                    result = clMenuDrawText_internal(clMenu->font, clMenu->textLayouter, bfGetMemoryManager(framework), coreRenderer, countString, width * 0.5f, lineHeight, KZ_NULL, KZ_NULL);
                    kzsErrorForward(result);
                    result = kzcStringDelete(countString);
                    kzsErrorForward(result);
                }

                result = kzcFontGetLineHeight(clMenu->font, &lineHeight);
                kzsErrorForward(result);

                while (kzcDynamicArrayIterate(it))
                {
                    struct CluDeviceInfo* info = (struct CluDeviceInfo*)kzcDynamicArrayIteratorGetValue(it);
                    kzFloat yMod;
                    kzMutableString str;
                    kzFloat xPosition = width * 0.5f + (kzFloat)(i - clMenu->selectedDevice) * maxWidthDevice;

                    if(i == clMenu->selectedDevice)
                    {
                        result = kzcFontSetColor(clMenu->font, KZC_COLOR_RED);
                        kzsErrorForward(result);
                    }

                    result = kzcStringConcatenateMultiple(kzcMemoryGetManager(clMenu), 9, 
                        &str, 
                        info->name, "\n",
                        info->vendor, "\n",
                        info->deviceVersion, "\n",
                        info->vendorId, "\n",
                        info->deviceType);
                    kzsErrorForward(result);

                    result = clMenuDrawText_internal(clMenu->font, clMenu->textLayouter, bfGetMemoryManager(framework), coreRenderer, str, xPosition, height * 0.5f, KZ_NULL,  &yMod);
                    kzsErrorForward(result);

                    if(selectActive && i == clMenu->selectedDevice)
                    {
                        result = clMenuDrawText_internal(clMenu->font, clMenu->textLayouter, bfGetMemoryManager(framework), coreRenderer, str, xPosition + 1, height * 0.5f + 1, KZ_NULL,  &yMod);
                        kzsErrorForward(result);
                    }
                    if(leftActive && i < clMenu->selectedDevice)
                    {
                        result = clMenuDrawText_internal(clMenu->font, clMenu->textLayouter, bfGetMemoryManager(framework), coreRenderer, str, xPosition + 1, height * 0.5f + 1, KZ_NULL,  &yMod);
                        kzsErrorForward(result);
                    }
                    if(rightActive && i >clMenu->selectedDevice)
                    {
                        result = clMenuDrawText_internal(clMenu->font, clMenu->textLayouter, bfGetMemoryManager(framework), coreRenderer, str, xPosition + 1, height * 0.5f + 1, KZ_NULL,  &yMod);
                        kzsErrorForward(result);
                    }

                    result = kzcStringDelete(str);
                    kzsErrorForward(result);

                    if(i == clMenu->selectedDevice)
                    {
                        result = kzcFontSetColor(clMenu->font, KZC_COLOR_BLACK);
                        kzsErrorForward(result);
                    }

                    /* Check if CL version is good enough and print warning if not. */
                    {
                        kzInt openCLVersion[2]; /* [0] = Major, [1] = Minor. */
                        kzString p;
                        kzMutableString versionString;
                        kzInt clVersionIndex = 0;
                        result = kzcStringCopy(kzcMemoryGetManager(renderer), info->deviceVersion, &versionString);
                        kzsErrorForward(result);
                        p = kzsStrtok(versionString, ". ");
                        while(clVersionIndex < 3)
                        {
                            if(clVersionIndex == 1 || clVersionIndex == 2)
                            {
                                openCLVersion[clVersionIndex - 1] = kzsAtoi(p);
                            }
                            p = strtok(KZ_NULL, ". ");
                            clVersionIndex++;
                        }
                        result = kzcStringDelete(versionString);
                        kzsErrorForward(result);

                        if((kzsTimeGetCurrentTimestamp() % 1000) < 750)
                        {
                            if(openCLVersion[0] < 1 || (openCLVersion[0] == 1 && openCLVersion[1] == 0))
                            {
                                kzString errorMessage1 = "OpenCL 1.1 required. Benchmark may not work correctly on this device.";

                                result = kzcFontSetColor(clMenu->font, KZC_COLOR_RED);
                                kzsErrorForward(result);

                                kzcTextLayouterSetHorizontalAlignment(clMenu->textLayouter, KZC_TEXT_HORIZONTAL_ALIGNMENT_CENTER);
                                kzcTextLayouterSetVerticalAlignment(clMenu->textLayouter, KZC_TEXT_VERTICAL_ALIGNMENT_BASELINE);

                                result = clMenuDrawText_internal(clMenu->font, clMenu->textLayouter, bfGetMemoryManager(framework), coreRenderer, errorMessage1, 
                                    width * 0.5f, lineHeight * 3, KZ_NULL, KZ_NULL);
                                kzsErrorForward(result);
                                result = clMenuDrawText_internal(clMenu->font, clMenu->textLayouter, bfGetMemoryManager(framework), coreRenderer, errorMessage1, 
                                    width * 0.5f + 1, lineHeight * 3 + 1, KZ_NULL, KZ_NULL);
                                kzsErrorForward(result);

                                result = kzcFontSetColor(clMenu->font, KZC_COLOR_BLACK);
                                kzsErrorForward(result);
                            }
                        }
                    }

                    i++;
                }

            } 
            else
            {
                kzString errorMessage1 = "No OpenCL devices for platform available.";

                kzcTextLayouterSetHorizontalAlignment(clMenu->textLayouter, KZC_TEXT_HORIZONTAL_ALIGNMENT_CENTER);
                kzcTextLayouterSetVerticalAlignment(clMenu->textLayouter, KZC_TEXT_VERTICAL_ALIGNMENT_BASELINE);

                result = clMenuDrawText_internal(clMenu->font, clMenu->textLayouter, bfGetMemoryManager(framework), coreRenderer, errorMessage1, width * 0.5f, height * 0.5f, KZ_NULL, KZ_NULL);
                kzsErrorForward(result);
            }
            break;
        }
        default:
        {
            kzsAssertText(KZ_FALSE, "Invalid state.");
            break;
        }
    }

    kzsSuccess();
}

kzsError clMenuKeyDeviceHandler(struct BenchmarkFramework* framework, struct ClMenu* clMenu, const struct KzsKeyDeviceInputData* inputData)
{
    enum KzsInputKey button;
    enum KzsInputKeyDeviceState state;

    kzsAssert(kzcIsValidPointer(clMenu));

    clMenu->lastEventWasPointer = KZ_FALSE;
    button = kzsInputGetKeyDeviceButton(inputData);
    state = kzsInputGetKeyDeviceState(inputData);

    if(state == KZS_KEY_DEVICE_STATE_DOWN || state == KZS_KEY_DEVICE_STATE_UP)
    {
        kzBool pressed = (state == KZS_KEY_DEVICE_STATE_DOWN);
        switch(button)
        {
            case KZS_KEY_ARROW_UP:
            {
                clMenu->upPressed = pressed;
                break;
            }
            case KZS_KEY_ARROW_DOWN:
            {
                clMenu->downPressed = pressed;
                break;
            }
            case KZS_KEY_ARROW_LEFT:
            {
                clMenu->leftPressed = pressed;
                break;
            }
            case KZS_KEY_ARROW_RIGHT:
            {
                clMenu->rightPressed = pressed;
                break;
            }
            case KZS_KEY_ENTER:
            {
                clMenu->selectPressed = pressed;
                break;
            }
            case KZS_KEY_BACKSPACE:
            {
                clMenu->backPressed = pressed;
                break;
            }
            default:
            {
                break;
            }
        }
    }

    kzsSuccess();
}

kzsError clMenuPointingDeviceHandler(struct BenchmarkFramework* framework, struct ClMenu* clMenu, const struct KzsPointingDeviceInputData* inputData)
{
    kzsError result;
    enum KzsInputPointingDeviceState state;
    kzBool pointerEvent = KZ_FALSE;
    
    kzsAssert(kzcIsValidPointer(clMenu));

    clMenu->pointerX = kzsInputGetPointingDeviceX(inputData);
    clMenu->pointerY = kzsInputGetPointingDeviceY(inputData);

    state = kzsInputGetPointingDeviceState(inputData);

    switch(state)
    {
        case KZS_POINTING_DEVICE_STATE_DOWN:
        {
            pointerEvent = KZ_TRUE;
            clMenu->pointerDown = KZ_TRUE;
            break;
        }
        case KZS_POINTING_DEVICE_STATE_UP:
        {
            pointerEvent = KZ_TRUE;
            clMenu->pointerDown = KZ_FALSE;
            clMenu->pointerUp = KZ_TRUE;
            break;
        }
        case KZS_POINTING_DEVICE_STATE_MOVE:
        {
            break;
        }
        case KZS_POINTING_DEVICE_STATE_DRAG:
        {
            pointerEvent = KZ_TRUE;
            break;
        }
        default:
        {
            break;
        }
    }

    if(pointerEvent && !clMenu->lastEventWasPointer)
    {
        result = clMenuUpdateColors(framework, clMenu, KZ_TRUE);
        kzsErrorForward(result);
        clMenu->lastEventWasPointer = KZ_TRUE;
    }

    kzsSuccess();
}

kzInt clMenuGetIndex(const struct KzuUiComponentNode* node)
{
    kzInt returnValue = -1;
    kzUint index = 0;
    struct KzuObjectNode* object = kzuUiComponentNodeToObjectNode(node);
    kzString objectName = kzuObjectNodeGetName(object);

    while(index < OBJECT_NAME_COUNT)
    {
        if(kzcStringCompare(objectName, testObjectNames[index]) == 0)
        {
            returnValue = index;
            break;
        }
        index++;
    }
    return returnValue;
}

KZ_CALLBACK kzsError clMenuHoverHandler(const struct KzuUiComponentNode* node, void* userData, struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;
    {
        struct KzuObjectNode* labelNode = kzuObjectNodeGetChildAtIndex(kzuUiComponentNodeToObjectNode(node), 0);
        if(testAvailable[clMenuGetIndex(node)]) 
        {
            if(kzuObjectNodeGetType(labelNode) == KZU_OBJECT_TYPE_MESH)
            {
                struct KzuMeshNode* meshNode = kzuMeshNodeFromObjectNode(labelNode);
                struct KzuMaterial* material = kzuMeshGetMaterial(kzuMeshNodeGetMesh(meshNode));
                struct KzuPropertyManager* propertyManager = kzuMaterialGetPropertyManager(material);
                const struct KzuPropertyType* ambientPropertyType = kzuPropertyManagerFindPropertyType(propertyManager, "userColor");
                const kzFloat color = 1.0f;
                struct KzcColorRGBA colorRGBA = kzcColorRGBA(color, color, color, 1.0f);
                result = kzuPropertyManagerSetColor(propertyManager, material, ambientPropertyType, colorRGBA);
                kzsErrorForward(result);
            }
        }   
    }
    kzsSuccess();
}

KZ_CALLBACK kzsError clMenuDehoverHandler(const struct KzuUiComponentNode* node, void* userData, struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;
    {
        struct KzuObjectNode* labelNode = kzuObjectNodeGetChildAtIndex(kzuUiComponentNodeToObjectNode(node), 0);
        if(testAvailable[clMenuGetIndex(node)]) 
        {
            if(kzuObjectNodeGetType(labelNode) == KZU_OBJECT_TYPE_MESH)
            {
                struct KzuMeshNode* meshNode = kzuMeshNodeFromObjectNode(labelNode);
                struct KzuMaterial* material = kzuMeshGetMaterial(kzuMeshNodeGetMesh(meshNode));
                struct KzuPropertyManager* propertyManager = kzuMaterialGetPropertyManager(material);
                const struct KzuPropertyType* ambientPropertyType = kzuPropertyManagerFindPropertyType(propertyManager, "userColor");
                const kzFloat color = 0.5f;
                struct KzcColorRGBA colorRGBA = kzcColorRGBA(color, color, color, 1.0f);
                result = kzuPropertyManagerSetColor(propertyManager, material, ambientPropertyType, colorRGBA);
                kzsErrorForward(result);
            }
        }
    }
    kzsSuccess();
}


static kzsError clMenuDrawText_internal(struct KzcFont* font, const struct KzcTextLayouter* textLayouter, const struct KzcMemoryManager* memoryManager,
    struct KzcRenderer* renderer, kzString text, kzFloat x, kzFloat y, kzFloat* out_xSpace, kzFloat* out_ySpace)
{
    kzsError result;
    kzFloat spaceX;
    kzFloat spaceY;
    struct KzcTextLayout* textLayout;

    result = kzcTextLayouterCreateLayout(textLayouter, memoryManager, text, &textLayout);
    kzsErrorForward(result);
    spaceX = textLayout->width;
    spaceY = textLayout->height;

    result = kzcFontDrawTextLayout(textLayout, renderer, x, y);
    kzsErrorForward(result);

    result = kzcTextLayoutDelete(textLayout);
    kzsErrorForward(result);

    if(out_xSpace != KZ_NULL)
    {
        *out_xSpace = spaceX;
    }

    if(out_ySpace != KZ_NULL)
    {
        *out_ySpace = spaceY;
    }

    kzsSuccess();
}

kzsError clMenuCreateContextAndSkipPlatformMenu(struct ClMenu* clMenu, struct BenchmarkFramework* framework, kzInt platform, kzInt device, kzBool* out_success)
{
    kzsError result;
    cl_context context;
    kzBool success = KZ_FALSE;
    struct CluInfo* cluInfo = bfGetCluInfo(framework);

    kzsAssert(kzcIsValidPointer(cluInfo));

#if CLU_OPENCL_VERSION == CLU_OPENCL_NONE

    clMenu->state = CL_MENU_STATE_SPLASH;
    result = kzaApplicationSetScenePath(bfGetApplication(framework), "Scenes/splash_scene");
    kzsErrorForward(result);
    kzcLogDebug("Skipping platform selection. OPENCL_NONE; No OpenCL is used in this build.");

#else

    if(platform < 0 || device < 0)
    {
        clMenu->requestedDevice = device;
        clMenu->requestedPlatform = platform;
        success = KZ_FALSE;
    }
    else
    {
        if(platform < (kzInt)kzcDynamicArrayGetSize(cluInfo->platforms))
        {
            struct CluPlatformInfo* platformInfo = (struct CluPlatformInfo*)kzcDynamicArrayGet(cluInfo->platforms, platform);
            kzUint deviceCount;

            kzsAssert(kzcIsValidPointer(platformInfo));
            deviceCount = kzcDynamicArrayGetSize(platformInfo->devices);

            if(device < (kzInt)deviceCount)
            {
                cluInfo->usedPlatform = platform;
                cluInfo->usedDevice = device;
                result = bfReportCreate(framework);
                kzsErrorForward(result);
    #if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
                result = cluCreateGPUcontext(bfGetMemoryManager(framework), kzsSurfaceGetNativeSurface(kzsWindowGetSurface(bfGetWindow(framework))), platform, &device, &context);
    #else
                result = cluCreateGPUcontext(bfGetMemoryManager(framework), KZ_NULL, platform, device, &context);
    #endif
                kzsErrorForward(result);
                bfSetClContext(framework, context);
                {
                    kzInt skipSplash;
                    result = settingGetInt(bfGetSettings(framework), "SkipSplashScreen", &skipSplash);
                    kzsErrorForward(result);
                    if(skipSplash == 0)
                    {
                        clMenu->state = CL_MENU_STATE_SPLASH;
                        result = kzaApplicationSetScenePath(bfGetApplication(framework), "Scenes/splash_scene");
                        kzsErrorForward(result);
                    }
                    else
                    {
                        clMenu->state = CL_MENU_STATE_MENU;
                        result = kzaApplicationSetScenePath(bfGetApplication(framework), "Scenes/main_menu_scene");
                        kzsErrorForward(result);
                    }
                }
                clMenu->scene = kzaApplicationGetScene(bfGetApplication(framework));
                kzuAnimationPlayerSetTime(kzuSceneGetAnimationPlayer(clMenu->scene), 0.0f);
                result = kzuAnimationPlayerUpdate(kzuSceneGetAnimationPlayer(clMenu->scene), 0.0f);
                kzsErrorForward(result);
                success = KZ_TRUE;
                clMenu->selectedDevice = device;
                clMenu->selectedPlatform = platform;
            }
        }
        if(!success)
        {
            if(platform >= 0 && device >= 0)
            {
                result = kzcLog(bfGetMemoryManager(framework), KZS_LOG_LEVEL_WARNING, "Invalid OpenCL platform/device combination found in settings file. Fallbacking to platform menu.");
                kzsErrorForward(result);
            }
        }
    }

#endif

    *out_success = success;
    kzsSuccess();
}

KZ_CALLBACK kzsError clMenuSelectHandler(const struct KzuUiComponentNode* node, void* userData, struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;
    struct ClMenu* menu = userData;

    struct KzcHashMapIterator it = kzcHashMapGetIterator(parameters);
    while(kzcHashMapIterate(it))
    {
        kzString name = (kzString)kzcHashMapIteratorGetKey(it);
        if(kzcStringCompare(name, "IntValue") == 0)
        {
            struct KzuIntProperty* property = kzcHashMapIteratorGetValue(it);
            kzInt value = kzuIntPropertyGetValue(property);
            menu->selectedMenuItem = value * 10;
            menu->menuCategory = value * 10;
            result = kzuIntPropertyDelete(property);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

KZ_CALLBACK kzsError clMenuBackHandler(const struct KzuUiComponentNode* node, void* userData, struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    struct ClMenu* menu = userData;

    menu->selectPressed = KZ_FALSE;
    menu->selectedMenuItem = INITIALLY_SELECTED_MENU_ITEM;
    menu->menuCategory = INITIALLY_SELECTED_MENU_ITEM;

    kzsSuccess();
}

kzsError clMenuUpdateColors(struct BenchmarkFramework* framework, struct ClMenu* clMenu, kzBool resetToInactive)
{
    kzsError result;

    struct KzuProjectObjectIterator it = kzuProjectGetObjectsOfType(bfGetProject(framework), KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE);
    while(kzuProjectObjectIterate(it))
    {
        struct KzuObjectNode* node = (struct KzuObjectNode*)kzuProjectObjectIteratorGetValue(it);

        if(kzuObjectNodeGetType(node) == KZU_OBJECT_TYPE_UI_COMPONENT)
        {
            kzInt itemIndex = clMenuGetIndex(kzuUiComponentNodeFromObjectNode(node));
            kzsAssert(itemIndex >= 0);
            if(testAvailable[itemIndex])
            {
                struct KzuObjectNode* labelNode = kzuObjectNodeGetChildAtIndex(node, 0);
                if(kzuObjectNodeGetType(labelNode) == KZU_OBJECT_TYPE_MESH)
                {
                    struct KzuMeshNode* meshNode = kzuMeshNodeFromObjectNode(labelNode);
                    struct KzuMaterial* material = kzuMeshGetMaterial(kzuMeshNodeGetMesh(meshNode));
                    struct KzuPropertyManager* propertyManager = kzuMaterialGetPropertyManager(material);
                    const struct KzuPropertyType* ambientPropertyType = kzuPropertyManagerFindPropertyType(propertyManager, "userColor");
                    struct KzcColorRGBA colorRGBA; 
                    if(itemIndex == (kzInt)clMenu->selectedMenuItem && !resetToInactive)
                    {
                        const kzFloat color = 1.0f;
                        colorRGBA = kzcColorRGBA(color, color, color, 1.0f);
                    }
                    else
                    {
                        const kzFloat color = 0.5f;
                        colorRGBA = kzcColorRGBA(color, color, color, 1.0f);
                    }
                    result = kzuPropertyManagerSetColor(propertyManager, material, ambientPropertyType, colorRGBA);
                    kzsErrorForward(result);

                    if(((clMenu->selectPressed && itemIndex == (kzInt)clMenu->selectedMenuItem) /* selection */
                     || (clMenu->backPressed && clMenu->selectedMenuItem < INITIALLY_SELECTED_MENU_ITEM && itemIndex == BACK_BUTTON_ID)) /* back button */
                     && !resetToInactive)
                    {
                        struct KzuUiEvent* event = kzuUiScreenGetEvent(kzuSceneGetUiScreen(clMenu->scene));
                        result = kzuUiComponentNodeHandleEvents((struct KzuUiComponentNode*)node, event, KZU_UI_BUTTON_RELEASED_EVENT_STRING, 
                            kzuEngineGetMessageQueue(bfGetEngine(framework)));
                        kzsErrorForward(result);
                        clMenu->selectPressed = KZ_FALSE;
                        clMenu->backPressed = KZ_FALSE;
                    }
                }
            }
        }
    }
    kzsSuccess();
}

kzBool clMenuEscapePressed(struct BenchmarkFramework* framework, struct ClMenu* clMenu)
{
    kzBool value = KZ_TRUE;
    if(clMenu->selectedMenuItem < INITIALLY_SELECTED_MENU_ITEM)
    {
        clMenu->backPressed = KZ_TRUE;
        value = KZ_FALSE;
    }
    if(clMenu->state == CL_MENU_STATE_SELECT_DEVICE)
    {
        clMenu->backPressed = KZ_TRUE;
        value = KZ_FALSE;
    }
    return value;
}

kzBool clMenuIsInSubmenu(struct ClMenu* clMenu)
{
    kzBool inSubMenu = KZ_TRUE;
    if(clMenu->selectedMenuItem >= RUN_ALL_BUTTON_ID)
    {
        inSubMenu = KZ_FALSE;
    }
    if(clMenu->selectedMenuItem == BACK_BUTTON_ID)
    {
        inSubMenu = KZ_TRUE;
    }
    return inSubMenu;
}

kzsError clMenuGetTestIndexForName(struct KzcMemoryManager* memoryManager, kzString benchmarkTestName, kzInt* out_value)
{
    kzsError result;
    kzMutableString lowerCaseTestName;
    kzInt i;
    kzInt value = -1;
    
    result = kzcStringAsciiToLowerCase(memoryManager, benchmarkTestName, &lowerCaseTestName);
    kzsErrorForward(result);
    
    for(i = 0; i < OBJECT_NAME_COUNT; ++i)
    {
        kzMutableString testNameLowerCase;
        result = kzcStringAsciiToLowerCase(memoryManager, testObjectNames[i], &testNameLowerCase);
        kzsErrorForward(result);

        if(kzcStringCompare(lowerCaseTestName, testNameLowerCase) == 0)
        {
            value = i;
        }

        result = kzcStringDelete(testNameLowerCase);
        kzsErrorForward(result);

        if(value != -1)
        {
            break;
        }
    }

    result = kzcStringDelete(lowerCaseTestName);
    kzsErrorForward(result);

    *out_value = value;
    kzsSuccess();
}
