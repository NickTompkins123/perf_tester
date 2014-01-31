#ifdef GL_ES
precision mediump float;
#endif

uniform float blendFactor;
uniform sampler2D tex;

varying vec2 v_TexCoord;

void main(void)
{
	gl_FragColor = texture2D(tex, v_TexCoord) * blendFactor;
}