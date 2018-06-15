$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D fbo_texture1;
uniform sampler2D fbo_texture2;

varying vec4 texCoordVarying;


void main()
{
    vec4 sceneColor = texture2D(fbo_texture1, texCoordVarying.xy);
    vec4 blurColor = texture2D(fbo_texture2, texCoordVarying.xy);
    
    gl_FragColor = sceneColor + blurColor;
    
}

