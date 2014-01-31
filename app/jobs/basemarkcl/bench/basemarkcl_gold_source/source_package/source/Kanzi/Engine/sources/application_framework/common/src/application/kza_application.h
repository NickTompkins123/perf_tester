/**
 * \file
 * Application framework.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZA_APPLICATION_H
#define KZA_APPLICATION_H


#include <user/util/kzu_transform_gizmo.h>
#include <user/util/kzu_free_camera.h>
#include <user/ui/kzu_ui_manager.h>

#include <core/util/collection/kzc_dynamic_array.h>

#include <system/display/kzs_window.h>
#include <system/display/kzs_surface.h>
#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzaApplication;
struct KzaPlatform;
struct KzaTransition;
struct KzuScript;
struct KzuLua;
struct KzuObjectNode;
struct KzuCameraNode;
struct KzuScene;
struct KzcMatrix4x4;
struct KzsDisplay;
struct KzcFont;
struct KzsWindowProperties;


/** Enumeration for different application states. */
enum KzaApplicationState
{
    KZA_APPLICATION_STATE_UNINITIALIZED,    /**< Application is not yet initialized. */
    KZA_APPLICATION_STATE_RUNNING,          /**< Application is running normally. */
    KZA_APPLICATION_STATE_PAUSED,           /**< Application is paused. Screen is updated but the timer is stopped. */
    KZA_APPLICATION_STATE_QUITTING,         /**< Application is quitting without errors, will be closed immediately. */
    KZA_APPLICATION_STATE_ERROR             /**< Application has encountered an error and will be shut down immediately. */
};

/** Enumeration for different event types. */
enum KzaApplicationChangeEventType
{
    KZA_APPLICATION_RESTARTED, /**< Application restart requested. */
    KZA_APPLICATION_CHANGE_EVENT_SCENE_CHANGED, /**< Scene changed. */
    KZA_APPLICATION_RESUMED, /**< Application resumed from pause. */
    KZA_APPLICATION_PAUSED /**< Application paused. */
};

enum KzaToolPickMode
{
    KZA_TOOL_PICK_MODE_NODE,
    KZA_TOOL_PICK_MODE_MATERIAL
};

/** Configurable application properties. */
struct KzaApplicationProperties
{
    kzUint memoryPoolCount; /**< Number of memory pools to allocate. */
    kzUint memoryPoolSize; /**< Size of a single memory pool. Total allocated memory is therefore count * size. */
    struct KzsWindowProperties windowProperties; /**< Configuration for primary window created by application. */
    struct KzsSurfaceProperties surfaceProperties; /**< Properties for surface of primary window. */
    kzString binaryName; /**< Path to either the binary config file listing all .kzb files that are loaded to project, or a path to a single .kzb file. */
    kzString scenePath; /**< Path of the default scene in .KZB. Can be KZ_NULL. */
    KzuUiActionCallback mainHandler; /**< Main UI handler. */
    kzBool gridEnabled;                         /**< Is grid enabled by default. */
    kzBool debugObjectsEnabled;                 /**< Are debug objects enabled by default. */
    kzBool freeCameraEnabled;                   /**< Is free camera enabled by default. */
    kzBool pickingEnabled;                      /**< Is picking enabled by default. */
    kzBool transformGizmoEnabled;               /**< Is transform gizmo enabled by default. */
    kzBool boundingBoxEnabled;                  /**< Is bounding box enabled by default. */
    kzBool fpsInfoEnabled;                      /**< Is FPS info enabled by default. */
    kzBool logVisualizationEnabled;             /**< Is log visualization enabled by default. */
    kzBool clearBackgroundEnabled;              /**< Is clearing background enabled / disabled. */
    kzBool loadStartupScene;                    /**< Is startup scene loaded from project. */
};

/** Read-only values from the system. */
struct KzaSystemProperties
{
    kzString* programArguments; /**< An array of program arguments from the system. First element in the array contains the full path and executable name (if applicable) and the actual command line parameters are after that.*/
    kzUint programArgumentCount; /**< Number of arguments in the array. */
};

/** Change events. */
struct KzaApplicationChangeEvent
{
    enum KzaApplicationChangeEventType type;
    union 
    {
        struct KzaApplicationChangeEventSceneChanged
        {
            struct KzuScene* oldScene;
            struct KzuScene* newScene;
        } sceneChanged;
    } data;
};


