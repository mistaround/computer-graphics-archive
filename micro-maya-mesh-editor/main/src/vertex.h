#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>
#include <QListWidgetItem>

// By using forward declarations, you're telling the compiler that the HalfEdge class exists and will be defined later.
class HalfEdge;

class Vertex : public QListWidgetItem
{
public:
    Vertex();

    // A vec3 for storing its position
    glm::vec3 pos;

    // A pointer to one of the HalfEdges that points to this Vertex
    HalfEdge* edge;

    // A unique integer to identify the Vertex in menus and while debugging
    int id;

    // this way you can assign a copy of the value stored in that variable to the next component you create and have an easy way of making sure the ID is unique.
    static int lastVertexId;

    // store which joints influence its transformation and by how much they influence it, id -> weight
    std::vector<std::pair<int, float>> joints;
};

#endif // VERTEX_H
