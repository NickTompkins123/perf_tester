/**
* \file
* Application framework main entry point. QNX implementation.
*
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include <application/kza_application.h>
#include <application/kza_application_interface.h>
#include <application/kza_qnx_application_interface.h>

#include <core/util/collection/kzc_dynamic_array.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>
#include <system/debug/kzs_log.h>

#include <system/time/kzs_time.h>
#include <system/time/kzs_tick.h>
#include <system/display/kzs_window.h>
#include <system/display/kzs_window_native.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/io.h>

#include <KD/kd.h>

#define EXIT_SUCCESS    0
#define EXIT_FAILURE    1

struct KzaApplication* application;

/*int main(int argc, char* argv[])*/
KDint kdMain(int argc, const char *const *argv)
{
    kzsError result;
    int exitStatus = EXIT_SUCCESS;

    kzUint currentTime;
    kzUint previousTime;

    struct KzaSystemProperties systemProperties;

    systemProperties.programArgumentCount = argc;
    systemProperties.programArguments = argv;

    /* Initialize application framework. Allocates memory for application struct. */
    result = kzaApplicationCreate(&systemProperties, &application);

    /* Init application. */
    result = kzaApplicationInitialize(application);
    if(result != KZS_SUCCESS)
    {
        kzsLog(KZS_LOG_LEVEL_ERROR, "ApplicationInitialize failed!");
        printf("Error %d\n", result);

        /* TODO: exit here or just skip the main loop? */
        return EXIT_FAILURE;
    }
    kzsLog(KZS_LOG_LEVEL_INFO, "Starting program main loop");

    kzaApplicationSetState(application, KZA_APPLICATION_STATE_RUNNING);
    
    currentTime = kzsTimeGetCurrentTimestamp();
    previousTime = currentTime;    
    
    /* Actual main loop. */
    while(kzaApplicationGetState(application) == KZA_APPLICATION_STATE_RUNNING)
    {
        /* Delta time. */
        kzUint deltaTime;
        currentTime = kzsTimeGetCurrentTimestamp();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        /* Update application state. */
        result = kzaApplicationUpdate(application, deltaTime);
        if(result != KZS_SUCCESS)
        {
            printf("Error %d\n", result);
            kzaApplicationSetState(application, KZA_APPLICATION_STATE_ERROR);
        }
    }
    /* Exited from loop, shutting down. */

    /* Clean up application. */
    result = kzApplicationUninitialize(application);
    if(result != KZS_SUCCESS)
    {
        kzsLog(KZS_LOG_LEVEL_ERROR, "ApplicationUninitialize failed!");
        printf("Error %d\n", result);
        exitStatus = EXIT_FAILURE;
    }

    /* Clean up application framework and internal systems. */
    result = kzaApplicationDelete(application);
    if(result != KZS_SUCCESS)
    {
        /* Not much we can do at this point.. */
        printf("Error %d\n", result);
        exitStatus = EXIT_FAILURE;
    }

    return exitStatus;
}
