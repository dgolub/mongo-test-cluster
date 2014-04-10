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

private slots:
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
