#ifndef MESH_H
#define MESH_H

#include "drawable.h"
#include "smartpointerhelp.h"
#include "face.h"
#include "vertex.h"
#include "halfedge.h"

class Mesh : public Drawable
{
public:
    Mesh(OpenGLContext* context);

    std::vector<uPtr<Face>> faces;
    std::vector<uPtr<HalfEdge>> edges;
    std::vector<uPtr<Vertex>> verts;

    // Populates the VBOs of the Drawable.
    void create() override;

    // return GL_TRIANGLES
    GLenum drawMode() override;

    void addVertex(HalfEdge *edge);
    void triangulate(Face *face);
    void subdivision();

    static glm::vec3 getRandomColor();

private:
    void addCentroid(Face *face, std::unordered_map<int, Vertex*> &map);
    void addMidpoint(HalfEdge* edge, std::unordered_map<int, Vertex*> &map);
    void smoothVertex(Vertex *vert, std::unordered_map<int, Vertex*> &map);
    void quadrangulate(Face *face, Vertex *centroid,
                       std::vector<HalfEdge*> &originEdges,
                       std::vector<HalfEdge*> &newEdges);
};

#endif // MESH_H
