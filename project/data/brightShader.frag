$B_SHADER_VERSION
#ifdef GL_ES
precision lowp float;
#endif

uniform sampler2D fbo_texture;

varying vec4 texCoordVarying;

void main()
{
    //vec4 color = texture2D(fbo_texture, texCoordVarying.xy);
    gl_FragColor = texture2D(fbo_texture, texCoordVarying.xy);
//    float brightness = (color.r * 0.2126) + (color.g * 0.7152) + (color.b * 0.0722);
//    if (brightness > 0.4) {
//        gl_FragColor = color;
//    } else {
//        gl_FragColor = vec4(0.0);
//    }

}
