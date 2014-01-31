// uniforms used by the vertex shader
uniform mat4 MMat;
uniform mat4 MVPMat;

// attributes input to the vertex shader
attribute vec4 a_Position0;
attribute vec4 a_Normal0;

// varying variables – input to the fragment shader
varying vec4 v_PositionW;
varying vec4 v_NormalW;
 
void main()
{
    gl_Position = MVPMat * a_Position0;

	v_PositionW = MMat * a_Position0;
    v_NormalW = MMat * a_Normal0;
}