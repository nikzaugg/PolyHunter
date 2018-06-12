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

uniform vec3 viewPos;
uniform vec3 ambientColor;
uniform float lightIntensity_0;
uniform vec3 lightDiffuseColor_0;
uniform vec3 lightSpecularColor_0;
uniform vec4 lightPositionViewSpace_0;
uniform vec4 lighPos_World_0;

uniform vec3 skyColor;
uniform vec3 fogColor;

varying mediump float visibility;

// World Space Coordinates
varying highp vec3 v_normal;
varying highp vec4 v_position;
varying mediump vec3 v_tangent;
varying mediump vec3 v_bitangent;

// texture Coords and Color
varying lowp vec4 v_texCoord;
varying lowp vec4 v_shadowCoord;
varying lowp vec4 v_color;

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

    return shadow;
}

void main()
{
    // In the bloom Pass we draw everything black, as terrain should not get the bloom effect.
    if (bloomPass > 0.0) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        
    } else {
        vec4 position = v_position;
        vec3 normal = normalize(v_normal);
        vec3 lightVector = normalize(vec3(lighPos_World_0) - vec3(position));
        
        // ambient part
        vec4 ambientPart = vec4(ambientColor * Ks * lightIntensity_0, 1.0);
        ambientPart = clamp(ambientPart, 0.0, 1.0);
        
        // diffuse part
        float intensityFactor = clamp(dot(normal, lightVector), 0.0, 1.0);
        vec3 diffuseTerm = Kd * intensityFactor *  lightDiffuseColor_0;
        vec4 diffusePart = vec4(clamp(diffuseTerm, 0.0, 1.0), 1.0);
        
        // specular part
        vec4 specular = vec4(0.0);
        if (intensityFactor > 0.0) {
            vec3 viewDir = vec3(normalize(vec4(viewPos, 1.0) - position));
            vec3 reflectDir = -normalize(reflect(lightVector, normal));
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), Ns);
            spec = clamp(spec, 0.0, 1.0);
            specular = clamp(vec4(Ks * spec, 1.0), 0.0, 1.0);
        }
        
        // shadow-value
        float shadow = ShadowCalculation(v_normal, lightVector);
        vec4 totalDiffuse = diffusePart * shadow;
        
        vec4 outColor = (ambientPart + totalDiffuse) * v_color + specular;
        gl_FragColor = mix(vec4(vec3(fogColor), 1.0), outColor, visibility);

        
    }
}
