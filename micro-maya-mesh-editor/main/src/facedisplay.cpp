#include "facedisplay.h"
#include "halfedge.h"

FaceDisplay::FaceDisplay(OpenGLContext* context)
    : Drawable(context), representedFace(nullptr)
{}

void FaceDisplay::create() {
    if (representedFace) {
        std::vector<glm::vec4> positions;
        std::vector<glm::vec4> colors;
        std::vector<GLuint> indices;

        HalfEdge* edge = representedFace->edge;
        int edgeCount = 0;

        // Colored with the opposite color of the Face.
        do {
            positions.push_back(glm::vec4(edge->vert->pos, 1));
            colors.push_back(glm::vec4(1.f - representedFace->color, 1));
            edge = edge->next;
            edgeCount ++;
        } while (edge != representedFace->edge);

        // An edge line has two vertex
        for (int i = 0; i < edgeCount; i ++) {
            indices.push_back(i);
            indices.push_back((i + 1) % edgeCount);
        }

        generatePos();
        bindPos();
        mp_context->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);

        generateCol();
        bindCol();
        mp_context->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);

        generateIdx();
        bindIdx();
        mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        count = indices.size();
    }
}

GLenum FaceDisplay::drawMode() {
    return GL_LINES;
}

void FaceDisplay::updateFace(Face* face) {
    representedFace = face;
}

