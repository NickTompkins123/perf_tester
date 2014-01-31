/**
* \file
* Application framework main entry point. Android implementation.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include <application/kza_application.h>
#include <application/kza_application_interface.h>
#include <application/kza_android_application_interface.h>

#include <system/android.h>
#include <system/kzs_types.h>
#include <system/time/kzs_time.h>
#include <system/time/kzs_tick.h>
#include <system/debug/kzs_error.h>
#include <system/debug/kzs_log.h>
#include <system/wrappers/kzs_memory.h>
#include <system/input/kzs_input.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>

#include <android/log.h>
#include <jni.h>

static struct KzaApplication* application = KZ_NULL;
static kzBool applicationRunning = KZ_FALSE;
static kzUint currentTime;
static kzUint previousTime;

static kzsError init(void);
static kzsError pause(void);
static kzsError resume(void);
static kzsError deinit(void);
static enum KzaApplicationState update(void);
static void handleKeyEvents(int buttonCode, int state);

JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_init(JNIEnv* env, jobject obj);
JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_deinit(JNIEnv* env, jobject obj);
JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_pause(JNIEnv* env, jobject obj);
JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_resume(JNIEnv* env, jobject obj);
/* update returns JNI_TRUE on success, JNI_FALSE if KZA_APPLICATION_STATE_QUITTING */
JNIEXPORT jboolean JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_update(JNIEnv* env, jobject obj);
JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_touchEvent(JNIEnv* env, jobject obj, jint x, jint y, jint state);
JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_focusEvent(JNIEnv* env, jobject obj, jboolean focusState);
JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_keyEvent(JNIEnv* env, jobject obj, jint buttonCode, jint state);
JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_setView(JNIEnv* env, jobject obj, jobject kanziView);

JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_toggleHUD(JNIEnv* env, jobject obj);
JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_setHUD(JNIEnv* env, jobject obj, jboolean onoff);
JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_toggleFreeCamera(JNIEnv* env, jobject obj);
JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_setFreeCamera(JNIEnv* env, jobject obj, jboolean onoff);


void doNothing()
{
}

struct KzaApplication *kzaAndroidGetApplication()
{
    return application;
}

/* TODO: Rename to kzaApplicationInitialize_internal, or just inline the content of the function */
static kzsError init()
{
    struct KzaSystemProperties systemProperties;
    kzsError result;
    
    if(application == KZ_NULL)
    {
        /* kzsLog(KZS_LOG_LEVEL_INFO, "Entering init"); */
        __android_log_write(ANDROID_LOG_INFO, "KANZI kza_android_main", "init STARTED");

        systemProperties.programArgumentCount = 0;
        systemProperties.programArguments = NULL;

        /* Initialize application framework. Allocates memory for application struct. */
        result = kzaApplicationCreate(&systemProperties, &application);
        kzsErrorForward(result);

        __android_log_write(ANDROID_LOG_INFO, "KANZI kza_android_main", "init 1");

        /* Init application. */
        result = kzaApplicationInitialize(application);
        kzsErrorForward(result);

        __android_log_write(ANDROID_LOG_INFO, "KANZI kza_android_main", "init 2");

        kzaApplicationSetState(application, KZA_APPLICATION_STATE_RUNNING);
        applicationRunning = KZ_TRUE;

        currentTime = kzsTimeGetCurrentTimestamp();
        previousTime = currentTime;

        /* kzsLog(KZS_LOG_LEVEL_INFO, "init DONE"); */
        __android_log_write(ANDROID_LOG_INFO, "KANZI kza_android_main", "init DONE");
    }
    else
    {
        result = resume();
        kzsErrorForward(result);
    }

    kzsSuccess();
}

static kzsError pause()
{
    kzsError result;

    kzsLog(KZS_LOG_LEVEL_INFO, "Pause start");
    result = kzaApplicationPause(application);
    kzsErrorForward(result);

    kzsLog(KZS_LOG_LEVEL_INFO, "Pause end");

    kzsSuccess();
}

static kzsError resume()
{
    kzsError result;

    kzsLog(KZS_LOG_LEVEL_INFO, "Resume start");
    result = kzaApplicationResume(application);
    kzsErrorForward(result);

    kzsLog(KZS_LOG_LEVEL_INFO, "Resume end");

    kzsSuccess();
}

