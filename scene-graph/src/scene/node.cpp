#include "node.h"

/* ------------------- Node -------------------- */

Node::Node(const QString name, Polygon2D *geometry) :
    children(std::vector<uPtr<Node>>()), geometry(geometry), name(name) {
    this->setText(0, name);
}

Node::Node(const Node &node) :
    QTreeWidgetItem(node), geometry(node.geometry), name(node.name) {
    for (const auto &child : node.children) {
        uPtr<Node> copy = child->copy();
        copy->setNodeColor(node.color);
        children.push_back(std::move(copy));
    }
}

Node::~Node() {
    // default, all handled by smart pointers
}

Node &Node::addNodeChild(uPtr<Node> node) {
    Node &ref = *node;
    this->addChild(&ref);
    this->children.push_back(std::move(node));
    return ref;
}

void Node::setNodeColor(const glm::vec3 &color) {
    this->color = color;
}

const std::vector<uPtr<Node>> &Node::getNodeChildren() {
    return this->children;
}

const glm::vec3 Node::getNodeColor() const {
    return this->color;
}


const QString Node::getNodeName() const {
    return this->name;
}

void Node::setNodeGeometry(Polygon2D *geometry) {
    this->geometry = geometry;
}

Polygon2D *Node::getNodeGeometry() const {
    return this->geometry;
}

/* ------------------- TranslateNode -------------------- */

TranslateNode::TranslateNode(const QString name, Polygon2D *geometry, const float tx, const float ty)
    : Node(name, geometry), tx(tx), ty(ty) {}

TranslateNode::TranslateNode(const TranslateNode &node)
    : Node(node), tx(node.tx), ty(node.ty) {}

glm::mat3 TranslateNode::transformation() {
    return glm::translate(glm::mat3(), glm::vec2(this->tx, this->ty));
}

float TranslateNode::getTx() {
    return this->tx;
}

float TranslateNode::getTy() {
    return this->ty;
}

void TranslateNode::setTx(float tx) {
    this->tx = tx;
}

void TranslateNode::setTy(float ty) {
    this->ty = ty;
}

/* ------------------- RotateNode -------------------- */

RotateNode::RotateNode(const QString name, Polygon2D *geometry, const float angle)
    : Node(name, geometry), angle(angle) {}

RotateNode::RotateNode(const RotateNode &node)
    : Node(node), angle(node.angle) {}

glm::mat3 RotateNode::transformation() {
    return glm::rotate(glm::mat3(), glm::radians(this->angle));
}

float RotateNode::getAngle() {
    return this->angle;
}

void RotateNode::setAngle(float angle) {
    this->angle = angle;
}


/* ------------------- ScaleNode -------------------- */

ScaleNode::ScaleNode(const QString name, Polygon2D *geometry, const float sx, const float sy)
    : Node(name, geometry), sx(sx), sy(sy) {}

ScaleNode::ScaleNode(const ScaleNode &node)
    : Node(node), sx(node.sx), sy(node.sy) {}

glm::mat3 ScaleNode::transformation() {
    return glm::scale(glm::mat3(), glm::vec2(this->sx, this->sy));
}

float ScaleNode::getSx() {
    return this->sx;
}

float ScaleNode::getSy() {
    return this->sy;
}

void ScaleNode::setSx(float sx) {
    this->sx = sx;
}

void ScaleNode::setSy(float sy) {
    this->sy = sy;
}

