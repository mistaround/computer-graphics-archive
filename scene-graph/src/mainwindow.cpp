#include "mainwindow.h"
#include <ui_mainwindow.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();

    // Connects MyGL's signal that contains the root node of
    // your scene graph to a slot in MainWindow that adds the
    // root node to the GUI's Tree Widget.
            // Widget that emits the signal
    connect(ui->mygl,
            // Signal name
            SIGNAL(sig_sendRootNode(QTreeWidgetItem*)),
            // Widget with the slot that receives the signal
            this,
            // Slot name
            SLOT(slot_addRootToTreeWidget(QTreeWidgetItem*)));

    // Connects the Tree Widget's signal containing the Node that you
    // clicked on to MyGL's slot that updates MyGL's mp_selectedNode
    // member variable to the clicked Node.
    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            ui->mygl, SLOT(slot_setSelectedNode(QTreeWidgetItem*)));

    // Update the node value dynamiclly
    connect(ui->mygl, SIGNAL(sig_translateNodeSelected(float,float)),
            this, SLOT(slot_updateTranslateNode(float,float)));
    connect(ui->mygl, SIGNAL(sig_rotateNodeSelected(float)),
            this, SLOT(slot_updateRotateNode(float)));
    connect(ui->mygl, SIGNAL(sig_scaleNodeSelected(float,float)),
            this, SLOT(slot_updateScaleNode(float,float)));

    // Connects the X-translate spin box's signal containing its new value
    // to MyGL, which has a slot that will update the selected node's
    // X-translate value (you have to go to mygl.cpp and implement
    // the slot, we have just provided a dummy definition)
    connect(ui->txSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setTX(double)));
    // TODO: Mirroring the above syntax, connect spin box signals
    // to slots in MyGL that update the other transformation attributes
    connect(ui->tySpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setTY(double)));
    connect(ui->rSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setR(double)));
    connect(ui->sxSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setSX(double)));
    connect(ui->sySpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setSY(double)));
    // Connects the "Add Translate Node" button's "clicked" signal
    // to a slot in MyGL that will add a child to the currently selected
    // Node.
    connect(ui->tNodeAddButton, SIGNAL(clicked()),
            ui->mygl, SLOT(slot_addTranslateNode()));
    connect(ui->rNodeAddButton, SIGNAL(clicked()),
            ui->mygl, SLOT(slot_addRotateNode()));
    connect(ui->sNodeAddButton, SIGNAL(clicked()),
            ui->mygl, SLOT(slot_addScaleNode()));
    connect(ui->geomSetButton, SIGNAL(clicked()),
            ui->mygl, SLOT(slot_setGeometry()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::slot_addRootToTreeWidget(QTreeWidgetItem *i) {
    ui->treeWidget->addTopLevelItem(i);
}

void MainWindow::slot_updateTranslateNode(float tx, float ty) {
    ui->txSpinBox->setValue((double) tx);
    ui->tySpinBox->setValue((double) ty);
    ui->txSpinBox->setDisabled(false);
    ui->tySpinBox->setDisabled(false);
    ui->rSpinBox->setDisabled(true);
    ui->sxSpinBox->setDisabled(true);
    ui->sySpinBox->setDisabled(true);
}

void MainWindow::slot_updateRotateNode(float angle) {
    ui->rSpinBox->setValue((double) angle);
    ui->txSpinBox->setDisabled(true);
    ui->tySpinBox->setDisabled(true);
    ui->rSpinBox->setDisabled(false);
    ui->sxSpinBox->setDisabled(true);
    ui->sySpinBox->setDisabled(true);
}

void MainWindow::slot_updateScaleNode(float sx, float sy) {
    ui->sxSpinBox->setValue((double) sx);
    ui->sySpinBox->setValue((double) sy);
    ui->txSpinBox->setDisabled(true);
    ui->tySpinBox->setDisabled(true);
    ui->rSpinBox->setDisabled(true);
    ui->sxSpinBox->setDisabled(false);
    ui->sySpinBox->setDisabled(false);
}
