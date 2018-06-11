$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform highp mat4 depthMVP;
uniform highp mat4 depthView;
uniform highp mat4 depthProjection;
uniform highp mat4 depthOffset;
uniform sampler2D shadowMap;

uniform highp mat4 InverseViewMatrix;
uniform highp mat4 ViewMatrix;
uniform highp mat4 ModelMatrix;
uniform highp mat4 ModelViewMatrix;
uniform highp mat4 ProjectionMatrix;
uniform highp mat3 NormalMatrix;

uniform lowp vec3 Ka;   // ambient material coefficient
uniform lowp vec3 Kd;   // diffuse material coefficient
uniform lowp vec3 Ks;   // specular material coefficient

uniform mediump float Ns;   // specular material exponent (shininess)

uniform vec3 ambientColor;
uniform float lightIntensity_0;
uniform vec3 lightDiffuseColor_0;
uniform vec3 lightSpecularColor_0;
uniform vec4 lightPositionViewSpace_0;

uniform vec4 lightPositionWorldSpace_0;

uniform float amplitude;
uniform vec3 skyColor;
uniform vec3 playerPos;

uniform float fogGradient;
uniform float fogDensity;

attribute highp vec4 Position;
attribute vec3 Normal;
attribute vec3 Tangent;
attribute vec3 Bitangent;
attribute vec4 TexCoord;

varying lowp vec4 shadowCoord_varying;
varying lowp vec4 vertexColor_varying;
varying lowp vec4 texCoord_varying;

// World Space
varying mediump vec3 normal_varying_WorldSpace;
varying mediump vec4 position_varying_WorldSpace;

// Everything in View Space
varying mediump vec4 position_varying_ViewSpace;
varying mediump vec3 normal_varying_ViewSpace;
varying mediump vec3 tangent_varying_ViewSpace;

// Shadow variables
uniform float shadowDistance;

varying mediump float visibility;

vec4 biome()
{
    // Slope doesn't work well with split terrain
    mediump float slope = 1.0 - normal_varying_ViewSpace.y;
	mediump float normHeight = Position.y / amplitude;

    if (normHeight > 0.8)
    {
        return vec4(0.36, 0.37, 0.36, 1.0);
    }
    else if (normHeight > 0.5)
    {
        return vec4(0.48, 0.32, 0.19, 1.0);
    }
    return vec4(0.34, 0.58, 0.19, 1.0);
}

void main()
{
    vec3 normal_WorldSpace = normalize(NormalMatrix * (Normal * vec3(-1.0, -1.0, 1.0)));
    vec4 position_WorldSpace = ModelMatrix * Position;
    vec3 normal_ViewSpace = normalize(mat3(ModelViewMatrix) * (Normal * vec3(-1.0, -1.0, 1.0)));
    vec3 tangent_ViewSpace = mat3(ModelViewMatrix) * Tangent;
    vec3 bitangent_ViewSpace = mat3(ModelViewMatrix) * Bitangent;
	vec4 posViewSpace = ModelViewMatrix * Position;
    vec3 posRelativeToPlayer = playerPos - vec3(posViewSpace);
    
    // Outputs to Fragment Shader
    shadowCoord_varying = depthOffset * depthProjection  * depthView * Position;
    normal_varying_ViewSpace = normal_ViewSpace;
    tangent_varying_ViewSpace = tangent_ViewSpace;
    position_varying_ViewSpace = posViewSpace;
    texCoord_varying = TexCoord;
    vertexColor_varying = biome();
    normal_varying_WorldSpace = normal_WorldSpace;
    position_varying_WorldSpace = position_WorldSpace;
    
    float dist = length(posRelativeToPlayer.xyz);
    visibility = exp(-pow((dist * fogDensity), fogGradient));
    
    // Position of Vertex
    gl_Position = ProjectionMatrix * ModelViewMatrix * Position;
}
