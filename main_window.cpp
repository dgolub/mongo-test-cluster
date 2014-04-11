/* (C) Copyright 2014, MongoDB, Inc. */

#include "main_window.h"
#include "add_host_dialog.h"
#include "console_output_dialog.h"
#include "test_cluster_model.h"

#include <QFileDialog>
#include <QMessageBox>

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

    _updateTimer = new QTimer(this);
    _updateTimer->start(100);

    connect(_ui.actionOpenCluster, SIGNAL(triggered()), this, SLOT(openCluster()));
    connect(_ui.actionSaveClusterAs, SIGNAL(triggered()), this, SLOT(saveClusterAs()));
    connect(_ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(_ui.pushButtonStartAll, SIGNAL(clicked()), this, SLOT(startAllHosts()));
    connect(_ui.pushButtonStopAll, SIGNAL(clicked()), this, SLOT(stopAllHosts()));
    connect(_ui.pushButtonAddHost, SIGNAL(clicked()), this, SLOT(addHost()));
    connect(
        _ui.treeView,
        SIGNAL(customContextMenuRequested(const QPoint&)),
        this,
        SLOT(doHostContextMenu(const QPoint&)));
    connect(
        _ui.treeView,
        SIGNAL(doubleClicked(const QModelIndex&)),
        this,
        SLOT(hostDoubleClicked(const QModelIndex&)));
    connect(_updateTimer, SIGNAL(timeout()), this, SLOT(updateHosts()));
}

void MainWindow::openCluster() {
    QString fileName = QFileDialog::getOpenFileName(
        this,
        QString(),
        QString(),
        "JSON Files (*.json)");
    if (fileName.isEmpty()) {
        return;
    }
    if (!_model->loadFromFile(fileName)) {
        QMessageBox::critical(this, QString(), "Failed to load cluster from file!");
    }
}

void MainWindow::saveClusterAs() {
    QString fileName = QFileDialog::getSaveFileName(
        this,
        QString(),
        QString(),
        "JSON Files (*.json)");
    if (fileName.isEmpty()) {
        return;
    }
    _model->saveToFile(fileName);
}

void MainWindow::startAllHosts() {
    _model->startAllHosts();
}

void MainWindow::stopAllHosts() {
    _model->stopAllHosts();
}

void MainWindow::addHost() {
    AddHostDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }
    _model->addHost(dlg.type(), dlg.port(), dlg.dbPath(), dlg.replicaSet());
}

void MainWindow::doHostContextMenu(const QPoint& pos) {
    QModelIndexList indexes = _ui.treeView->selectionModel()->selectedRows();
    if (indexes.isEmpty()) {
        return;
    }
    QAction* action = _hostContextMenu->exec(_ui.treeView->mapToGlobal(pos));
    for (QModelIndex index : indexes) {
        if (action == _startHostAction) {
            _model->startHost(index);
        } else if (action == _stopHostAction) {
            _model->stopHost(index);
        }
    }
}

void MainWindow::hostDoubleClicked(const QModelIndex& index) {
    ConsoleOutputDialog dlg(_model, index, this);
    dlg.exec();
}

void MainWindow::updateHosts() {
    _model->updateHosts();
}
