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
uniform float heightPercent;
uniform vec3 skyColor;
uniform vec3 playerPos;

uniform float sunIntensity;

uniform float fogDensity;
uniform float fogGradient;

attribute highp vec4 Position;
attribute vec3 Normal;
attribute vec3 Tangent;
attribute vec3 Bitangent;
attribute vec4 TexCoord;

varying mediump vec3 v_normal;
varying mediump vec3 v_nomal_viewspace;
varying mediump vec4 v_position;
varying mediump vec3 v_tangent;
varying mediump vec3 v_bitangent;

// texture Coords
varying lowp vec4 v_texCoord;

// Visibility for Fog
varying mediump float visibility;

void main()
{
    // if in ssao render pass
    if (writeNormalsOnly > 0.0) {
        v_nomal_viewspace = normalize(mat3(ModelViewMatrix) * (Normal * vec3(1.0, 1.0, -1.0))) * vec3(1.0, 1.0, -1.0);
    } else {
    v_normal = normalize(NormalMatrix * (Normal * vec3(1.0, 1.0, -1.0)));
    v_tangent = normalize(NormalMatrix * Tangent);
    v_bitangent = normalize(NormalMatrix * Bitangent);
    v_position = ModelMatrix * Position;
    v_texCoord = TexCoord;
    
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
    }
    
    // Position of Vertex
    gl_Position = ProjectionMatrix * ModelViewMatrix * Position;

}

