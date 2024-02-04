#include "vertex.h"
#include "halfedge.h"

Vertex::Vertex() :
    QListWidgetItem(),
    pos(glm::vec3()),
    edge(nullptr),
    id(lastVertexId++)
{
    this->setText(QString::number(this->id));
}

int Vertex::lastVertexId = 0;
