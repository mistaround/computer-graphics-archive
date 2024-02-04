#version 330

in vec2 fs_UV;

out vec3 color;

uniform sampler2D u_RenderedTexture;
uniform int u_Time;
uniform ivec2 u_Dimensions;

const float Horizontal[9] = float[9]( 3, 0,  -3,
                                     10, 0,  -10,
                                      3, 0,  -3);

const float Vertical[9] = float[9]( 3, 10,   3,
                                    0,  0,   0,
                                    -3, -10, -3);

void main()
{
    // TODO Homework 5
    vec2 onePixel = vec2(1) / vec2(u_Dimensions);

    vec3 dx = vec3(0);
    vec3 dy = vec3(0);

    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            dx += Horizontal[(y + 1) + (x + 1) * 3] * texture(u_RenderedTexture, fs_UV + vec2(x, y) * onePixel).rgb;
            dy += Vertical[(y + 1) + (x + 1) * 3] * texture(u_RenderedTexture, fs_UV + vec2(x, y) * onePixel).rgb;
        }
    }

    color = vec3(dx * dx + dy * dy);
}
