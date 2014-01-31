// uniforms used by the fragment shader
uniform samplerCube cubeTex;
uniform vec4 skyColor;

// input from vertex shader
varying vec4 v_PositionW;
varying vec4 v_NormalW;

void main(void)
{
	gl_FragColor = 0.75 * (vec4(0.07, 0.15, 0.2, 1.0) + vec4(textureCube(cubeTex, v_NormalW.xyz).xyz, 0.0));
}