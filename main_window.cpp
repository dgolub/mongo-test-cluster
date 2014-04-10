/* (C) Copyright 2014, MongoDB, Inc. */

#include "main_window.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    _ui.setupUi(this);
    connect(_ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
}
