$B_SHADER_VERSION
#ifdef GL_ES
precision mediump float;
#endif

uniform mediump mat4 ViewMatrix;
uniform mediump mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;

uniform lowp vec3 Ka;   // ambient material coefficient
uniform lowp vec3 Kd;   // diffuse material coefficient
uniform lowp vec3 Ks;   // specular material coefficient

uniform mediump float Ns;   // specular material exponent (shininess)

uniform vec3 ambientColor;
uniform float lightIntensity_0;
uniform vec3 lightDiffuseColor_0;
uniform vec3 lightSpecularColor_0;
uniform vec4 lightPositionViewSpace_0;

uniform float amplitude;

attribute vec4 Position;
attribute vec3 Normal;
attribute vec3 Tangent;
attribute vec3 Bitangent;
attribute vec4 TexCoord;

varying vec4 texCoordVarying;
varying lowp vec4 heightColor;
varying mediump vec4 posVarying;        // pos in world space
varying mediump vec4 camPosVarying;        // pos in world space
varying mediump vec3 normalVarying;     // normal in world space
varying mediump vec3 tangentVarying;    // tangent in world space

void main()
{
    heightColor = vec4(Position.y/amplitude, Position.y/amplitude, Position.y/amplitude, 1.0);
    lowp float heightNormalized = Position.y / amplitude;

    // if (heightNormalized > 2.0) {
    //     heightColor = vec4(1.0, 1.0, 1.0, 1.0);
    // } else if (heightNormalized > 0.8) {
    //      heightColor = vec4(0.0, 1.0, 0.0, 1.0);
    // } else if (heightNormalized > 0.7) {
    //      heightColor = vec4(1.0, 0.0, 0.0, 1.0);
    // }

    camPosVarying = ModelViewMatrix * Position;
	posVarying = ModelMatrix * Position; // posViewSpace
    // need to invert z-value of normal to get the normal right
    // TODO: export blender normal in the right coordinate system, so that
    // we can remove this
    normalVarying = normalize(NormalMatrix * (Normal * vec3(1.0, 1.0, -1.0)));
    tangentVarying = normalize(NormalMatrix * Tangent);

    texCoordVarying = TexCoord;
    // gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(Position.x, 0.0, Position.z, 1.0);
    gl_Position = ProjectionMatrix * ModelViewMatrix * Position;
}