/**
* \file
* Application framework main entry point. Maemo implementation.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include <application/kza_application.h>
#include <application/kza_application_interface.h>
#include <application/kza_maemo_application.h>

#include <core/util/collection/kzc_dynamic_array.h>

#include <system/kzs_types.h>
#include <system/file/kzs_file_base.h>
#include <system/time/kzs_time.h>
#include <system/display/kzs_window.h>
#include <system/display/kzs_window_native.h>

#include <system/debug/kzs_error.h>
#include <system/debug/kzs_log.h>
#include <system/wrappers/kzs_opengl_base.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h> 
#include <sys/io.h>

/*#include <libosso.h>*/
#define EXIT_SUCCESS    0
#define EXIT_FAILURE    1

struct KzaApplication* application = KZ_NULL;
/*osso_context_t* ossoContext;*/

/*void hardwareEventHandler(osso_hw_state_t *state, gpointer data);*/

int main(int argc, char* argv[])
{
    kzsError result;
    int exitStatus = EXIT_SUCCESS;

    struct KzsTime applicationStartTime;
    kzUint currentTime;
    kzUint previousTime;        
       
    char pwd[512];
    
    /* Parse working directory. */
#if 0
    if(argv[0])
    {
        int i = strlen(argv[0]);
        while (i > 0)
        {
            --i;
            if(argv[0][i] == '\\' || argv[0][i] == '/')
            {
                strncpy(pwd, argv[0], i);
                continue;
            }
        }
        pwd[i] = "\0"; 
        kzsFileBaseSetResourceDirectory(pwd);   /* only pointer will be stored */
    }
#endif
    
    struct KzaSystemProperties systemProperties;

    systemProperties.programArgumentCount = argc;
    systemProperties.programArguments = argv;    

    applicationStartTime = kzsTimeGetTime();

    /* Initialize application framework. Allocates memory for application struct. */
    result = kzaApplicationCreate(&systemProperties, &application);
    
    /*ossoContext = osso_initialize("Kanzi", "1.0.0", FALSE, NULL);*/
    
    /* Handler for hardware D-BUS messages */
    /*osso_hw_set_event_cb( ossoContext, NULL, hardwareEventHandler, (gpointer)application);    */

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
        if(result != KZS_SUCCESS)
        {
            printf("Error while updating: %d\n", result);
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

/*
void hardwareEventHandler(osso_hw_state_t *state, gpointer data)
{
    AppData *appdata;
    appdata = (AppData *) data;

    if (state->shutdown_ind)
    {
        hildon_banner_show_information(GTK_WIDGET(appdata->window), NULL, 
                                       "Shutdown event!");
    }
    if (state->memory_low_ind)
    {
        hildon_banner_show_information(GTK_WIDGET(appdata->window), NULL, 
                                       "Memory low event!");
    }
    if (state->save_unsaved_data_ind)
    {
        hildon_banner_show_information(GTK_WIDGET(appdata->window), NULL, 
                                       "Must save unsaved data event!");
    }
    if (state->system_inactivity_ind)
    {
        hildon_banner_show_information(GTK_WIDGET(appdata->window), NULL, 
                                       "Minimize application inactivity event!");
    }
    
}
*/
