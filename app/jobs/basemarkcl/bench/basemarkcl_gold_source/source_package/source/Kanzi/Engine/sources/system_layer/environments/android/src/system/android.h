/**
 * \file
 * Platform-specific globals and constants for Android platform
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZS_ANDROID_H
#define KZS_ANDROID_H

#include <jni.h>

/* Newline character on Android platform. String because newline might be several char on some platform. */
#define KZ_NEWLINE (kzString)"\n"

/* Globally available Java Virtual Machine object */
extern JavaVM* g_javaVM;

/* User-defined Android view class derived from com.rightware.kanzi.KanziView */
extern jclass g_View_class; 

/* Instance of the user-defined Android view class currently in use */
extern jobject g_view; 

/* The correct way to get JNIEnv (see http://java.sun.com/docs/books/jni/html/other.html#26206):
    JNIEnv *env;
    (*g_javaVM)->AttachCurrentThread(g_javaVM, &env, NULL);
*/     

#endif
