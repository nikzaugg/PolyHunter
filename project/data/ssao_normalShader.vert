$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mat3 NormalMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

attribute highp vec4 Position;
attribute vec3 Normal;
attribute vec3 Tangent;
attribute vec3 Bitangent;
attribute vec4 TexCoord;

uniform float flipNormal;

varying mediump vec3 v_normal;

void main()
{
    vec4 posViewSpace = ModelViewMatrix * Position;
    
    // NormalMatrix is the inverse transpose of the ModelViewMatrix
    if (flipNormal > 0.0) {
        // flip for terrain vec3(-1.0, -1.0, 1.0)
        v_normal = mat3(ModelViewMatrix) * (Normal * vec3(-1.0, -1.0, 1.0));
    } else {
        // basic
        v_normal = mat3(ModelViewMatrix) * (Normal * vec3(1.0, 1.0, -1.0));
    }

    // Position of Vertex
    gl_Position = ProjectionMatrix * ModelViewMatrix * Position;
    
    
}

