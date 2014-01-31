// uniforms used by the vertex shader
uniform mat4 VPMatInv;

// attributes input to the vertex shader
attribute vec4 a_Position0;

// varying variables – input to the fragment shader
varying vec4 v_PositionW;
 
void main()
{
    gl_Position = a_Position0;

	vec4 posW = VPMatInv * a_Position0;
    v_PositionW = posW / posW.w;
}