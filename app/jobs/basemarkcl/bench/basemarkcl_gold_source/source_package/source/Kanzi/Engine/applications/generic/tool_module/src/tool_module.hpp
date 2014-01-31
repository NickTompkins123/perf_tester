/**
 * \file
 * Generic application tool module.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#pragma once


extern "C"
{
#include <windows.h>

#include <application/kza_application.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>
#include <system/debug/kzs_log.h>
}


using namespace System;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace Rightware::Kanzi::ToolInterface;


namespace Rightware {
namespace Kanzi {
namespace Applications {
namespace DefaultApplication {
namespace ToolModule {


void WrapError(kzsError error);


public ref class ToolModule abstract : 
    public MarshalByRefObject, PreviewApplicationInterfaceV1, IDisposable
{
private:
    static ToolModule^ theInstance;
    IntPtr hInstance;
    IntPtr hwndParent;
 
    String^ activeScenePath;
    kzMutableString resourcePath;
    
    // All interface calls must be originated from same thread, this is used for asserting that
    Thread^ onlyThread; 

protected:
    KzaApplication* application;
    KzaApplicationState currentState;

public:
    ToolModule();
    ~ToolModule();

    /* ApplicationInterface implementation */
    virtual event ApplicationCreateWindowEventArgs::Handler^ CreateApplicationWindow;

    virtual event EventHandler<ApplicationStateChangeEventArgs^>^ StateChanged;

    virtual event EventHandler<LogMessageReceivedEventArgs^>^ LogMessageReceived;

    virtual event EventHandler<ClientEventArgs^>^ FullBinaryRequested;

    virtual event EventHandler<ToolInterface::ControlPlaybackEventArgs^>^ PlaybackCommandRequested;

    virtual event EventHandler<ToolInterface::PropertyValueChangedEventArgs^>^ PropertyValueChanged;

    virtual void Initialize(String^ binaryName, IntPtr hInstance, IntPtr hwndParent, String^ resourceDirectory);

    virtual void Uninitialize();

    virtual void OnStateChanged(ApplicationStateChangeEventArgs^ e);

    virtual void Reset();

    virtual void Render();

    virtual void Update(int deltaTimeMs);

    virtual void SetPaused(bool paused);

    virtual void WindowSetWidth(IntPtr windowHandle, unsigned int width);

    virtual void WindowSetHeight(IntPtr windowHandle, unsigned int height);

    virtual bool ApplyBinaryPatch(array<byte>^ binaryData);

    virtual void ApplyBinary(String^ clientId, array<byte>^ binaryData);

    virtual IEnumerable<PreviewOption^>^ GetPreviewOptions() abstract;

    virtual void SetPreviewOptionValue(String^ previewOptionName, Object^ previewOptionValue) abstract;

    virtual void SetActiveScene(String^ scenePath);

    virtual IEnumerable<String^>^ GetJumpPoints();

    virtual void JumpToJumpPoint(String^ jumpPointName);

    virtual IntPtr WndProcEventHandler(IntPtr hwnd, unsigned int message, IntPtr wParam, IntPtr lParam, bool% out_handled);

    virtual ToolInterface::OpenGlVersion GetOpenGlVersion();

    virtual System::Object^ InitializeLifetimeService() override;

    virtual unsigned int GetKZBVersion();

    /* End of ApplicationInterface */

    virtual IntPtr OnCreateApplicationWindow(ApplicationCreateWindowEventArgs^ e);

    void OnEngineLogMessageReceived(KzsLogLevel level, kzsError errorCode, kzString message, kzString file, kzUint line);

    static ToolModule^ GetToolModule(KzaApplication* application);


    virtual void OnFullBinaryRequested();

    virtual void SetCameraTransformation();
    
    virtual void SetTool(PreviewTool tool);

    virtual void SetSelectionMode(SelectionMode selectionMode);

    virtual void SetSelection(IEnumerable<String^>^ selectedNodePaths);

    virtual IEnumerable<String^>^ GetSelection();

    virtual event EventHandler<SelectionChangedEventArgs^>^ SelectionChanged;

    virtual event EventHandler<NodeTransformationChangedEventArgs^>^ NodeTransformationChanged;

    virtual void SetTime(Int64 time);

    virtual void SetTimeMultiplier(float multiplier);

    virtual void OnSelectionChanged(SelectionChangedEventArgs^ e);

    virtual void OnNodeTransformationChanged(NodeTransformationChangedEventArgs^ e);

    virtual String^ PickItem(int mouseX, int mouseY, ToolInterface::SelectionMode itemType);

    virtual array<float>^ GetPickVector(int mouseX, int mouseY, String^ renderPassPath);

    virtual System::Object^ GetPropertyValue(String^ propertyPath);

    virtual void SetPropertyValue(String^ propertyPath, System::Object^ value);

    virtual void SubscribeToPropertyValueChanges(String^ propertyPath);

    virtual void UnsubscribeFromPropertyValueChanges(String^ propertyPath);

    virtual void OnPropertyValueChanged(String^ propertyPath, Object^ newValue, Object^ oldValue);

    virtual void RequestPlaybackCommand(ToolInterface::PlaybackCommand command);

protected:
    virtual void SetState(KzaApplicationState newState, bool updateApplicationState);

    virtual void ApplyScene();

    virtual void OnEngineLogMessageReceived(LogMessageReceivedEventArgs^ e);


private:
    /** Gets an application window by its handle. */
    KzsWindow* GetWindow(HWND hWnd);

    /** Asserts that the function call is called from the original thread. */
    void AssertThread();
};


}}}}}
