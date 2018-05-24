$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D fbo_texture1;
uniform sampler2D fbo_texture2;
uniform sampler2D fbo_depth1;
uniform sampler2D fbo_depth2;

varying vec4 texCoordVarying;

float near = 0.25;
float far  = 10000.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    vec4 sceneColor = texture2D(fbo_texture1, texCoordVarying.xy);
    vec4 blurColor = texture2D(fbo_texture2, texCoordVarying.xy);
    
    float sceneDepth = texture2D(fbo_depth1, texCoordVarying.xy).r;
    float crystalDepth = texture2D(fbo_depth2, texCoordVarying.xy).r;
    
    gl_FragColor = sceneColor + blurColor;
    
//    if(crystalDepth < sceneDepth) {
//        gl_FragColor = sceneColor;
//    } else {
//        gl_FragColor = sceneColor + blurColor;
//    }
}

