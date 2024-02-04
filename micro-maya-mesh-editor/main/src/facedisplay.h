#ifndef FACEDISPLAY_H
#define FACEDISPLAY_H

#include "face.h"
#include <drawable.h>

class FaceDisplay : public Drawable {
protected:
    Face *representedFace;

public:
    FaceDisplay(OpenGLContext* context);

    // Creates VBO data to make a visual representation of the currently selected Face
    void create() override;

    // A selected Face should be surrounded by a strip of GL_LINES
    GLenum drawMode() override;

    // Change which Face representedFace points to
    void updateFace(Face*);
};

#endif // FACEDISPLAY_H
