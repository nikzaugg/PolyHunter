$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

attribute vec4 Position;
attribute vec4 TexCoord;

varying vec4 texCoordVarying;
varying vec4 posViewSpace;

void main()
{
    texCoordVarying = TexCoord;
    vec4 posView = ModelViewMatrix * Position;
    gl_Position = ProjectionMatrix * posView;
}
