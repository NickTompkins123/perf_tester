package com.rightware.kanzi;

import android.util.Log;

public class KanziNativeLibrary
{   
    private static String TAG = "KanziNativeLibrary";
    public static final boolean DEBUG = false;
    
    /* loadlibrary moved over to activity
    public static void loadlib(KanziView kanziObject)
    {
    	Log.w(TAG, "loadlib STARTED");
        System.loadLibrary("kzb_player");
        setView(kanziObject);
        Log.w(TAG, "loadlib DONE");
    }
    */
    
    public static void resumelib()
    {
    	Log.w(TAG, "resumelib STARTED");
        pause();
        resume();
        Log.w(TAG, "resumelib DONE");
    }

    public static native void init();
    public static native void deinit();
    public static native boolean update();    // returns false if KZA_APPLICATION_STATE_QUITTING
    public static native void pause();    
    public static native void resume();
    public static native void touchEvent(int x, int y, int state);
    public static native void focusEvent(boolean focusState);
    public static native void keyEvent(int buttonCode, int state);
    
    /** debug function: toggle Head-Up Display */
    public static native void toggleHUD();
    /** debug function: switch Head-Up Display on or off */
    public static native void setHUD(boolean state);
    /** debug function: toggle free camera */
    public static native void toggleFreeCamera();
    /** debug function: switch free camera movement on or off */
    public static native void setFreeCamera(boolean state);
    
    /** update the native side reference to the current active view */
    public static native void setView(Object kanziView);
}