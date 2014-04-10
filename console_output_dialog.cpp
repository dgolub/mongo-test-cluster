/* (C) Copyright 2014, MongoDB, Inc. */

#include "console_output_dialog.h"
#include "test_cluster_model.h"

ConsoleOutputDialog::ConsoleOutputDialog(
    const TestClusterModel* model,
    const QModelIndex& index,
    QWidget* parent)
    : QDialog(parent)
{
    _ui.setupUi(this);
    _ui.plainTextEdit->setPlainText(model->hostConsoleOutput(index));
}
