#version 330

in vec2 fs_UV;

out vec3 color;

uniform sampler2D u_RenderedTexture;

void main()
{
    // TODO Homework 5
    // grey = 0.21 * red + 0.72 * green + 0.07 * blue

    vec4 c = texture(u_RenderedTexture, fs_UV);
    float grey = c.r * 0.21 + c.g * 0.72 + c.b * 0.07;
    float dist = distance(fs_UV, vec2(0.5, 0.5));

    color = clamp(vec3(grey - dist), 0, 1);
}
