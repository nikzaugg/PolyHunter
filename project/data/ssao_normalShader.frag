$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

varying mediump vec3 v_normal;

void main()
{
    gl_FragColor = normalize(vec4(v_normal, 1.0));

}

