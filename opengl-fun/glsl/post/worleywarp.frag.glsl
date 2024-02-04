#version 330

uniform ivec2 u_Dimensions;
uniform int u_Time;

in vec2 fs_UV;

out vec3 color;

uniform sampler2D u_RenderedTexture;

const float Horizontal[9] = float[9]( 3, 0,  -3,
                                     10, 0,  -10,
                                      3, 0,  -3);

const float Vertical[9] = float[9]( 3, 10,   3,
                                    0,  0,   0,
                                    -3, -10, -3);


// Random function and noise
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

vec2 randomPoint(vec2 st) {
    return fract(sin(vec2(dot(st, vec2(127.1, 311.7)),
                          dot(st, vec2(269.5, 183.3)))) * 43758.5453);
}

float noise(vec2 p){
    vec2 ip = floor(p);
    vec2 u = fract(p);
    u = u * u * (3.0 - 2.0 * u);

    float res = mix(
        mix(random(ip), random(ip + vec2(1.0, 0.0)), u.x),
        mix(random(ip + vec2(0.0, 1.0)),random(ip + vec2(1.0, 1.0)), u.x), u.y);
    return res * res;
}

float worley(vec2 st) {
    float closestDistance = 2.0;  // Initialize with a value greater than 1.0
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            vec2 gridCell = vec2(x,y);
            vec2 point = randomPoint(st + gridCell);
            float distance = length(gridCell + point);
            closestDistance = min(closestDistance, distance);
        }
    }
    return closestDistance;
}

// Rotation matrix
const mat2 mtx = mat2( 0.80,  0.60, -0.60,  0.80 );

// fBM effect
float fbm( vec2 p )
{
    float f = 0.0;

    f += 0.500000 * noise( p + (u_Time * 0.25)  );
    p = mtx * p * 2.02;
    f += 0.031250 * noise( p );
    p = mtx * p * 2.01;
    f += 0.250000 * noise( p );
    p = mtx * p * 2.03;
    f += 0.125000 * noise( p );
    p = mtx * p * 2.01;
    f += 0.062500 * noise( p );
    p = mtx * p * 2.04;
    f += 0.015625 * noise( p + sin((u_Time * 0.25)) );

    return f / 0.96875;
}

float pattern( in vec2 p )
{
    return fbm(p + fbm(p + fbm(p)));
}

void main()
{  
    // Apply FBM warping effect
    vec2 fbmWarp = vec2(fbm(fs_UV), fbm(fs_UV + vec2(2.3, 14.4)));
    vec2 warpedUV = fs_UV + fbmWarp * 0.01;

    // Scale UVs to create more or fewer cells
    vec2 scaledUV = warpedUV * 10;

    // Get Worley noise value
    float w = worley(scaledUV + u_Time * 0.001);

    // Warp the UVs using Worley noise
//    warpedUV = warpedUV + (vec2(w, w) - 0.25) * 0.01;
    warpedUV = warpedUV + (vec2(w, w) - 0.5) * (0.01 + 0.1 * cos(u_Time * 0.01) * cos(u_Time * 0.01));

    // Apply Mosaic Effect
//    float mosaicSize = 150.0 - 140 * cos(u_Time * 0.01) * cos(u_Time * 0.01);
//    vec2 mosaicUV = floor(warpedUV * mosaicSize) / mosaicSize;
//    color = texture(u_RenderedTexture, mosaicUV).rgb;

//    color = texture(u_RenderedTexture, warpedUV).rgb;

    vec2 onePixel = vec2(1) / vec2(u_Dimensions);

    vec3 dx = vec3(0);
    vec3 dy = vec3(0);

    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            dx += Horizontal[(y + 1) + (x + 1) * 3] * texture(u_RenderedTexture, warpedUV + vec2(x, y) * onePixel).rgb;
            dy += Vertical[(y + 1) + (x + 1) * 3] * texture(u_RenderedTexture, warpedUV + vec2(x, y) * onePixel).rgb;
        }
    }

    color = vec3(dx * dx + dy * dy);
}


