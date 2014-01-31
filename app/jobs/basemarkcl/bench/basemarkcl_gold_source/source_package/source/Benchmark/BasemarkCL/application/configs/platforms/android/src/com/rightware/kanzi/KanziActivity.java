package com.rightware.kanzi;

import android.app.Activity;
import android.os.Bundle;
import android.os.Debug;
import android.util.Log;
import android.os.Looper;
import android.os.Handler;
import android.os.PowerManager;
import android.content.Context;

import com.rightware.kanzi.ErrorBox.onErrorOk;
import com.rightware.kanzi.ErrorBox.onWarningOk;
import com.rightware.kanzi.KanziView.Renderer.onSurfaceStart;


public class KanziActivity extends Activity implements onErrorOk, onWarningOk, onSurfaceStart
{
    private static String TAG = "KanziActivity";    
    public KanziView mView = null;
    private PowerManager.WakeLock wl;
    private Handler handler = new Handler(); 
    
    @Override
    protected void onCreate(Bundle icicle) {
        onCreate(icicle, this);
    }
    
    protected void onCreate(Bundle icicle, KanziActivity me) 
    {
        mView = new KanziView(getApplication());
      
        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        wl = pm.newWakeLock(PowerManager.FULL_WAKE_LOCK, TAG);    
        
        String fileName = "/sdcard/rightware/kanzi/basemarkcl/application.cfg";
        boolean fileRead = SurfaceConfig.readFile(fileName, getApplication(), "application.cfg");
        if(!fileRead)
        { 	
           ErrorBox.showError(this, "Failed to read configuration file from path: '" + fileName + "'");
           
           // Dummy settings provided to make sure we get a surface on which we can draw error box and quit.
           mView.redSize = 5;
        	 mView.greenSize = 6;
        	 mView.blueSize = 5;
        	 mView.alphaSize = 0;
        	 mView.stencilSize = 0;
         	 mView.depthSize = 24;
         	 mView.fsaaSamples = 0;
        }
        else
        { 
          mView.redSize = SurfaceConfig.getValue("SurfaceBitsRed");
        	mView.greenSize = SurfaceConfig.getValue("SurfaceBitsGreen");
        	mView.blueSize = SurfaceConfig.getValue("SurfaceBitsBlue");
        	mView.alphaSize = SurfaceConfig.getValue("SurfaceBitsAlpha");
        	mView.stencilSize = SurfaceConfig.getValue("SurfaceBitsStencil");
        	mView.depthSize = SurfaceConfig.getValue("SurfaceBitsDepthBuffer");
        	mView.fsaaSamples = SurfaceConfig.getValue("SurfaceSamplesAntialiasing");
        	
        	String errorString = "Invalid:";
        	if(mView.redSize < 0) errorString += "\n'SurfaceBitsRed'";
        	if(mView.greenSize < 0) errorString += "\n'SurfaceBitsGreen'";
        	if(mView.blueSize < 0) errorString += "\n'SurfaceBitsBlue'";
        	if(mView.alphaSize < 0) errorString += "\n'SurfaceBitsAlpha'";
        	if(mView.stencilSize < 0) errorString += "\n'SurfaceBitsStencil'";
        	if(mView.depthSize < 0) errorString += "\n'SurfaceBitsDepthBuffer'";
        	if(mView.fsaaSamples < 0) errorString += "\n'SurfaceSamplesAntialiasing'";

        	if(errorString.compareTo("Invalid:") != 0)
        	{
        			errorString += " in: \n'" + fileName + "'";
        			ErrorBox.showError(this, errorString);
        	}
        }
		  	
          System.loadLibrary("kanzi");
      
    	    super.onCreate(icicle);
    	    mView = me.getView();
    	    mView.init(this);
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
    
    @Override protected void onStop() 
    {
        super.onStop();
        Log.w(TAG, "Kanzi stopped");
        KanziNativeLibrary.pause();
        if (KanziNativeLibrary.DEBUG) Debug.stopMethodTracing();
    }
    
    @Override
    protected void onPause() 
    {
        try 
        {
            super.onPause();
            Log.w(TAG, "KanziActivity.onPause");
            mView.onPause();     
            wl.release();   
        }
        catch(Exception e) 
        {
            Log.v(TAG, "Exception intercepted at onPause!");
        }
    }
    
    @Override
    protected void onResume()	
    {
        try 
        {
            super.onResume();
            Log.w(TAG, "KanziActivity.onResume");
            mView.onResume();     
            wl.acquire();          
        }
        catch(Exception e) 
        {
            Log.v(TAG, "Exception intercepted at onResume!");
        }
    }
 
    public void onErrorOk()
	  {
	  		System.exit(0);
	  }
	  
	  public void onWarningOk()
	  {
	  		// Continue execution	normally
	  }
 
    public void onSurfaceStart(final String info)
	  {
        if(mView.getUsingFallback())
        {
        		final Activity activity = this;
        		
						handler.post( 
							new Runnable () 
							{ 
								public void run () 
								{ 
        					ErrorBox.showWarning(activity, info);
        				}
        			}
        		);
      	}
	  }   
}
