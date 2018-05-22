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

varying mediump float visibility;

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

	//if(color.a < 0.1)
    //    discard;
    
    vec4 outColor = (ambientPart + diffusePart) * color;
    // gl_FragColor = mix(vec4(skyColor, 1.0),  outColor, visibility);
    vec4 lightColor = vec4(1.0, 0.7, 1.0, 1.0);
    
	gl_FragColor = vec4(ambientColor, 0.0) + lightColor * color;

    
    // Color according to normals
//     vec3 normal_test = normal/2.0 + vec3(0.5);
//     gl_FragColor = vec4(normal_test, 1.0);
}

