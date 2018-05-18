$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D fbo_texture1;
uniform sampler2D fbo_texture2;
uniform sampler2D fbo_depth1;
uniform sampler2D fbo_depth2;

varying vec4 texCoordVarying;

float near = -1.0;
float far  = 1.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    vec4 sceneColor = texture2D(fbo_texture1, texCoordVarying.xy);
    vec4 blurColor = texture2D(fbo_texture2, texCoordVarying.xy);
    
    vec4 sceneDepth = texture2D(fbo_depth1, texCoordVarying.xy);
    vec4 crystalDepth = texture2D(fbo_depth2, texCoordVarying.xy);
    
    float sceneZ = LinearizeDepth(sceneDepth.x);
    float crystalZ = LinearizeDepth(crystalDepth.x);
    
    gl_FragColor = sceneColor + blurColor;
    
//    if (sceneZ < crystalZ) {
//        gl_FragColor = sceneColor + blurColor;
//    } else {
//        gl_FragColor = sceneColor;
//    }
    
//    float depth = LinearizeDepth(crystalDepth.x) / far; // divide by far for demonstration
//    gl_FragColor = vec4(vec3(depth), 1.0);
//    gl_FragColor = vec4(sceneDepth.x/far, 0.0, 0.0, 1.0);
}

