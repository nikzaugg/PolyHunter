$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4 ProjectionMatrix;
uniform mat4 newProjectionMatrix;
uniform mat4 invProjectionMatrix;
uniform mat4 ViewMatrix;

uniform sampler2D depthMap;
uniform sampler2D positionMap;
uniform sampler2D normalMap;
uniform sampler2D noiseTex;

uniform vec3 samples[32]; // kernel samples, sent from CPU

varying vec4 texCoordVarying;

float random_size;
float g_sample_rad = 0.01;
float g_intensity = 1.0;
float g_scale = 0.001;
float g_bias = 0.0025;

vec3 getPosition(vec2 uv)
{
    return texture2D(positionMap,uv).xyz;
}

vec3 getNormal(vec2 uv)
{
    return normalize(texture2D(normalMap, uv).xyz * 2.0 - 1.0);
}

vec2 getRandom(vec2 uv)
{
    return normalize(texture2D(noiseTex, uv).xy * 2.0 - 1.0);
}

float doAmbientOcclusion(vec2 tcoord,vec2 uv, vec3 p, vec3 cnorm)
{
    vec3 diff = getPosition(tcoord + uv) - p;
    vec3 v = normalize(diff);
    float d = length(diff)*g_scale;
    return max(0.0,dot(cnorm,v)-g_bias)*(1.0/(1.0+d))*g_intensity;
}

void main()
{
    vec2 vec[4];
    vec[0] = vec2(1,0);
    vec[1] = vec2(-1,0);
    vec[2] = vec2(0,1);
    vec[3] = vec2(0,-1);
    vec3 p = getPosition(texCoordVarying.xy);
    vec3 n = getNormal(texCoordVarying.xy);
    vec2 rand = getRandom(texCoordVarying.xy);
    float ao = 0.0;
    float rad = g_sample_rad/p.z;
    
    //**SSAO Calculation**//
    int iterations = 4;
    for (int j = 0; j < iterations; ++j)
    {
        vec2 coord1 = reflect(vec[j],rand)*rad;
        vec2 coord2 = vec2(coord1.x*0.707 - coord1.y*0.707, coord1.x*0.707 + coord1.y*0.707);
        
        ao += doAmbientOcclusion(texCoordVarying.xy,coord1*0.25, p, n);
        ao += doAmbientOcclusion(texCoordVarying.xy,coord2*0.5, p, n);
        ao += doAmbientOcclusion(texCoordVarying.xy,coord1*0.75, p, n);
        ao += doAmbientOcclusion(texCoordVarying.xy,coord2, p, n);
    }
    
    ao = ao / (4.0*4.0);
    float outColor = 1.0 - ao;
    gl_FragColor = vec4(vec3(outColor), 1.0);
}
