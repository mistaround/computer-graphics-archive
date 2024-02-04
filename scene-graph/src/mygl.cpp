#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>

MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
    prog_flat(this),
    m_geomGrid(this),
    m_geomSquare(Polygon2D(this, {glm::vec3(0.5f, 0.5f, 1.f),
                                glm::vec3(-0.5f, 0.5f, 1.f),
                                glm::vec3(-0.5f, -0.5f, 1.f),
                                glm::vec3(0.5f, -0.5f, 1.f)})),
    m_geomRectangle(Polygon2D(this, {glm::vec3(0.5f, 1.f, 1.f),
                                glm::vec3(-0.5f, 1.f, 1.f),
                                glm::vec3(-0.5f, -1.f, 1.f),
                                glm::vec3(0.5f, -1.f, 1.f)})),
    m_geomTriangle(Polygon2D(this, {glm::vec3(0.5f, 0.5f, 1.0f),
                                glm::vec3(-0.5f, 0.5f, 1.0f),
                                glm::vec3(0.0f, -0.5f, 1.0f)})),
    m_geomRightTriangle(Polygon2D(this, {glm::vec3(0.f, 0.f, 1.f),
                                glm::vec3(1.f, 0.f, 1.f),
                                glm::vec3(1.f, 1.73205f, 1.f)})),
    m_geomRightTriangleMirrored(Polygon2D(this, {glm::vec3(0.f, 0.f, 1.f),
                                glm::vec3(-1.f, 0.f, 1.f),
                                glm::vec3(-1.f, 1.73205f, 1.f)})),
    m_geomCricle(Polygon2D(this, 120)),
    m_showGrid(true),
    mp_selectedNode(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();

    glDeleteVertexArrays(1, &vao);
    m_geomSquare.destroy();
    m_geomGrid.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
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

    //Create the scene geometry
    m_geomGrid.create();
    m_geomSquare.create();
    m_geomRectangle.create();
    m_geomTriangle.create();
    m_geomRightTriangle.create();
    m_geomRightTriangleMirrored.create();
    m_geomCricle.create();

    // Create and set up the flat lighting shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

    // TODO: Call your scene graph construction function here
    this->root = initializeSceneGraph();
    emit sig_sendRootNode(this->root.get());
}

void MyGL::resizeGL(int w, int h)
{
    glm::mat3 viewMat = glm::scale(glm::mat3(), glm::vec2(0.2, 0.2)); // Screen is -5 to 5

    // Upload the view matrix to our shader (i.e. onto the graphics card)
    prog_flat.setViewMatrix(viewMat);

    printGLErrorLog();
}

// This function is called by Qt any time your GL window is supposed to update
// For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_showGrid)
    {
        prog_flat.setModelMatrix(glm::mat3());
        prog_flat.draw(*this, m_geomGrid);
    }

    // Here is a good spot to call your scene graph traversal function.
    // Any time you want to draw an instance of geometry, call
    // prog_flat.draw(*this, yourNonPointerGeometry);
    traverseSceneGraph(this->root, glm::mat3());
}

