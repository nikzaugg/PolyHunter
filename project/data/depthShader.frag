$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D depthMap;
varying vec4 texCoordVarying;

void main()
{

    float c = texture2D(depthMap, texCoordVarying.st).x;
    gl_FragColor = vec4(c, c, c, 1.0);
}
