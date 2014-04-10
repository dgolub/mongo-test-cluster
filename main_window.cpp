/* (C) Copyright 2014, MongoDB, Inc. */

#include "main_window.h"
#include "add_host_dialog.h"
#include "test_cluster_model.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    _model = new TestClusterModel(this);

    _ui.setupUi(this);
    _ui.treeView->setModel(_model);

    connect(_ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(_ui.pushButtonAddHost, SIGNAL(clicked()), this, SLOT(addHost()));
}

void MainWindow::addHost() {
    AddHostDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }
    _model->addHost(dlg.port(), dlg.dbPath(), dlg.type());
}
