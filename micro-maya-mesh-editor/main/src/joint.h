#ifndef JOINT_H
#define JOINT_H

#include <QTreeWidgetItem>
#include "la.h"
#include <glm/glm.hpp>

class Joint : public QTreeWidgetItem
{
public:
    int id;

    // The name of this joint which will be displayed in your QTreeWidget of joints.
    QString name;

    // The joint that is the parent of this joint.
    Joint *parent;

    // The set of joints that have this joint as their parent.
    std::vector<Joint*> children;

    // The position of this joint relative to its parent joint.
    glm::vec3 pos;

    // The quaternion that represents this joint's current orientation.
    glm::quat rot;

    // The inverse of the joint's compound transformation matrix at the time a mesh is bound to the joint's skeleton.
    glm::mat4 bind;

    // Return a mat4 that represents the concatenation of a joint's position and rotation.
    glm::mat4 getLocalTransformation();

    // Return a mat4 that represents the concatentation of this joint's local transformation with the transformations of its chain of parent joints.
    glm::mat4 getOverallTransformation();

    static int lastJointId;

    Joint();
    Joint(QString name, glm::vec3 position, glm::vec4 rotation);
};

#endif // JOINT_H
