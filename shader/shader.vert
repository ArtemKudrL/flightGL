#version 330 core

layout(location = 0) in vec3 xyz;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 uv;

out vec2 UV;
out vec3 POS;
out vec3 EYE;
out vec3 LIGHT;
out vec3 NORM;

uniform mat4 p;
uniform mat4 v;
uniform mat4 m;
uniform vec3 light;

void main()
{
    gl_Position = p * v * m * vec4(xyz, 1);

    vec3 xyzCam = (v * m * vec4(xyz, 1)).xyz;
    vec3 lightCam = (v * m * vec4(light, 1)).xyz;

    UV = uv;
    POS = (m * vec4(xyz, 1)).xyz;
    EYE = vec3(0, 0, 0) - xyzCam;
    LIGHT = lightCam + EYE;
    NORM = (v * m * vec4(norm, 0)).xyz;
}
