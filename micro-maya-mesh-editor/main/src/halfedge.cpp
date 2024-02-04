#include "halfedge.h"

HalfEdge::HalfEdge() :
    QListWidgetItem(),
    next(nullptr),
    sym(nullptr),
    face(nullptr),
    vert(nullptr),
    id(lastEdgeId++)
{
    this->setText(QString::number(this->id));
}

int HalfEdge::lastEdgeId = 0;
