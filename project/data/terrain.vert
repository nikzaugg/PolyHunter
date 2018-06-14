$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform highp mat4 depthMVP;
uniform highp mat4 depthView;
uniform highp mat4 depthProjection;
uniform highp mat4 depthOffset;
uniform sampler2D shadowMap;

uniform highp mat4 ModelMatrix;
uniform highp mat4 ViewMatrix;
uniform highp mat4 ProjectionMatrix;
uniform highp mat4 ModelViewMatrix;
uniform highp mat3 NormalMatrix;

uniform lowp vec3 Ka;   // ambient material coefficient
uniform lowp vec3 Kd;   // diffuse material coefficient
uniform lowp vec3 Ks;   // specular material coefficient

uniform mediump float Ns;   // specular material exponent (shininess)

// Light-Info: SUN
uniform vec4 lightPositionViewSpace_0;
uniform vec4 lightPos_World_0;
uniform float lightIntensity_0;
uniform float lightAttenuation_0;
uniform float lightRadius_0;
uniform vec3 lightDiffuseColor_0;
uniform vec3 lightSpecularColor_0;
varying highp float intensityBasedOnDist_0;

// Light-Info: TORCH
uniform vec4 lightPositionViewSpace_1;
uniform vec4 lightPos_World_1;
uniform float lightIntensity_1;
uniform float lightAttenuation_1;
uniform float lightRadius_1;
uniform vec3 lightDiffuseColor_1;
uniform vec3 lightSpecularColor_1;
varying highp float intensityBasedOnDist_1;

uniform vec3 viewPos;
uniform vec3 ambientColor;

uniform float amplitude;
uniform vec3 skyColor;
uniform vec3 playerPos;

uniform float sunIntensity;

uniform float fogGradient;
uniform float fogDensity;

attribute highp vec4 Position;
attribute vec3 Normal;
attribute vec3 Tangent;
attribute vec3 Bitangent;
attribute vec4 TexCoord;

// Shadow variables
uniform float shadowDistance;

// World Space Coordinates
varying highp vec3 v_normal;
varying highp vec4 v_position;
varying mediump vec3 v_tangent;
varying mediump vec3 v_bitangent;

// texture Coords and Color
varying lowp vec4 v_texCoord;
varying lowp vec4 v_shadowCoord;
varying lowp vec4 v_color;

// Visibility for Fog
varying mediump float visibility;

vec4 biome()
{
    // Slope doesn't work well with split terrain
    mediump float slope = 1.0 - v_normal.y;
	mediump float normHeight = Position.y / amplitude;

    if (normHeight > 0.8)
    {
        return vec4(0.36, 0.37, 0.36, 1.0);
    }
    else if (normHeight > 0.5)
    {
        return vec4(0.48, 0.32, 0.19, 1.0);
    } 
    else if (normHeight > 0.4)
    {
        return vec4(0.44, 0.7, 0.25, 1.0);
    }
    else if (normHeight > 0.3)
    {
        return vec4(0.34, 0.58, 0.19, 1.0);
    }
    else if (normHeight > 0.2)
    {
        return vec4(0.12, 0.44, 0.12, 1.0);
    }
    return vec4(0.07, 0.15, 0.04, 1.0);
   
}

void main()
{
    v_normal = normalize(NormalMatrix * (Normal * vec3(-1.0, -1.0, 1.0)));
    v_tangent = normalize(NormalMatrix * Tangent);
    v_bitangent = normalize(NormalMatrix * Bitangent);
    v_position = ModelMatrix * Position;
    v_texCoord = TexCoord;
    v_shadowCoord = depthOffset * depthProjection * depthView * Position;
    v_color = biome();
    
    vec3 posRelativeToPlayer = playerPos - vec3(v_position);
    float dist = length(posRelativeToPlayer.xyz);
    
    visibility = exp(-pow((dist * fogDensity), fogGradient));
    
    float lightDistance = 0.0;
    lightDistance = distance(v_position, lightPos_World_0);
    intensityBasedOnDist_0 = 0.0;
    if (lightDistance <= lightRadius_0) {
        intensityBasedOnDist_0 = sunIntensity;
    };
    
    lightDistance = distance(v_position, lightPos_World_1);
    intensityBasedOnDist_1 = 0.0;
    if (lightDistance <= lightRadius_1) {
        intensityBasedOnDist_1 = clamp(lightIntensity_1 / (lightAttenuation_1*lightDistance*lightDistance), 0.0, 1.0);
    };

    // Position of Vertex
    gl_Position = ProjectionMatrix * ModelViewMatrix * Position;
}
