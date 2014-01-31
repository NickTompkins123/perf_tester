/**
* \file
* Application framework main entry point. Win32 implementation.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include <windows.h>

#include <application/kza_application.h>
#include <application/kza_application_interface.h>
#include <application/kza_win32_application.h>

#include <core/util/collection/kzc_dynamic_array.h>

#include <system/kzs_types.h>
#include <system/time/kzs_time.h>
#include <system/time/kzs_tick.h> 
#include <system/debug/kzs_error.h>
#include <system/debug/kzs_log.h>

#include <system/display/kzs_window_native.h>
#include <system/wrappers/kzs_opengl.h>

#include <stdio.h>
#include <crtdbg.h>


kzsError kzaWin32Main(const struct KzaSystemProperties* systemProperties);


static void kzaWin32ErrorDialog_internal(kzsError error)
{
    kzString errorString = kzsErrorGetLastErrorMessage();
    char errorTitle[40];
    sprintf_s(errorTitle, 40, "Error: %d", error);
    MessageBoxA(NULL, errorString, errorTitle, MB_OK | MB_TOPMOST);
}


#ifdef _DEBUG
int main(int argc, char* argv[])
{
    kzsError result;
    int exitStatus = EXIT_SUCCESS;
    struct KzaSystemProperties systemProperties;

    systemProperties.programArgumentCount = argc;
    systemProperties.programArguments = (kzString*)argv;

    /* Enable memory leak checks and heap validation. */
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetBreakAlloc(-1);

    /* Set console title. */
    SetConsoleTitle(L"Kanzi debug console");

    result = kzaWin32Main(&systemProperties);
    kzsErrorIf(result)
    {
        kzaWin32ErrorDialog_internal(result);

        exitStatus = EXIT_FAILURE;
    }

    return exitStatus;
}
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    kzsError result;
    int exitStatus = EXIT_SUCCESS;
    int i;
    struct KzaSystemProperties systemProperties;
    kzMutableString* programArguments;

    LPTSTR cmdLine = GetCommandLine();
    LPWSTR* cmdLineArgs;
    int argumentCount;
    cmdLineArgs = CommandLineToArgvW(cmdLine, &argumentCount);

    systemProperties.programArgumentCount = (kzUint)argumentCount;

    /* Allocate space for pointers */
    programArguments = (kzMutableString*)malloc(argumentCount * sizeof(kzString));

    /* Convert wide char arguments to UTF8 */
    for(i = 0; i < argumentCount; ++i)
    {
        int length;
        kzMutableString argument;

        /* Get length of the resulting string first. */
        length = WideCharToMultiByte(CP_UTF8, 0, cmdLineArgs[i], -1, NULL, 0, NULL, NULL);

        /* Convert */
        argument = (kzMutableString)malloc(length * sizeof(kzChar));
        WideCharToMultiByte(CP_UTF8, 0, cmdLineArgs[i], -1, argument, length, NULL, NULL);

        programArguments[i] = argument;
    }

    systemProperties.programArguments = programArguments;

    result = kzaWin32Main(&systemProperties);
    kzsErrorIf(result)
    {
        kzaWin32ErrorDialog_internal(result);

        exitStatus = EXIT_FAILURE;
    }

    /* Free the converted arguments */
    for(i = 0; i < argumentCount; ++i)
    {
        free(programArguments[i]);
    }
    free(programArguments);

    return exitStatus;
}
#endif

kzsError kzaWin32Main(const struct KzaSystemProperties* systemProperties)
{
    kzsError result;
    struct KzaApplication* application = KZ_NULL;

    kzUint currentTime;
    kzUint previousTime;

    /* Initialize application framework. Allocates memory for application struct. */
    result = kzaApplicationCreate(systemProperties, &application);
    kzsErrorForward(result);

    /* init kzApplication */
    kzaPlatformSetInstanceHandle(application, GetModuleHandle(NULL));

    /* Init application. */
    result = kzaApplicationInitialize(application);
    kzsErrorForward(result);

    kzsLog(KZS_LOG_LEVEL_INFO, "Starting program main loop");

    kzaApplicationSetState(application, KZA_APPLICATION_STATE_RUNNING);

    currentTime = kzsTimeGetCurrentTimestamp();
    previousTime = currentTime;

    /* Actual main loop. */
    while(kzaApplicationGetState(application) == KZA_APPLICATION_STATE_RUNNING ||
          kzaApplicationGetState(application) == KZA_APPLICATION_STATE_PAUSED)
    {
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
        struct KzsWindow* window = kzaApplicationGetWindow(application);
        struct KzsInputEventQueue* queue = kzsWindowGetInputEventQueue(window);
        kzsInputEventHandleFunction handler = kzsWindowGetInputEventHandler(window);
#endif
        kzUint deltaTime;

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
        kzsAssert(queue != NULL);
 
        if(handler != KZ_NULL)
        {
            void *userData = kzsWindowGetUserData(window);
            result = handler(queue, userData);
            kzsErrorForward(result);
        }
#endif

        /* Delta time. */
        currentTime = kzsTimeGetCurrentTimestamp();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        /* Update application state. */
        result = kzaApplicationUpdate(application, deltaTime);
        kzsErrorIf(result)
        {
            kzaApplicationSetState(application, KZA_APPLICATION_STATE_ERROR);
        }
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
        kzsInputEventQueueClear(queue);

        if(kzaApplicationGetState(application) == KZA_APPLICATION_STATE_RUNNING)
        {
           result = kzaApplicationUpdateWindows(application);
           kzsErrorIf(result)
           {
               kzaApplicationSetState(application, KZA_APPLICATION_STATE_ERROR);
           }
        }
#endif
    }
    /* Exited from loop, shutting down. */

    /* Clean up application. */
    result = kzaApplicationUninitialize(application);
    kzsErrorForward(result);

    /* Clean up application framework and internal systems. */
    result = kzaApplicationDelete(application);
    kzsErrorForward(result);

    /* If debugger is present, a pause is required to keep the console output visible. Otherwise the pause is automatic. */
    if (IsDebuggerPresent())
    {
        system("pause");
    }

    kzsSuccess();
}
