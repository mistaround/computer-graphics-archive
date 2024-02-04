#ifndef HALFEDGE_H
#define HALFEDGE_H

#include "face.h"
#include "vertex.h"
#include <QListWidgetItem>

// By using forward declarations, you're telling the compiler that the HalfEdge class exists and will be defined later.
class Vertex;
class Face;

class HalfEdge : public QListWidgetItem
{
public:
    HalfEdge();

    // A pointer to the next HalfEdge in the loop of HalfEdges that lie on this HalfEdge's Face
    HalfEdge* next;

    // A pointer to the HalfEdge that lies parallel to this HalfEdge and which travels in the opposite direction and is part of an adjacent Face, i.e. this HalfEdge's symmetrical HalfEdge
    HalfEdge* sym;

    // A pointer to the Face on which this HalfEdge lies
    Face* face;

    // A pointer to the Vertex between this HalfEdge and its next HalfEdge
    Vertex* vert;

    // A unique integer to identify the HalfEdge in menus and while debugging
    int id;

    // his way you can assign a copy of the value stored in that variable to the next component you create and have an easy way of making sure the ID is unique.
    static int lastEdgeId;

};

#endif // HALFEDGE_H
