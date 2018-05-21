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

uniform vec3 ambientColor;
uniform float lightIntensity_0;
uniform vec3 lightDiffuseColor_0;
uniform vec3 lightSpecularColor_0;
uniform vec4 lightPositionViewSpace_0;

varying lowp vec4 shadowCoord_varying;
varying lowp vec4 vertexColor_varying;
varying lowp vec4 texCoord_varying;
varying mediump vec3 normal_ModelSpace;
// Everything in View Space
varying mediump vec4 position_varying_ViewSpace;
varying mediump vec3 normal_varying_ViewSpace;
varying mediump vec3 tangent_varying_ViewSpace;

void main()
{
    float bias = 0.005;

    float objectNearestLight = texture2D(shadowMap, shadowCoord_varying.xy).r;
    float lightFactor = 1.0;
    if (shadowCoord_varying.z - bias > objectNearestLight) {
        lightFactor = 1.0 - 0.4;
    }
    
    vec4 position = position_varying_ViewSpace;
    vec3 normal = normalize(normal_varying_ViewSpace);
    vec3 normal_modelSpace = normalize(normal_ModelSpace);
    vec4 lightPosition = lightPositionViewSpace_0;
    vec4 lightVector = normalize(lightPosition - position);
    
    // ambient part
    vec4 ambientPart = vec4(ambientColor * lightIntensity_0, 1.0);
    ambientPart = clamp(ambientPart, 0.0, 1.0);
    
    // diffuse part
    float intensityFactor = dot(normal, lightVector.xyz);
    vec3 diffuseTerm = Kd * clamp(intensityFactor, 0.0, 1.0) * lightDiffuseColor_0;
    vec4 diffusePart = vec4(clamp(diffuseTerm, 0.0, 1.0), 1.0);
    
    diffusePart = diffusePart * lightFactor;
    
    gl_FragColor = (ambientPart + diffusePart) * vertexColor_varying;
    
    // gl_FragColor = vec4(vec3(objectNearestLight), 1.0);
    
    // Color according to normals
    // vec3 normal_test = normal/2.0 + vec3(0.5);
    // gl_FragColor = vec4(normal_test, 1.0);
    
    // Color accordin to model space normals
    //gl_FragColor = vec4(normal_modelSpace, 1.0) * vec4(1.0);
}
