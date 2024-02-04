#include "polygon.h"
#include <glm/gtx/transform.hpp>

void Polygon::Triangulate()
{
    //TODO: Populate list of triangles
    for (size_t i = 0; i < m_verts.size() - 2; i++)
    {
        Triangle tri;
        tri.m_indices[0] = 0;
        tri.m_indices[1] = i + 1;
        tri.m_indices[2] = i + 2;
        m_tris.push_back(tri);
    }
    
}

Segment::Segment(glm::vec4 start, glm::vec4 end) :
    m_start(start), m_end(end), dX(end.x - start.x), dY(end.y - start.y)
{}

glm::vec3 GetImageColor(const glm::vec2 &uv_coord, const QImage* const image)
{
    if(image)
    {
        int X = glm::min(image->width() * uv_coord.x, image->width() - 1.0f);
        int Y = glm::min(image->height() * (1.0f - uv_coord.y), image->height() - 1.0f);
        QColor color = image->pixel(X, Y);
        return glm::vec3(color.red(), color.green(), color.blue());
    }
    return glm::vec3(255.f, 255.f, 255.f);
}

float TriangleArea(const glm::vec4 v1, const glm::vec4 v2, const glm::vec4 v3)
{
    glm::vec3 a = glm::vec3(v2 - v1);
    glm::vec3 b = glm::vec3(v3 - v1);
    a.z = 0;
    b.z = 0;
    return 0.5f * glm::length(glm::cross(a, b));
}


bool Segment::getIntersections(float &x, int y)
{
    // Case that outside bbox
    if (y > std::max(m_start.y, m_end.y) || y < std::min(m_start.y, m_end.y)) return false;

    // Case that the slop is undefined
    if (dX == 0) {
        x = m_start.x;
        return true;
    }

    // Case that the slope is zero
    if (dY == 0) return false;

    x = (y - m_start.y) / (dY / dX) + m_start.x;
    return true;
}

std::array<float, 4> Polygon::GetBoudingBox(const Triangle &tri)
{
    Vertex v1 = m_pixel_verts[tri.m_indices[0]];
    Vertex v2 = m_pixel_verts[tri.m_indices[1]];
    Vertex v3 = m_pixel_verts[tri.m_indices[2]];

    // lx, ly, hx, hy
    std::array<float, 4> bbox;
    bbox[0] = std::min(std::min(v1.m_pos.x, v2.m_pos.x), v3.m_pos.x);
    bbox[1] = std::min(std::min(v1.m_pos.y, v2.m_pos.y), v3.m_pos.y);
    bbox[2] = std::max(std::max(v1.m_pos.x, v2.m_pos.x), v3.m_pos.x);
    bbox[3] = std::max(std::max(v1.m_pos.y, v2.m_pos.y), v3.m_pos.y);

    return bbox;
}

std::array<Segment, 3> Polygon::GetTriangleEdges(const Triangle &tri)
{
    Vertex v1 = m_pixel_verts[tri.m_indices[0]];
    Vertex v2 = m_pixel_verts[tri.m_indices[1]];
    Vertex v3 = m_pixel_verts[tri.m_indices[2]];

    Segment e1 = Segment(v1.m_pos, v2.m_pos);
    Segment e2 = Segment(v2.m_pos, v3.m_pos);
    Segment e3 = Segment(v3.m_pos, v1.m_pos);

    std::array<Segment, 3> edges = {e1, e2, e3};

    return edges;
}


glm::vec3 Polygon::GetBarycentricWeight(const Triangle &tri, float x, float y)
{
    Vertex v1 = m_pixel_verts[tri.m_indices[0]];
    Vertex v2 = m_pixel_verts[tri.m_indices[1]];
    Vertex v3 = m_pixel_verts[tri.m_indices[2]];

    glm::vec4 pos  = glm::vec4(x, y, 0.f, 0.f);

    float S  = TriangleArea(v1.m_pos, v2.m_pos, v3.m_pos);
    float S1 = TriangleArea(pos, v2.m_pos, v3.m_pos);
    float S2 = TriangleArea(pos, v1.m_pos, v3.m_pos);
    float S3 = TriangleArea(pos, v1.m_pos, v2.m_pos);

    return glm::vec3(S1 / (v1.m_pos.z + 1e-9), S2 / (v2.m_pos.z + 1e-9), S3 / (v3.m_pos.z + 1e-9)) / S;
}

glm::vec3 Polygon::GetBarycentricColor(const Triangle &tri, float x, float y)
{
    Vertex v1 = m_verts[tri.m_indices[0]];
    Vertex v2 = m_verts[tri.m_indices[1]];
    Vertex v3 = m_verts[tri.m_indices[2]];

    glm::vec3 weight = GetBarycentricWeight(tri, x, y);
    float z = 1 / (weight.x + weight.y + weight.z);

    return z * (v1.m_color * weight.x + v2.m_color * weight.y + v3.m_color * weight.z);
}

