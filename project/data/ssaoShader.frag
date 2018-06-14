$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4 ProjectionMatrix;

uniform sampler2D depthMap;
uniform sampler2D normalMap;
uniform sampler2D noiseTex;

uniform vec3 samples[16];

varying vec4 texCoordVarying;

mediump vec2 res = vec2(1024.0, 768.0);
float ssaoStrength = 1.0;
float ssaoBase = 0.2;
float ssaoArea = 0.025;
float ssaoFalloff = 0.000001;
float ssaoRadius = 0.2;

const int nrSamples = 16;
vec3 sampleSphere[nrSamples];

vec3 GetNormalFromDepth(float depth, vec2 uv)
{
    const vec2 offset1 = vec2(0.0,0.02);
    const vec2 offset2 = vec2(0.02,0.0);
    
    float depth1 = texture2D(depthMap, uv + offset1).r;
    float depth2 = texture2D(depthMap, uv + offset2).r;
    
    vec3 p1 = vec3(offset1, depth1 - depth);
    vec3 p2 = vec3(offset2, depth2 - depth);
    
    vec3 normal = cross(p1, p2);
    normal.z = -normal.z;
    
    return normalize(normal);
}

void main()
{
    float depth = texture2D(depthMap, texCoordVarying.xy).r;
    
    mediump vec3 random = texture2D(noiseTex, 1.0 - texCoordVarying.xy).rgb * 2.0 - 1.0;
    vec3 position = vec3(texCoordVarying.xy, depth);
    position *= 2.0 - 1.0;
    mediump vec3 normal = texture2D(normalMap, texCoordVarying.xy).rgb * 2.0 - 1.0;
    
    sampleSphere[0] = vec3( 0.5381, 0.1856,-0.4319);
    sampleSphere[1] = vec3( 0.1379, 0.2486, 0.4430);
    sampleSphere[2] = vec3( 0.3371, 0.5679,-0.0057);
    sampleSphere[3] = vec3(-0.6999,-0.0451,-0.0019);
    sampleSphere[3] = vec3( 0.0689,-0.1598,-0.8547);
    sampleSphere[5] = vec3( 0.0560, 0.0069,-0.1843);
    sampleSphere[6] = vec3(-0.0146, 0.1402, 0.0762);
    sampleSphere[7] = vec3( 0.0100,-0.1924,-0.0344);
    sampleSphere[8] = vec3(-0.3577,-0.5301,-0.4358);
    sampleSphere[9] = vec3(-0.3169, 0.1063, 0.0158);
    sampleSphere[10] = vec3( 0.0103,-0.5869, 0.0046);
    sampleSphere[11] = vec3(-0.0897,-0.4940, 0.3287);
    sampleSphere[12] = vec3( 0.7119,-0.0154,-0.0918);
    sampleSphere[13] = vec3(-0.0533, 0.0596,-0.5411);
    sampleSphere[14] = vec3( 0.0352,-0.0631, 0.5460);
    sampleSphere[15] = vec3(-0.4776, 0.2847,-0.0271);
    
    float radiusDepth = ssaoRadius/depth;
    float occlusion = 0.0;
    for(int i=0; i < nrSamples; i++)
    {
        vec3 ray = radiusDepth * reflect(samples[i], random);
        vec3 hemiRay = position + sign(dot(ray, normal)) * ray;
        
        float occDepth = texture2D(depthMap, clamp(hemiRay.xy, 0.0, 1.0)).r;
        float difference = depth - occDepth;
        
//        occlusion += step(ssaoFalloff, difference) * (1.0 - smoothstep(ssaoFalloff, ssaoArea, difference));
//
        float rangeCheck = abs(difference) < radiusDepth ? 1.0 : 0.0;
        occlusion += (occDepth <= position.z ? 1.0 : 0.0) * rangeCheck;
    }
    
    float ao = 1.0 - ssaoStrength * occlusion * (1.0 / float(nrSamples));
    
    gl_FragColor = vec4(clamp(ao + ssaoBase, 0.0, 1.0));
//    gl_FragColor = vec4(vec3(normal), 1.0);
}
