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
    _ui.treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    _hostContextMenu = new QMenu(this);
    _startHostAction = _hostContextMenu->addAction("Start");
    _stopHostAction = _hostContextMenu->addAction("Stop");

    connect(_ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(_ui.pushButtonAddHost, SIGNAL(clicked()), this, SLOT(addHost()));
    connect(
        _ui.treeView,
        SIGNAL(customContextMenuRequested(const QPoint&)),
        this,
        SLOT(doHostContextMenu(const QPoint&)));
}

void MainWindow::addHost() {
    AddHostDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }
    _model->addHost(dlg.port(), dlg.dbPath(), dlg.type());
}

void MainWindow::doHostContextMenu(const QPoint& pos) {
    QAction* action = _hostContextMenu->exec(_ui.treeView->mapToGlobal(pos));
    if (action == _startHostAction) {
        _model->startHost(_ui.treeView->currentIndex());
    } else if (action == _stopHostAction) {
        _model->stopHost(_ui.treeView->currentIndex());
    }
}
