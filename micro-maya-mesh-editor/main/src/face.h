#ifndef FACE_H
#define FACE_H

#include <QListWidgetItem>
#include <glm/glm.hpp>

// By using forward declarations, you're telling the compiler that the HalfEdge class exists and will be defined later.
class HalfEdge;

class Face : public QListWidgetItem
{
public:
    Face();

    // A vec3 to represent this Face's color as an RGB value
    glm::vec3 color;

    // A pointer to one of the HalfEdges that lies on this Face
    HalfEdge* edge;

    // A unique integer to identify the Face in menus and while debugging
    int id;

    // his way you can assign a copy of the value stored in that variable to the next component you create and have an easy way of making sure the ID is unique.
    static int lastFaceId;
};

#endif // FACE_H
