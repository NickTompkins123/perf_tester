/**
 * \file
 * Application tool module.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "tool_module.hpp"


extern "C"
{
#include <application/kza_application_interface.h>
#include <application/kza_win32_application.h>
#include <application/kza_error_codes.h>

#include <user/engine/kzu_engine.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_transformed_scene.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_mesh.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/material/kzu_material.h>
#include <user/project/kzu_project.h>
#include <user/project/kzu_project_loader_object_node.h>
#include <user/binary/kzu_binary_directory.h>
#include <user/util/kzu_free_camera.h>
#include <user/util/kzu_raypick.h>

#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/string/kzc_string.h>
#include <core/util/math/kzc_ray.h>

#include <system/display/kzs_window_native.h>
#include <system/debug/kzs_error_win32.h>
#include <system/file/kzs_file_base.h>
#include <system/wrappers/kzs_string.h>
#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_opengl_base.h>
#include <system/debug/kzs_error.h>
}

#include <stdio.h>

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace Rightware::Kanzi::ToolInterface;


namespace Rightware {
namespace Kanzi {
namespace Applications {
namespace DefaultApplication {
namespace ToolModule {


void __cdecl CreateWindowForApplicationFramework(KzaApplication* application, KzsWindowProperties* windowProperties, struct KzsDisplay* display, KzsWindow** out_window);
kzsError __cdecl ReceiveEngineLog(KzsLogLevel level, kzsError errorCode, kzString message, kzString file, kzUint line, void* userData);
void __cdecl ToolRequestReload();
kzsError __cdecl PickCallback(kzString objectPath, void* userData);
kzsError __cdecl TransformGizmoCallback(kzString objectPath, void* userData);
kzsError __cdecl FreeCameraUpdateStateCallback(struct KzcMatrix4x4* transformation, void* userData);



KzaApplication* globalApplication;


ref class Lock 
{
private:
   Object^ object;
public:
   Lock(Object ^ parameterObject) : object(parameterObject) 
   {
      Monitor::Enter(object);
   }
   ~Lock() 
   {
      Monitor::Exit(object);
   }
};


ToolModule::ToolModule()
{
    Debug::WriteLine("ApplicationToolModule::ApplicationToolModule()");
    currentState = KZA_APPLICATION_STATE_UNINITIALIZED;
#ifdef _DEBUG
    /* This must be set to false on commit. Otherwise tool hangs when e.g. log is written. */
    kzsErrorSetBreakEnabled(KZ_FALSE);
#else
    /* Debug breaking is never needed in release build. */
    kzsErrorSetBreakEnabled(KZ_FALSE);
#endif
    onlyThread = Thread::CurrentThread;
}

ToolModule::~ToolModule()
{
    Debug::WriteLine("ApplicationToolModule::~ApplicationToolModule()");
}

unsigned int ToolModule::GetKZBVersion()
{
    return KZU_BINARY_FORMAT_VERSION;
}

/** Gets an application window by its handle. */
KzsWindow* ToolModule::GetWindow(HWND hWnd)
{
    AssertThread();
    KzsWindow* foundWindow = KZ_NULL;
    KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(kzaApplicationGetWindowArray_private(application));

    while (kzcDynamicArrayIterate(it))
    {
        KzsWindow* window = (KzsWindow*)kzcDynamicArrayIteratorGetValue(it);
        KzsWindowNative* nativeWindow = kzsWindowGetNative(window);
        HWND iteratedWindowHandle = kzsWindowNativeGetHandle(nativeWindow);

        if (iteratedWindowHandle == hWnd)
        {
            foundWindow = window;
            break;
        }
    }

    return foundWindow;
}

IntPtr ToolModule::OnCreateApplicationWindow(ApplicationCreateWindowEventArgs^ e)
{
    return CreateApplicationWindow(this, e);
}

System::Object^ ToolModule::InitializeLifetimeService()
{
    // This is to insure that when created, the instance never dies, no matter how long between client calls.
    return nullptr;
} 

