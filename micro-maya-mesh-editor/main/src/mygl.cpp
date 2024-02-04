#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <random>

MyGL::MyGL(QWidget *parent) :
    OpenGLContext(parent),
    m_geomSquare(this),
    m_progLambert(this),
    m_progFlat(this),
    m_progSkeleton(this),
    m_glCamera(),
    m_loadedMesh(this),
    m_loadedSkeleton(this),
    m_chosenVertex(nullptr),
    m_chosenHalfEdge(nullptr),
    m_chosenFace(nullptr),
    m_vertDisplay(this),
    m_halfEdgeDisplay(this),
    m_faceDisplay(this)
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    m_geomSquare.destroy();
    m_loadedMesh.destroy();
    m_loadedSkeleton.destroy();
    m_vertDisplay.destroy();
    m_halfEdgeDisplay.destroy();
    m_faceDisplay.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    // Create the instances of Cylinder and Sphere.
    m_geomSquare.create();

    // Create the instances of loaded mesh
    m_loadedMesh.create();

    // Create the instances of loaded skeleton
    m_loadedSkeleton.create();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    // Create and set up the skeleton rendering shader
    m_progSkeleton.create(":/glsl/skeleton.vert.glsl", ":/glsl/skeleton.frag.glsl");
    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);


}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_glCamera = Camera(w, h);
    glm::mat4 viewproj = m_glCamera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);
    m_progSkeleton.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_progFlat.setViewProjMatrix(m_glCamera.getViewProj());
    m_progFlat.setModelMatrix(glm::mat4(1.f));

    m_progLambert.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setCamPos(m_glCamera.eye);
    m_progLambert.setModelMatrix(glm::mat4(1.f));

    m_progSkeleton.setViewProjMatrix(m_glCamera.getViewProj());
    m_progSkeleton.setModelMatrix(glm::mat4(1.f));

    if (m_loadedMesh.verts.empty() || m_loadedMesh.verts[0]->joints.empty()) {
        m_progLambert.draw(m_loadedMesh);
    } else {
        m_progSkeleton.draw(m_loadedMesh);
    }

    glDisable(GL_DEPTH_TEST);

    if (m_chosenVertex) m_progFlat.draw(m_vertDisplay);
    if (m_chosenHalfEdge) m_progFlat.draw(m_halfEdgeDisplay);
    if (m_chosenFace) m_progFlat.draw(m_faceDisplay);
    m_progFlat.draw(m_loadedSkeleton);

    glEnable(GL_DEPTH_TEST);
}

