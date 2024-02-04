#include "rasterizer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <QDebug>

Rasterizer::Rasterizer(const std::vector<Polygon>& polygons)
    : m_polygons(polygons)
{}

QImage Rasterizer::RenderScene()
{
    render_width = window_width * antialiasing;
    render_height = window_width * antialiasing;

    QImage result(render_width, render_height, QImage::Format_RGB32);
    result.fill(qRgb(0.f, 0.f, 0.f));
    std::vector<float> z_buffer = std::vector<float>(render_width * render_height, std::numeric_limits<float>::infinity());

    // Scan each polygon
    for (Polygon &poly : m_polygons) {
        // Transform vetices
        TransformToPixelSpace(poly);

        // Scan each triangle
        for (Triangle &tri : poly.m_tris) {
            std::array<float, 4> bbox = poly.GetBoudingBox(tri);

            float lx = bbox[0];
            float ly = bbox[1];
            float hx = bbox[2];
            float hy = bbox[3];

            // Whole triangle outside of screen, skip
            if (lx >= render_width || ly >= render_height || hx < 0 || hy < 0) continue;

            // Scan each row
            int row_start = std::max((int) floor(ly), 0);
            int row_end = std::min((int) ceil(hy), render_height);
            for (int row = row_start; row < row_end; row ++) {
                RenderRow(result, (float) row, poly, tri, bbox, z_buffer);
            }
        }
    }

    return result.scaled(window_width, window_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);;
}

void Rasterizer::RenderRow(QImage &result, float row, Polygon &poly, Triangle &tri, std::array<float, 4> &bbox, std::vector<float> &z_buffer)
{
    std::array<Segment, 3> edges = poly.GetTriangleEdges(tri);

    float left = (float) render_width - 1.f;
    float right = 0.f;

    for (int i = 0; i < 3; i++) {
        float x;
        if (edges[i].getIntersections(x, row))
        {
            if (x < bbox[0] || x > bbox[2]) continue;
            left = std::min(left, x);
            right = std::max(right, x);
        }
    }

    int col_start = std::max((int) ceil(left), 0);
    int col_end = std::min((int) ceil(right), render_width);

    for (int col = col_start; col < col_end; col ++) {
        // Render only if it's smaller in z
        float z = poly.GetBarycentricZ(tri, (float) col, (float) row);
        int idx = col + render_height * row;
        if (z < z_buffer[idx]) {
            z_buffer[idx] = z;

            glm::vec2 UV = poly.GetBarycentricUV(tri, (float) col, (float) row);
            glm::vec3 color = GetImageColor(UV, poly.mp_texture);
            if (shader == 1) {
                color = GetLambertianColor(color,
                                           poly.GetBarycentricNormal(tri, col, row),
                                           -camera.forward,
                                           1.f,
                                           .3f);
            }
            else if (shader == 2) {
                color = GetToonColor(color,
                                     poly.GetBarycentricNormal(tri, col, row),
                                     -camera.forward,
                                     1.f,
                                     .3f,
                                     3);
            }
            color = glm::clamp(color, 0.f, 255.f);

            result.setPixel(col, row, qRgb(color.r, color.g, color.b));
        }
    }
}

void Rasterizer::TransformToPixelSpace(Polygon &poly)
{
    poly.m_pixel_verts = std::vector<Vertex>(poly.m_verts);
    glm::mat4 T = camera.GetProjectionMatrix() * camera.GetViewMatrix();
    for (size_t i = 0; i < poly.m_verts.size(); i ++)
    {
        glm::vec4 transformedPos = T * poly.m_verts[i].m_pos;

        // Normalize Z coords
        transformedPos = transformedPos / transformedPos.w;

        // NDC -> Pixel
        transformedPos.x = (transformedPos.x + 1) * render_width / 2;
        transformedPos.y = (1 - transformedPos.y) * render_height / 2;
        poly.m_pixel_verts[i].m_pos = transformedPos;
    }
}

glm::vec3 Rasterizer::GetLambertianColor(glm::vec3 color, glm::vec4 normal, glm::vec4 lightDir, float albedo, float ambient)
{
    lightDir = glm::normalize(lightDir);
    float attenuate = glm::clamp(glm::dot(normal, lightDir), 0.f, 1.f);
    return albedo * (attenuate + ambient) * color;
}

glm::vec3 Rasterizer::GetToonColor(glm::vec3 color, glm::vec4 normal, glm::vec4 lightDir, float albedo, float ambient, int numTones)
{
    lightDir = glm::normalize(lightDir);
    float attenuate = glm::clamp(glm::dot(normal, lightDir), 0.f, 1.f);
    float quantized = std::round(attenuate * numTones) / numTones;
    return color * (albedo * (quantized + ambient));
}

Camera::Camera() :
    forward(glm::vec4(0.f, 0.f, -1.f, 0.f)),
    right(glm::vec4(1.f, 0.f, 0.f, 0.f)),
    up(glm::vec4(0.f, 1.f, 0.f, 0.f)),
    pos(glm::vec4(0.f, 0.f, 10.f, 1.f)),
    FOV(45.f),
    near(0.01f),
    far(100.f),
    ratio(1.f) {}

glm::mat4 Camera::GetViewMatrix()
{
    glm::mat4 O = glm::mat4(
        glm::vec4(right.x, up.x, forward.x, 0.f),
        glm::vec4(right.y, up.y, forward.y, 0.f),
        glm::vec4(right.z, up.z, forward.z, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    );

    glm::mat4 T = glm::mat4(
        glm::vec4(1.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 1.f, 0.f),
        glm::vec4(-pos.x, -pos.y, -pos.z, 1.f)
    );

    return O * T;
}

glm::mat4 Camera::GetProjectionMatrix()
{
    float S = 1 / glm::tan(FOV/2);
    float A = ratio;
    float P = far / (far - near);
    float Q = -far * near / (far - near);

    return glm::mat4(
        glm::vec4(S/A, 0.f, 0.f, 0.f),
        glm::vec4(0.f, S, 0.f, 0.f),
        glm::vec4(0.f, 0.f, P, 1.f),
        glm::vec4(0.f, 0.f, Q, 0.f)
        );
}

void Camera::TranslateForward(const float c)
{
    pos += forward * c;
}

void Camera::TranslateRight(const float c)
{
    pos += right * c;
}

void Camera::TranslateUp(const float c)
{
    pos += up * c;
}

void Camera::RotateForward(const float angle)
{
    glm::mat4 R = glm::rotate(glm::mat4(), angle, glm::vec3(forward));
    forward = R * forward;
    right = R * right;
    up = R * up;
}

void Camera::RotateRight(const float angle)
{
    glm::mat4 R = glm::rotate(glm::mat4(), angle, glm::vec3(right));
    forward = R * forward;
    right = R * right;
    up = R * up;
}

void Camera::RotateUp(const float angle)
{
    glm::mat4 R = glm::rotate(glm::mat4(), angle, glm::vec3(up));
    forward = R * forward;
    right = R * right;
    up = R * up;
}


void Rasterizer::ClearScene()
{
    m_polygons.clear();
}