static kzsError deinit()
{    
    kzsError result;
    __android_log_write(ANDROID_LOG_INFO, "KANZI kza_android_main", "deinit");
    
    result = kzaApplicationUninitialize(application);
    kzsErrorForward(result);
    
    result = kzaApplicationDelete(application);    
    kzsErrorForward(result);

    applicationRunning = KZ_FALSE;
    kzsSuccess();
}

static void handleKeyEvents(int buttonCode, int state)
{
    struct KzsKeyDeviceInputData* inputData;
    struct KzsWindow* window = kzaApplicationGetWindow(application);
    struct KzsInputEventQueue* inputEventQueue = kzsWindowGetInputEventQueue(window);
    enum KzsInputKeyDeviceState buttonState = KZS_KEY_DEVICE_STATE_DOWN;

    if (state == 0) buttonState = KZS_KEY_DEVICE_STATE_DOWN;
    if (state == 1) buttonState = KZS_KEY_DEVICE_STATE_UP;
    if (state == 2) buttonState = KZS_KEY_DEVICE_STATE_REPEAT;
        
    /*  TODO: Skip case 2 as it's not needed for now      
        TODO: Check if button code needs to be translated to Kanzi from Android. */

    kzsInputEventQueueAddKeyDeviceEvent(inputEventQueue, buttonCode, buttonState);
}

/** Store the Android view and view class reference and make the refs global so that they don't get invalidated by JVM */
JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_setView(JNIEnv* env, jobject obj, jobject kanziView)
{
    jclass localClass;

    /* TODO: delete old global ref? */
    /* if (g_view) (*env)->DeleteGlobalRef(env, g_view); */
    g_view = (*env)->NewGlobalRef(env, kanziView);    

    localClass = (*env)->GetObjectClass(env, kanziView);
    /* if (g_View_class) (*env)->DeleteGlobalRef(env, g_View_class); */
    g_View_class = (*env)->NewGlobalRef(env, localClass);
}

JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_init(JNIEnv* env, jobject obj)
{
    kzsError result;
    __android_log_write(ANDROID_LOG_INFO, "KANZI Java_com_rightware_kanzi_KanziNativeLibrary_init", "init called");
    
    result = init(); /* TODO: Pass result forward or throw a Java exception */
}

JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_deinit(JNIEnv* env, jobject obj)
{
    kzsError result;
    __android_log_write(ANDROID_LOG_INFO, "KANZI Java_com_rightware_kanzi_KanziNativeLibrary_deinit", "deinit called");
        
    (*env)->DeleteGlobalRef(env, g_view);
    (*env)->DeleteGlobalRef(env, g_View_class);    
    
    result = deinit();
}

JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_pause(JNIEnv* env, jobject obj) {
    kzsError result = pause();    
}

JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_resume(JNIEnv* env, jobject obj) {
    kzsError result = resume();    
}


/* commented-out code: update KanziView reference on every update */
JNIEXPORT jboolean JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_update(JNIEnv* env, jobject obj/*, jobject kanziView*/)
{
    kzsError result;
    jboolean returnValue = JNI_TRUE;
    kzUint deltaTime;
    struct KzsWindow* window = kzaApplicationGetWindow(application);
    struct KzsInputEventQueue* inputEventQueue = kzsWindowGetInputEventQueue(window);
    kzsInputEventHandleFunction handler = kzsWindowGetInputEventHandler(window);    

    /* kzsLog(KZS_LOG_LEVEL_INFO, "Enter Update"); */
    /* __android_log_write(ANDROID_LOG_INFO, "KANZI kza_android_main", "update"); */

    if (handler != KZ_NULL)
    {
        void* userData = kzsWindowGetUserData(window);
        result = handler(inputEventQueue, userData);
        kzsErrorForward(result);
    }    

    /* g_kanziView = (*env)->NewGlobalRef(env, kanziView);
    kzaApplicationPlatformSetKanziView(kanziView); */
    
    /* Delta time. */
    currentTime = kzsTimeGetCurrentTimestamp();
    deltaTime = currentTime - previousTime;
    previousTime = currentTime;

    if (kzaApplicationGetState(application) == KZA_APPLICATION_STATE_RUNNING)
    {
        kzaApplicationUpdate(application, deltaTime);
        kzaApplicationUpdateWindows(application);
    }

    if (kzaApplicationGetState(application) == KZA_APPLICATION_STATE_QUITTING)
    {
        returnValue = JNI_FALSE;
    }

    kzsInputEventQueueClear(inputEventQueue);

    return returnValue;
}

JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_keyEvent(JNIEnv* env, jobject obj, jint buttonCode, jint state)
{
    handleKeyEvents(buttonCode, state);
}

JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_focusEvent(JNIEnv* env, jobject obj, jboolean focusState)
{
    kzsError result;
    struct KzsWindow* window = kzaApplicationGetWindow(application);
    kzsWindowEventHandlerFunction eventHandler = kzsWindowGetEventHandler(window);
    if (focusState)
    {
        kzsWindowSetState(window, KZS_WINDOW_STATE_ACTIVE);
        result = eventHandler(window, KZS_WINDOW_EVENT_FOCUS_GAINED, kzsWindowGetUserData(window));        
        /* kzsErrorForward(result); */
    }
    else
    {
        kzsWindowSetState(window, KZS_WINDOW_STATE_INACTIVE);
        result = eventHandler(window, KZS_WINDOW_EVENT_FOCUS_LOST, kzsWindowGetUserData(window));        
        /* kzsErrorForward(result); */
    } 
}


JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_touchEvent(JNIEnv* env, jobject obj, /*jobject kanziView,*/ jint x, jint y, jint state)
{
    struct KzsPointingDeviceInputData* inputData;
    struct KzsWindow* window = kzaApplicationGetWindow(application);
    struct KzsInputEventQueue* inputEventQueue = kzsWindowGetInputEventQueue(window);
    struct KzsPointingDeviceInputData* pointingData;

    kzU32 buttons = 0;
    kzFloat pressure = (state == 1) ? 0.0f : 1.0f;
    enum KzsInputPointingDeviceState buttonState;

    if (state == 0) buttonState = KZS_POINTING_DEVICE_STATE_DOWN; /* TODO: Always use braces with if */
    if (state == 1) buttonState = KZS_POINTING_DEVICE_STATE_UP;
    if (state == 2) buttonState = KZS_POINTING_DEVICE_STATE_DRAG;

    pointingData = kzsWindowSetActivePointer(window, 0);
    if(pointingData != KZ_NULL)
    {
        kzsInputSetPointingDeviceInputData(pointingData, 0, x, y, pressure, buttons, buttonState);
    }
}

JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_toggleHUD(JNIEnv* env, jobject obj)
{
    kzBool enabled;    
    kzsAssert(application != KZ_NULL);
    enabled = kzaApplicationIsFpsInfoEnabled(application);
    kzaApplicationSetFpsInfoEnabled(application, !enabled);
}

JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_setHUD(JNIEnv* env, jobject obj, jboolean onoff)
{
    kzsAssert(application != KZ_NULL);
    kzaApplicationSetFpsInfoEnabled(application, (onoff == JNI_TRUE) ? KZ_TRUE : KZ_FALSE);
}

JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_toggleFreeCamera(JNIEnv* env, jobject obj)
{
    kzBool enabled;
    kzsError result;
    kzsAssert(application != KZ_NULL);
    enabled = kzaApplicationIsFreeCameraEnabled(application);
    result = kzaApplicationSetFreeCameraEnabled(application, !enabled);    
}

JNIEXPORT void JNICALL Java_com_rightware_kanzi_KanziNativeLibrary_setFreeCamera(JNIEnv* env, jobject obj, jboolean onoff)
{
    kzsError result;
    kzsAssert(application != KZ_NULL);
    result = kzaApplicationSetFreeCameraEnabled(application, (onoff == JNI_TRUE) ? KZ_TRUE : KZ_FALSE);
}


jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env;
    jclass localClass;

    kzsLog(KZS_LOG_LEVEL_INFO, "JNI_OnLoad STARTED");

    g_javaVM = vm;
    if ((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_4) != JNI_OK)
    {
        kzsLog(KZS_LOG_LEVEL_INFO, "JNI version test (>= 1.4) failed.");
        return -1;
    }       
    
    localClass = (*env)->FindClass(env, "com/rightware/kanzi/KanziView");
    g_View_class = (jclass)((*env)->NewGlobalRef(env, localClass));    

    return JNI_VERSION_1_4;
}