void MyGL::slot_loadMesh() {
    QString fileName = QFileDialog::getOpenFileName(nullptr,
                                                   tr("Load .obj"), "../obj_files", tr(".obj Files (*.obj)"));
    if (!fileName.isNull()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {

            m_loadedMesh.verts.clear();
            m_loadedMesh.edges.clear();
            m_loadedMesh.faces.clear();

            Vertex::lastVertexId = 0;
            Face::lastFaceId = 0;
            HalfEdge::lastEdgeId = 0;

            // Examples
            // v 2.229345 -0.992723 -0.862826 vertex
            // vt 0.048321 0.967508 texture
            // vn -0.410409 0.393346 -0.822705 normal
            // f 947/644/5398 967/670/5399 948/614/5400 vertex/texture/normal

            std::vector<glm::vec3> vertices;
            std::vector<glm::vec2> textures;
            std::vector<glm::vec3> normals;
            // For sym, <vertIdx, vertIdx> -> HE
            std::map<std::pair<int, int>, HalfEdge*> edgeMap;

            QTextStream in(&file);

            while (!in.atEnd()) {
                QString line = in.readLine();
                QTextStream lineStream(&line);
                QString type;

                lineStream >> type;

                if (type == "v") {
                    glm::vec3 vert;
                    lineStream >> vert.x >> vert.y >> vert.z;
                    vertices.push_back(vert);

                    uPtr<Vertex> p_vert = mkU<Vertex>();
                    p_vert->pos = vert;
                    m_loadedMesh.verts.push_back(std::move(p_vert));

                } else if (type == "vt") {
                    glm::vec2 text;
                    lineStream >> text.x >> text.y;
                    textures.push_back(text);

                } else if (type == "vn") {
                    glm::vec3 norm;
                    lineStream >> norm.x >> norm.y >> norm.z;
                    normals.push_back(norm);

                } else if (type == "f") {
                    uPtr<Face> p_face = mkU<Face>();
                    p_face->color = Mesh::getRandomColor();

                    QStringList vertIndices = line.split(' ');
                    // -1 because of the initial "f"
                    int numVerts = vertIndices.size() - 1;

                    HalfEdge* prevEdge = nullptr;
                    HalfEdge* firstEdge = nullptr;

                    for (int i = 1; i <= numVerts; i++) {
                        int vertIdx = vertIndices[i].split('/')[0].toInt() - 1;
                        int prevIdx = i != 1 ?
                                      vertIndices[i - 1].split('/')[0].toInt() - 1:
                                      vertIndices[numVerts].split('/')[0].toInt() - 1;

                        uPtr<HalfEdge> p_edge = mkU<HalfEdge>();
                        p_edge->face = p_face.get();
                        p_edge->vert = m_loadedMesh.verts[vertIdx].get();
                        m_loadedMesh.verts[vertIdx]->edge = p_edge.get();
                        p_face->edge = p_edge.get();

                        if (prevEdge) {
                            prevEdge->next = p_edge.get();
                        } else {
                            firstEdge = p_edge.get();
                        }

                        // Add to the map or set the sym pointer
                        auto curEdge = std::make_pair(vertIdx, prevIdx);
                        auto symEdge = std::make_pair(prevIdx, vertIdx);

                        if (edgeMap.find(symEdge) != edgeMap.end()) {
                            p_edge->sym = edgeMap[symEdge];
                            edgeMap[symEdge]->sym = p_edge.get();
                        } else {
                            edgeMap[curEdge] = p_edge.get();
                        }

                        prevEdge = p_edge.get();

                        m_loadedMesh.edges.push_back(std::move(p_edge));

                        // If it's the last vertex of the face, close the loop
                        if (i == numVerts) {
                            prevEdge->next = firstEdge;
                        }

                    }

                    p_face->edge = firstEdge;
                    m_loadedMesh.faces.push_back(std::move(p_face));
                }
            }

            m_chosenVertex = nullptr;
            m_chosenHalfEdge = nullptr;
            m_chosenFace = nullptr;

            m_loadedMesh.destroy();
            m_loadedMesh.create();
            update();

            emit sig_buildComponentList(&m_loadedMesh);

            file.close();
        }
    }
}

void MyGL::slot_loadSkeleton() {
    QString fileName = QFileDialog::getOpenFileName(nullptr,
                                                    tr("Load .json"), "../jsons", tr(".json Files (*.json)"));
    if (!fileName.isNull()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            // Cleaning last skeleton and affliations
            for (auto &vert : m_loadedMesh.verts) {
                vert->joints.clear();
            }
            Joint::lastJointId = 0;
            m_loadedSkeleton.chosenJoint = nullptr;
            m_loadedSkeleton.joints.clear();

            // Parse JSON file
            QByteArray data = file.readAll();
            QJsonDocument loadDoc(QJsonDocument::fromJson(data));
            QJsonObject json = loadDoc.object();

            // Build joints for skeleton
            m_loadedSkeleton.buildJoints(json["root"].toObject());
        }
        file.close();
    }

    emit sig_buildJointList(&m_loadedSkeleton);
    m_loadedSkeleton.destroy();
    m_loadedSkeleton.create();
    update();
}

void MyGL::slot_bindMesh() {
    if (m_loadedSkeleton.joints.empty()) return;

    std::vector<glm::mat4> transformations;
    std::vector<glm::mat4> bindMatrices;

    // Reserve space for performance
    transformations.reserve(m_loadedSkeleton.joints.size());
    bindMatrices.reserve(m_loadedSkeleton.joints.size());

    for (const auto& joint : m_loadedSkeleton.joints) {
        glm::mat4 transformation = joint->getOverallTransformation();
        joint->bind = glm::inverse(transformation);
        transformations.push_back(transformation);
        bindMatrices.push_back(joint->bind);
    }

    for (auto& vert : m_loadedMesh.verts) {
        // Pair of index and distance
        std::pair<int, float> nearest[2] = {{-1, FLT_MAX}, {-1, FLT_MAX}};

        for (int i = 0; i < static_cast<int>(transformations.size()); ++i) {
            float dist = glm::distance2(glm::vec4(vert->pos, 1.0f), transformations[i] * glm::vec4(0, 0, 0, 1));

            if (dist < nearest[0].second) {
                nearest[1] = nearest[0]; // Replace the second nearest
                nearest[0] = {i, dist}; // Set new nearest
            } else if (dist < nearest[1].second) {
                nearest[1] = {i, dist};
            }
        }

        float weightSum = nearest[0].second + nearest[1].second;
        vert->joints.clear(); // Ensure no previous bindings remain
        vert->joints.push_back({nearest[0].first, 1.0f - nearest[0].second / weightSum});
        vert->joints.push_back({nearest[1].first, 1.0f - nearest[1].second / weightSum});
    }

    // Pass to shader
    m_progSkeleton.setTransformationMatrices(transformations);
    m_progSkeleton.setBindMatrices(bindMatrices);

    m_loadedMesh.destroy();
    m_loadedMesh.create();
    m_loadedSkeleton.destroy();
    m_loadedSkeleton.create();

    update();
}

