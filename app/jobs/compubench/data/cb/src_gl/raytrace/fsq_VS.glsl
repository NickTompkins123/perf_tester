attribute vec3 in_Position;
attribute vec2 in_TexCoord;

varying vec2 v_TexCoord;
 
void main()
{
    gl_Position = vec4(in_Position, 1.0);
    v_TexCoord = in_TexCoord;
}