void ToolModule::Initialize(String^ binaryName, IntPtr hInstance, IntPtr hwndParent, String^ resourceDirectory)
{
    AssertThread();
    theInstance = this;
    this->hwndParent = hwndParent;
    this->hInstance = hInstance;

    // Set resource directory.
    {
        kzMutableString resourcePath;
        pin_ptr<unsigned char> resourceDirectoryUTF8 = &Text::Encoding::UTF8->GetBytes(resourceDirectory)[0];

        // Copy the string without memory manager.
        {
            kzUint length;
            kzString source = (kzString)resourceDirectoryUTF8;
            kzsAssert(source != KZ_NULL);

            length = kzcStringLength(source);
            resourcePath = (kzMutableString)kzsMalloc(length + 1);

            kzsStrcpy(resourcePath, source);
        }
        kzsFileBaseSetResourceDirectory(resourcePath);
        this->resourcePath = resourcePath;
    }

    // Initialize.
    {
        /* Initialize application framework. Allocates memory for application struct. */
        struct KzaSystemProperties systemProperties;
        KzaApplication* application;

        systemProperties.programArgumentCount = 0;
        systemProperties.programArguments = KZ_NULL;

        WrapError(kzaApplicationCreateAdvanced(&systemProperties, CreateWindowForApplicationFramework, KZ_TRUE, &application));
        WrapError(kzsLogRegisterHandler(ReceiveEngineLog, KZ_NULL));
        WrapError(kzaApplicationSetToolData(application, ToolRequestReload));
        this->application = application;
        globalApplication = application;

        WrapError(kzaApplicationInitialize(application));
        kzaApplicationSetTransformGizmoCallback(application, TransformGizmoCallback, application);
        SetState(KZA_APPLICATION_STATE_RUNNING, true);
        kzaApplicationSetFreeCameraCallback(application, FreeCameraUpdateStateCallback, application);

        activeScenePath = nullptr;
    }
}

void ToolModule::Uninitialize()
{
    AssertThread();
    SetState(KZA_APPLICATION_STATE_QUITTING, true);

    kzsFree(resourcePath);

    WrapError(kzApplicationUninitialize(application));

    WrapError(kzaApplicationDelete(application));
}

void ToolModule::OnStateChanged(ApplicationStateChangeEventArgs^ e)
{
    StateChanged(this, e);
}

void ToolModule::SetState(KzaApplicationState newState, bool updateApplicationState)
{
    AssertThread();
    if (updateApplicationState)
    {
        kzaApplicationSetState(application, newState);
    }

    ApplicationStateChangeEventArgs^ e = gcnew ApplicationStateChangeEventArgs();
    e->OldState = (ApplicationState)currentState;
    e->NewState = (ApplicationState)newState;
    currentState = newState;
    OnStateChanged(e);
}

void ToolModule::OnSelectionChanged(SelectionChangedEventArgs^ e)
{
    SelectionChanged(this, e);
}

void ToolModule::OnNodeTransformationChanged(NodeTransformationChangedEventArgs^ e)
{
    NodeTransformationChanged(this, e);
}

IEnumerable<String^>^ ToolModule::GetSelection()
{
    return gcnew array<String^>(0);
}

void ToolModule::SetSelection(IEnumerable<String^>^ selectedNodePaths)
{
    AssertThread();
    pin_ptr<unsigned char> objectPath = nullptr;
    IEnumerator<String^>^ enumerator = selectedNodePaths->GetEnumerator();
    enumerator->MoveNext();
    String^ string = enumerator->Current;

    if (string)
    {
        objectPath = &Text::Encoding::UTF8->GetBytes(string)[0];
    }

    kzaApplicationSetTransformGizmoTarget(application, (kzString)objectPath);
}

void ToolModule::SetTool(PreviewTool tool)
{
    if (tool == PreviewTool::DISABLED)
    {
        kzaApplicationSetPickCallback(application, KZ_NULL, application);
        kzaApplicationSetTransformGizmoMode(application, KZU_TRANSFORM_GIZMO_MODE_NONE);
    }
    else
    {
        // Picking has no transform gizmo mode.
        KzuTransformGizmoMode mode = KZU_TRANSFORM_GIZMO_MODE_NONE;

        switch (tool)
        {
            case PreviewTool::TRANSLATION:
                mode = KZU_TRANSFORM_GIZMO_MODE_TRANSLATE;
                break;

            case PreviewTool::ROTATION:
                mode = KZU_TRANSFORM_GIZMO_MODE_ROTATE;
                break;

            case PreviewTool::SCALING:
                mode = KZU_TRANSFORM_GIZMO_MODE_SCALE;
                break;
        }

        kzaApplicationSetPickCallback(application, PickCallback, application);
        kzaApplicationSetTransformGizmoMode(application, mode);
    }
}

