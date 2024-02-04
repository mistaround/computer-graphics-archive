#pragma once
#include <QTreeWidgetItem>
#include "smartpointerhelp.h"
#include "polygon.h"

class Node : public QTreeWidgetItem {
    private:
        // * A set of unique_ptrs to the node's children.
        std::vector<uPtr<Node>> children;

        // * A raw, C-style pointer to one instance of Polygon2D.
        Polygon2D *geometry;

        // * The color with which to draw the Polygon2D pointed to by the node, if applicable.
        glm::vec3 color;

        // * A QString to represent a name for the node.
        QString name;

    public:
        // Constructors
        Node(const QString name, Polygon2D *geometry);
        Node(const Node &node);
        virtual uPtr<Node> copy() const = 0; // for copy constructor since this is an abstract class

        // Destructors
        virtual ~Node();

        // Methods
        // * A purely virtual function that computes and returns a 3x3 homogeneous matrix representing the transformation in the node.
        virtual glm::mat3 transformation() = 0;

        // * A function that adds a given unique_ptr as a child to this node.
        Node &addNodeChild(uPtr<Node> node);

        // * A function that allows the user to modify the color stored in this node.
        void setNodeColor(const glm::vec3 &color);

        const std::vector<uPtr<Node>> &getNodeChildren();
        const glm::vec3 getNodeColor() const;
        const QString getNodeName() const;

        void setNodeGeometry(Polygon2D *geometry);
        Polygon2D *getNodeGeometry() const;
};

class TranslateNode : public Node {
    private:
        float tx;
        float ty;

    public:
        TranslateNode(const QString name, Polygon2D *geometry, const float tx, const float ty);
        TranslateNode(const TranslateNode &node);
        uPtr<Node> copy() const override {
            return mkU<TranslateNode>(*this);
        }

        glm::mat3 transformation() override;
        float getTx();
        float getTy();
        void setTx(float tx);
        void setTy(float ty);
};

class RotateNode : public Node {
    private:
        float angle;

    public:
        RotateNode(const QString name, Polygon2D *geometry, const float angle);
        RotateNode(const RotateNode &node);
        uPtr<Node> copy() const override {
            return mkU<RotateNode>(*this);
        }

        glm::mat3 transformation() override;
        float getAngle();
        void setAngle(float angle);
};

class ScaleNode : public Node {
    private:
        float sx;
        float sy;

    public:
        ScaleNode(const QString name, Polygon2D *geometry, const float sx, const float sy);
        ScaleNode(const ScaleNode &node);
        uPtr<Node> copy() const override {
            return mkU<ScaleNode>(*this);
        }

        glm::mat3 transformation() override;
        float getSx();
        float getSy();
        void setSx(float sx);
        void setSy(float sy);
};
