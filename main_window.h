/* (C) Copyright 2014, MongoDB, Inc. */

#pragma once

#include <QMainWindow>

#include "ui_main_window.h"

class MainWindow : public QMainWindow {
public:
    MainWindow(QWidget* parent = nullptr);

private:
    Ui::MainWindow _ui;
};
