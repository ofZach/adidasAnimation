#version 120

varying vec2 texCoordVarying;

void main()
{
    texCoordVarying = gl_MultiTexCoord0.xy;
    vec4 position = ftransform();
    gl_Position = position;
}