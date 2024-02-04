#ifndef HalfEdgeDISPLAY_H
#define HalfEdgeDISPLAY_H

#include "halfedge.h"
#include <drawable.h>

class HalfEdgeDisplay : public Drawable {
protected:
    HalfEdge *representedHalfEdge;

public:
    HalfEdgeDisplay(OpenGLContext* context);

    // Creates VBO data to make a visual representation of the currently selected HalfEdge
    void create() override;

    // A selected HalfEdge should be represented as a single GL_LINES
    GLenum drawMode() override;

    // Change which HalfEdge representedHalfEdge points to
    void updateHalfEdge(HalfEdge*);
};

#endif // HalfEdgeDISPLAY_H
