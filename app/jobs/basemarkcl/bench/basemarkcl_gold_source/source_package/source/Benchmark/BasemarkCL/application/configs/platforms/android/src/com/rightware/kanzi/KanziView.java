package com.rightware.kanzi;


import android.app.Activity;
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


public class KanziView extends GLSurfaceView 
{
    private static String TAG = "KanziView";
    private static final boolean DEBUG = false;    
    private Renderer mRenderer;
	  private ConfigChooser configChooser;
	  
	  public int redSize;
	  public int greenSize;
	  public int blueSize;
	  public int alphaSize;
	  public int stencilSize;
	  public int depthSize;
	  public int fsaaSamples;
	  
    public KanziView(Context context)
    {
        super(context);
        
        KanziResourceFile.getInstance().setContext(context);
    }
				
    public boolean init(final Activity activity)
    {
        setEGLContextFactory(new ContextFactory());

		    mRenderer = new Renderer(activity);
		
				configChooser = new ConfigChooser(redSize, greenSize, blueSize, alphaSize, depthSize, stencilSize, fsaaSamples, mRenderer);
        setEGLConfigChooser(configChooser);

        setRenderer(mRenderer);
        
        setFocusable(true);
        
        return true;
    }
		
    /** prevent the screen from going to sleep. Set this in constructor of the view */
    public void setWakeLock(Context context) 
    {
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

    protected static class Renderer implements GLSurfaceView.Renderer, ConfigChooser.onFallbackDetermined
    {
     		protected Activity mActivity;
        String fallbackMessage;
    		
    		static interface onSurfaceStart
    		{
    				void onSurfaceStart(String info);    			
    		}
    		
    		
    	  public Renderer(final Activity activity)
    	  {
    	  	super();
    	  	mActivity = activity;
    	  	fallbackMessage = "No EGL configuration matching 'script.lua' -file found.";
    		}
    		
    		public void onFallbackDetermined(String info)
    	  {
			  		fallbackMessage = info;
			  }	  
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
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config)
        {
        	Log.w(TAG, "SurfaceCreated (GLSurfaceView.Renderer)");
            newTouchEvent = false;            
        }
    }
    
    
    private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser 
    {
    		private Renderer mRenderer;
    		
    		static interface onFallbackDetermined
    		{
    				void onFallbackDetermined(String info);    			
    		}
    		
    		public ConfigChooser(int r, int g, int b, int a, int depth, int stencil, int samples, Renderer renderer) 
        {
            mRedSize = r;
            mGreenSize = g;
            mBlueSize = b;
            mAlphaSize = 0;
            mDepthSize = depth;
            mStencilSize = stencil;
            mSamples = samples;
            mUsingFallback = false;
            mRenderer = renderer;
        }

        /* This EGL config specification is used to specify 2.0 rendering. We use a minimum size of 4 bits for red/green/blue, but will perform actual matching in chooseConfig() below. */
        private static int EGL_OPENGL_ES2_BIT = 4;
        private static int[] s_configAttribsSimple =
        {
            EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL10.EGL_NONE
        };

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) 
        { 
						Log.w(TAG, "Requesting configuration: rgba: "+mRedSize +", "+ mGreenSize +", "+ mBlueSize +", "+ mAlphaSize +", depth: "+ mDepthSize +", stencil: "+ mStencilSize+ ", fsaa: "+ mSamples);
        	        	
        	  int[] num_config = new int[1];
            egl.eglChooseConfig(display, s_configAttribsSimple, null, 0, num_config);

            int numConfigs = num_config[0];

            if (numConfigs <= 0) 
            {
                throw new IllegalArgumentException("No configs match configSpec");
            }
        		
            EGLConfig[] configs = new EGLConfig[numConfigs];
            egl.eglChooseConfig(display, s_configAttribsSimple, configs, numConfigs, num_config);
            
            EGLConfig configuration = chooseConfig(egl, display, configs);
            
						if(configuration == null)
           	{
						    if(mDepthSize > 24)
						    {
						        Log.w(TAG, "eglChooseConfig() requested EGL_DEPTH_SIZE not available. Fallbacking to EGL_DEPTH_SIZE = 24.\n");
						        mDepthSize = 24;
						        configuration = chooseConfig(egl, display, configs);
						    }
						    if(configuration == null && mDepthSize > 16)
						    {
						        Log.w(TAG, "eglChooseConfig() requested EGL_DEPTH_SIZE not available. Fallbacking to EGL_DEPTH_SIZE = 16.\n");
						        mDepthSize = 16;
						        configuration = chooseConfig(egl, display, configs);
						    }
          	}
            
            if(configuration == null)
            {
            	configuration = chooseConfigFallback(egl, display, configs);
            	mUsingFallback = true;
            	
              int r = findConfigAttrib(egl, display, configuration, EGL10.EGL_RED_SIZE, 0);
              int g = findConfigAttrib(egl, display, configuration, EGL10.EGL_GREEN_SIZE, 0);
              int b = findConfigAttrib(egl, display, configuration, EGL10.EGL_BLUE_SIZE, 0);
              int a = findConfigAttrib(egl, display, configuration, EGL10.EGL_ALPHA_SIZE, 0);            	
              int depth = findConfigAttrib(egl, display, configuration, EGL10.EGL_DEPTH_SIZE, 0);
              int stencil = findConfigAttrib(egl, display, configuration, EGL10.EGL_STENCIL_SIZE, 0);
            	int buffers = findConfigAttrib(egl, display, configuration, EGL10.EGL_SAMPLE_BUFFERS, 0);
							int samples = findConfigAttrib(egl, display, configuration, EGL10.EGL_SAMPLES, 0);

							String fallbackNote = "Could not use EGL configuration matching preset configuration.\nBenchmark results will not be valid.\n\nFallbacking to following configuration:\n";
							String configurationNote = "RGBA: "+ r +" "+ g +" "+ b +" "+ a +" \nDepth: "+ depth +" Stencil: "+ stencil +" \nSample buffers: "+ buffers +" Samples: " + samples;            	
							
            	Log.w(TAG, "eglChooseConfig() No config found. Fallbacking to RGBA: "+ r +" "+ g +" "+ b +" "+ a +" \nDepth: "+ depth +" Stencil: "+ stencil +" \nSample buffers: "+ buffers +" Samples: " + samples);            	
            	((onFallbackDetermined)mRenderer).onFallbackDetermined(fallbackNote + configurationNote);
            }
            
            return configuration;
        }
        

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs) 
        {
            for(EGLConfig config : configs) 
            {
                int depth = findConfigAttrib(egl, display, config, EGL10.EGL_DEPTH_SIZE, 0);
                int stencil = findConfigAttrib(egl, display, config, EGL10.EGL_STENCIL_SIZE, 0);

                if (depth != mDepthSize || stencil != mStencilSize)
                {
                		continue;
								}

								if(mSamples > 0)
								{
									int buffers = findConfigAttrib(egl, display, config, EGL10.EGL_SAMPLE_BUFFERS, 0);
									int samples = findConfigAttrib(egl, display, config, EGL10.EGL_SAMPLES, 0);
									if(buffers <= 0 || samples != mSamples)  
									{
										continue;										
									}
								}

                int r = findConfigAttrib(egl, display, config, EGL10.EGL_RED_SIZE, 0);
                int g = findConfigAttrib(egl, display, config, EGL10.EGL_GREEN_SIZE, 0);
                int b = findConfigAttrib(egl, display, config, EGL10.EGL_BLUE_SIZE, 0);
                int a = findConfigAttrib(egl, display, config, EGL10.EGL_ALPHA_SIZE, 0);

                if (r == mRedSize && g == mGreenSize && b == mBlueSize && a == mAlphaSize)
                {
                		printConfig(egl, display, config);
                	  return config;
                }
            }
            
            return null;
        }
        
        public EGLConfig chooseConfigFallback(EGL10 egl, EGLDisplay display, EGLConfig[] configs) 
        {
            for(EGLConfig config : configs) 
            {
                int r = findConfigAttrib(egl, display, config, EGL10.EGL_RED_SIZE, 0);
                int g = findConfigAttrib(egl, display, config, EGL10.EGL_GREEN_SIZE, 0);
                int b = findConfigAttrib(egl, display, config, EGL10.EGL_BLUE_SIZE, 0);

                if (r < 5 || g < 6 || b < 5)
									continue;
                    
                int depth = findConfigAttrib(egl, display, config, EGL10.EGL_DEPTH_SIZE, 0);
                    
                if (depth < 16)
                	continue;
                
            		return config;
          	}
          	return null;
        }


        private int findConfigAttrib(EGL10 egl, EGLDisplay display, EGLConfig config, int attribute, int defaultValue) 
        {
            if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) 
                return mValue[0];
            return defaultValue;
        }

        private void printConfigs(EGL10 egl, EGLDisplay display, EGLConfig[] configs) 
        {
            int numConfigs = configs.length;
            Log.w(TAG, String.format("%d configurations", numConfigs));
            for (int i = 0; i < numConfigs; i++) 
            {
                Log.w(TAG, String.format("Configuration %d:\n", i));
                printConfig(egl, display, configs[i]);
            }
        }

        private void printConfig(EGL10 egl, EGLDisplay display, EGLConfig config) 
        {
            int[] attributes = 
            {
                    EGL10.EGL_BUFFER_SIZE,
                    EGL10.EGL_ALPHA_SIZE,
                    EGL10.EGL_BLUE_SIZE,
                    EGL10.EGL_GREEN_SIZE,
                    EGL10.EGL_RED_SIZE,
                    EGL10.EGL_DEPTH_SIZE,
                    EGL10.EGL_STENCIL_SIZE,
                    EGL10.EGL_CONFIG_CAVEAT,
                    EGL10.EGL_CONFIG_ID,
                    EGL10.EGL_LEVEL,
                    EGL10.EGL_MAX_PBUFFER_HEIGHT,
                    EGL10.EGL_MAX_PBUFFER_PIXELS,
                    EGL10.EGL_MAX_PBUFFER_WIDTH,
                    EGL10.EGL_NATIVE_RENDERABLE,
                    EGL10.EGL_NATIVE_VISUAL_ID,
                    EGL10.EGL_NATIVE_VISUAL_TYPE,
                    0x3030, // EGL10.EGL_PRESERVED_RESOURCES,
                    EGL10.EGL_SAMPLES,
                    EGL10.EGL_SAMPLE_BUFFERS,
                    EGL10.EGL_SURFACE_TYPE,
                    EGL10.EGL_TRANSPARENT_TYPE,
                    EGL10.EGL_TRANSPARENT_RED_VALUE,
                    EGL10.EGL_TRANSPARENT_GREEN_VALUE,
                    EGL10.EGL_TRANSPARENT_BLUE_VALUE,
                    0x3039, // EGL10.EGL_BIND_TO_TEXTURE_RGB,
                    0x303A, // EGL10.EGL_BIND_TO_TEXTURE_RGBA,
                    0x303B, // EGL10.EGL_MIN_SWAP_INTERVAL,
                    0x303C, // EGL10.EGL_MAX_SWAP_INTERVAL,
                    EGL10.EGL_LUMINANCE_SIZE,
                    EGL10.EGL_ALPHA_MASK_SIZE,
                    EGL10.EGL_COLOR_BUFFER_TYPE,
                    EGL10.EGL_RENDERABLE_TYPE,
                    0x3042 // EGL10.EGL_CONFORMANT
            };
            String[] names = 
            {
                    "EGL_BUFFER_SIZE",
                    "EGL_ALPHA_SIZE",
                    "EGL_BLUE_SIZE",
                    "EGL_GREEN_SIZE",
                    "EGL_RED_SIZE",
                    "EGL_DEPTH_SIZE",
                    "EGL_STENCIL_SIZE",
                    "EGL_CONFIG_CAVEAT",
                    "EGL_CONFIG_ID",
                    "EGL_LEVEL",
                    "EGL_MAX_PBUFFER_HEIGHT",
                    "EGL_MAX_PBUFFER_PIXELS",
                    "EGL_MAX_PBUFFER_WIDTH",
                    "EGL_NATIVE_RENDERABLE",
                    "EGL_NATIVE_VISUAL_ID",
                    "EGL_NATIVE_VISUAL_TYPE",
                    "EGL_PRESERVED_RESOURCES",
                    "EGL_SAMPLES",
                    "EGL_SAMPLE_BUFFERS",
                    "EGL_SURFACE_TYPE",
                    "EGL_TRANSPARENT_TYPE",
                    "EGL_TRANSPARENT_RED_VALUE",
                    "EGL_TRANSPARENT_GREEN_VALUE",
                    "EGL_TRANSPARENT_BLUE_VALUE",
                    "EGL_BIND_TO_TEXTURE_RGB",
                    "EGL_BIND_TO_TEXTURE_RGBA",
                    "EGL_MIN_SWAP_INTERVAL",
                    "EGL_MAX_SWAP_INTERVAL",
                    "EGL_LUMINANCE_SIZE",
                    "EGL_ALPHA_MASK_SIZE",
                    "EGL_COLOR_BUFFER_TYPE",
                    "EGL_RENDERABLE_TYPE",
                    "EGL_CONFORMANT"
            };
            
            int[] value = new int[1];
            for (int i = 0; i < attributes.length; i++) 
            {
                int attribute = attributes[i];
                String name = names[i];
                if ( egl.eglGetConfigAttrib(display, config, attribute, value)) 
                {
                    Log.i(TAG, String.format("  %s: %d\n", name, value[0]));
                }
                else 
                {
                    while (egl.eglGetError() != EGL10.EGL_SUCCESS);
                }
            }
        }
		
				public boolean mUsingFallback;
        protected int mRedSize;
        protected int mGreenSize;
        protected int mBlueSize;
        protected int mAlphaSize;
        protected int mDepthSize;
        protected int mStencilSize;
        protected int mSamples;
        private int[] mValue = new int[1];
    }
    
    @Override protected void onFocusChanged(boolean gainFocus, int direction, Rect previouslyFocusedRect)    
    {
        KanziNativeLibrary.focusEvent(gainFocus);
    }
    
    @Override
    public boolean onKeyDown(final int keyCode, KeyEvent event) 
    {
        final int touchState;
        switch (event.getAction()) {
            case KeyEvent.ACTION_DOWN:
                touchState = 0;
                break;
            default:
            case KeyEvent.ACTION_UP:
                touchState = 1;
                break;
            case KeyEvent.ACTION_MULTIPLE:
                touchState = 2;
                break;
        }

        System.out.println("key"+keyCode);
        Log.v("INFO________: ", "key: " + keyCode + " - " +touchState);

        // The touch event is queued and will be forwarded to Kanzi before the next update
        queueEvent(new Runnable() 
        {
            public void run() {
                KanziNativeLibrary.keyEvent(keyCode, touchState);
            }
        });

        return super.onKeyDown(keyCode, event);
    }

    @Override public boolean onTrackballEvent(MotionEvent event)
    {
    	return super.onTrackballEvent(event);
    }

    @Override public boolean onTouchEvent(MotionEvent event)
    {
        final float touchX;
        final float touchY;
        final int cursorState;
        int action = event.getAction();

        if(action == MotionEvent.ACTION_DOWN)
        {
            cursorState = 0;
        }
        else if(action == MotionEvent.ACTION_MOVE)
        {
            cursorState = 2;
        }
        else if((action & MotionEvent.ACTION_POINTER_DOWN) == MotionEvent.ACTION_POINTER_DOWN)
        {
            cursorState = 0;
        }
        else if((action & MotionEvent.ACTION_POINTER_UP) == MotionEvent.ACTION_POINTER_UP)
        {
            cursorState = 1;
        }
        else /* Default. */
        {
            cursorState = 1;
        }

        touchX = event.getX(0);
        touchY = event.getY(0);

        // The touch event is queued and will be forwarded to Kanzi before the next update
        queueEvent(new Runnable()
        {
            public void run()
            {
                // 0 = down
                // 1 = up
                // 2 = drag
                KanziNativeLibrary.touchEvent((int)touchX, (int)touchY, cursorState);
            }
        });

        return true;
    }
    public boolean getUsingFallback()
    {
    		return configChooser.mUsingFallback;
    }    

    static private int touchX;
    static private int touchY;
    static private int touchState;
    static private boolean newTouchEvent;
}
