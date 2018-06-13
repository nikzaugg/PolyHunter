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

uniform vec3 skyColor;
uniform vec3 skydomeCenter;

uniform vec3 ambientColor;
uniform float lightIntensity_0;
uniform vec3 lightDiffuseColor_0;
uniform vec3 lightSpecularColor_0;
uniform vec4 lightPositionViewSpace_0;

varying lowp vec4 v_color;
varying highp vec4 v_position;

void main()
{
    vec3 domeToCenter = normalize(vec3(v_position) - skydomeCenter);
    vec3 domeUp = vec3(0.0, 1.0, 0.0);
    
    float angle = clamp(dot(domeToCenter, domeUp), 0.0, 1.0);
    vec3 outColor = (1.0 - angle) * vec3(skyColor);

    
    gl_FragColor = vec4(vec3(outColor), 1.0);
}


