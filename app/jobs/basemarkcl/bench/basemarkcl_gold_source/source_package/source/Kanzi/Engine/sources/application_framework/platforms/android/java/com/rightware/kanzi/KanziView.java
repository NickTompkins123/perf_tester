/* TODO: Clean up the file. */
package com.rightware.kanzi;

import android.content.Context;
import android.graphics.Rect;
import android.opengl.GLSurfaceView;
import android.os.PowerManager;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

import android.app.ActivityManager;
import java.util.List;


public class KanziView extends GLSurfaceView {
    private static String TAG = "KanziView";
    private static final boolean DEBUG = false;    

    public KanziView(Context context)
    {
        super(context);
        
        KanziResourceFile.getInstance().setContext(context);
        
		listServices(context);
		
        /* Comment out the following line to use GLES 1.1 */
        setEGLContextFactory(new ContextFactory());

        setEGLConfigChooser(new ConfigChooser());
        setRenderer(new Renderer());
    }
	
	private void listServices(Context context)
	{
		//ActivityManager am = (ActivityManager)this.getSystemService(ACTIVITY_SERVICE);
		ActivityManager am = (ActivityManager)context.getSystemService(context.ACTIVITY_SERVICE);

		List<ActivityManager.RunningServiceInfo> rs = am.getRunningServices(50);

		for (int i=0; i<rs.size(); i++) {
			ActivityManager.RunningServiceInfo rsi = rs.get(i);
			Log.i("Service", "Process " + rsi.process + " with component " + rsi.service.getClassName());
		}
	}
	
    /** prevent the screen from going to sleep. Set this in constructor of the view */
    public void setWakeLock(Context context) {
		PowerManager powermanager = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
		wl = powermanager.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "My Tag");
		wl.acquire();
    }
	private PowerManager.WakeLock wl;

    public int getScreenWidth()
    {
        return getWidth();
    }

    public int getScreenHeight()
    {
        return getHeight();
    }

    // TODO: copy accelerometer implementation from PhysicsDemo
    // TODO: handle the case of no accelerometer (check in constructor)
    public int accelerometerAvailable()
    {
        return 0;
    }

    public float getAccelerationX()
    {
        return 0;
    }

    public float getAccelerationY()
    {
        return 0;
    }

    public float getAccelerationZ()
    {
        return 0;
    }
    public float[] getAcceleration()
    {
    	float[] a = {0.f, 0.f, 0.f};
        return a;
    }

    
    private static class ContextFactory implements GLSurfaceView.EGLContextFactory
    {
        private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
        public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig)
        {
            Log.w(TAG, "creating OpenGL ES 2.0 context");
            checkEglError("Before eglCreateContext", egl);
            int[] attrib_list = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
            EGLContext context = egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list);
            checkEglError("After eglCreateContext", egl);
            return context;
        }

        public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context)
        {
            egl.eglDestroyContext(display, context);
        }
    }

    private static void checkEglError(String prompt, EGL10 egl)
    {
        int error;
        while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS)
        {
            Log.e(TAG, String.format("%s: EGL error: 0x%x", prompt, error));
        }
    }

    private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser
    {
        public ConfigChooser()
        {

        }

        private static int EGL_OPENGL_ES2_BIT = 4;
        private static int[] s_configAttribs =
        {
            EGL10.EGL_RED_SIZE, EGL10.EGL_DONT_CARE,
            EGL10.EGL_GREEN_SIZE, EGL10.EGL_DONT_CARE,
            EGL10.EGL_BLUE_SIZE, EGL10.EGL_DONT_CARE,
            EGL10.EGL_DEPTH_SIZE, 16,

            /* Comment out the following line to use GLES 1.1 */
            EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,

            EGL10.EGL_NONE
        };

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display)
        {

            int[] num_config = new int[1];
            EGLConfig[] config = new EGLConfig[1];
            egl.eglChooseConfig(display, s_configAttribs, config, 1, num_config);

            return config[0];
        }
    }

    private class Renderer implements GLSurfaceView.Renderer
    {
        public void onDrawFrame(GL10 gl)
        {
            newTouchEvent = false;
            KanziNativeLibrary.update();
        }

        public void onSurfaceChanged(GL10 gl, int width, int height)
        {
            Log.w(TAG, "SurfaceChanged (GLSurfaceView.Renderer)");
            newTouchEvent = false;
            KanziNativeLibrary.init();
            //KanziNativeLibrary.resumelib();	// TODO: check that this isn't called right after init, or that it doesn't slow down the startup
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config)
        {
        	Log.w(TAG, "SurfaceCreated (GLSurfaceView.Renderer)");
            newTouchEvent = false;            
        }
    }
    
    @Override protected void onFocusChanged(boolean gainFocus, int direction, Rect previouslyFocusedRect)    
    {
        KanziNativeLibrary.focusEvent(gainFocus);
    }
    
    @Override public boolean onKeyDown(int keyCode, KeyEvent event)
    {
    	return super.onKeyDown(keyCode, event);
    }

    @Override public boolean onTrackballEvent(MotionEvent event)
    {
    	return super.onTrackballEvent(event);
    }

    @Override public boolean onTouchEvent(MotionEvent event)
    {        
        // Process the most recent event
        touchX = (int)event.getX();
        touchY = (int)event.getY();
        
        switch (event.getAction())
        {
            case MotionEvent.ACTION_DOWN: touchState = 0; break;
            
            case MotionEvent.ACTION_CANCEL:
            case MotionEvent.ACTION_UP: touchState = 1; break;
            
            case MotionEvent.ACTION_MOVE:
                // if the down action hasn't been registered, don't overwrite it with move actions
                if (!newTouchEvent) touchState = 2; 
                break;
        }
        
        // Send the touch event forward to Kanzi
        KanziNativeLibrary.touchEvent(touchX, touchY, touchState);

        // Signal that there is an unhandled touch event waiting
        newTouchEvent = true;

        return true;
    }

    static private int touchX;
    static private int touchY;
    static private int touchState;
    static private boolean newTouchEvent;
}
