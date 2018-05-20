$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

attribute vec4 Position;
attribute vec4 TexCoord;

varying vec4 texCoordVarying;

void main()
{
    texCoordVarying = TexCoord;
    vec4 posViewSpace = ModelViewMatrix * Position;
    gl_Position = ProjectionMatrix*posViewSpace;
}
