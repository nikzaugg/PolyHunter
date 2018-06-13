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

// World Space Coordinates
varying mediump vec3 v_normal;
varying mediump vec4 v_position;
varying mediump vec3 v_tangent;
varying mediump vec3 v_bitangent;

// texture Coords
varying lowp vec4 v_texCoord;

varying mediump float noise;


//#define NOISE fbm
//#define NUM_NOISE_OCTAVES 5
//
//float hash(float n) { return fract(sin(n) * 1e4); }
//float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }
//
//
//float noise(vec2 x) {
//    vec2 i = floor(x);
//    vec2 f = fract(x);
//
//    // Four corners in 2D of a tile
//    float a = hash(i);
//    float b = hash(i + vec2(1.0, 0.0));
//    float c = hash(i + vec2(0.0, 1.0));
//    float d = hash(i + vec2(1.0, 1.0));
//
//    // Simple 2D lerp using smoothstep envelope between the values.
//    // return vec3(mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
//    //            mix(c, d, smoothstep(0.0, 1.0, f.x)),
//    //            smoothstep(0.0, 1.0, f.y)));
//
//    // Same code, with the clamps in smoothstep and common subexpressions
//    // optimized away.
//    vec2 u = f * f * (3.0 - 2.0 * f);
//    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
//}
//
//float fbm(vec2 x) {
//    float v = 0.0;
//    float a = 0.5;
//    vec2 shift = vec2(100);
//    // Rotate to reduce axial bias
//    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));
//    for (int i = 0; i < NUM_NOISE_OCTAVES; ++i) {
//        v += a * noise(x);
//        x = rot * x * 2.0 + shift;
//        a *= 0.5;
//    }
//    return v;
//}

void main()
{
//    float noiseValue = fbm(v_position.xy);
//    vec4 color;
//
//    if (noiseValue > 0.5) {
//        // color =  vec4(0.96, 0.95, 0.78, 1.0);
//        color = vec4(1.0, 1.0, 1.0, 1.0);
//    } else {
//        color = vec4(0.0, 0.0, 0.0, 1.0);
//    }
//
////    gl_FragColor = color;
//     gl_FragColor = vec4(vec2(v_texCoord), 0.0, 1.0);
//
//    // Color according to normals
////     vec3 normal_test = normal/2.0 + vec3(0.5);
////     gl_FragColor = vec4(normal_test, 1.0);
    
    // compose the colour using the UV coordinate
    // and modulate it with the noise like ambient occlusion
    // vec3 color = vec3( v_texCoord.xy * ( 1. - 2. * noise ), 0.0 );
    vec3 outColor = noise * vec3(1.0, 1.0, 0.0);
    gl_FragColor = vec4(vec3(outColor), 1.0 );
}

