$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D fbo_texture;

varying vec4 texCoordVarying;

const float near = 0.1; // projection matrix's near plane
const float far = 100.0; // projection matrix's far plane

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    float depth = texture2D(fbo_texture, texCoordVarying.st).r;
    gl_FragColor = vec4(vec3(depth), 1.0);
    // gl_FragColor = vec4(vec3(texCoordVarying), 1.0);
//    float lin_depth = LinearizeDepth(depth);
//    gl_FragColor = vec4(vec3(lin_depth) ,1.0);
}
