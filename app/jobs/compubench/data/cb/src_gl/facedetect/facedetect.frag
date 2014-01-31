#ifdef HAVE_EGL
	precision mediump float;
#endif

uniform sampler2D texBase;
varying vec2 v_TexCoord;

void main()
{
	gl_FragColor = texture2D(texBase, v_TexCoord);
}
