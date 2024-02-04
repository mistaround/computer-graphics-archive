#ifndef VERTEXDISPLAY_H
#define VERTEXDISPLAY_H

#include "vertex.h"
#include <drawable.h>

class VertexDisplay : public Drawable {
protected:
    Vertex *representedVertex;

public:
    VertexDisplay(OpenGLContext* context);

    // Creates VBO data to make a visual representation of the currently selected Vertex
    void create() override;

    // A selected Vertex should be represented as a white GL_POINTS
    GLenum drawMode() override;

    // Change which Vertex representedVertex points to
    void updateVertex(Vertex*);
};

#endif // VERTEXDISPLAY_H