float Polygon::GetBarycentricZ(const Triangle &tri, float x, float y)
{
    glm::vec3 weight = GetBarycentricWeight(tri, x, y);
    return 1 / (weight.x + weight.y + weight.z);
}

glm::vec2 Polygon::GetBarycentricUV(const Triangle &tri, float x, float y)
{
    Vertex v1 = this->m_verts[tri.m_indices[0]];
    Vertex v2 = this->m_verts[tri.m_indices[1]];
    Vertex v3 = this->m_verts[tri.m_indices[2]];

    glm::vec3 weight = GetBarycentricWeight(tri, x, y);
    float z = 1 / (weight.x + weight.y + weight.z);

    return z * (v1.m_uv * weight.x + v2.m_uv * weight.y + v3.m_uv * weight.z);
}

glm::vec4 Polygon::GetBarycentricNormal(const Triangle &tri, float x, float y)
{
    Vertex v1 = this->m_verts[tri.m_indices[0]];
    Vertex v2 = this->m_verts[tri.m_indices[1]];
    Vertex v3 = this->m_verts[tri.m_indices[2]];

    glm::vec3 weight = GetBarycentricWeight(tri, x, y);
    float z = 1 / (weight.x + weight.y + weight.z);

    return z * (v1.m_normal * weight.x + v2.m_normal * weight.y + v3.m_normal * weight.z);
}

// Creates a polygon from the input list of vertex positions and colors
Polygon::Polygon(const QString& name, const std::vector<glm::vec4>& pos, const std::vector<glm::vec3>& col)
    : m_tris(), m_verts(), m_name(name), mp_texture(nullptr), mp_normalMap(nullptr)
{
    for(unsigned int i = 0; i < pos.size(); i++)
    {
        m_verts.push_back(Vertex(pos[i], col[i], glm::vec4(), glm::vec2()));
    }
    Triangulate();
}

// Creates a regular polygon with a number of sides indicated by the "sides" input integer.
// All of its vertices are of color "color", and the polygon is centered at "pos".
// It is rotated about its center by "rot" degrees, and is scaled from its center by "scale" units
Polygon::Polygon(const QString& name, int sides, glm::vec3 color, glm::vec4 pos, float rot, glm::vec4 scale)
    : m_tris(), m_verts(), m_name(name), mp_texture(nullptr), mp_normalMap(nullptr)
{
    glm::vec4 v(0.f, 1.f, 0.f, 1.f);
    float angle = 360.f / sides;
    for(int i = 0; i < sides; i++)
    {
        glm::vec4 vert_pos = glm::translate(glm::vec3(pos.x, pos.y, pos.z))
                           * glm::rotate(rot, glm::vec3(0.f, 0.f, 1.f))
                           * glm::scale(glm::vec3(scale.x, scale.y, scale.z))
                           * glm::rotate(i * angle, glm::vec3(0.f, 0.f, 1.f))
                           * v;
        m_verts.push_back(Vertex(vert_pos, color, glm::vec4(), glm::vec2()));
    }

    Triangulate();
}

Polygon::Polygon(const QString &name)
    : m_tris(), m_verts(), m_name(name), mp_texture(nullptr), mp_normalMap(nullptr)
{}

Polygon::Polygon()
    : m_tris(), m_verts(), m_name("Polygon"), mp_texture(nullptr), mp_normalMap(nullptr)
{}

Polygon::Polygon(const Polygon& p)
    : m_tris(p.m_tris), m_verts(p.m_verts), m_name(p.m_name), mp_texture(nullptr), mp_normalMap(nullptr)
{
    if(p.mp_texture != nullptr)
    {
        mp_texture = new QImage(*p.mp_texture);
    }
    if(p.mp_normalMap != nullptr)
    {
        mp_normalMap = new QImage(*p.mp_normalMap);
    }
}

Polygon::~Polygon()
{
    delete mp_texture;
}

void Polygon::SetTexture(QImage* i)
{
    mp_texture = i;
}

void Polygon::SetNormalMap(QImage* i)
{
    mp_normalMap = i;
}

void Polygon::AddTriangle(const Triangle& t)
{
    m_tris.push_back(t);
}

void Polygon::AddVertex(const Vertex& v)
{
    m_verts.push_back(v);
}

void Polygon::ClearTriangles()
{
    m_tris.clear();
}

Triangle& Polygon::TriAt(unsigned int i)
{
    return m_tris[i];
}

Triangle Polygon::TriAt(unsigned int i) const
{
    return m_tris[i];
}

Vertex &Polygon::VertAt(unsigned int i)
{
    return m_verts[i];
}

Vertex Polygon::VertAt(unsigned int i) const
{
    return m_verts[i];
}
