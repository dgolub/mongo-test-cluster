/* (C) Copyright 2014, MongoDB, Inc. */

#pragma once

#include <QDialog>

#include "ui_console_output_dialog.h"

class TestClusterModel;

class ConsoleOutputDialog : public QDialog {
public:
    ConsoleOutputDialog(const TestClusterModel* model, const QModelIndex& index, QWidget* parent = nullptr);

private:
    Ui::ConsoleOutputDialog _ui;
};
