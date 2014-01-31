#ifdef HAVE_EGL
	precision mediump float;
#endif

varying vec4 v_Color;

void main()
{
   gl_FragColor = v_Color;
}