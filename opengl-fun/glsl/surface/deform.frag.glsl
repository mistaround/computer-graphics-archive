#version 330

uniform sampler2D u_Texture; // The texture to be read from by this shader
uniform int u_Time;

in vec3 fs_Pos;
in vec3 fs_Nor;
in vec3 fs_LightVec;

layout(location = 0) out vec3 out_Col;

void main()
{
    // TODO Homework 4
    float speed = 0.01f;
    float PI = 3.141592653;
    float t = clamp(dot(normalize(fs_LightVec), fs_Nor), 0, 1) * (1 + cos(u_Time * speed) * cos(u_Time * speed));

    float r = 0.5 + 0.5 * cos(2 * PI * (t + 0.80));
    float g = 0.5 + 0.5 * cos(2 * PI * (t + 0.90));
    float b = 0.5 + 0.5 * cos(2 * PI * (t + 0.00));

    out_Col = vec3(r, g, b).rgb;
}
