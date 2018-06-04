$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

varying highp vec4 fragPos;

void main()
{
    gl_FragColor = fragPos;
}

