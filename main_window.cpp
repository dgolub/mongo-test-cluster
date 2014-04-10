/* (C) Copyright 2014, MongoDB, Inc. */

#include "main_window.h"
#include "add_host_dialog.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    _ui.setupUi(this);
    connect(_ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(_ui.pushButtonAddHost, SIGNAL(clicked()), this, SLOT(addHost()));
}

void MainWindow::addHost() {
    AddHostDialog dlg(this);
    dlg.exec();
}