/** Function pointer to function callback used for delivering events to application. The events available can be found from enum KzaApplicationChangeEventType. */ 
typedef kzsError (*KzaApplicationChangeHandler)(struct KzaApplication* application, struct KzaApplicationChangeEvent* changeEvent);
/** Function pointer to the function that handles the window creation. */ 
typedef void (*KzaApplicationWindowCreatorFunction)(struct KzaApplication* application, struct KzsWindowProperties* windowProperties, struct KzsDisplay* display, struct KzsWindow** out_window);
/** Function pointer to the function which calls tool full reload. */
typedef void (*KzaApplicationToolReloadHandler)(void);
/** Function pointer to the function that handles applying a scene. This is called by tool. */
typedef kzsError (*KzaApplicationApplySceneHandler)(struct KzaApplication* application, kzString scenePath);
/** Function pointer to the function that handles patching the project. This is called by tool. */
typedef kzsError (*KzaApplicationPatchProjectHandler)(struct KzaApplication* application, kzUint numBytes, const kzByte* bytes, kzBool* out_patchSuccess);
/** Function pointer to the function that handles reloading the project. This is called by tool. */
typedef kzsError (*KzaApplicationReloadProjectHandler)(struct KzaApplication* application, kzUint numBytes, const kzByte* bytes);

/** Callback function type for pointing device input handling. */
typedef kzsError (*KzaApplicationPointingDeviceHandler)(struct KzaApplication* application, const struct KzsPointingDeviceInputData* inputData);
/** Callback function type for key input device handling. */
typedef kzsError (*KzaApplicationKeyDeviceHandler)(struct KzaApplication* application, const struct KzsKeyDeviceInputData* inputData);

typedef kzsError (*KzaApplicationWindowEventHandler)(struct KzaApplication* application, enum KzsWindowEventType eventType,
                                                     struct KzsWindow* window);

/** Initialization. */
kzsError kzaApplicationInitialize(struct KzaApplication* application);
/** Update. */
kzsError kzaApplicationUpdate(struct KzaApplication* application, kzUint deltaTime);
/** Uninitialize. */
kzsError kzaApplicationUninitialize(struct KzaApplication* application);
/** Render. */
kzsError kzaApplicationUpdateWindows(struct KzaApplication* application);

/** Pointing device handler. */
kzsError kzaApplicationInputEventHandler(const struct KzsInputEventQueue* queue, void* userData);
/** Keyboard device handler. */
kzsError kzaApplicationWindowEventHandler(struct KzsWindow* window, enum KzsWindowEventType type, void* userData);

/** Gets the application level pointing device handler function. */
KzaApplicationPointingDeviceHandler kzaApplicationGetPointingDeviceHandler(const struct KzaApplication* application);
/** Sets the application level pointing device handler function. */
void kzaApplicationSetPointingDeviceHandler(struct KzaApplication* application, KzaApplicationPointingDeviceHandler handler);

/** Gets the application level key device handler function. */
KzaApplicationKeyDeviceHandler kzaApplicationGetKeyDeviceHandler(const struct KzaApplication* application);
/** Sets the application level key device handler function. */
void kzaApplicationSetKeyDeviceHandler(struct KzaApplication* application, KzaApplicationKeyDeviceHandler handler);

/** Gets the application level window event handler function. */
KzaApplicationWindowEventHandler kzaApplicationGetWindowEventHandler(const struct KzaApplication* application);
/** Sets the application level window event handler function. */
void kzaApplicationSetWindowEventHandler(struct KzaApplication* application, KzaApplicationWindowEventHandler handler);


/** Creates the application framework and allocates internal memory managers. */
kzsError kzaApplicationCreate(const struct KzaSystemProperties* systemProperties, struct KzaApplication** out_application);
/** Creates the application framework and allocates internal memory managers. */
kzsError kzaApplicationCreateAdvanced(const struct KzaSystemProperties* systemProperties, KzaApplicationWindowCreatorFunction windowCreatorFunction,
                                      kzBool useSystemAsApplicationMemoryManager, struct KzaApplication** out_application);
/** Deletes the application framework. */
kzsError kzaApplicationDelete(struct KzaApplication* application);


/** Pauses the application. No rendering or update is occurred when paused. */
kzsError kzaApplicationPause(struct KzaApplication* application);
/** Resumes to the application. */
kzsError kzaApplicationResume(struct KzaApplication* application);