uPtr<Node> MyGL::initializeSceneGraph() {
    uPtr<Node> root =  mkU<TranslateNode>("root", nullptr, 0, 0);
    const glm::vec3 purple = glm::vec3(0.44, 0.19, 0.60);
    const glm::vec3 red = glm::vec3(1, 0, 0);
    const glm::vec3 brown = glm::vec3(0.65, 0.35, 0.2);
    const glm::vec3 gray = glm::vec3(0.7, 0.7, 0.7);

    Node &torso_T = root->addNodeChild(mkU<TranslateNode>("torso_T", nullptr, 0, -1));
    Node &torso_R = torso_T.addNodeChild(mkU<RotateNode>("torso_R", nullptr, 0));
    Node &torso_S = torso_R.addNodeChild(mkU<ScaleNode>("torso_S", &m_geomRectangle, 2, 2));
    torso_S.setNodeColor(purple);

    Node &left_arm_T = torso_S.addNodeChild(mkU<TranslateNode>("left_arm_T", nullptr, -0.9, 0.5));
    Node &left_arm_R = left_arm_T.addNodeChild(mkU<RotateNode>("left_arm_R", nullptr, 0));
    Node &left_arm_S = left_arm_R.addNodeChild(mkU<ScaleNode>("left_arm_S", &m_geomSquare, 1.0, 0.3));
    left_arm_S.setNodeColor(purple);
    Node &left_bar_T = left_arm_S.addNodeChild(mkU<TranslateNode>("left_bar_T", nullptr, -0.4, 0.5));
    Node &left_bar_R = left_bar_T.addNodeChild(mkU<RotateNode>("left_bar_R", nullptr, -90));
    Node &left_bar_S = left_bar_R.addNodeChild(mkU<ScaleNode>("left_bar_S", &m_geomSquare, 1.4, 0.2));
    left_bar_S.setNodeColor(brown);
    Node &left_blade_T = left_bar_S.addNodeChild(mkU<TranslateNode>("left_blade_T", nullptr, -1.35, -0.7));
    Node &left_blade_R = left_blade_T.addNodeChild(mkU<RotateNode>("left_blade_R", nullptr, 0));
    Node &left_blade_S = left_blade_R.addNodeChild(mkU<ScaleNode>("left_blade_S", &m_geomSquare, 1.7, 2.5));
    left_blade_S.setNodeColor(gray);
    Node &left_tip_T = left_blade_S.addNodeChild(mkU<TranslateNode>("left_tip_T", nullptr, -0.8, 0.5));
    Node &left_tip_R = left_tip_T.addNodeChild(mkU<RotateNode>("left_tip_R", nullptr, 180));
    Node &left_tip_S = left_tip_R.addNodeChild(mkU<ScaleNode>("left_tip_S", &m_geomRightTriangleMirrored, 0.3, 0.6));
    left_tip_S.setNodeColor(gray);

    Node &right_arm_T = torso_S.addNodeChild(mkU<TranslateNode>("right_arm_T", nullptr, 0.9, 0.5));
    Node &right_arm_R = right_arm_T.addNodeChild(mkU<RotateNode>("right_arm_R", nullptr, 0));
    Node &right_arm_S = right_arm_R.addNodeChild(mkU<ScaleNode>("right_arm_S", &m_geomSquare, 1.0, 0.3));
    right_arm_S.setNodeColor(purple);
    Node &right_bar_T = right_arm_S.addNodeChild(mkU<TranslateNode>("right_bar_T", nullptr, 0.4, 0.5));
    Node &right_bar_R = right_bar_T.addNodeChild(mkU<RotateNode>("right_bar_R", nullptr, 90));
    Node &right_bar_S = right_bar_R.addNodeChild(mkU<ScaleNode>("right_bar_S", &m_geomSquare, 1.4, 0.2));
    right_bar_S.setNodeColor(brown);
    Node &right_blade_T = right_bar_S.addNodeChild(mkU<TranslateNode>("right_blade_T", nullptr, 1.35, -0.7));
    Node &right_blade_R = right_blade_T.addNodeChild(mkU<RotateNode>("right_blade_R", nullptr, 0));
    Node &right_blade_S = right_blade_R.addNodeChild(mkU<ScaleNode>("right_blade_S", &m_geomSquare, 1.7, 2.5));
    right_blade_S.setNodeColor(gray);
    Node &right_tip_T = right_blade_S.addNodeChild(mkU<TranslateNode>("right_tip_T", nullptr, 0.8, 0.5));
    Node &right_tip_R = right_tip_T.addNodeChild(mkU<RotateNode>("right_tip_R", nullptr, -180));
    Node &right_tip_S = right_tip_R.addNodeChild(mkU<ScaleNode>("right_tip_S", &m_geomRightTriangle, 0.3, 0.6));
    right_tip_S.setNodeColor(gray);

    Node &head_T = torso_R.addNodeChild(mkU<TranslateNode>("head_T", nullptr, 0, 3));
    Node &head_R = head_T.addNodeChild(mkU<RotateNode>("head_R", nullptr, 0));
    Node &head_S = head_R.addNodeChild(mkU<ScaleNode>("head_S", &m_geomCricle, 3, 3));
    head_S.setNodeColor(purple);

    Node &left_ear_T = head_S.addNodeChild(mkU<TranslateNode>("left_ear_T", nullptr, -0.6, 0.2));
    Node &left_ear_R = left_ear_T.addNodeChild(mkU<RotateNode>("left_ear_R", nullptr, 0));
    Node &left_ear_S = left_ear_R.addNodeChild(mkU<ScaleNode>("left_ear_S", &m_geomRightTriangle, 0.3, 0.3));
    left_ear_S.setNodeColor(red);

    Node &right_ear_T = head_S.addNodeChild(mkU<TranslateNode>("right_ear_T", nullptr, 0.6, 0.2));
    Node &right_ear_R = right_ear_T.addNodeChild(mkU<RotateNode>("right_ear_R", nullptr, 0));
    Node &right_ear_S = right_ear_R.addNodeChild(mkU<ScaleNode>("right_ear_S", &m_geomRightTriangleMirrored, 0.3, 0.3));
    right_ear_S.setNodeColor(red);

    Node &left_eye_T = head_R.addNodeChild(mkU<TranslateNode>("left_eye_T", nullptr, -1.0, 0.4));
    Node &left_eye_R = left_eye_T.addNodeChild(mkU<RotateNode>("left_eye_R", nullptr, -95));
    Node &left_eye_S = left_eye_R.addNodeChild(mkU<ScaleNode>("left_eye_S", &m_geomRightTriangle, 0.5, 0.4));
    left_eye_S.setNodeColor(red);

    Node &right_eye_T = head_R.addNodeChild(mkU<TranslateNode>("right_eye_T", nullptr, 1.0, 0.4));
    Node &right_eye_R = right_eye_T.addNodeChild(mkU<RotateNode>("right_eye_R", nullptr, 95));
    Node &right_eye_S = right_eye_R.addNodeChild(mkU<ScaleNode>("right_eye_S", &m_geomRightTriangleMirrored, 0.5, 0.4));
    right_eye_S.setNodeColor(red);

    Node &mouth_T = head_R.addNodeChild(mkU<TranslateNode>("mouth_T", nullptr, 0, -0.6));
    Node &mouth_R = mouth_T.addNodeChild(mkU<RotateNode>("mouth_R", nullptr, 5));
    Node &mouth_S = mouth_R.addNodeChild(mkU<ScaleNode>("mouth_S", &m_geomSquare, 1.2, 0.1));
    mouth_S.setNodeColor(red);

    return root;
}

