#include "halfedgedisplay.h"
#include <QDebug>

HalfEdgeDisplay::HalfEdgeDisplay(OpenGLContext* context)
    : Drawable(context), representedHalfEdge(nullptr)
{}

void HalfEdgeDisplay::create() {
    if (representedHalfEdge) {
        std::vector<glm::vec4> positions;
        std::vector<glm::vec4> colors;
        std::vector<GLuint> indices;

        positions.push_back(glm::vec4(representedHalfEdge->vert->pos, 1));
        colors.push_back(glm::vec4(1, 1, 0, 1)); // Yellow
        indices.push_back(0);

        positions.push_back(glm::vec4(representedHalfEdge->sym->vert->pos, 1));
        colors.push_back(glm::vec4(1, 0, 0, 1)); // Red
        indices.push_back(1);

        generatePos();
        bindPos();
        mp_context->glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);

        generateCol();
        bindCol();
        mp_context->glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);

        generateIdx();
        bindIdx();
        mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        count = 2;
    }
}

GLenum HalfEdgeDisplay::drawMode() {
    return GL_LINES;
}

void HalfEdgeDisplay::updateHalfEdge(HalfEdge* halfEdge) {
    representedHalfEdge = halfEdge;
}

