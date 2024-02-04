#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();
    // Function Button
    connect(ui->loadMeshButton, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_loadMesh()));
    connect(ui->loadSkeletonButton, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_loadSkeleton()));
    connect(ui->addVertexButton, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_addVertex()));
    connect(ui->triangulateButton, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_triangulate()));
    connect(ui->subdivisionButton, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_subdivision()));
    connect(ui->BindMeshButton, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_bindMesh()));
    // Update Visual Display
    connect(ui->vertsListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            ui->mygl, SLOT(slot_setChosenVertex(QListWidgetItem*)));
    connect(ui->facesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            ui->mygl, SLOT(slot_setChosenFace(QListWidgetItem*)));
    connect(ui->halfEdgesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            ui->mygl, SLOT(slot_setChosenHalfEdge(QListWidgetItem*)));
    connect(ui->jointsTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            ui->mygl, SLOT(slot_setChosenJoint(QTreeWidgetItem*)));
    // Update List Display
    connect(ui->mygl, SIGNAL(sig_buildComponentList(Mesh*)),
            this, SLOT(slot_buildComponentList(Mesh*)));
    connect(ui->mygl, SIGNAL(sig_buildJointList(Skeleton*)),
            this, SLOT(slot_buildJointList(Skeleton*)));
    // Update Value Display
    connect(ui->mygl, &MyGL::sig_updatePosition, [this](const QVector3D& position) {
        ui->vertPosXSpinBox->setValue(position.x());
        ui->vertPosYSpinBox->setValue(position.y());
        ui->vertPosZSpinBox->setValue(position.z());
    });
    connect(ui->mygl, &MyGL::sig_updateColor, [this](const QColor& color) {
        ui->faceRedSpinBox->setValue(color.redF());
        ui->faceGreenSpinBox->setValue(color.greenF());
        ui->faceBlueSpinBox->setValue(color.blueF());
    });
    connect(ui->mygl, &MyGL::sig_updateJointPos, [this](const QVector3D& position) {
        ui->jointPosXSpinBox->setValue(position.x());
        ui->jointPosYSpinBox->setValue(position.y());
        ui->jointPosZSpinBox->setValue(position.z());
    });
    connect(ui->mygl, &MyGL::sig_updateRotation, [this](const QVector3D& rotation) {
        ui->Roll->setText(QString::number(rotation.x()));
        ui->Pitch->setText(QString::number(rotation.y()));
        ui->Yaw->setText(QString::number(rotation.z()));
    });
    // Vertex Position
    connect(ui->vertPosXSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setPositionX(double)));
    connect(ui->vertPosYSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setPositionY(double)));
    connect(ui->vertPosZSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setPositionZ(double)));
    // Face Color
    connect(ui->faceRedSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setColorR(double)));
    connect(ui->faceGreenSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setColorG(double)));
    connect(ui->faceBlueSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setColorB(double)));
    // Joint Position
    connect(ui->jointPosXSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setJointPosX(double)));
    connect(ui->jointPosYSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setJointPosY(double)));
    connect(ui->jointPosZSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setJointPosZ(double)));
    // Joint Rotation
    connect(ui->plusX, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_setJointRotXP()));
    connect(ui->plusY, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_setJointRotYP()));
    connect(ui->plusZ, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_setJointRotZP()));
    connect(ui->minusX, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_setJointRotXM()));
    connect(ui->minusY, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_setJointRotYM()));
    connect(ui->minusZ, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_setJointRotZM()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}

void MainWindow::slot_buildComponentList(Mesh* mesh)
{
    for (auto& vert : mesh->verts) {
        ui->vertsListWidget->addItem(vert.get());
    }
    for (auto& edge : mesh->edges) {
        ui->halfEdgesListWidget->addItem(edge.get());
    }
    for (auto& face : mesh->faces) {
        ui->facesListWidget->addItem(face.get());
    }
}

void MainWindow::slot_buildJointList(Skeleton* skeleton)
{
    for (auto &joint : skeleton->joints) {
        ui->jointsTreeWidget->addTopLevelItem(joint.get());
    }
}
