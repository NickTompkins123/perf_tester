/**
* \file
* EGL functions.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include <system/display/kzs_egl.h>
#include <system/kzs_opengl_extensions.h>

#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>
#include <system/debug/kzs_log.h>
#include <system/kzs_error_codes.h>


kzsError kzsEGLDisplayInitialize(NativeDisplayType displayType, EGLDisplay* out_eglDisplay)
{
    EGLDisplay eglDisplay;

    eglDisplay = eglGetDisplay(displayType);
    
    kzsErrorTest((eglDisplay != EGL_NO_DISPLAY), KZS_ERROR_DISPLAY_INITIALIZATION_FAILED, "Unable to init EGL display.");

    {
        /* Got a valid display, now init EGL. */
        EGLint eglVersionMajor, eglVersionMinor;
        EGLBoolean initializeResult = eglInitialize(eglDisplay, &eglVersionMajor, &eglVersionMinor);
        kzsErrorTest(initializeResult == EGL_TRUE, KZS_ERROR_ILLEGAL_ARGUMENT, "EGL initialization failed.");
    }

    *out_eglDisplay = eglDisplay;
    kzsSuccess();
}

kzInt kzsEGLGetNumConfigurations(EGLDisplay eglDisplay)
{
    kzInt configurationCount;
    EGLBoolean result;

    result = eglGetConfigs(eglDisplay, NULL, 0, &configurationCount);
    if(result != EGL_TRUE)
    {
        configurationCount = 0;
    }

    return configurationCount;
}

/* TODO: see problem described in kzsEGLConfigure, this function is broken!
kzInt kzsEGLGetNumMatchingConfigurations(EGLDisplay eglDisplay, EGLint *eglConfigurationAttributeArray)
{
    kzInt numConfigurations;
    EGLBoolean result;

    result = eglChooseConfig(eglDisplay, eglConfigurationAttributeArray, NULL, 0, &numConfigurations);
    if(result != EGL_TRUE)
    {
        numConfigurations = 0;
    }
    return numConfigurations;
}
*/

kzsError kzsEGLGetAllConfigurations(EGLDisplay eglDisplay, kzInt configurationCount, EGLConfig* out_configurations)
{
    kzInt returnedConfigurationCount;
    EGLBoolean result;

    result = eglGetConfigs(eglDisplay, out_configurations, configurationCount, &returnedConfigurationCount);
    kzsErrorTest((result == EGL_TRUE), KZS_ERROR_SURFACE_PARAMETER_QUERY_FAILED, "Unable to query EGL configurations.");
    kzsErrorTest((configurationCount == returnedConfigurationCount), KZS_ERROR_SURFACE_PARAMETER_QUERY_FAILED, "Wrong amount of EGL configurations received.");

    kzsSuccess();
}

/* TODO: see problem described in kzsEGLConfigure, changed this function */
kzsError kzsEGLGetMatchingConfigurations(EGLDisplay eglDisplay, EGLint *eglConfigurationAttributeArray, kzInt configurationCount, EGLConfig* out_configurations, kzInt *numReturnedConfigurations)
{    
    EGLBoolean result;

    result = eglChooseConfig(eglDisplay, eglConfigurationAttributeArray, out_configurations, configurationCount, numReturnedConfigurations);
    kzsErrorTest((result == EGL_TRUE), KZS_ERROR_SURFACE_PARAMETER_QUERY_FAILED, "Unable to query EGL configurations.");
    /* kzsErrorTest((numConfigurations == numReturnedConfigurations), KZS_ERROR_SURFACE_PARAMETER_QUERY_FAILED, "Wrong amount of EGL configurations received."); */

    kzsSuccess();
}

