uniform mat4 u_MVMatrix;
uniform mat4 u_MVPMatrix;

uniform float u_pointRadius;
uniform float u_pointScale;

attribute vec4 a_Position;
attribute vec4 a_Color;

varying vec4 v_Color;

void main()
{
	gl_Position = u_MVPMatrix * a_Position;

	vec4 posEye = u_MVMatrix * a_Position;
    float dist = length(posEye.xyz);

	gl_PointSize = u_pointRadius * (u_pointScale / dist);
	
	v_Color = a_Color;
}