/** Creates the platform specific portion of the framework. */
kzsError kzaApplicationPlatformCreate(struct KzaApplication* application, struct KzaPlatform** out_platform);
/** Deletes the platform specific portion of the framework. */
kzsError kzaApplicationPlatformDelete(struct KzaApplication* application, struct KzaPlatform* platform);


/** Gets the window creator function for the application */
KzaApplicationWindowCreatorFunction kzaApplicationGetWindowCreator(const struct KzaApplication* application);

/** Gets the current state of the application. */
enum KzaApplicationState kzaApplicationGetState(const struct KzaApplication* application);
/** Sets the current state of the application. */
void kzaApplicationSetState(struct KzaApplication* application, enum KzaApplicationState state);

/* Request application framework to shut down the application. Does not quit immediately. */
void kzaApplicationQuit(struct KzaApplication* application);

/** Gets the system specific application data. */
struct KzaPlatform* kzaApplicationGetPlatform(const struct KzaApplication* application);

/** Gets the application windows. The items are of type <struct KzsWindow*> */
struct KzcDynamicArray* kzaApplicationGetWindowArray_private(const struct KzaApplication* application);
/** Gets the application displays. The items are of type <struct KzsDisplay*> */
struct KzcDynamicArray* kzaApplicationGetDisplayArray(const struct KzaApplication* application);
/** Gets the application surfaces as dynamic array. The items are of type <struct KzsSurface*> */
struct KzcDynamicArray* kzaApplicationGetSurfaceArray(const struct KzaApplication* application);

/** Gets the user data associated with the application. */
void* kzaApplicationGetUserData(const struct KzaApplication* application);
/** Sets the user data associated with the application. Used for application specific structures for example. */
void kzaApplicationSetUserData(struct KzaApplication* application, void* userData);

/** Gets the application system memory manager of the application. */
struct KzcMemoryManager* kzaApplicationGetSystemMemoryManager(const struct KzaApplication* application);
/** Gets the application's memory manager of the application. */
struct KzcMemoryManager* kzaApplicationGetApplicationMemoryManager(const struct KzaApplication* application);

/** Sets the application's window creator function. */
void kzaApplicationSetWindowCreator(struct KzaApplication* application, KzaApplicationWindowCreatorFunction windowCreatorFunction);


/** Set tool reload callback and other tool information to application. This must not be called from application code. */
kzsError kzaApplicationSetToolData(struct KzaApplication* application, KzaApplicationToolReloadHandler toolReloadFunction);
/** Returns true if running application is integrated to Kanzi Tool. */
kzBool kzaToolIsPresent(const struct KzaApplication* application);
/** Requests a full reload from Kanzi Tool. */
void kzaToolRequestReload(const struct KzaApplication* application);

/** Sets a change handler to application. */
void kzaApplicationSetChangeHandler(struct KzaApplication* application, KzaApplicationChangeHandler changeHandler);

/** Sets a change handler to patch project. */
void kzaApplicationSetPatchProjectHandler(struct KzaApplication* application, KzaApplicationPatchProjectHandler handler);
/** Sets a change handler to reload project. */
void kzaApplicationSetReloadProjectHandler(struct KzaApplication* application, KzaApplicationReloadProjectHandler handler);

/** Sets a callback for applying scene. This replaces the default functionality of engine apply scene. */
void kzaApplicationSetApplySceneHandler(struct KzaApplication* application, KzaApplicationApplySceneHandler handler);

/** Changes scene of the application and applies free camera. */
kzsError kzaApplicationSetScene(struct KzaApplication* application, struct KzuScene* scene);
/** Gets the current scene from application. */
struct KzuScene* kzaApplicationGetScene(const struct KzaApplication* application);
/** Gets free camera from application. */
struct KzuFreeCamera* kzaApplicationGetFreeCamera(const struct KzaApplication* application);
/** Gets the actual view camera of the current scene, which was used as the base for free camera. */
struct KzuCameraNode* kzaApplicationGetViewCamera(const struct KzaApplication* application);
/** Gets the project patcher from application. */
struct KzuProjectPatcher* kzaApplicationGetProjectPatcher(const struct KzaApplication* application);
/** Gets the engine from application. */
struct KzuEngine* kzaApplicationGetEngine(const struct KzaApplication* application);
/** Gets the transformed scene from application. */
struct KzuTransformedScene* kzaApplicationGetTransformedScene(const struct KzaApplication* application);
/** Sets the active project camera. */
void kzaApplicationSetCamera(const struct KzaApplication* application, struct KzuCameraNode* cameraNode);
/** Gets the window from application. */
struct KzsWindow* kzaApplicationGetWindow(const struct KzaApplication* application);
/** Gets the binary directory from application. */
struct KzuBinaryDirectory* kzaApplicationGetBinaryDirectory(const struct KzaApplication* application);
/** Gets the project from application. */
struct KzuProject* kzaApplicationGetProject(const struct KzaApplication* application);