/**
* kzsEGLConfigure searches for an EGLConfig that matches eglConfigurationAttributeArray
*   as closely as possible. An additional search is done to ensure just the correct color
*   format if it's available. If not, the configuration suggested by eglChooseConfig is taken.
*   Anti-aliasing is disabled if there are no configs that support it. If no configs are found
*   that match or exceed the requirements of eglConfigurationAttributeArray, any suitable is
*   taken; again, primarily one that matches the color format, secondarily some other.
*/
kzsError kzsEGLConfigure(EGLDisplay eglDisplay, EGLint* eglConfigurationAttributeArray, EGLConfig *out_eglConfiguration)
{
    kzInt i = 0;
    kzsError result;
    EGLBoolean eglResult = EGL_TRUE;

    EGLint redBits = 0;                     /* bit depths in the config that was returned */
    EGLint greenBits = 0;
    EGLint blueBits = 0;            
    EGLint redAsked = 5;                    /* bit depths that were requested in eglConfigurationAttributeArray */
    EGLint greenAsked = 6;
    EGLint blueAsked = 5;

    kzInt eglConfigurationCount;             /* number of configurations returned by eglChooseConfig */
    kzInt eglConfigurationTotalCount;        /* number of configurations returned by eglGetConfigs */
    EGLConfig *configurationArray;          /* configs to test for correct color depth */
    EGLConfig eglConfig = 0;

    /* Store the configurations for a search */
    /* TODO: Problem: eglChooseConfig gives ALL available configurations when configuration array == NULL,
                        instead of only the suitable ones. It however gives less configurations when actually
                        given a non-NULL configuration array pointer. (Win32)
             Solution: use totalNumEglConfigurations for memory allocation.

    numEglConfigurations = kzsEGLGetNumMatchingConfigurations(eglDisplay, eglConfigurationAttributeArray); */
    eglConfigurationTotalCount = kzsEGLGetNumConfigurations(eglDisplay);
    kzsErrorTest((eglConfigurationTotalCount > 0), KZS_ERROR_SURFACE_INITIALIZATION_FAILED, "Unable to get an EGL configuration");

    configurationArray = (EGLConfig *)kzsMalloc(eglConfigurationTotalCount * sizeof(EGLConfig));
    result = kzsEGLGetMatchingConfigurations(eglDisplay, eglConfigurationAttributeArray, eglConfigurationTotalCount, configurationArray, &eglConfigurationCount);
    /* result = kzsEGLGetAllConfigurations(eglDisplay, totalNumEglConfigurations, configurationArray); */
    kzsErrorForward(result);

    /* If no configs found, check if AA was requested and try again without it. */
    if (eglConfigurationCount == 0)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "Could not find an EGL configuration with given settings. Trying again without anti-aliasing.");

        for (i = 0; eglConfigurationAttributeArray[i] != EGL_NONE; i++)
        {
            if(eglConfigurationAttributeArray[i] == EGL_SAMPLE_BUFFERS)
            {
                ++i;
                eglConfigurationAttributeArray[i] = EGL_DONT_CARE;
            }
            if(eglConfigurationAttributeArray[i] == EGL_SAMPLES)
            {
                ++i;
                eglConfigurationAttributeArray[i] = EGL_DONT_CARE;
            }
        }

        result = kzsEGLGetMatchingConfigurations(eglDisplay, eglConfigurationAttributeArray, eglConfigurationTotalCount, configurationArray, &eglConfigurationCount);        
        kzsErrorForward(result);
    }

    /* Still none found: just take the ones that are available and see if one with correct color depth can be found */
    if (eglConfigurationCount == 0)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "Could not find an EGL configuration that matches the requirements. Performance may be reduced.");
        result = kzsEGLGetAllConfigurations(eglDisplay, eglConfigurationTotalCount, configurationArray);
        kzsErrorForward(result);
        eglConfigurationCount = eglConfigurationTotalCount;
    }

    /* kzsErrorTest((numEglConfigurations != 0), KZS_ERROR_SURFACE_INITIALIZATION_FAILED, "Unable to get an EGL configuration that matches the requirements"); */

    /* Find out which bit depths were requested */
    for (i = 0; eglConfigurationAttributeArray[i] != EGL_NONE; i++)
    {
        switch (eglConfigurationAttributeArray[i]) {
            case EGL_RED_SIZE:
                i++;
                redAsked = eglConfigurationAttributeArray[i];
                break;
            case EGL_GREEN_SIZE:
                i++;
                greenAsked = eglConfigurationAttributeArray[i];
                break;
            case EGL_BLUE_SIZE:
                i++;
                blueAsked = eglConfigurationAttributeArray[i];
                break;
            default:
                break;
        }
    }

    /* Search for a configuration that has the correct color format */
    for (i = 0; i < eglConfigurationCount; i++)
    {
        eglResult  = eglGetConfigAttrib(eglDisplay, configurationArray[i], EGL_RED_SIZE, &redBits);
        eglResult &= eglGetConfigAttrib(eglDisplay, configurationArray[i], EGL_GREEN_SIZE, &greenBits);
        eglResult &= eglGetConfigAttrib(eglDisplay, configurationArray[i], EGL_BLUE_SIZE, &blueBits);

        if (eglResult == EGL_TRUE && redBits == redAsked && blueBits == blueAsked && greenBits == greenAsked) {
            eglConfig = configurationArray[i];
            break;
        }
    }
    
    /* None found, pick the first one */
    if (eglConfig == 0) 
    {
        eglConfig = configurationArray[0];
    }

    kzsFree(configurationArray);

    *out_eglConfiguration = eglConfig;

    kzsSuccess();
}

