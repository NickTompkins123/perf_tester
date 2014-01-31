package com.rightware.kanzi.basemarkcl;

import android.content.Context;
import android.os.Bundle;
import android.os.PowerManager;
import android.util.Log;
import com.rightware.kanzi.*;

public class BasemarkCL extends KanziActivity
{
    private static String TAG = "BasemarkCL";
    private PowerManager.WakeLock wl;

    @Override protected void onCreate(Bundle icicle)
    {
        super.onCreate(icicle);
        
        Log.v(TAG, "setting wakelock");  
        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        wl = pm.newWakeLock(PowerManager.FULL_WAKE_LOCK, TAG);
    }
}
