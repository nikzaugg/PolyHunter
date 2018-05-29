$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mediump mat4 ViewMatrix;
uniform mediump mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

uniform lowp vec3 Ka;   // ambient material coefficient
uniform lowp vec3 Kd;   // diffuse material coefficient
uniform lowp vec3 Ks;   // specular material coefficient

uniform mediump float Ns;   // specular material exponent (shininess)

uniform sampler2D DiffuseMap;

uniform vec3 ambientColor;
uniform float lightIntensity_0;
uniform vec3 lightDiffuseColor_0;
uniform vec3 lightSpecularColor_0;
uniform vec4 lightPositionViewSpace_0;

uniform vec3 skyColor;

varying lowp vec4 vertexColor_varying;
varying lowp vec4 texCoord_varying;
// Everything in View Space
varying mediump vec4 position_varying_ViewSpace;
varying mediump vec3 normal_varying_ViewSpace;
varying mediump vec3 tangent_varying_ViewSpace;
varying mediump vec4 position_varying;

varying mediump float visibility;

float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}


float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

#define OCTAVES 6
float fbm (in vec2 st, in vec2 shift) {
    // Initial values
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.1;
    //
    // Loop of octaves
    for (int i = 0; i < OCTAVES; i++) {
        value += amplitude * noise(st);
        st *= 2. + shift;
        amplitude *= .5;
    }
    return value;
}

void main()
{
    vec4 position = position_varying_ViewSpace;
    vec3 normal = normalize(normal_varying_ViewSpace);
    vec4 lightPosition = lightPositionViewSpace_0;
    vec4 lightVector = normalize(lightPosition - position);
    
    // ambient part
    vec4 ambientPart = vec4(ambientColor * lightIntensity_0, 1.0);
    
    // diffuse part
    float intensityFactor = dot(normal, lightVector.xyz);
    vec3 diffuseTerm = Kd * clamp(intensityFactor, 0.0, 1.0) * lightDiffuseColor_0;
    vec4 diffusePart = vec4(clamp(diffuseTerm, 0.0, 1.0), 1.0);
    
    vec4 color = texture2D(DiffuseMap, texCoord_varying.st);

    vec4 outColor = (ambientPart + diffusePart) * color;
    vec4 lightColor = vec4(1.0, 0.7, 1.0, 1.0);

    float noiseValue = (fbm(texCoord_varying.st, vec2(100)) +  fbm(texCoord_varying.st, vec2(75)) +  fbm(texCoord_varying.st, vec2(50)) + fbm(texCoord_varying.st, vec2(25))) / 4;
    
    if (noiseValue > 0.5) {
        color = vec4(0.96, 0.95, 0.26, 1.0);
    }
    
    
	gl_FragColor = color;
    // gl_FragColor = vec4((vec3(texCoord_varying.st, 0.0) + vec3(1.0)) * 0.5, 1.0);
    // Color according to normals
//     vec3 normal_test = normal/2.0 + vec3(0.5);
//     gl_FragColor = vec4(normal_test, 1.0);
}