void ToolModule::SetSelectionMode(SelectionMode selectionMode)
{
    switch(selectionMode)
    {
        case SelectionMode::MATERIAL:
            kzaApplicationSetPickMode(application, KZA_TOOL_PICK_MODE_MATERIAL);
            break;
        default:
            kzaApplicationSetPickMode(application, KZA_TOOL_PICK_MODE_NODE);
            break;
    }
}

void ToolModule::SetTime(Int64 time)
{
}

void ToolModule::SetTimeMultiplier(float multiplier)
{
}

ToolModule^ ToolModule::GetToolModule(KzaApplication* application)
{
    return theInstance;
}

void ToolModule::Reset()
{
    AssertThread();
}

void ToolModule::Render()
{
    AssertThread();
    WrapError(kzaApplicationUpdateWindows(application));
}

void ToolModule::Update(int deltaTimeMs)
{
    struct KzsWindow* window;
    struct KzsInputEventQueue* inputEventQueue;
    kzsInputEventHandleFunction handler;
    void* userData;
    int clampedDeltaTime;

    AssertThread();

    /* Clamping fixes issue with negative deltas coming from tool. */
    clampedDeltaTime = (deltaTimeMs < 0) ? 0 : deltaTimeMs;

    window = kzaApplicationGetWindow(application);
    handler = kzsWindowGetInputEventHandler(window);
    inputEventQueue = kzsWindowGetInputEventQueue(window);
    userData = kzsWindowGetUserData(window);

    WrapError(handler(inputEventQueue, userData));

    kzsInputEventQueueClear(inputEventQueue);

    WrapError(kzaApplicationUpdate(application, clampedDeltaTime));

    KzaApplicationState newState = kzaApplicationGetState(application);

    if (newState != currentState)
    {
        currentState = newState;    
        SetState(newState, false);
    }
}

void ToolModule::SetPaused(bool paused)
{
    AssertThread();
}

void ToolModule::WindowSetWidth(IntPtr windowHandle, unsigned int width)
{
    AssertThread();
    KzsWindow* window = GetWindow((HWND)(windowHandle.ToPointer()));
    
    if (window == KZ_NULL)
    {
        throw gcnew ArgumentException("No window found with the given handle.");
    }

    kzsWindowSetWidth_private(window, width);
}

void ToolModule::WindowSetHeight(IntPtr windowHandle, unsigned int height)
{
    AssertThread();
    KzsWindow* window = GetWindow((HWND)(windowHandle.ToPointer()));
    
    if (window == KZ_NULL)
    {
        throw gcnew ArgumentException("No window found with the given handle.");
    }

    kzsWindowSetHeight_private(window, height);
}

bool ToolModule::ApplyBinaryPatch(array<byte>^ binaryData)
{
    AssertThread();

    pin_ptr<unsigned char> nativeBytes = &binaryData[0];
    kzBool patchSuccess;
    WrapError(kzaApplicationPatchProject(application, binaryData->Length, nativeBytes, &patchSuccess));

    return (patchSuccess == KZ_TRUE);
}

void ToolModule::ApplyBinary(String^ clientId, array<byte>^ binaryData)
{
    AssertThread();

    pin_ptr<unsigned char> nativeBytes = &binaryData[0];
    WrapError(kzaApplicationReloadProject(application, binaryData->Length, nativeBytes));

    ApplyScene();
}

void ToolModule::SetActiveScene(String^ scenePath)
{
    AssertThread();
    activeScenePath = scenePath;
    ApplyScene();
}

void ToolModule::ApplyScene()
{
    AssertThread();
    pin_ptr<unsigned char> scenePathUTF8 = nullptr;

    if (activeScenePath != nullptr)
    {
        scenePathUTF8 = &Text::Encoding::UTF8->GetBytes(activeScenePath)[0];
    }

    if (scenePathUTF8 != nullptr)
    {
        WrapError(kzaApplicationSetScenePath(application, (kzString)scenePathUTF8));
    }
}

