/* (C) Copyright 2014, MongoDB, Inc. */

#include "main_window.h"
#include "add_host_dialog.h"
#include "console_output_dialog.h"
#include "test_cluster_model.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QProgressDialog>
#include <QSet>
#include <QTime>

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
    _hostContextMenu->addSeparator();
    _openMongoShellAction = _hostContextMenu->addAction("Open Mongo Shell");
    _hostContextMenu->addSeparator();
    _removeAction = _hostContextMenu->addAction("Remove");

    _updateTimer = new QTimer(this);
    _updateTimer->start(100);

    _ui.actionNewCluster->setShortcut(Qt::CTRL + Qt::Key_N);
    _ui.actionOpenCluster->setShortcut(Qt::CTRL + Qt::Key_O);
    _ui.actionSaveCluster->setShortcut(Qt::CTRL + Qt::Key_S);

    connect(_ui.actionNewCluster, SIGNAL(triggered()), this, SLOT(newCluster()));
    connect(_ui.actionOpenCluster, SIGNAL(triggered()), this, SLOT(openCluster()));
    connect(_ui.actionSaveCluster, SIGNAL(triggered()), this, SLOT(saveCluster()));
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

void MainWindow::closeEvent(QCloseEvent* event) {
    if (!promptToSave()) {
        event->ignore();
        return;
    }
    if (_model->anyStarted()) {
        int result = QMessageBox::question(
            this,
            QString(),
            "One or more hosts are still running.  If you exit, they will be stopped.  "
            "Are you sure you want to exit?");
        if (result == QMessageBox::No) {
            event->ignore();
            return;
        }
        stopAllHosts();
        waitForAllStopped();
    }
    event->accept();
}

void MainWindow::waitForAllStopped() {
    QProgressDialog dlg(this);
    dlg.setRange(0, 0);
    dlg.setLabelText("Waiting for all hosts to stop...");
    dlg.setCancelButton(nullptr);
    dlg.show();
    QTime startTime = QTime::currentTime();
    while (_model->anyStarted() && startTime.secsTo(QTime::currentTime()) < 10) {
        QApplication::processEvents();
    }
}

bool MainWindow::promptToSave() {
    if (!_model->isDirty()) {
        return true;
    }
    int result = QMessageBox::question(
        this,
        QString(),
        "Do you want to save your changes?",
        QMessageBox::Yes,
        QMessageBox::No,
        QMessageBox::Cancel);
    if (result == QMessageBox::Cancel) {
        return false;
    } else if (result == QMessageBox::No) {
        return true;
    }
    saveCluster();
    return !_model->isDirty();
}

bool MainWindow::promptToStopAll() {
    if (!_model->anyStarted()) {
        return true;
    }
    int result = QMessageBox::question(
        this,
        QString(),
        "One or more hosts are still running.  If you continue, they will be stopped.  "
        "Do you wish to proceed?");
    if (result == QMessageBox::No) {
        return false;
    }
    stopAllHosts();
    waitForAllStopped();
    return true;
}

void MainWindow::newCluster() {
    if (!promptToSave() || !promptToStopAll()) {
        return;
    }
    _model->clearCluster();
}

void MainWindow::openCluster() {
    if (!promptToSave() || !promptToStopAll()) {
        return;
    }
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

void MainWindow::saveCluster() {
    if (_model->hasFileName()) {
        _model->saveToFile();
    } else {
        saveClusterAs();
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
    QSet<QString> replicaSets;
    for (int i = 0; i < _model->rowCount(); i++) {
        QString rs = _model->hostReplicaSet(_model->index(i, 0));
        if (!rs.isEmpty()) {
            replicaSets.insert(rs);
        }
    }
    AddHostDialog dlg(replicaSets.toList(), this);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }
    _model->addHost(dlg.type(), dlg.port(), dlg.dbPath(), dlg.replicaSet(), dlg.configDB());
}

void MainWindow::doHostContextMenu(const QPoint& pos) {
    QModelIndexList indexes = _ui.treeView->selectionModel()->selectedRows();
    if (indexes.isEmpty()) {
        return;
    }
    bool anyStarted = false;
    bool anyStopped = false;
    for (QModelIndex index : indexes) {
        bool running = _model->hostRunning(index);
        anyStarted = anyStarted || running;
        anyStopped = anyStopped || !running;
    }
    _startHostAction->setEnabled(anyStopped);
    _stopHostAction->setEnabled(anyStarted);
    _openMongoShellAction->setEnabled(anyStarted && indexes.size() == 1);
    _removeAction->setEnabled(!anyStarted);
    QAction* action = _hostContextMenu->exec(_ui.treeView->mapToGlobal(pos));
    for (QModelIndex index : indexes) {
        if (action == _startHostAction) {
            _model->startHost(index);
        } else if (action == _stopHostAction) {
            _model->stopHost(index);
        } else if (action == _openMongoShellAction) {
            QStringList arguments;
            arguments.append("--port");
            arguments.append(QString::number(_model->hostPort(index)));
            QProcess().startDetached("mongo", arguments);
        } else if (action == _removeAction) {
            _model->removeHost(index);
        }
    }
}

void MainWindow::hostDoubleClicked(const QModelIndex& index) {
    ConsoleOutputDialog dlg(_model, index, this);
    dlg.exec();
}

void MainWindow::updateHosts() {
    _model->updateHosts();
    _ui.pushButtonStartAll->setEnabled(_model->anyStopped());
    _ui.pushButtonStopAll->setEnabled(_model->anyStarted());
}
