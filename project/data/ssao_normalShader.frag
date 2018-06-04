$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

varying mediump vec3 v_normal;

void main()
{
    // map normals to range [0.0 , 1.0]
    vec3 normal = v_normal;
    normal = normal / 2.0 + 0.5;
    normal = normalize(normal);
    gl_FragColor = vec4(vec3(normal), 1.0);

}