/** Loads binary from given file. */
kzsError kzaApplicationLoadBinary(struct KzaApplication* application, kzString binaryPath);

/** Loads binary from given config file. */
kzsError kzaApplicationLoadBinaryFromConfig(struct KzaApplication* application, kzString configPath);

/** Sets the time to the current scene's animation player. */
void kzaApplicationSetAnimationPlayerTime(struct KzaApplication* application, kzInt millisecondsFromStart);


/** Project reloading callback. Called from tool module. Reloads the current project with the given byte array. */
kzsError kzaApplicationReloadProject(struct KzaApplication* application, kzUint byteCount, const kzByte* bytes);

/** Project patching callback. Called from tool module. Patches the current project with the given byte array. */
kzsError kzaApplicationPatchProject(struct KzaApplication* application, kzUint byteCount, const kzByte* bytes, kzBool* out_patchSuccess);

/** Sets the active project scene. */
kzsError kzaApplicationSetScenePath(struct KzaApplication* application, kzString scenePath);


/** Enables or disables debug objects. */
void kzaApplicationSetDebugObjectsEnabled(struct KzaApplication* application, kzBool enabled);
/** Checks if debug objects are enabled. */
kzBool kzaApplicationIsDebugObjectsEnabled(const struct KzaApplication* application);

/** Sets grid enabled or not. */
void kzaApplicationSetGridEnabled(struct KzaApplication* application, kzBool enabled);
/** Checks if grid is enabled or not. */
kzBool kzaApplicationIsGridEnabled(const struct KzaApplication* application);

/** Sets fps info enabled or not. */
void kzaApplicationSetFpsInfoEnabled(struct KzaApplication* application, kzBool enabled);
/** Checks if fps info is enabled or not. */
kzBool kzaApplicationIsFpsInfoEnabled(const struct KzaApplication* application);

/** Sets log visualization enabled or not. */
void kzaApplicationSetLogVisualizationEnabled(struct KzaApplication* application, kzBool enabled);
/** Checks if log visualization is enabled or not. */
kzBool kzaApplicationIsLogVisualizationEnabled(const struct KzaApplication* application);

/** Enables picking. */
kzsError kzaApplicationEnablePicking(struct KzaApplication* application);
/** Picking callback prototype. */
typedef kzsError (*KzaPickCallback)(kzString objectPath, void* userData);
/** Sets a callback for picking. Pass KZ_NULL to disable. */
void kzaApplicationSetPickCallback(struct KzaApplication* application, KzaPickCallback callback, void* userData);
/** Sets pick mode. */
void kzaApplicationSetPickMode(struct KzaApplication* application, enum KzaToolPickMode mode);
/** Pick an object. Picking must be enabled. */
kzsError kzaApplicationPick(const struct KzaApplication* application, kzUint x, kzUint y,  struct KzuObjectNode** out_node, kzUint* out_cluster);

/** Enables transform gizmo. */
kzsError kzaApplicationEnableTransformGizmo(struct KzaApplication* application);
/** Transform gizmo callback prototype. */
typedef kzsError (*KzaTransformGizmoCallback)(kzString objectPath, void* userData);
/** Sets a callback for transform gizmo. Pass KZ_NULL to disable. */
void kzaApplicationSetTransformGizmoCallback(const struct KzaApplication* application, KzaTransformGizmoCallback callback, void* userData);
/** Sets transform gizmo target. */
kzsError kzaApplicationSetTransformGizmoTarget(const struct KzaApplication* application, kzString objectPath);
/** Sets transform gizmo mode. */
kzsError kzaApplicationSetTransformGizmoMode(const struct KzaApplication* application, enum KzuTransformGizmoMode mode);
/** Tell whether or not grid snap is enabled. */
kzBool kzaApplicationIsTransformGizmoGridSnapEnabled(const struct KzaApplication* application);
/** Sets the grid snap on or off in the transform gizmo.
*
* If transform gizmo is not available, do nothing.
*/
void kzaApplicationSetTransformGizmoGridSnapEnabled(const struct KzaApplication* application, kzBool enabled);
/** Get the size of the grid in the transform gizmo.
*
* If transform gizmo is not available, this will return 0.0f (which cannot be returned if it is). */
kzFloat kzaApplicationGetTransformGizmoGridSize(const struct KzaApplication* application);
/** Set the grid size for the transform gizmo.
*
* If transform gizmo is not available, do nothing.
*/
void kzaApplicationSetTransformGizmoGridSize(const struct KzaApplication* application, kzFloat size);
/** Get the current grid snap mode.
*
* Returns the mode NONE if no transform gizmo exists. */
enum KzuTransformGizmoGridSnapMode kzaApplicationGetTransformGizmoGridSnapMode(const struct KzaApplication* application);
/** Set the grid snap mode.
*
* If no transform gizmo exists, do nothing. */
void kzaApplicationSetTransformGizmoGridSnapMode(const struct KzaApplication* application,
                                                 enum KzuTransformGizmoGridSnapMode mode);