QVector3D extractFromQuat(glm::quat q) {
    float yaw = atan2(2.0 * (q.y * q.z + q.w * q.x), q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);
    float pitch = asin(-2.0 * (q.x * q.z - q.w * q.y));
    float roll = atan2(2.0 * (q.x * q.y + q.w * q.z), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z);
    return QVector3D(roll, pitch, yaw);
}

void MyGL::slot_setChosenVertex(QListWidgetItem* vert) {
    if (!vert) return;
    m_chosenVertex = dynamic_cast<Vertex*>(vert);
    m_vertDisplay.destroy();
    m_vertDisplay.updateVertex(m_chosenVertex);
    m_vertDisplay.create();

    emit sig_updatePosition(QVector3D(m_chosenVertex->pos.x, m_chosenVertex->pos.y, m_chosenVertex->pos.z));
    update();
}

void MyGL::slot_setChosenHalfEdge(QListWidgetItem* edge) {
    if (!edge) return;
    m_chosenHalfEdge = dynamic_cast<HalfEdge*>(edge);
    m_halfEdgeDisplay.destroy();
    m_halfEdgeDisplay.updateHalfEdge(m_chosenHalfEdge);
    m_halfEdgeDisplay.create();
    update();
}

void MyGL::slot_setChosenFace(QListWidgetItem* face) {
    if (!face) return;
    m_chosenFace = dynamic_cast<Face*>(face);
    m_faceDisplay.destroy();
    m_faceDisplay.updateFace(m_chosenFace);
    m_faceDisplay.create();

    emit sig_updateColor(QColor::fromRgbF(m_chosenFace->color.r, m_chosenFace->color.g, m_chosenFace->color.b));
    update();
}

void MyGL::slot_setChosenJoint(QTreeWidgetItem* joint) {
    if (!joint) return;
    m_loadedSkeleton.chosenJoint = dynamic_cast<Joint*>(joint);

    m_loadedSkeleton.destroy();
    m_loadedSkeleton.create();

    emit sig_updateJointPos(QVector3D(m_loadedSkeleton.chosenJoint->pos.x,
                                      m_loadedSkeleton.chosenJoint->pos.y,
                                      m_loadedSkeleton.chosenJoint->pos.z));
    emit sig_updateRotation(extractFromQuat(m_loadedSkeleton.chosenJoint->rot));

    update();
}

void MyGL::slot_setPositionX(double val) {
    if (!m_chosenVertex) return;
    m_chosenVertex->pos.x = val;
    m_loadedMesh.destroy();
    m_vertDisplay.destroy();
    m_loadedMesh.create();
    m_vertDisplay.create();
    update();
}

void MyGL::slot_setPositionY(double val) {
    if (!m_chosenVertex) return;
    m_chosenVertex->pos.y = val;
    m_loadedMesh.destroy();
    m_vertDisplay.destroy();
    m_loadedMesh.create();
    m_vertDisplay.create();
    update();
}

void MyGL::slot_setPositionZ(double val) {
    if (!m_chosenVertex) return;
    m_chosenVertex->pos.z = val;
    m_loadedMesh.destroy();
    m_vertDisplay.destroy();
    m_loadedMesh.create();
    m_vertDisplay.create();
    update();
}

void MyGL::slot_setJointPosX(double val) {
    if (!m_loadedSkeleton.chosenJoint) return;
    m_loadedSkeleton.chosenJoint->pos.x = val;
    m_progSkeleton.setTransformationMatrices(m_loadedSkeleton.getTransformations());
    emit sig_updateRotation(extractFromQuat(m_loadedSkeleton.chosenJoint->rot));

    m_loadedSkeleton.destroy();
    m_loadedSkeleton.create();
    m_loadedMesh.destroy();
    m_loadedMesh.create();
    update();
}

void MyGL::slot_setJointPosY(double val) {
    if (!m_loadedSkeleton.chosenJoint) return;
    m_loadedSkeleton.chosenJoint->pos.y = val;
    m_progSkeleton.setTransformationMatrices(m_loadedSkeleton.getTransformations());
    emit sig_updateRotation(extractFromQuat(m_loadedSkeleton.chosenJoint->rot));

    m_loadedSkeleton.destroy();
    m_loadedSkeleton.create();
    m_loadedMesh.destroy();
    m_loadedMesh.create();
    update();
}

