package com.rightware.kanzi;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

import java.io.IOException;
import java.io.InputStream;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

public class KanziView extends GLSurfaceView {
    private static String TAG = "KanziView";
    private static final boolean DEBUG = false;

    public KanziView(Context context)
    {
        super(context);
        
        KanziResourceFile.getInstance().setContext(context);
                      
        /* Comment out the following line to use GLES 1.1 */
        setEGLContextFactory(new ContextFactory());

        setEGLConfigChooser(new ConfigChooser());
        setRenderer(new Renderer());
    }

    public int getScreenWidth()
    {
        return getWidth();
    }

    public int getScreenHeight()
    {
        return getHeight();
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
            if(newTouchEvent)
            {
                KanziNativeLibrary.touchEvent(touchX, touchY, touchState);
                newTouchEvent = false;
            }

            KanziNativeLibrary.update();
        }

        public void onSurfaceChanged(GL10 gl, int width, int height)
        {
            newTouchEvent = false;
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config)
        {
            newTouchEvent = false;
            KanziNativeLibrary.initlib(KanziView.this);
        }
    }

    @Override public boolean onTouchEvent(MotionEvent event)
    {
        int action = event.getAction();
        int state = 0;
        if(action == MotionEvent.ACTION_DOWN) state = 0;
        if(action == MotionEvent.ACTION_UP) state = 1;
        if(action == MotionEvent.ACTION_MOVE) state = 2;
        if(action == MotionEvent.ACTION_CANCEL) state = 1;

        touchX = (int)event.getX();
        touchY = (int)event.getY();

        if(!newTouchEvent || state == 0 || state == 1) touchState = state;
        newTouchEvent = true;

        return true;
    }

    static private int touchX;
    static private int touchY;
    static private int touchState;
    static private boolean newTouchEvent;
}