IEnumerable<String^>^ ToolModule::GetJumpPoints()
{
    AssertThread();
    return gcnew array<String^>(0);
}

void ToolModule::JumpToJumpPoint(String^ jumpPointName)
{
    AssertThread();
}

IntPtr ToolModule::WndProcEventHandler(IntPtr hwnd, unsigned int message, IntPtr wParam, IntPtr lParam, bool% out_handled)
{
    AssertThread();

    kzBool handled;
    WrapError(kzsWindowNativeWndProcHandler((HWND)hwnd.ToPointer(), message, (WPARAM)wParam.ToPointer(),
                                            (LPARAM)lParam.ToPointer(), &handled));

    out_handled = handled ? true : false;
    return IntPtr(0);
}

ToolInterface::OpenGlVersion ToolModule::GetOpenGlVersion()
{
    AssertThread();
#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    return ToolInterface::OpenGlVersion::ES_11;
#elif KZ_OPENGL_VERSION == KZ_OPENGL_ES_2_0
    return ToolInterface::OpenGlVersion::ES_20;
#else
    throw gcnew EngineException(KZS_ERROR_ILLEGAL_OPERATION);
#endif
}

void ToolModule::OnEngineLogMessageReceived(KzsLogLevel level, kzsError errorCode, kzString message, kzString file, kzUint line)
{
    LogMessageReceivedEventArgs^ e = gcnew LogMessageReceivedEventArgs();
    e->Level = (int)level;
    e->ErrorCode = (int)errorCode;
    e->Message = gcnew String(message);
    e->File = gcnew String(file);
    e->Line = line;
    OnEngineLogMessageReceived(e);
}

void ToolModule::OnEngineLogMessageReceived(LogMessageReceivedEventArgs^ e)
{
    LogMessageReceived(this, e);
}

void ToolModule::OnFullBinaryRequested()
{
    ClientEventArgs^ e = gcnew ClientEventArgs();
    FullBinaryRequested(this, e);
}

void ToolModule::SetCameraTransformation()
{
    struct KzuScene* scene = kzaApplicationGetScene(this->application);
    struct KzuFreeCamera* freeCamera = kzaApplicationGetFreeCamera(this->application);
    struct KzuCameraNode* viewCamera = kzaApplicationGetViewCamera(this->application);

    if (viewCamera != KZ_NULL)
    {
        struct KzuObjectNode* sceneRoot = kzuSceneGetRootNode(scene);
        struct KzcMatrix4x4 freeCameraWorldTransformation;
        kzuFreeCameraInitializeMatrix(freeCamera, &freeCameraWorldTransformation);

        struct KzuObjectNode* viewCameraObjectNode = kzuCameraNodeToObjectNode(viewCamera);
        struct KzuObjectNode* viewCameraParentObjectNode = kzuObjectNodeGetParent(viewCameraObjectNode);

        kzsAssert(viewCameraParentObjectNode != KZ_NULL);

        struct KzcMatrix4x4 parentWorldTransformation;
        kzBool parentFound = kzuObjectNodeCalculateRelativeTransformation(sceneRoot, viewCameraParentObjectNode, &parentWorldTransformation);

        if (!parentFound)
        {
            throw gcnew EngineException(KZS_ERROR_ILLEGAL_OPERATION);
        }

        struct KzcMatrix4x4 viewParentInverse;
        struct KzcMatrix4x4 resultTransformation;
        kzcMatrix4x4Inverse(&parentWorldTransformation, &viewParentInverse);
        kzcMatrix4x4Multiply(&freeCameraWorldTransformation, &viewParentInverse, &resultTransformation);

        kzuObjectNodeSetTransformation(viewCameraObjectNode, &resultTransformation);
        
        WrapError(FreeCameraUpdateStateCallback(&resultTransformation, application));
    }
}

