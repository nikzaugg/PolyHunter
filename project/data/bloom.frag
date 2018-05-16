$B_SHADER_VERSION
#ifdef GL_ES
precision lowp float;
#endif

uniform sampler2D fbo_texture;

varying vec4 texCoordVarying;

void main()
{
    vec4 FragColor = texture2D(fbo_texture, texCoordVarying.xy);
//    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
//    gl_FragColor = vec4(average, average, average, 1.0);
     gl_FragColor = FragColor;
    
}
