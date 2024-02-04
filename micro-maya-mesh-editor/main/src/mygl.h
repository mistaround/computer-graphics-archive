#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/squareplane.h>
#include "camera.h"
#include "facedisplay.h"
#include "halfedgedisplay.h"
#include "mesh.h"
#include "skeleton.h"
#include "vertexdisplay.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>


class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    SquarePlane m_geomSquare;// The instance of a unit cylinder we can use to render any cylinder
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)
    ShaderProgram m_progSkeleton;// A shader program rendering skeleton

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera m_glCamera;

public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    Mesh m_loadedMesh;
    Skeleton m_loadedSkeleton;

    Vertex* m_chosenVertex;
    HalfEdge* m_chosenHalfEdge;
    Face* m_chosenFace;

    VertexDisplay m_vertDisplay;
    HalfEdgeDisplay m_halfEdgeDisplay;
    FaceDisplay m_faceDisplay;


public slots:
    void slot_loadMesh();
    void slot_loadSkeleton();
    void slot_bindMesh();

    void slot_setChosenVertex(QListWidgetItem*);
    void slot_setChosenHalfEdge(QListWidgetItem*);
    void slot_setChosenFace(QListWidgetItem*);
    void slot_setChosenJoint(QTreeWidgetItem*);

    void slot_setPositionX(double);
    void slot_setPositionY(double);
    void slot_setPositionZ(double);

    void slot_setJointPosX(double);
    void slot_setJointPosY(double);
    void slot_setJointPosZ(double);

    void slot_setJointRotXP();
    void slot_setJointRotYP();
    void slot_setJointRotZP();
    void slot_setJointRotXM();
    void slot_setJointRotYM();
    void slot_setJointRotZM();

    void slot_setColorR(double);
    void slot_setColorG(double);
    void slot_setColorB(double);

    void slot_addVertex();
    void slot_triangulate();
    void slot_subdivision();

signals:
    void sig_buildComponentList(Mesh*);
    void sig_buildJointList(Skeleton*);

    void sig_updatePosition(const QVector3D&);
    void sig_updateColor(const QColor&);
    void sig_updateJointPos(const QVector3D&);
    void sig_updateRotation(const QVector3D&);
protected:
    void keyPressEvent(QKeyEvent *e);
};


#endif // MYGL_H
