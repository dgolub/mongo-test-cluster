/* (C) Copyright 2014, MongoDB, Inc. */

#pragma once

#include <QDialog>

#include "ui_console_output_dialog.h"

class TestClusterModel;

class ConsoleOutputDialog : public QDialog {
    Q_OBJECT
public:
    ConsoleOutputDialog(const TestClusterModel* model, const QModelIndex& index, QWidget* parent = nullptr);

private slots:
    void consoleOutputChanged(int row);

private:
    Ui::ConsoleOutputDialog _ui;
    const TestClusterModel* _model;
    QModelIndex _index;
};
