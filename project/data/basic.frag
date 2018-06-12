$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4 depthMVP;
uniform mat4 depthView;
uniform mat4 depthProjection;
uniform sampler2D shadowMap;

uniform mediump mat4 ViewMatrix;
uniform mediump mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

uniform lowp vec3 Ka;   // ambient material coefficient
uniform lowp vec3 Kd;   // diffuse material coefficient
uniform lowp vec3 Ks;   // specular material coefficient

uniform mediump float Ns;   // specular material exponent (shininess)

uniform sampler2D DiffuseMap;

// Light-Info: SUN
uniform vec4 lightPositionViewSpace_0;
uniform vec4 lightPos_World_0;
uniform float lightIntensity_0;
uniform float lightAttenuation_0;
uniform float lightRadius_0;
uniform vec3 lightDiffuseColor_0;
uniform vec3 lightSpecularColor_0;
varying float intensityBasedOnDist_0;

// Light-Info: TORCH
uniform vec4 lightPositionViewSpace_1;
uniform vec4 lightPos_World_1;
uniform float lightIntensity_1;
uniform float lightAttenuation_1;
uniform float lightRadius_1;
uniform vec3 lightDiffuseColor_1;
uniform vec3 lightSpecularColor_1;
varying float intensityBasedOnDist_1;

uniform vec3 viewPos;
uniform vec3 ambientColor;

uniform vec3 skyColor;
uniform vec3 fogColor;

// World Space Coordinates
varying mediump vec3 v_normal;
varying mediump vec4 v_position;
varying mediump vec3 v_tangent;
varying mediump vec3 v_bitangent;

// texture Coords and Color
varying lowp vec4 v_texCoord;

// Visibility for Fog
varying mediump float visibility;

// Torch Uniforms
uniform vec3 torchDir;
uniform float torchInnerCutOff;
uniform float torchOuterCutOff;

void main()
{
    vec4 diffuse = vec4(0.0,0.0,0.0,1.0);
    vec4 specular = vec4(0.0,0.0,0.0,1.0);
    float specularCoefficient = 0.0;
    
    vec4 position = v_position;
    vec3 normal = normalize(v_normal);
    vec3 lightVector_0 = normalize(vec3(lightPos_World_0) - vec3(position));
    vec3 lightVector_1 = normalize(vec3(lightPos_World_1) - vec3(position));
    vec3 surfaceToCamera = vec3(normalize(vec4(viewPos, 1.0) - position));
    
    // SUN-LIGHT
    float intensity = 0.0;
    if (intensityBasedOnDist_0 > 0.0 && (intensity = max(dot(normal, normalize(lightVector_0)), 0.0)) > 0.0){
        intensity = clamp(intensity, 0.0, 1.0);
        diffuse += vec4(lightDiffuseColor_0 * (intensity * intensityBasedOnDist_0), 0.0);
        specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-normalize(lightVector_0), normal))), Ns);
        specular += vec4(lightSpecularColor_0 * (specularCoefficient * intensity * intensityBasedOnDist_0), 0.0);
    }
    
    // TORCH-LIGHT
    if (intensityBasedOnDist_1 > 0.0 && (intensity = max(dot(normal, normalize(lightVector_1)), 0.0)) > 0.0){
        float theta     = dot(lightVector_1, normalize(-torchDir));
        float epsilon   = torchInnerCutOff - torchOuterCutOff;
        float coneInstensity = clamp((theta - torchOuterCutOff) / epsilon, 0.0, 1.0);
        
        intensity = clamp(intensity, 0.0, 1.0);
        intensity *= coneInstensity;
        diffuse += vec4(lightDiffuseColor_1 * (intensity * intensityBasedOnDist_1), 0.0);
        specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-normalize(lightVector_1), normal))), Ns);
        specular += vec4(lightSpecularColor_1 * (specularCoefficient * intensity * intensityBasedOnDist_1), 0.0);
    }

    vec4 color = texture2D(DiffuseMap, v_texCoord.st);
    vec4 ambient = vec4(ambientColor * Ks, 1.0);
    ambient = clamp(ambient, 0.0, 1.0);
    diffuse = diffuse * vec4(Kd,1.0);
    specular = specular  * vec4(Ks, 0.0);
    vec4 outColor = clamp(ambient+diffuse+specular, 0.0, 1.0) * color;
    gl_FragColor = mix(vec4(vec3(fogColor), 1.0), outColor, visibility);
}

