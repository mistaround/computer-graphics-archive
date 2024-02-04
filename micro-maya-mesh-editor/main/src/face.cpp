#include "face.h"
#include "halfedge.h"

Face::Face() :
    QListWidgetItem(),
    color(glm::vec3()),
    edge(nullptr),
    id(lastFaceId++)
{
    this->setText(QString::number(this->id));
}

int Face::lastFaceId = 0;