/** Sets bounding box visualization enabled or disabled. */
void kzaApplicationSetBoundingBoxVisualizationEnabled(struct KzaApplication* application, kzBool enabled);
/** Checks if bounding box visualization is enabled or not. */
kzBool kzaApplicationIsBoundingBoxVisualizationEnabled(const struct KzaApplication* application);

/** Sets allocated layout bounding box visualization enabled or disabled. */
void kzaApplicationSetAllocatedLayoutBoundingBoxVisualizationEnabled(struct KzaApplication* application, kzBool enabled);
/** Checks if allocated layout bounding box visualization is enabled or not. */
kzBool kzaApplicationIsAllocatedLayouBoundingBoxVisualizationEnabled(const struct KzaApplication* application);
/** Sets actual layout bounding box visualization enabled or disabled. */
void kzaApplicationSetActualLayoutBoundingBoxVisualizationEnabled(struct KzaApplication* application, kzBool enabled);
/** Checks if actual layout bounding box visualization is enabled or not. */
kzBool kzaApplicationIsActualLayouBoundingBoxVisualizationEnabled(const struct KzaApplication* application);
/** Checks whether or not actual layout visualization is recursive. */
kzBool kzaApplicationIsActualLayoutVisualizationReacursive(const struct KzaApplication* application);
/** Sets the recursive state of actual layout visualization. */
void kzaApplicationSetActualLayoutVisualizationRecursive(struct KzaApplication* application, kzBool enabled);

/** Enables or disables free camera. */
kzsError kzaApplicationSetFreeCameraEnabled(struct KzaApplication* application, kzBool enabled);
/** Checks if free camera is enabled or disabled. */
kzBool kzaApplicationIsFreeCameraEnabled(const struct KzaApplication* application);
/** Sets a callback for free cameera. Pass KZ_NULL to disable. */
void kzaApplicationSetFreeCameraCallback(const struct KzaApplication* application, KzuFreeCameraUpdateStateFunction updateStateFunction, void* userData);

/** Enables or disables default key event handling. */
void kzaApplicationSetDefaultKeyHandlingEnabled(struct KzaApplication* application, kzBool enabled);

/** Prints renderer info. */
kzsError kzaApplicationPrintRendererInfo(const struct KzaApplication* application);


/** Sets user interface enabled or disabled. */
void kzaApplicationSetUserInterfaceEnabled(struct KzaApplication* application, kzBool enabled);
/** Checks if user interface is enabled or disabled. */
kzBool kzaApplicationGetUserInterfaceEnabled(const struct KzaApplication* application);


/** Gets the transition from the application. */
struct KzaTransition* kzaApplicationGetTransition(const struct KzaApplication* application);

/** Gets the Lua state from the application. */
struct KzuLua* kzaApplicationGetLua(const struct KzaApplication* application);

/** Compile all script files application's LUA state. */
kzsError kzaApplicationCompileLuaScripts(const struct KzaApplication* application);
/** Compile single script file to application's LUA state. */
kzsError kzaApplicationCompileLuaScript(const struct KzaApplication* application, const struct KzuScript* script);

/** Gets the default font from application. */
struct KzcFont* kzaApplicationGetDefaultFont(const struct KzaApplication* application);

/** Gets system properties from application. */
struct KzaSystemProperties* kzaApplicationGetSystemProperties(const struct KzaApplication* application);


#endif
