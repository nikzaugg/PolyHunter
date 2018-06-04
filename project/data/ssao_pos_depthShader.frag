$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

varying highp vec3 fragPos;

void main()
{
    // store the position as color
    gl_FragColor = vec4(fragPos, 1.0);
}