String^ ToolModule::PickItem(int mouseX, int mouseY, ToolInterface::SelectionMode itemType)
{
    String^ name = nullptr;
    KzuObjectNode* node;
    kzUint cluster;

    kzaApplicationPick(application, kzUint(mouseX), kzUint(mouseY), &node, &cluster);

    if (node != KZ_NULL)
    {
        struct KzuProject* project = kzaApplicationGetProject(application);
        if (itemType == ToolInterface::SelectionMode::MESH_CLUSTER)
        {
            kzString objectPath = kzuProjectGetObjectPath(project, node);
            name = String::Format("{0}/Cluster{1}", gcnew String(objectPath, 0, kzcStringLength(objectPath), Encoding::UTF8), cluster);
        }
        else if (itemType == ToolInterface::SelectionMode::NODES)
        {
            kzString objectPath = kzuProjectGetObjectPath(project, node);
            name = String::Format("{0}", gcnew String(objectPath, 0, kzcStringLength(objectPath), Encoding::UTF8));
        }
        else if (itemType == ToolInterface::SelectionMode::MATERIAL)
        {
            KzuMeshNode* meshNode = kzuMeshNodeFromObjectNode(node);
            KzuMesh* mesh = kzuMeshNodeGetMesh(meshNode);
            KzuMeshCluster* meshCluster = kzuMeshGetClusterAtIndex(mesh, cluster);
            KzuMaterial* material = kzuMeshClusterGetMaterial(meshCluster);
            kzString materialPath = kzuProjectGetObjectPath(project, material);
            name = gcnew String(materialPath, 0, kzcStringLength(materialPath), Encoding::UTF8);
        }
    }

    return name;
}

array<float>^ ToolModule::GetPickVector(int mouseX, int mouseY, String^ renderPassPath)
{
    KzcRay ray;
    array<float>^ result = gcnew array<float>(6);
    KzuEngine* engine = kzaApplicationGetEngine(application);
    KzuTransformedScene* transformedScene = kzaApplicationGetTransformedScene(application);
    struct KzcDynamicArray* objectList = kzuTransformedSceneGetObjects(transformedScene);
    struct KzuTransformedObjectNode* transformedCamera = kzuTransformedObjectListFindObject(objectList, kzuCameraNodeToObjectNode(kzuTransformedSceneGetViewCamera(transformedScene)));
    kzuRayPickCalculateRay(mouseX, mouseY, kzuEngineGetRenderer(engine), kzuEngineGetActiveWindow(engine), transformedCamera, &ray);

    result[0] = ray.origin.data[0];
    result[1] = ray.origin.data[1];
    result[2] = ray.origin.data[2];

    result[3] = ray.direction.data[0];
    result[4] = ray.direction.data[1];
    result[5] = ray.direction.data[2];

    return result;
}

void ToolModule::SubscribeToPropertyValueChanges(String^ propertyPath)
{
    // TODO: implement
}

void ToolModule::UnsubscribeFromPropertyValueChanges(String^ propertyPath)
{
    // TODO: implement
}

System::Object^ ToolModule::GetPropertyValue(String^ propertyPath)
{
    // TODO: implement
    return nullptr;
}

void ToolModule::SetPropertyValue(String^ propertyPath, System::Object^ value)
{
    // TODO: implement
}

void ToolModule::OnPropertyValueChanged(String^ propertyPath, Object^ newValue, Object^ oldValue)
{
    PropertyValueChangedEventArgs^ e = gcnew PropertyValueChangedEventArgs(propertyPath, newValue, oldValue);
    PropertyValueChanged(this, e);
}

void ToolModule::RequestPlaybackCommand(ToolInterface::PlaybackCommand command)
{
    ControlPlaybackEventArgs^ e = gcnew ControlPlaybackEventArgs(command);
    PlaybackCommandRequested(this, e);
}

void ToolModule::AssertThread()
{
    if (onlyThread != Thread::CurrentThread)
    {
        throw gcnew EngineException(KZA_ERROR_INVALID_THREAD);
    }
}

void __cdecl CreateWindowForApplicationFramework(KzaApplication* application, KzsWindowProperties* windowProperties, struct KzsDisplay* display, KzsWindow** out_window)
{
    ToolModule^ toolModule = ToolModule::GetToolModule(application);

    ApplicationCreateWindowEventArgs^ eventArgs = gcnew ApplicationCreateWindowEventArgs();
    eventArgs->Width = windowProperties->width;
    eventArgs->Height = windowProperties->height;
    eventArgs->WindowHandle = IntPtr::Zero;
    windowProperties->style = KZS_WINDOW_STYLE_STUDIO_PREVIEW;

    IntPtr windowHandle = toolModule->OnCreateApplicationWindow(eventArgs);

    KzsWindowNative* windowNative;
    kzsWindowNativeCreateEmpty(&windowNative);
    kzsWindowNativeSetHandle(windowNative, (HWND)windowHandle.ToPointer());

    KzsWindow* window;
    kzsWindowCreateFromNative(windowNative, windowProperties, &window);

    Debug::WriteLine("Initialize::CreateWindow " + windowHandle.ToString());

    *out_window = window;
}

