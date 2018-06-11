$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

uniform sampler2D depthMap;
uniform sampler2D normalMap;
uniform sampler2D noiseTex;

uniform vec3 samples[16]; // kernel samples, sent from CPU

varying vec4 texCoordVarying;

float kernelSize = 16.0; // 32 samples
float radius = 0.5; // radius of hemisphere
float bias = 0.025;

vec3 normalFromDepth(float depth, vec2 texcoords)
{
    
    vec2 offset1 = vec2(0.0,0.001);
    vec2 offset2 = vec2(0.001,0.0);
    
    float depth1 = texture2D(depthMap, texcoords + offset1).r;
    float depth2 = texture2D(depthMap, texcoords + offset2).r;
    
    
    vec3 p1 = vec3(offset1, depth1 - depth);
    vec3 p2 = vec3(offset2, depth2 - depth);
    
    vec3 normal = cross(p1, p2);
    normal.z = -normal.z;
    
    return normalize(normal);
}

void main()
{
    // depth of fragment
    float depth = texture2D(depthMap, texCoordVarying.xy).r;
    
    // construct position from depth
    vec2 fragPosXY = texCoordVarying.xy * 2.0 -1.0;
    float fragPosZ = texture2D(depthMap, texCoordVarying.xy).r;
    vec3 fragPos = vec3(fragPosXY, fragPosZ);
    fragPos = vec3( ProjectionMatrix * vec4(fragPos, 1.0) );
    
    // normal from texture (rescale to [-1.0 , 1.0])
    vec3 normal = texture2D(normalMap, texCoordVarying.xy).rgb * 2.0 -1.0;
    normal = normalize(normal);
    
    // random vector for the fragments -> points in xy-direction
    vec3 randomVec = texture2D(noiseTex, texCoordVarying.xy).xyz * 2.0 - 1.0;
    randomVec = normalize(randomVec);

    // construct TBN matrix to transform from tangent to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    float occlusion = 0.0;
    // for each kernel-sample, test depth values
    for (int i = 0; i < 16; ++i) {
        
        vec3 samp = TBN * samples[i];
        samp = samp * radius + fragPos;

        vec4 offset = vec4(samp, 1.0);
        offset = ProjectionMatrix * offset; // from view to clip-space
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5; // tranform to range [0.0, 1.0]

        // get depth of the sample
        float sampleDepth = texture2D(depthMap, offset.xy).z;
        
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samp.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    // compute occlusion-weight for current fragment
    occlusion = 1.0 - (occlusion / kernelSize);

    // output gray-scale color to texture
    gl_FragColor = vec4(vec3(occlusion), 1.0);
}
