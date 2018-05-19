$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 lightSpaceMatrix;
uniform mat4 ModelMatrix;

attribute vec4 Position;
attribute vec4 TexCoord;

varying vec4 texCoordVarying;

void main()
{
    texCoordVarying = TexCoord;
    // gl_Position = lightSpaceMatrix * ModelMatrix * Position;
    gl_Position = ProjectionMatrix * ModelViewMatrix * Position;
}
