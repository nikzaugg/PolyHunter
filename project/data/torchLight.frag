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

uniform vec3 viewPos;
uniform vec3 ambientColor;
uniform float lightIntensity_0;
uniform vec3 lightDiffuseColor_0;
uniform vec3 lightSpecularColor_0;
uniform vec4 lightPositionViewSpace_0;
uniform vec4 lighPos_World_0;

// World Space Coordinates
varying mediump vec3 v_normal;
varying mediump vec4 v_position;
varying mediump vec3 v_tangent;
varying mediump vec3 v_bitangent;

// texture Coords and Color
varying lowp vec4 v_texCoord;

void main()
{
    vec4 position = v_position;
    vec3 normal = normalize(v_normal);
    vec4 lightVector = normalize(lighPos_World_0 - position);
    
    // ambient part
    vec4 ambientPart = vec4(ambientColor * lightIntensity_0, 1.0);
    
    // diffuse part
    float intensityFactor = dot(normal, lightVector.xyz);
    vec3 diffuseTerm = Kd * clamp(intensityFactor, 0.0, 1.0) * lightDiffuseColor_0;
    vec4 diffusePart = vec4(clamp(diffuseTerm, 0.0, 1.0), 1.0);
    
    // specular term
    vec4 specular = vec4(0.0);
    if (intensityFactor > 0.0) {
        vec3 viewDir = vec3(normalize(vec4(viewPos, 1.0) - position));
        vec3 reflectDir = -normalize(reflect(lightVector.xyz, normal));
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), Ns);
        spec = clamp(spec, 0.0, 1.0);
        specular = clamp(vec4(Ks * spec, 1.0), 0.0, 1.0);
    }
    
    vec4 color = texture2D(DiffuseMap, v_texCoord.st);
    
    vec4 outColor = (ambientPart + diffusePart) * color;
    gl_FragColor = outColor;
//    gl_FragColor = vec4(normal, 1.0);
}

