$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

// for ssao render pass, writes normals into texture
uniform float writeNormalsOnly;

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

// Light-Info: SUN
uniform vec4 lightPositionViewSpace_0;
uniform float lightIntensity_0;
uniform float lightAttenuation_0;
uniform float lightRadius_0;
uniform vec3 lightDiffuseColor_0;
uniform vec3 lightSpecularColor_0;
varying highp float intensityBasedOnDist_0;
uniform vec4 lightPos_World_0;

// Light-Info: TORCH
uniform vec4 lightPositionViewSpace_1;
uniform float lightIntensity_1;
uniform float lightAttenuation_1;
uniform float lightRadius_1;
uniform vec3 lightDiffuseColor_1;
uniform vec3 lightSpecularColor_1;
varying highp float intensityBasedOnDist_1;
uniform vec4 lightPos_World_1;

uniform vec3 viewPos;
uniform vec3 ambientColor;
uniform vec3 skyColor;
uniform vec3 fogColor;

varying mediump float visibility;

// World Space Coordinates
varying highp vec3 v_normal;
varying highp vec3 v_normal_viewspace;
varying highp vec4 v_position;
varying mediump vec3 v_tangent;
varying mediump vec3 v_bitangent;

// texture Coords and Color
varying lowp vec4 v_texCoord;
varying lowp vec4 v_shadowCoord;
varying lowp vec4 v_color;

// Torch Uniforms
uniform float torchDamper;
uniform vec3 torchDir;
uniform float torchInnerCutOff;
uniform float torchOuterCutOff;

mediump vec2 poissonDisk[4];
int pcfCount = 3;
int totalTexels = (pcfCount * 2 + 1) * (pcfCount * 2 + 1);

uniform float bloomPass;

float ShadowCalculation(vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 shadowCoords = v_shadowCoord.xyz/v_shadowCoord.w;
    
    float mapSize = 1024.0;
    float texelSize = 1.0/ mapSize;
    float total = 0.0;
    
    float lightIntensity = dot(normal, lightDir);
    lightIntensity = clamp(lightIntensity, 0.0, 1.0);
    float bias = 0.005*tan(acos(lightIntensity)); // cosTheta is dot( n,l ), clamped between 0 and 1
    bias = clamp(bias, 0.0, 0.01);

    for (int x = -pcfCount; x < pcfCount; x++) {
        for (int y=-pcfCount; y < pcfCount; y++) {
            float closestDepth = texture2D(shadowMap, shadowCoords.xy + vec2(x,y) * texelSize).r;
            if (shadowCoords.z - bias > closestDepth) {
                total += 1.0;
            }
        }
    }
    float totTex = float(totalTexels);
    total /= totTex;
    
    float shadow = 1.0 - (total);

    if (v_shadowCoord.w > 1.0) {
        shadow = 1.0;
    }
    
    if (abs(shadowCoords.x) > 1.0 ||
        abs(shadowCoords.y) > 1.0 ||
        abs(shadowCoords.z) > 1.0 )
        shadow = 1.0;

    return shadow + 0.6;
}

void main()
{
    // if in ssao render pass
    if (writeNormalsOnly > 0.0) {
        vec3 normal = normalize(v_normal_viewspace);
        normal = normal / 2.0 + 0.5;
        gl_FragColor = vec4(normal, 1.0);
    } else if(bloomPass > 0.0) {
        // In the bloom Pass we draw everything black, as terrain should not get the bloom effect.
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        vec4 diffuse = vec4(0.0,0.0,0.0,1.0);
        vec4 specular = vec4(0.0,0.0,0.0,1.0);
        float specularCoefficient = 0.0;
        
        vec4 position = v_position;
        vec3 normal = normalize(v_normal);
        vec3 lightVector_0 = normalize(vec3(lightPos_World_0) - vec3(position));
        vec3 lightVector_1 = normalize(vec3(lightPos_World_1) - vec3(position));
        vec3 surfaceToCamera = vec3(normalize(vec4(viewPos, 1.0) - position));
        
        float shadow = 1.0;
        // shadow-value
        shadow = ShadowCalculation(v_normal, lightVector_0);
        
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
            intensity *= coneInstensity * torchDamper;
            diffuse += vec4(lightDiffuseColor_1 * (intensity * intensityBasedOnDist_1), 0.0);
            specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-normalize(lightVector_1), normal))), Ns);
            specular += vec4(lightSpecularColor_1 * (specularCoefficient * intensity * intensityBasedOnDist_1), 0.0);
        }

        
        // ambient part
        vec4 ambient = vec4(ambientColor * Ks, 1.0);
        ambient = clamp(ambient, 0.0, 1.0);
        diffuse = diffuse * vec4(Kd,1.0);
        specular = specular  * vec4(Ks, 0.0);
        vec4 outColor = clamp((ambient+ shadow * diffuse) * v_color +specular, 0.0, 1.0);

        gl_FragColor = mix(vec4(vec3(fogColor), 1.0), outColor, visibility);
    }
}
