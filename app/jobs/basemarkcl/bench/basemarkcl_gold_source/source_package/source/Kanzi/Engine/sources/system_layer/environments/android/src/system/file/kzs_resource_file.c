/**
* \file
* Resource file reading functions.
*
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_resource_file.h"

#include <system/file/kzs_file_base.h>
#include <system/file/kzs_file.h>
#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_math.h>
#include <system/wrappers/kzs_string.h>
#include <system/kzs_error_codes.h>
#include <system/debug/kzs_log.h>

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <jni.h>
/* #include <android/log.h> */
#include <stdio.h>

typedef void APKFile;

/* Globally available Java VM object (defined in kza_android_application_interface.c) */
/* TODO: define in system layer instead, also get the JavaVM reference either by
            callback from JNI_OnLoad or having the JNI_OnLoad here. */
extern JavaVM* g_javaVM;


/**
 * Resource structure
 */
struct KzsResourceFile
{
    kzsFile* file;
    APKFile* apkfile;
    kzUint position;
};

static int apkinitialized = 0;
static jobject s_globalThiz;
static jclass classAPKFile;
static jclass classResourceFile;
static jmethodID s_openFile;
static jmethodID s_closeFile;
static jmethodID s_skipFile;
static jmethodID s_readFile;
static jfieldID s_dataId;

void APKInit(void);
APKFile* APKOpen(char const* filename);
void APKClose(APKFile* file);
kzInt APKRead(void* ptr, kzUint byteCount, APKFile* stream);
long APKSkip(APKFile* stream, long offset);


void APKInit()
{    
    jmethodID getInstance;
    jobject thiz;
    JNIEnv *env;        

    (*g_javaVM)->AttachCurrentThread(g_javaVM, (void **)&env, NULL);

    apkinitialized = 1;
    
    classResourceFile = (*env)->FindClass(env, "com/rightware/kanzi/KanziResourceFile");
    getInstance = (*env)->GetStaticMethodID(env, classResourceFile, "getInstance", "()Lcom/rightware/kanzi/KanziResourceFile;");
    classAPKFile = (*env)->FindClass(env, "com/rightware/kanzi/KanziResourceFile$ResourceFile");

    s_openFile = (*env)->GetMethodID(env, classResourceFile, "APKOpenFile", "(Ljava/lang/String;)Lcom/rightware/kanzi/KanziResourceFile$ResourceFile;");
    s_closeFile = (*env)->GetMethodID(env, classResourceFile, "APKCloseFile", "(Lcom/rightware/kanzi/KanziResourceFile$ResourceFile;)V");
    s_skipFile = (*env)->GetMethodID(env, classResourceFile, "APKSkipFile", "(Lcom/rightware/kanzi/KanziResourceFile$ResourceFile;I)J");
    s_readFile = (*env)->GetMethodID(env, classResourceFile, "APKReadFile", "(Lcom/rightware/kanzi/KanziResourceFile$ResourceFile;I)J");
    s_dataId = (*env)->GetFieldID(env, classAPKFile, "data", "[B");

    thiz = (*env)->CallStaticObjectMethod(env, classResourceFile, getInstance);
    s_globalThiz = (*env)->NewGlobalRef(env, thiz);
}

APKFile* APKOpen(char const* filename)
{
    jstring filenameUTF;
    jobject fileHandle;
    JNIEnv *env;        

    (*g_javaVM)->AttachCurrentThread(g_javaVM, (void **)&env, NULL);

    if (apkinitialized == 0) APKInit();

    filenameUTF = (*env)->NewStringUTF(env, filename);
    fileHandle = (*env)->CallObjectMethod(env, s_globalThiz, s_openFile, filenameUTF);
    (*env)->DeleteLocalRef(env, (jobject)filenameUTF);

    return (APKFile*)fileHandle;
}

void APKClose(APKFile* file)
{
    JNIEnv *env;        

    (*g_javaVM)->AttachCurrentThread(g_javaVM, (void **)&env, NULL);
    
    if (apkinitialized == 0) APKInit();

    (*env)->CallVoidMethod(env, s_globalThiz, s_closeFile, (jobject)file);
    (*env)->DeleteLocalRef(env, (jobject)file);
}

kzInt APKRead(void* ptr, kzUint byteCount, APKFile* stream)
{
    JNIEnv *env;        
    jlong ret;
    
    (*g_javaVM)->AttachCurrentThread(g_javaVM, (void **)&env, NULL);
    ret = (*env)->CallLongMethod(env, s_globalThiz, s_readFile, (jobject)stream, (jint)byteCount);

    if(ret > 0)
    {
        jbyteArray byteArray = (jbyteArray) (*env)->GetObjectField(env, (jobject)stream, s_dataId);
        char *data = (char *) (*env)->GetByteArrayElements(env, byteArray, NULL);
        
        memcpy(ptr, data, ret);
        
        (*env)->ReleaseByteArrayElements(env, byteArray, (jbyte*)data, 0);
        (*env)->DeleteLocalRef(env, byteArray);
    }
    else
    {
        /* EOF or error */
        ret = 0;
    }

    return ret;
}