void __cdecl ToolRequestReload()
{
    ToolModule^ toolModule = ToolModule::GetToolModule(nullptr);
    toolModule->OnFullBinaryRequested();
}

kzsError __cdecl ReceiveEngineLog(KzsLogLevel level, kzsError errorCode, kzString message, kzString file, kzUint line, void* userData)
{
    ToolModule^ toolModule = ToolModule::GetToolModule(nullptr);
    toolModule->OnEngineLogMessageReceived(level, errorCode, message, file, line);
    return KZS_SUCCESS;
}

void WrapError(kzsError error)
{
    if (error != KZS_SUCCESS)
    {
        kzString errorString = kzsErrorGetLastErrorMessage();
        String^ managedErrorString = (errorString != KZ_NULL ? gcnew String(errorString, 0, kzcStringLength(errorString), Encoding::UTF8) : nullptr);
        throw gcnew EngineException(error, managedErrorString);
    }
}

kzsError __cdecl PickCallback(kzString objectPath, void* userData)
{
    KzaApplication* application = (KzaApplication*)userData;
    ToolModule^ toolModule = ToolModule::GetToolModule(application);
    SelectionChangedEventArgs^ args = gcnew SelectionChangedEventArgs;
    args->SelectedNodePaths = gcnew array<String^>(1);
    args->SelectedNodePaths[0] = gcnew String(objectPath, 0, kzcStringLength(objectPath), Encoding::UTF8);
    toolModule->OnSelectionChanged(args);  
    kzaApplicationSetTransformGizmoTarget(application, objectPath);
    kzsSuccess();
}

kzsError __cdecl FreeCameraUpdateStateCallback(struct KzcMatrix4x4* worldTransformation, void* userData)
{
    KzaApplication* application = (KzaApplication*)userData;
    ToolModule^ toolModule = ToolModule::GetToolModule(application);
    SelectionChangedEventArgs^ args = gcnew SelectionChangedEventArgs;
    NodeTransformationChangedEventArgs^ nodeTransformationArgs = gcnew NodeTransformationChangedEventArgs;
    kzString objectPath = kzuProjectGetObjectPath(kzaApplicationGetProject(application), kzaApplicationGetViewCamera(application));
    String^ objectPathString = gcnew String(objectPath, 0, kzcStringLength(objectPath), Encoding::UTF8);

    args->SelectedNodePaths = gcnew array<String^>(1);
    args->SelectedNodePaths[0] = objectPathString;
    toolModule->OnSelectionChanged(args);  
    
    nodeTransformationArgs->NodePath = objectPathString;
    nodeTransformationArgs->TransformationMatrix = gcnew array<Single>(16);
    for (size_t i = 0, end = 16; i != end; ++i)
    {
        nodeTransformationArgs->TransformationMatrix[i] = worldTransformation->data[i];
    }

    toolModule->OnNodeTransformationChanged(nodeTransformationArgs);

    kzsSuccess();
}

kzsError __cdecl TransformGizmoCallback(kzString objectPath, void* userData)
{
    KzaApplication* application = (KzaApplication*)userData;
    ToolModule^ toolModule = ToolModule::GetToolModule(application);

    KzuProject* project = kzaApplicationGetProject(application);
    KzuObjectNode* node = static_cast<KzuObjectNode*>(kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, objectPath));
    KzcMatrix4x4 transform = kzuObjectNodeGetTransformation(node);

    NodeTransformationChangedEventArgs^ args = gcnew NodeTransformationChangedEventArgs;
    args->NodePath = gcnew String(objectPath, 0, kzcStringLength(objectPath), Encoding::UTF8);
    args->TransformationMatrix = gcnew array<Single>(16);

    for (size_t i = 0, end = 16; i != end; ++i)
    {
        args->TransformationMatrix[i] = transform.data[i];
    }

    toolModule->OnNodeTransformationChanged(args);

    kzsSuccess();
}


}}}}}
