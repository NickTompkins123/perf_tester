// uniforms used by the fragment shader
uniform samplerCube cubeTex;
uniform vec4 eyePosW;

// input from vertex shader
varying vec4 v_PositionW;

void main(void)
{
	vec4 viewDir3 = normalize(eyePosW - v_PositionW);
	gl_FragColor = textureCube(cubeTex, viewDir3.xyz);
}