package com.rightware.kanzi;

import android.util.Log;

public class KanziNativeLibrary
{   
    public static void initlib(Object kanziObject)
    {
        System.loadLibrary("kanzi");
        init(kanziObject);
    }

    public static native void init(Object kanziObject);
    public static native void update();
    public static native void touchEvent(int x, int y, int state);
}
