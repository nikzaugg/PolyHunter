$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

// for ssao render pass, writes normals into texture
uniform float writeNormalsOnly;

uniform mat4 depthMVP;
uniform mat4 depthView;
uniform mat4 depthProjection;
uniform mat4 depthOffset;
uniform sampler2D shadowMap;

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
uniform vec3 skyColor;
uniform vec3 playerPos;
uniform vec3 fogColor;
uniform float fogDensity;
uniform float fogGradient;

attribute highp vec4 Position;
attribute vec3 Normal;
attribute vec3 Tangent;
attribute vec3 Bitangent;
attribute vec4 TexCoord;

varying lowp vec4 shadowCoord_varying;
varying lowp vec4 vertexColor_varying;
varying lowp vec4 texCoord_varying;
// Everything in View Space
varying mediump vec4 position_varying_ViewSpace;
varying mediump vec3 normal_varying_ViewSpace;
varying mediump vec3 tangent_varying_ViewSpace;
// WorldSpace
varying mediump vec3 normal_varying_WorldSpace;

varying mediump float visibility;

void main()
{
    vec4 posViewSpace = ModelViewMatrix * Position;
    
    // if in ssao render pass
    if (writeNormalsOnly > 0.0) {
        normal_varying_ViewSpace = normalize(mat3(ModelViewMatrix) * (Normal * vec3(1.0, 1.0, -1.0))) * vec3(1.0, 1.0, -1.0);
    } else {
        // Need to flip z-coord of Normal
        vec3 normal_ViewSpace = normalize(mat3(ModelViewMatrix) * (Normal * vec3(1.0, 1.0, -1.0)));
        vec3 tangent_ViewSpace = mat3(ModelViewMatrix) * Tangent;
        vec3 bitangent_ViewSpace = mat3(ModelViewMatrix) * Bitangent;
        vec3 posRelativeToPlayer = playerPos - vec3(posViewSpace);
        
        // Outputs to Fragment Shader
        normal_varying_ViewSpace = normal_ViewSpace;
        tangent_varying_ViewSpace = tangent_ViewSpace;
        texCoord_varying = TexCoord;
        normal_varying_WorldSpace = normalize(mat3(ModelMatrix) * (Normal * vec3(1.0, 1.0, -1.0)));
        
        float dist = length(posRelativeToPlayer.xyz);
        
        visibility = exp(-pow((dist * fogDensity), fogGradient));
    }
    
    position_varying_ViewSpace = posViewSpace;
    // Position of Vertex
    gl_Position = ProjectionMatrix*posViewSpace;
}

