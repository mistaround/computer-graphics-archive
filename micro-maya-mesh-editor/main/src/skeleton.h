#ifndef SKELETON_H
#define SKELETON_H

#include "drawable.h"
#include "joint.h"
#include "smartpointerhelp.h"
#include <QJsonObject>
#include <QJsonArray>

class Skeleton : public Drawable
{
public:
    Joint* chosenJoint;

    std::vector<uPtr<Joint>> joints;

    Skeleton(OpenGLContext *context);

    Joint* buildJoints(QJsonObject root);

    std::vector<glm::mat4> getTransformations();

    void create() override;

    GLenum drawMode() override;
};

#endif // SKELETON_H