void MyGL::traverseSceneGraph(const uPtr<Node> &node, glm::mat3 transformation) {
    transformation = transformation * node->transformation();

    for(const uPtr<Node> &child : node->getNodeChildren()) {
        traverseSceneGraph(child, transformation);
    }

    if (node->getNodeGeometry() != nullptr) {
        Polygon2D *geom = node->getNodeGeometry();
        geom->setColor(node->getNodeColor());
        prog_flat.setModelMatrix(transformation);
        prog_flat.draw(*this, *geom);
    }
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    switch(e->key())
    {
    case(Qt::Key_Escape):
        QApplication::quit();
        break;

    case(Qt::Key_G):
        m_showGrid = !m_showGrid;
        break;
    }
}

void MyGL::slot_setSelectedNode(QTreeWidgetItem *i) {
    mp_selectedNode = static_cast<Node*>(i);

    if (dynamic_cast<TranslateNode*>(mp_selectedNode)) {
        TranslateNode* node = dynamic_cast<TranslateNode*>(mp_selectedNode);
        emit sig_translateNodeSelected(node->getTx(), node->getTy());
    } else if (dynamic_cast<RotateNode*>(mp_selectedNode)) {
        RotateNode* node = dynamic_cast<RotateNode*>(mp_selectedNode);
        emit sig_rotateNodeSelected(node->getAngle());
    } else if (dynamic_cast<ScaleNode*>(mp_selectedNode)) {
        ScaleNode* node = dynamic_cast<ScaleNode*>(mp_selectedNode);
        emit sig_scaleNodeSelected(node->getSx(), node->getSy());
    } else {
        throw std::runtime_error("Undecalred type of node, this should not happen");
    }

}

void MyGL::slot_setTX(double tx) {
    // TODO update the currently selected Node's
    // X translation value IF AND ONLY IF
    // the currently selected node can be validly
    // dynamic_cast to a TranslateNode.
    // Remember that a failed dynamic_cast
    // will return a null pointer.
    TranslateNode* node = dynamic_cast<TranslateNode*>(mp_selectedNode);
    if (node != nullptr) node->setTx((float) tx);
}

void MyGL::slot_setTY(double ty) {
    TranslateNode* node = dynamic_cast<TranslateNode*>(mp_selectedNode);
    if (node != nullptr) node->setTy((float) ty);
}

void MyGL::slot_setR(double angle) {
    RotateNode* node = dynamic_cast<RotateNode*>(mp_selectedNode);
    if (node != nullptr) node->setAngle((float) angle);
}

void MyGL::slot_setSX(double sx) {
    ScaleNode* node = dynamic_cast<ScaleNode*>(mp_selectedNode);
    if (node != nullptr) node->setSx((float) sx);
}

void MyGL::slot_setSY(double sy) {
    ScaleNode* node = dynamic_cast<ScaleNode*>(mp_selectedNode);
    if (node != nullptr) node->setSy((float) sy);
}

void MyGL::slot_addTranslateNode() {
    // TODO invoke the currently selected Node's
    // addChild function on a newly-instantiated
    // TranslateNode.
    mp_selectedNode->addNodeChild(mkU<TranslateNode>("New Translate Node", nullptr, 0, 0));
}

void MyGL::slot_addRotateNode() {
    mp_selectedNode->addNodeChild(mkU<RotateNode>("New Rotate Node", nullptr, 0));
}

void MyGL::slot_addScaleNode() {
    mp_selectedNode->addNodeChild(mkU<ScaleNode>("New Scale Node", nullptr, 1, 1));
}

void MyGL::slot_setGeometry() {
    mp_selectedNode->setNodeGeometry(&m_geomSquare);
}
