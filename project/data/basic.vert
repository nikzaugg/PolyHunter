$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mediump mat4 ViewMatrix;
uniform mediump mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;

uniform lowp vec3 Ka;   // ambient material coefficient
uniform lowp vec3 Kd;   // diffuse material coefficient
uniform lowp vec3 Ks;   // specular material coefficient

uniform mediump float Ns;   // specular material exponent (shininess)

uniform vec3 ambientColor;
uniform float lightIntensity_0;
uniform vec3 lightDiffuseColor_0;
uniform vec3 lightSpecularColor_0;
uniform vec4 lightPositionViewSpace_0;

uniform float amplitude;
uniform float heightPercent;

attribute vec4 Position;
attribute vec3 Normal;
attribute vec3 Tangent;
attribute vec3 Bitangent;
attribute vec4 TexCoord;

varying lowp vec4 vertexColor_varying;
varying lowp vec4 texCoord_varying;
// Everything in View Space
varying mediump vec4 position_varying_ViewSpace;
varying mediump vec3 normal_varying_ViewSpace;
varying mediump vec3 tangent_varying_ViewSpace;

void main()
{
    /*READ THIS*/
    // Need to flip z-coord of Normal
    vec3 normal_ViewSpace = mat3(ModelViewMatrix) * (Normal * vec3(1.0, 1.0, -1.0));
    vec3 tangent_ViewSpace = mat3(ModelViewMatrix) * Tangent;
    vec3 bitangent_ViewSpace = mat3(ModelViewMatrix) * Bitangent;
    vec4 posViewSpace = ModelViewMatrix * Position;
    
    // Outputs to Fragment Shader
    normal_varying_ViewSpace = normal_ViewSpace;
    tangent_varying_ViewSpace = tangent_ViewSpace;
    position_varying_ViewSpace = posViewSpace;
    texCoord_varying = TexCoord;
    
    // Position of Vertex
    gl_Position = ProjectionMatrix*posViewSpace;
}