void MyGL::slot_setJointPosZ(double val) {
    if (!m_loadedSkeleton.chosenJoint) return;
    m_loadedSkeleton.chosenJoint->pos.z = val;
    m_progSkeleton.setTransformationMatrices(m_loadedSkeleton.getTransformations());
    emit sig_updateRotation(extractFromQuat(m_loadedSkeleton.chosenJoint->rot));

    m_loadedSkeleton.destroy();
    m_loadedSkeleton.create();
    m_loadedMesh.destroy();
    m_loadedMesh.create();
    update();
}

void MyGL::slot_setJointRotXP() {
    if (!m_loadedSkeleton.chosenJoint) return;
    m_loadedSkeleton.chosenJoint->rot *= glm::rotate(glm::quat(), glm::radians(5.f), glm::vec3(1, 0, 0));
    m_progSkeleton.setTransformationMatrices(m_loadedSkeleton.getTransformations());
    emit sig_updateRotation(extractFromQuat(m_loadedSkeleton.chosenJoint->rot));

    m_loadedSkeleton.destroy();
    m_loadedSkeleton.create();
    m_loadedMesh.destroy();
    m_loadedMesh.create();
    update();
}

void MyGL::slot_setJointRotYP() {
    if (!m_loadedSkeleton.chosenJoint) return;
    m_loadedSkeleton.chosenJoint->rot *= glm::rotate(glm::quat(), glm::radians(5.f), glm::vec3(0, 1, 0));
    m_progSkeleton.setTransformationMatrices(m_loadedSkeleton.getTransformations());
    emit sig_updateRotation(extractFromQuat(m_loadedSkeleton.chosenJoint->rot));

    m_loadedSkeleton.destroy();
    m_loadedSkeleton.create();
    m_loadedMesh.destroy();
    m_loadedMesh.create();
    update();
}

void MyGL::slot_setJointRotZP() {
    if (!m_loadedSkeleton.chosenJoint) return;
    m_loadedSkeleton.chosenJoint->rot *= glm::rotate(glm::quat(), glm::radians(5.f), glm::vec3(0, 0, 1));
    m_progSkeleton.setTransformationMatrices(m_loadedSkeleton.getTransformations());
    emit sig_updateRotation(extractFromQuat(m_loadedSkeleton.chosenJoint->rot));

    m_loadedSkeleton.destroy();
    m_loadedSkeleton.create();
    m_loadedMesh.destroy();
    m_loadedMesh.create();
    update();
}

void MyGL::slot_setJointRotXM() {
    if (!m_loadedSkeleton.chosenJoint) return;
    m_loadedSkeleton.chosenJoint->rot *= glm::rotate(glm::quat(), glm::radians(-5.f), glm::vec3(1, 0, 0));
    m_progSkeleton.setTransformationMatrices(m_loadedSkeleton.getTransformations());
    emit sig_updateRotation(extractFromQuat(m_loadedSkeleton.chosenJoint->rot));

    m_loadedSkeleton.destroy();
    m_loadedSkeleton.create();
    m_loadedMesh.destroy();
    m_loadedMesh.create();
    update();
}

void MyGL::slot_setJointRotYM() {
    if (!m_loadedSkeleton.chosenJoint) return;
    m_loadedSkeleton.chosenJoint->rot *= glm::rotate(glm::quat(), glm::radians(-5.f), glm::vec3(0, 1, 0));
    m_progSkeleton.setTransformationMatrices(m_loadedSkeleton.getTransformations());
    emit sig_updateRotation(extractFromQuat(m_loadedSkeleton.chosenJoint->rot));

    m_loadedSkeleton.destroy();
    m_loadedSkeleton.create();
    m_loadedMesh.destroy();
    m_loadedMesh.create();
    update();
}

void MyGL::slot_setJointRotZM() {
    if (!m_loadedSkeleton.chosenJoint) return;
    m_loadedSkeleton.chosenJoint->rot *= glm::rotate(glm::quat(), glm::radians(-5.f), glm::vec3(0, 0, 1));
    m_progSkeleton.setTransformationMatrices(m_loadedSkeleton.getTransformations());
    emit sig_updateRotation(extractFromQuat(m_loadedSkeleton.chosenJoint->rot));

    m_loadedSkeleton.destroy();
    m_loadedSkeleton.create();
    m_loadedMesh.destroy();
    m_loadedMesh.create();
    update();
}