kzsError kzsEGLSurfaceInitialize(EGLDisplay eglDisplay, NativeWindowType eglWindow, EGLConfig* eglConfiguration, EGLSurface* out_eglSurface, EGLContext* out_eglContext)
{
    kzsError result;
    EGLSurface eglSurface = EGL_NO_SURFACE;
    EGLContext eglContext = EGL_NO_CONTEXT;

    /* Create EGL surface with the configuration. */
    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfiguration, eglWindow, KZ_NULL);
    kzsErrorTest((eglSurface != EGL_NO_SURFACE), KZS_ERROR_SURFACE_INITIALIZATION_FAILED, "Unable to init EGL surface.");

    /* Create EGL context. */
#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_2_0
    {
        EGLint contextAttributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
        eglContext = eglCreateContext(eglDisplay, eglConfiguration, KZ_NULL, contextAttributes);
    }
#else 
    eglContext = eglCreateContext(eglDisplay, eglConfiguration, KZ_NULL, KZ_NULL);
#endif
    kzsErrorTest((eglContext != EGL_NO_CONTEXT), KZS_ERROR_SURFACE_INITIALIZATION_FAILED, "Unable to init EGL context.");

    /* Activate EGL surface. */
    result = kzsEGLMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    kzsErrorForward(result);

    *out_eglSurface = eglSurface;
    *out_eglContext = eglContext;
    kzsSuccess();
}

kzsError kzsEGLUpdateSurface(EGLDisplay eglDisplay, EGLSurface eglSurface)
{
    EGLBoolean result;

    result = eglSwapBuffers(eglDisplay, eglSurface);
    kzsErrorTest((result != EGL_FALSE), KZS_ERROR_SURFACE_UPDATE_FAILED, "Unable to update EGL surface.");

    kzsSuccess();
}

kzsError kzsEGLMakeCurrent(EGLDisplay eglDisplay, EGLSurface eglSurfaceRead, EGLSurface eglSurfaceWrite, EGLContext eglContext)
{
    EGLBoolean result;

    result = eglMakeCurrent(eglDisplay, eglSurfaceRead, eglSurfaceWrite, eglContext);
    kzsErrorTest((result != EGL_FALSE), KZS_ERROR_SURFACE_ACTIVATION_FAILED, "Unable to activate EGL surface.");

    kzsSuccess();
}

kzsError kzsEGLCopyBuffers(EGLDisplay eglDisplay, EGLSurface eglSurface, void* target)
{
    EGLBoolean result;
    result = eglCopyBuffers(eglDisplay, eglSurface, target);
#if 0 /* TODO: Why is this code disabled? */
    if(result != EGL_TRUE)
    {
        EGLint error = eglGetError();
        kzsErrorThrow(KZS_ERROR_ASSERTION_FAILED, "Error on eglCopyBuffers.");
    }
#endif
    kzsSuccess();
}

kzsError kzsEGLSwapInterval(EGLDisplay eglDisplay, EGLint interval)
{
    EGLBoolean result;
    result = eglSwapInterval(eglDisplay, interval);

    kzsSuccess();
}

void (*kzsGLGetProcAddress(kzString functionName))(void)
{
    return eglGetProcAddress(functionName);
}
