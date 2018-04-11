$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mediump mat4 ViewMatrix;
uniform mediump mat4 ModelMatrix;
uniform mediump mat4 ProjectionMatrix;

uniform mediump mat3 NormalMatrix;

uniform mediump vec4 LightPos;
uniform mediump vec3 EyePos;

uniform lowp vec3 Ka;   // ambient material coefficient
uniform lowp vec3 Kd;   // diffuse material coefficient
uniform lowp vec3 Ks;   // specular material coefficient

uniform mediump float Ns;   // specular material exponent (shininess)

uniform lowp vec3 Ia;   // ambient light intensity
uniform lowp vec3 Id;   // diffuse light intensity
uniform lowp vec3 Is;   // specular light intensity

uniform sampler2D DiffuseMap;

varying lowp vec4 ambientVarying;
varying lowp vec4 diffuseVarying;
varying lowp vec4 specularVarying;
varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;        // pos in world space
varying mediump vec3 normalVarying;     // normal in world space
varying mediump vec3 tangentVarying;    // tangent in world space

void main()
{
    lowp vec4 ambientResult = vec4(Ka * Ia, 1.0);
    lowp vec4 specular_ = vec4(0.0);

    mediump vec4 pos = posVarying;
    mediump vec3 n = normalize(normalVarying); 
    mediump vec3 l = normalize(LightPos - pos).xyz;
    
    lowp float intensity = dot(n, l);
    lowp vec3 diffuse = Kd * clamp(intensity, 0.0, 1.0) * Id;
    lowp vec4 diffuseResult = vec4(clamp(diffuse, 0.0, 1.0), 1.0);
    
    if(intensity > 0.0) {
        mediump vec4 direction = vec4(EyePos, 1.0) - pos;
        mediump vec3 V = direction.xyz;
        mediump vec3 H = (V+l)/length(V+l);

        lowp float angleFactor = pow(max(0.0, dot(n, H)), Ns);
        mediump vec3 specular = Ks * angleFactor * Is;
        specular_ = vec4(clamp(specular, 0.0, 1.0), 1.0);
    }

    lowp vec4 color = texture2D(DiffuseMap, texCoordVarying.st);
    gl_FragColor = (ambientResult + diffuseResult) * color;
	// gl_FragColor = vec4(normalVarying, 1.0);
    
}