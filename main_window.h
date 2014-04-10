/* (C) Copyright 2014, MongoDB, Inc. */

#pragma once

#include <QMainWindow>

#include "ui_main_window.h"

class TestClusterModel;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);

private slots:
    void addHost();

private:
    Ui::MainWindow _ui;
    TestClusterModel* _model;
};
