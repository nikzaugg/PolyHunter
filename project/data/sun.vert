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

attribute highp vec4 Position;
attribute vec3 Normal;
attribute vec3 Tangent;
attribute vec3 Bitangent;
attribute vec4 TexCoord;

// World Space Coordinates
varying mediump vec3 v_normal;
varying mediump vec3 v_normal_model;
varying highp vec4 v_position;
varying highp vec4 v_position_model;
varying mediump vec3 v_tangent;
varying mediump vec3 v_bitangent;

// texture Coords
varying lowp vec4 v_texCoord;

void main()
{
    v_normal = normalize(NormalMatrix * (Normal * vec3(1.0, 1.0, -1.0)));
    v_normal_model = Normal * vec3(1.0, 1.0, -1.0);
    v_tangent = normalize(NormalMatrix * Tangent);
    v_bitangent = normalize(NormalMatrix * Bitangent);
    v_position = ModelMatrix * Position;
    v_position_model = Position;
    v_texCoord = TexCoord;

    // Position of Vertex
    gl_Position = ProjectionMatrix*ModelViewMatrix*Position;
}