long APKSkip(APKFile* stream, long offset)
{
    JNIEnv *env;
    jlong ret;

    (*g_javaVM)->AttachCurrentThread(g_javaVM, (void **)&env, NULL);
    ret = (*env)->CallLongMethod(env, s_globalThiz, s_skipFile, (jobject)stream, (jint)offset);

    if(ret == -1)
    {
        ret = 0;
    }

    return ret;
}

kzsError kzsResourceFileCreate(kzString resourcePath, struct KzsResourceFile** out_resourceFile)
{
    struct KzsResourceFile* resourceFile;
    kzString resourceDirectory = kzsFileBaseGetResourceDirectory();

    resourceFile = kzsMalloc(sizeof(*resourceFile));
    kzsErrorTest(resourceFile != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Out of memory while opening resource.");

    /* If a path is defined, load the file from the file system, else load it from the .apk */
    if(resourceDirectory != KZ_NULL)
    {
        kzUint resourceDirectoryLength = kzsStrlen(resourceDirectory);
        kzUint resourcePathLength = kzsStrlen(resourcePath);
        /* resourceDirectory + '/' + resourcePath + '\0' */
        kzUint fullPathLength = resourceDirectoryLength + 1 + resourcePathLength + 1;
        kzMutableString fullPath = (kzMutableString)kzsMalloc(fullPathLength);
        kzsStrcpy(fullPath, resourceDirectory);
        kzsStrcat(fullPath, "/");
        kzsStrcat(fullPath, resourcePath);
        resourceFile->file = kzsFopen(fullPath, KZS_FILE_READONLY_BINARY);


        if(resourceFile->file == KZ_NULL && resourceFile->apkfile == KZ_NULL)
        {
            kzsLog(KZS_LOG_LEVEL_DETAIL, "Resource file failed to open from path!");
            kzsLog(KZS_LOG_LEVEL_DETAIL, fullPath);
            kzsErrorThrow(KZS_ERROR_FILE_OPEN_FAILED, "Failed to open resource file!");
        }

        kzsFree(fullPath);
    }
    else
    {
        resourceFile->apkfile = APKOpen(resourcePath);
        if(resourceFile->file == KZ_NULL && resourceFile->apkfile == KZ_NULL)
        {
            kzsLog(KZS_LOG_LEVEL_DETAIL, "Resource file failed to open!");
            kzsLog(KZS_LOG_LEVEL_DETAIL, resourcePath);
            kzsErrorThrow(KZS_ERROR_FILE_OPEN_FAILED, "Failed to open resource file!");
        }
    }

    resourceFile->position = 0;

    *out_resourceFile = resourceFile;
    kzsSuccess();
}

kzsError kzsResourceFileDelete(struct KzsResourceFile* resourceFile)
{
    kzString resourceDirectory = kzsFileBaseGetResourceDirectory();
    
    if(resourceDirectory != KZ_NULL)
    {
        kzInt result = kzsFclose(resourceFile->file);
        kzsErrorTest(result == 0, KZS_ERROR_FILE_OPERATION_FAILED, "Failed to close resource file.");
    }
    else
    {
        APKClose(resourceFile->apkfile);
    }

    kzsFree(resourceFile);

    kzsSuccess();
}

kzInt kzsResourceFileRead(struct KzsResourceFile* resourceFile, kzUint byteCount, kzByte* out_buffer)
{
    kzInt result;
    kzString resourceDirectory = kzsFileBaseGetResourceDirectory();
    
    if(resourceDirectory != KZ_NULL)
    {
        kzsFile* file = resourceFile->file;

        if (kzsFeof(file))
        {
            result = 0;
        }
        else
        {
            kzUint readByteCount = (kzUint)kzsFread(out_buffer, sizeof(kzByte), byteCount, file);
            resourceFile->position += readByteCount;

            if (kzsFerror(file) != 0)
            {
                result = KZS_RESOURCE_FILE_ERROR;
            }
            else
            {
                result = (kzInt)readByteCount;
            }
        }    
    }
    else
    {
        APKFile* file = resourceFile->apkfile;
        kzInt readByteCount = -1;

        if (file != KZ_NULL)
        {
            readByteCount = APKRead(out_buffer, byteCount, file);
        }

        if (readByteCount >= 0)
        {
            resourceFile->position += readByteCount;
            result = readByteCount;
        } else {
            result = KZS_RESOURCE_FILE_ERROR;
        }
    }

    return result;
}

kzInt kzsResourceFileSkip(struct KzsResourceFile* resourceFile, kzUint byteCount)
{
    kzInt result;
    kzString resourceDirectory = kzsFileBaseGetResourceDirectory();
    
    if(resourceDirectory != KZ_NULL)
    {
        kzsFile* file = resourceFile->file;
        result = kzsFileReadSkip(file, byteCount);

        if (result != KZS_FILE_ERROR)
        {
            resourceFile->position += (kzUint)result;
        }
        else
        {
            result = KZS_RESOURCE_FILE_ERROR;
        }
    }
    else
    {
        APKFile* file = resourceFile->apkfile;

        result = APKSkip(file, byteCount);
        resourceFile->position += result;

        if (result < 0)
        {
            result = KZS_RESOURCE_FILE_ERROR;
        }
    }

    return result;
}

kzUint kzsResourceFileGetPosition(const struct KzsResourceFile* resourceFile)
{
    return resourceFile->position;
}
