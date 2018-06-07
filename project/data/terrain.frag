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

uniform vec3 ambientColor;
uniform float lightIntensity_0;
uniform vec3 lightDiffuseColor_0;
uniform vec3 lightSpecularColor_0;
uniform vec4 lightPositionViewSpace_0;
uniform vec4 lightPositionWorldSpace_0;

varying lowp vec4 shadowCoord_varying;

uniform vec3 skyColor;
uniform vec3 fogColor;

varying mediump float visibility;

varying lowp vec4 vertexColor_varying;
varying lowp vec4 texCoord_varying;
varying mediump vec3 normal_varying_WorldSpace;
varying mediump vec4 position_varying_WorldSpace;
// Everything in View Space
varying mediump vec4 position_varying_ViewSpace;
varying mediump vec3 normal_varying_ViewSpace;
varying mediump vec3 tangent_varying_ViewSpace;

mediump vec2 poissonDisk[4];
int pcfCount = 3;
int totalTexels = (pcfCount * 2 + 1) * (pcfCount * 2 + 1);

uniform float bloomPass;

float ShadowCalculation(vec3 normal, vec4 lightDir)
{
    // perform perspective divide
    vec3 shadowCoords = shadowCoord_varying.xyz/shadowCoord_varying.w;
    
    float mapSize = 1024.0;
    float texelSize = 1.0/ mapSize;
    float total = 0.0;
    
    float lightIntensity = dot(normal, lightDir.xyz);
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

    if (shadowCoord_varying.w > 1.0) {
        shadow = 1.0;
    }
    
    if (abs(shadowCoords.x) > 1.0 ||
        abs(shadowCoords.y) > 1.0 ||
        abs(shadowCoords.z) > 1.0 )
        shadow = 1.0;

    return shadow;
}

void main()
{
    // if in ssao render pass
    if (writeNormalsOnly > 0.0) {
        vec3 normal = normalize(normal_varying_ViewSpace);
        normal = normal / 2.0 + 0.5;
        gl_FragColor = vec4(normal, 1.0);
    } else if(bloomPass > 0.0) {
        // In the bloom Pass we draw everything black, as terrain should not get the bloom effect.
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        vec4 position = position_varying_ViewSpace;
        vec4 position_world = position_varying_WorldSpace;
        vec3 normal = normalize(normal_varying_ViewSpace);
        vec3 normal_world = normalize(normal_varying_WorldSpace);
        vec4 lightPosition = normalize(lightPositionViewSpace_0);
        vec4 lightVector = normalize(lightPosition - position);
        vec4 lightPosition_world = normalize(lightPositionWorldSpace_0);
        vec4 lightVector_world = normalize(lightPosition_world - position_world);
        
        
        // ambient part
        vec4 ambientPart = vec4(ambientColor * lightIntensity_0, 1.0);
        ambientPart = clamp(ambientPart, 0.0, 1.0);
        
        // diffuse part
        float intensityFactor = dot(normal, lightVector.xyz);
        vec3 diffuseTerm = Kd * clamp(intensityFactor, 0.0, 1.0) * lightDiffuseColor_0;
        vec4 diffusePart = vec4(clamp(diffuseTerm, 0.0, 1.0), 1.0);
        
        // shadow-value
        float shadow = ShadowCalculation(normal_world, lightVector_world);
        
        vec4 totalDiffuse = diffusePart * shadow;
        // gl_FragColor = (ambientPart + diffusePart) * vertexColor_varying;
        
        vec4 outColor = (ambientPart + totalDiffuse) * vertexColor_varying;
//        gl_FragColor = mix(vec4(vec3(fogColor), 1.0), outColor, visibility);
        
        // gl_FragColor = vec4(visibility);
        // Color according to normals
        // vec3 normal_test = normal/2.0 + vec3(0.5);
        gl_FragColor = vec4(normal, 1.0);
    }
}
