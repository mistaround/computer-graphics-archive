#include "skeleton.h"
#include <QDebug>

Skeleton::Skeleton(OpenGLContext* context) :
    Drawable(context),
    chosenJoint(nullptr),
    joints(std::vector<uPtr<Joint>>())
{}

Joint* Skeleton::buildJoints(QJsonObject root) {
    QString name = root["name"].toString();
    QJsonArray pos = root["pos"].toArray();
    QJsonArray rot = root["rot"].toArray();
    QJsonArray children = root["children"].toArray();

    uPtr<Joint> joint = mkU<Joint>(name,
                                   glm::vec3(pos[0].toDouble(), pos[1].toDouble(), pos[2].toDouble()),
                                   glm::vec4(rot[0].toDouble(), rot[1].toDouble(), rot[2].toDouble(), rot[3].toDouble()));

    Joint *ptr = joint.get();
    joints.push_back(std::move(joint));

    for (auto childJson : children) {
        Joint *child = buildJoints(childJson.toObject());
        child->parent = ptr;
        ptr->children.push_back(child);
        ptr->addChild(child);
    }

    return ptr;
}

std::vector<glm::mat4> Skeleton::getTransformations() {
    std::vector<glm::mat4> transformations;
    for (auto &joint : joints) {
        transformations.push_back(joint->getOverallTransformation());
    }
    return transformations;
}

void generateCircles(const glm::mat4& transformation, int num_segments, float radius, int idx, bool color,
                    std::vector<GLuint> &indices,
                    std::vector<glm::vec4> &vertices,
                    std::vector<glm::vec4> &colors)
{
    float angleStep = 2 * M_PI / num_segments;
    float x; float y; float z;

    for (int i = 0; i < num_segments; ++i) {
        float angle = angleStep * i;
        // Parametric representation of a circle
        x = radius * cos(angle);
        y = radius * sin(angle);
        z = 0;
        vertices.push_back(transformation * glm::vec4(x, y, z, 1)); // X circle
        colors.push_back(color ? glm::vec4(1, 0, 0, 1) : glm::vec4(1, 1, 1, 1));

        x = radius * cos(angle);
        y = 0;
        z = radius * sin(angle);
        vertices.push_back(transformation * glm::vec4(x, y, z, 1)); // Y circle
        colors.push_back(color ? glm::vec4(0, 1, 0, 1) : glm::vec4(1, 1, 1, 1));

        x = 0;
        y = radius * cos(angle);
        z = radius * sin(angle);
        vertices.push_back(transformation * glm::vec4(x, y, z, 1)); // Z circle
        colors.push_back(color ? glm::vec4(0, 0, 1, 1) : glm::vec4(1, 1, 1, 1));

        indices.push_back(idx + i * 3);     // X circle
        indices.push_back((i == num_segments - 1) ? idx : idx + i * 3 + 3);

        indices.push_back(idx + i * 3 + 1); // Y circle
        indices.push_back((i == num_segments - 1) ? idx + 1 : idx + i * 3 + 4);

        indices.push_back(idx + i * 3 + 2); // Z circle
        indices.push_back((i == num_segments - 1) ? idx + 2 : idx + i * 3 + 5);

    }
}

void Skeleton::create() {
    std::vector<GLuint> indices;
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec4> colors;

    int curIdx = 0;
    int num_segments = 36;
    float radius = 0.5f;

    for (auto &joint : joints) {
        glm::mat4 transformations = joint->getOverallTransformation();

        // Draw the circles
        if (joint.get() == chosenJoint) {
            generateCircles(transformations, num_segments, radius, curIdx, false, indices, vertices, colors);
        } else {
            generateCircles(transformations, num_segments, radius, curIdx, true, indices, vertices, colors);
        }

        curIdx += 3 * num_segments;

        // Draw the links
        if (joint->parent) {
            // Child joint
            vertices.push_back(joint->getOverallTransformation() * glm::vec4(glm::vec3(), 1));
            colors.push_back(glm::vec4(1, 1, 0, 1)); // yellow
            indices.push_back(curIdx);
            curIdx ++;

            // Parent joint
            vertices.push_back(joint->parent->getOverallTransformation() * glm::vec4(glm::vec3(), 1));
            colors.push_back(glm::vec4(1, 0, 1, 1)); // magenta
            indices.push_back(curIdx);
            curIdx ++;
        }
    }

    generateIdx();
    bindIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    generatePos();
    bindPos();
    mp_context->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4), vertices.data(), GL_STATIC_DRAW);

    generateCol();
    bindCol();
    mp_context->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);

    count = indices.size();
}

GLenum Skeleton::drawMode() {
    return GL_LINES;
}
