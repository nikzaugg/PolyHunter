$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D shadowMap;
varying vec4 texCoordVarying;

float zNear = -1.0;    // TODO: Replace by the zNear of your perspective projection
float zFar  = 1.0; // TODO: Replace by the zFar  of your perspective projection

float LinearizeDepth(in vec2 uv)
{
    float depth = texture2D(shadowMap, uv).x;
    depth = normalize(depth);
    return (2.0 * zNear * zFar) / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{
    float c = texture2D(shadowMap, texCoordVarying.st).x;
    gl_FragColor = vec4(c, c, c, 1.0);
}
