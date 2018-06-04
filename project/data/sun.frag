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

uniform sampler2D DiffuseMap;

uniform vec3 ambientColor;
uniform float lightIntensity_0;
uniform vec3 lightDiffuseColor_0;
uniform vec3 lightSpecularColor_0;
uniform vec4 lightPositionViewSpace_0;

uniform vec3 skyColor;
uniform vec3 fogColor;

varying lowp vec4 shadowCoord_varying;
varying lowp vec4 vertexColor_varying;
varying lowp vec4 texCoord_varying;
varying highp vec4 position_varying;
// Everything in View Space
varying mediump vec4 position_varying_ViewSpace;
varying mediump vec3 normal_varying_ViewSpace;
varying mediump vec3 tangent_varying_ViewSpace;

varying mediump float visibility;

#define NOISE fbm
#define NUM_NOISE_OCTAVES 5

float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }


float noise(vec2 x) {
    vec2 i = floor(x);
    vec2 f = fract(x);

	// Four corners in 2D of a tile
	float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    // Simple 2D lerp using smoothstep envelope between the values.
	// return vec3(mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
	//			mix(c, d, smoothstep(0.0, 1.0, f.x)),
	//			smoothstep(0.0, 1.0, f.y)));

	// Same code, with the clamps in smoothstep and common subexpressions
	// optimized away.
    vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}



float fbm(vec2 x) {
	float v = 0.0;
	float a = 0.5;
	vec2 shift = vec2(100);
	// Rotate to reduce axial bias
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));
	for (int i = 0; i < NUM_NOISE_OCTAVES; ++i) {
		v += a * noise(x);
		x = rot * x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
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
   // gl_FragColor = mix(vec4(vec3(fogColor), 1.0), outColor, visibility);
   float noiseValue = fbm(position_varying.xy);

    if (noiseValue > 0.5) {
        // color =  vec4(0.96, 0.95, 0.78, 1.0);
        color = vec4(1.0, 1.0, 1.0, 1.0);
    } else {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
   
    
	gl_FragColor =color;
    // gl_FragColor = vec4(visibility);
    
    // Color according to normals
//     vec3 normal_test = normal/2.0 + vec3(0.5);
//     gl_FragColor = vec4(normal_test, 1.0);
}

