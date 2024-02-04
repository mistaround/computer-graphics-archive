#pragma once
#include <polygon.h>
#include <QImage>

class Camera
{
public:
    glm::vec4 forward;
    glm::vec4 right;
    glm::vec4 up;
    glm::vec4 pos;

    float FOV;
    float near;
    float far;
    float ratio;

    Camera();

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();

    void TranslateForward(const float c);
    void TranslateRight(const float c);
    void TranslateUp(const float c);

    void RotateForward(const float angle);
    void RotateRight(const float angle);
    void RotateUp(const float angle);
};

class Rasterizer
{
private:
    //This is the set of Polygons loaded from a JSON scene file
    int window_width = 512;
    int window_height = 512;
    std::vector<Polygon> m_polygons;

public:
    int antialiasing = 1;
    int render_width = 512;
    int render_height = 512;

    // 0 Nothing 1 Lambertian 2 Toon
    int shader = 0;

    Camera camera;
    Rasterizer(const std::vector<Polygon>& polygons);
    QImage RenderScene();
    void ClearScene();
    void RenderRow(QImage &result, float row, Polygon &poly, Triangle &tri, std::array<float, 4> &bbox, std::vector<float> &z_buffer);
    void TransformToPixelSpace(Polygon &poly);

    glm::vec3 GetLambertianColor(glm::vec3 color, glm::vec4 normal, glm::vec4 lightDir, float albedo, float ambient);
    glm::vec3 GetToonColor(glm::vec3 color, glm::vec4 normal, glm::vec4 lightDir, float albedo, float ambient, int numTones);

};



