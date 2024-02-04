#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mesh.h"
#include "skeleton.h"
#include <QMainWindow>


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();

    void on_actionCamera_Controls_triggered();

    void slot_buildComponentList(Mesh*);

    void slot_buildJointList(Skeleton*);

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
