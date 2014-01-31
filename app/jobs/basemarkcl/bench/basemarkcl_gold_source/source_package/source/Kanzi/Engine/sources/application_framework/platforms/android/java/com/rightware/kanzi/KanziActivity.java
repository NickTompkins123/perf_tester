package com.rightware.kanzi;

import android.app.Activity;
import android.os.Bundle;
import android.os.Debug;
import android.util.Log;

public class KanziActivity extends Activity {
    private static String TAG = "KanziActivity";    
    public KanziView mView = null;

	@Override
	protected void onCreate(Bundle icicle) {
        onCreate(icicle, this);
    }
    
    protected void onCreate(Bundle icicle, KanziActivity me) {
    	if (KanziNativeLibrary.DEBUG) Debug.startMethodTracing("kanzi");
        System.loadLibrary("kanzi");
    	super.onCreate(icicle);
    	mView = me.getView();
    	setContentView(mView);
    	KanziNativeLibrary.setView(mView);
	}
    
    protected KanziView getView()
    {
        if (mView == null)
        {
            mView = new KanziView(getApplication());
        }
        return mView;
    }

	@Override
	protected void onStop() {
		super.onStop();
		Log.w(TAG, "Kanzi stopped");
		KanziNativeLibrary.pause();
		if (KanziNativeLibrary.DEBUG) Debug.stopMethodTracing();
	}
   
    @Override
    protected void onPause() {
        super.onPause();
		Log.w(TAG, "KanziActivity.onPause");
        mView.onPause();        
    }

    @Override
    protected void onResume()	{
        super.onResume();
		Log.w(TAG, "Kanzi stopped.onResume");
        mView.onResume();        
    }
    
}
