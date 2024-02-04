#version 330 core

uniform mat4 u_Model;
uniform mat4 u_ViewProj;
uniform mat4 u_Transformations[100];
uniform mat4 u_Binds[100];

in vec4 vs_Pos;
in vec4 vs_Col;
in ivec2 vs_Joints;
in vec2 vs_Weights;

out vec4 fs_Col;

void main(void)
{
    fs_Col = vs_Col;

    vec4 pos1 = u_Transformations[vs_Joints[0]] * u_Binds[vs_Joints[0]] * vs_Pos;
    vec4 pos2 = u_Transformations[vs_Joints[1]] * u_Binds[vs_Joints[1]] * vs_Pos;

    vec4 modelposition = u_Model * vs_Weights[0] * pos1 + vs_Weights[1] * pos2;

    gl_Position = u_ViewProj * modelposition;
}
