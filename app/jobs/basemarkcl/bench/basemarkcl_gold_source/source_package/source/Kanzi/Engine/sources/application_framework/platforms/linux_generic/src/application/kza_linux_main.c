/**
* \file
* Application framework main entry point. Linux implementation.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include <application/kza_application.h>
#include <application/kza_application_interface.h>
#include <application/kza_linux_application_interface.h>

#include <core/util/collection/kzc_dynamic_array.h>

#include <system/kzs_types.h>
#include <system/file/kzs_file_base.h>
#include <system/time/kzs_time.h>
#include <system/time/kzs_tick.h>
#include <system/display/kzs_window.h>
#include <system/display/kzs_window_native.h>
#include <system/wrappers/kzs_string.h>

#include <system/debug/kzs_error.h>
#include <system/debug/kzs_log.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h> 
#include <sys/io.h>

/* #include <tslib.h> */

/* input devices are handled in platform-specific kzs_window_native.c */

#define EXIT_SUCCESS    0
#define EXIT_FAILURE    1

/* avoid memory allocation, use static max length */
#define MAX_PATH_LENGTH 512

struct KzaApplication* application;

int main(int argc, char* argv[])
{
    int exitStatus = EXIT_SUCCESS;
    kzsError result;

    struct KzaSystemProperties systemProperties;
    struct KzsTime applicationStartTime;
    kzUint currentTime;
    kzUint previousTime;        

    kzChar workingDirectory[MAX_PATH_LENGTH];
    
    /* Parse working directory. */
    if (argc >= 1)
    {
        kzUint i;
        kzChar path[MAX_PATH_LENGTH];
        kzUint len = kzsStrlen(argv[0]);
        if (len > MAX_PATH_LENGTH)
        {
            len = MAX_PATH_LENGTH;
        }
        
        for (i = len; i > 0; --i)
        {
            if (argv[0][i] == '\\' || argv[0][i] == '/')
            {
                break;
            }
        }
        kzsStrncpy(workingDirectory, argv[0], i);
        workingDirectory[i] = '\0';
    }
    
    kzsFileBaseSetResourceDirectory(workingDirectory);   /* only pointer will be stored */
    
    applicationStartTime = kzsTimeGetTime();

    /* Initialize application framework. Allocates memory for application struct. */
    systemProperties.programArgumentCount = (kzUint)argc;
    systemProperties.programArguments = (kzString *)argv;    
    result = kzaApplicationCreate(&systemProperties, &application);

    /* Init application. */
    result = kzaApplicationInitialize(application);
    if (result != KZS_SUCCESS)
    {
        kzsLog(KZS_LOG_LEVEL_ERROR, "ApplicationInitialize failed!");
        printf("Error %d\n", result);

        exitStatus = EXIT_FAILURE;
    }
    else
    {
        kzsLog(KZS_LOG_LEVEL_INFO, "Starting program main loop");

        kzaApplicationSetState(application, KZA_APPLICATION_STATE_RUNNING);

        currentTime = kzsTimeGetCurrentTimestamp();
        previousTime = currentTime;
    }    

    /* Actual main loop. */    
    while (kzaApplicationGetState(application) == KZA_APPLICATION_STATE_RUNNING)
    {
        /* Delta time. */
        kzUint deltaTime;
        currentTime = kzsTimeGetCurrentTimestamp();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        /* Update application state. */
        result = kzaApplicationUpdate(application, deltaTime);
        if (result != KZS_SUCCESS)
        {
            printf("Error while updating: %d\n", result);
            kzaApplicationSetState(application, KZA_APPLICATION_STATE_ERROR);
        }

        /* Render the screen. */
        result = kzaApplicationUpdateWindows(application);
        if (result != KZS_SUCCESS)
        {
            printf("Error while rendering: %d\n", result);
            kzaApplicationSetState(application, KZA_APPLICATION_STATE_ERROR);
        }
    }
    /* Exited from loop, shutting down. */

    /* Clean up application. */
    result = kzaApplicationUninitialize(application);
    if (result != KZS_SUCCESS)
    {
        kzsLog(KZS_LOG_LEVEL_ERROR, "ApplicationUninitialize failed!");
        printf("Error %d\n", result);
        exitStatus = EXIT_FAILURE;
    }

    /* Clean up application framework and internal systems. */
    result = kzaApplicationDelete(application);
    if (result != KZS_SUCCESS)
    {
        /* Not much we can do at this point.. */
        printf("Error %d\n", result);
        exitStatus = EXIT_FAILURE;
    }

    return exitStatus;
}
