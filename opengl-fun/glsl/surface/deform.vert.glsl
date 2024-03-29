#version 150

uniform mat4 u_Model;
uniform mat3 u_ModelInvTr;
uniform mat4 u_View;
uniform mat4 u_Proj;
uniform vec3 u_Cam;

uniform int u_Time;

in vec4 vs_Pos;
in vec4 vs_Nor;

out vec3 fs_Pos;
out vec3 fs_Nor;
out vec3 fs_LightVec;

void main()
{
    // TODO Homework 4
    fs_Nor = normalize(u_ModelInvTr * vec3(vs_Nor));

    float scale = 0.1f;
    float speed = 0.1f;
    float power = 10.f;
    float displacement = sin(vs_Pos.y * power + (u_Time * speed)) * cos(vs_Pos.x * power + (u_Time * speed)) * scale;

    vec4 endPos = vec4(vec3(vs_Pos) + (fs_Nor * displacement), 1);
    vec4 modelposition = u_Model * endPos;
    fs_Pos = vec3(modelposition);
    gl_Position = u_Proj * u_View * modelposition;

    fs_LightVec = u_Cam - vec3(modelposition);
}
