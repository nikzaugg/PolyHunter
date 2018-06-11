$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D ssao_Texture;

varying vec4 texCoordVarying;

void main()
{
    vec2 texelSize = vec2(1.0/1024.0, 1.0/768.0);
    float result = 0.0;
    for (int x = -2; x < 2; ++x) {
        for (int y = -2; y < 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture2D(ssao_Texture, texCoordVarying.xy + offset).r;
        }
    }

    result = result / (4.0 * 4.0);
    gl_FragColor = vec4(vec3(result), 1.0);
    
    gl_FragColor = texture2D(ssao_Texture, texCoordVarying.xy);
}
