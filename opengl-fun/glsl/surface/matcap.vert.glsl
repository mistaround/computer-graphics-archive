#version 150

uniform mat4 u_Model;
uniform mat3 u_ModelInvTr;
uniform mat4 u_View;
uniform mat4 u_Proj;

in vec4 vs_Pos;
in vec4 vs_Nor;
in vec2 vs_UV;

out vec2 fs_UV;

void main()
{
    // TODO Homework 4
    vec3 fs_Nor = normalize(u_ModelInvTr * vec3(vs_Nor));
    fs_Nor = mat3(u_View) * fs_Nor;
    // if the (x, y) is (-1, 0), that would map to the leftmost point of the circle.
    fs_UV = vec2(vs_Nor / 2 + 0.5);

    vec4 modelposition = u_Model * vs_Pos;
    gl_Position = u_Proj * u_View * modelposition;
}
