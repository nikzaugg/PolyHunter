$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4 ProjectionMatrix;
uniform mat4 newProjectionMatrix;
uniform mat4 ViewMatrix;

uniform sampler2D depthMap;
uniform sampler2D positionMap;
uniform sampler2D normalMap;
uniform sampler2D texNoise;

uniform vec3 samples[32]; // kernel samples

varying vec4 texCoordVarying;
varying vec4 posViewSpace;

float kernelSize = 32.0;
float radius = 0.5;
float bias = 0.025;
float falloff = 0.000001;
float area = 0.0075;
float base = 0.2;

float near = 0.1;
float far  = 10000.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

float random (in vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233)))*43758.5453123);
}

vec3 normalFromDepth(float depth, vec2 texcoords){
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
    float depth = texture2D(depthMap, texCoordVarying.xy).r;
    // depth = LinearizeDepth(depth);
    vec3 fragPos = vec3(texCoordVarying.xy, depth);
    // vec3 normal = normalFromDepth(depth, texCoordVarying.xy);
    // highp vec3 fragPos = texture2D(positionMap, texCoordVarying.xy).xyz * 2.0 - 1.0;
    vec3 normal = texture2D(normalMap, texCoordVarying.xy).rgb * 2.0 - 1.0;
    normal = normalize(normal);
    vec3 randomVec = vec3(1.0);
    randomVec.x = random(texCoordVarying.xx);
    randomVec.y = random(texCoordVarying.yy);
    randomVec.z = random(texCoordVarying.xy);
    randomVec = normalize(randomVec);
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);

    // mat3 TBN = mat3(tangent, bitangent, normal);
    
    vec3 firstRow = vec3(tangent.x, bitangent.x, normal.x);
    vec3 secondRow = vec3(tangent.y, bitangent.y, normal.y);
    vec3 thirdRow = vec3(tangent.z, bitangent.z, normal.z);
    mat3 TBN = mat3(firstRow, secondRow, thirdRow);
    
    float radius_depth = radius/depth;
    float occlusion = 0.0;
    for (int i = 0; i < 32; ++i) {
        
        vec3 ray = radius_depth * reflect(samples[i], randomVec);
        vec3 hemi_ray = fragPos + sign(dot(ray,normal)) * ray;
        float occ_depth = texture2D(depthMap, hemi_ray.xy).r;
        float difference = depth - occ_depth;
        
        occlusion += step(falloff, difference) * (1.0-smoothstep(falloff, area, difference));
        
//        vec3 samp = TBN * samples[i];
//        samp = fragPos * samp * radius;
//
//        vec4 offset = vec4(samp, 1.0);
//        offset = newProjectionMatrix * offset;
//        offset.xyz /= offset.w;
//        offset.xyz = offset.xyz * 0.5 + 0.5;
//
//        float sampleDepth = texture2D(depthMap, offset.xy).z;
//
//        float rangeCheck = smoothstep(0.0, 1.0, radius/abs(fragPos.z - sampleDepth));
//        occlusion += (sampleDepth >= samp.z + bias ? 1.0 : 0.0) * rangeCheck;
        
    }
    
    float ao = 1.0 - 1.0 * occlusion * (1.0 / kernelSize);
    gl_FragColor = vec4(vec3(ao), 1.0);
    
//    occlusion = 1.0 - (occlusion / kernelSize);
//    gl_FragColor = vec4(vec3(fragPos), 1.0);
//    gl_FragColor = vec4(vec3(normal), 1.0);
}
