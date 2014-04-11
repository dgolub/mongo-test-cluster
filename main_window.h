/* (C) Copyright 2014, MongoDB, Inc. */

#pragma once

#include <QAction>
#include <QMainWindow>
#include <QMenu>
#include <QTimer>

#include "ui_main_window.h"

class TestClusterModel;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);

protected:
    virtual void closeEvent(QCloseEvent* event);

private:
    void waitForAllStopped();
    bool promptToSave();
    bool promptToStopAll();

private slots:
    void newCluster();
    void openCluster();
    void saveCluster();
    void saveClusterAs();
    void startAllHosts();
    void stopAllHosts();
    void addHost();
    void doHostContextMenu(const QPoint& pos);
    void hostDoubleClicked(const QModelIndex& index);
    void updateHosts();

private:
    Ui::MainWindow _ui;
    TestClusterModel* _model;
    QMenu* _hostContextMenu;
    QAction* _startHostAction;
    QAction* _stopHostAction;
    QTimer* _updateTimer;
};
