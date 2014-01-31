#ifdef HAVE_EGL
	precision mediump float;
#endif

uniform sampler2D tex;
varying vec2 v_TexCoord;

void main()
{
	gl_FragColor = texture2D(tex, v_TexCoord);
}
