$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

attribute highp vec4 Position;
attribute vec3 Normal;
attribute vec3 Tangent;
attribute vec3 Bitangent;
attribute vec4 TexCoord;

varying highp vec4 fragPos;

void main()
{
    vec4 posViewSpace = ModelViewMatrix * Position;
    fragPos = ModelViewMatrix * Position;
    // Position of Vertex
    gl_Position = ProjectionMatrix * ModelViewMatrix * Position;
}

