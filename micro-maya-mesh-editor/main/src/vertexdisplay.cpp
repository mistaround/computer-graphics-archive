#include "vertexdisplay.h"

VertexDisplay::VertexDisplay(OpenGLContext* context)
   : Drawable(context), representedVertex(nullptr)
{}

void VertexDisplay::create() {
    if (representedVertex) {

        glm::vec4 pos = glm::vec4(representedVertex->pos, 1.f);
        glm::vec4 color = glm::vec4(1.f);
        GLuint idx = 0;

        generatePos();
        bindPos();
        mp_context->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4), &pos, GL_STATIC_DRAW);

        generateCol();
        bindCol();
        mp_context->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4), &color, GL_STATIC_DRAW);

        generateIdx();
        bindIdx();
        mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint), &idx, GL_STATIC_DRAW);

        count = 1;
    }
}

GLenum VertexDisplay::drawMode() {
    return GL_POINTS;
}

void VertexDisplay::updateVertex(Vertex* vert) {
    representedVertex = vert;
}
