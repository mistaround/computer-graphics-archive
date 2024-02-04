#version 330

uniform sampler2D u_Texture; // The texture to be read from by this shader

in vec3 fs_Nor;
in vec3 fs_LightVec;

layout(location = 0) out vec3 out_Col;

void main()
{
    // TODO Homework 4

    float PI = 3.141592653;
    float t = clamp(dot(normalize(fs_LightVec), fs_Nor), 0, 1);

    // 0.30, 0.20, 0.20
    float r = 0.5 + 0.5 * cos(2 * PI * (t + 0.30));
    float g = 0.5 + 0.5 * cos(2 * PI * (t + 0.20));
    float b = 0.5 + 0.5 * cos(2 * PI * (t + 0.20));

    out_Col = vec3(r, g, b).rgb;
}