void MyGL::slot_setColorR(double val) {
    if (!m_chosenFace) return;
    m_chosenFace->color.r = val;
    m_loadedMesh.destroy();
    m_faceDisplay.destroy();
    m_loadedMesh.create();
    m_faceDisplay.create();
    update();
}

void MyGL::slot_setColorG(double val) {
    if (!m_chosenFace) return;
    m_chosenFace->color.g = val;
    m_loadedMesh.destroy();
    m_faceDisplay.destroy();
    m_loadedMesh.create();
    m_faceDisplay.create();
    update();
}

void MyGL::slot_setColorB(double val) {
    if (!m_chosenFace) return;
    m_chosenFace->color.b = val;
    m_loadedMesh.destroy();
    m_faceDisplay.destroy();
    m_loadedMesh.create();
    m_faceDisplay.create();
    update();
}

void MyGL::slot_addVertex() {
    if (!m_chosenHalfEdge) return;
    m_loadedMesh.addVertex(m_chosenHalfEdge);
    m_chosenHalfEdge = nullptr;

    m_loadedMesh.destroy();
    m_loadedMesh.create();
    emit sig_buildComponentList(&m_loadedMesh);
    update();
}

void MyGL::slot_triangulate() {
    if (!m_chosenFace) return;
    m_loadedMesh.triangulate(m_chosenFace);
    m_chosenFace = nullptr;

    m_loadedMesh.destroy();
    m_loadedMesh.create();
    emit sig_buildComponentList(&m_loadedMesh);
    update();
}

void MyGL::slot_subdivision() {
    if (m_loadedMesh.verts.empty()) return;
    m_loadedMesh.subdivision();
    m_chosenVertex = nullptr;
    m_chosenHalfEdge = nullptr;
    m_chosenFace = nullptr;

    m_loadedMesh.destroy();
    m_loadedMesh.create();
    emit sig_buildComponentList(&m_loadedMesh);
    update();
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_glCamera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_glCamera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_glCamera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_glCamera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        m_glCamera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        m_glCamera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        m_glCamera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        m_glCamera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        m_glCamera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        m_glCamera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        m_glCamera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        m_glCamera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        m_glCamera = Camera(width(), height());
    } else {
        if (e->key() == Qt::Key_N) {
            // NEXT half-edge of the currently selected half-edge
            if (!m_chosenHalfEdge) return;
            m_chosenHalfEdge = m_chosenHalfEdge->next;
            m_chosenVertex = nullptr;
            m_chosenFace = nullptr;
        } else if (e->key() == Qt::Key_M) {
            // SYM half-edge of the currently selected half-edge
            if (!m_chosenHalfEdge) return;
            m_chosenHalfEdge = m_chosenHalfEdge->sym;
            m_chosenVertex = nullptr;
            m_chosenFace = nullptr;
        } else if (e->key() == Qt::Key_F) {
            // FACE of the currently selected half-edge
            if (!m_chosenHalfEdge) return;
            m_chosenFace = m_chosenHalfEdge->face;
            m_chosenHalfEdge = nullptr;
            m_chosenVertex = nullptr;
        } else if (e->key() == Qt::Key_V) {
            // VERTEX of the currently selected half-edge
            if (!m_chosenHalfEdge) return;
            m_chosenVertex = m_chosenHalfEdge->vert;
            m_chosenHalfEdge = nullptr;
            m_chosenFace = nullptr;
        } else if (e->modifiers() == Qt::ShiftModifier && e->key() == Qt::Key_H) {
            // HALF-EDGE of the currently selected face
            if (!m_chosenFace) return;
            m_chosenHalfEdge = m_chosenFace->edge;
            m_chosenVertex = nullptr;
            m_chosenFace = nullptr;
        } else if (e->key() == Qt::Key_H) {
            // HALF-EDGE of the currently selected vertex
            if (!m_chosenVertex) return;
            m_chosenHalfEdge = m_chosenVertex->edge;
            m_chosenVertex = nullptr;
            m_chosenFace = nullptr;
        }

        m_vertDisplay.destroy();
        m_halfEdgeDisplay.destroy();
        m_faceDisplay.destroy();

        m_vertDisplay.updateVertex(m_chosenVertex);
        m_halfEdgeDisplay.updateHalfEdge(m_chosenHalfEdge);
        m_faceDisplay.updateFace(m_chosenFace);

        m_vertDisplay.create();
        m_halfEdgeDisplay.create();
        m_faceDisplay.create();
    }



    m_glCamera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}
