$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D fbo_texture;
varying vec4 texCoordVarying;

void main()
{
    vec4 color = texture2D(fbo_texture, texCoordVarying.st);
    gl_FragColor = color;
}
