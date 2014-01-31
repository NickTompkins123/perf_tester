/**
 * \file
 * Platform-specific globals and constants for Android platform
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */

#include "android.h"

/* TODO: These should be static and named g_kzaKanziView etc. */
/* Static? Also, kzaKanziView might be misleading; the idea is to pass the View class
     of the application, not necessarily com.rightware.kanzi.KanziView  -jt */
/* TODO: maybe g_kzsJavaVM, g_kzsViewClass, g_kzsView. Comments? */

/* Needed in system layer to get the screen size in kzsDisplayNativeCreate. */

/* Globally available Java Virtual Machine object */
JavaVM* g_javaVM;

/* User-defined Android view class derived from com.rightware.kanzi.KanziView */
jclass g_View_class; 

/* Instance of the user-defined Android view class currently in use */
jobject g_view; 

