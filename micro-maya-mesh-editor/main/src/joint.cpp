#include "joint.h"

int Joint::lastJointId = 0;

Joint::Joint() :
    QTreeWidgetItem(),
    id(lastJointId ++),
    name(),
    parent(nullptr),
    children(),
    pos(glm::vec3()),
    rot(glm::quat()),
    bind(glm::mat4())
{
    setText(0, name);
}

Joint::Joint(QString name, glm::vec3 position, glm::vec4 rotation) :
    QTreeWidgetItem(),
    id(lastJointId ++),
    name(name),
    parent(nullptr),
    children(),
    pos(position),
    bind(glm::mat4())
{
    float theta = rotation.x / 2.f;
    // q = [cos(theta/2), sin(theta/2)vx, sin(theta/2)vy, sin(theta/2)vz]
    rot = glm::quat(cos(glm::radians(theta)),
                     sin(glm::radians(theta)) * rotation.x,
                     sin(glm::radians(theta)) * rotation.y,
                     sin(glm::radians(theta)) * rotation.z);
    setText(0, name);
}

glm::mat4 Joint::getLocalTransformation() {
    return glm::translate(glm::mat4(1.f), pos) * glm::toMat4(rot);
}

glm::mat4 Joint::getOverallTransformation() {
    if (parent == nullptr) {
        return getLocalTransformation();
    } else {
        return parent->getOverallTransformation() * getLocalTransformation();
    }
}

