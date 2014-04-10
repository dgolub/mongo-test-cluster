/* (C) Copyright 2014, MongoDB, Inc. */

#include "console_output_dialog.h"
#include "test_cluster_model.h"

ConsoleOutputDialog::ConsoleOutputDialog(
    const TestClusterModel* model,
    const QModelIndex& index,
    QWidget* parent)
    : QDialog(parent), _model(model), _index(index)
{
    _ui.setupUi(this);
    _ui.plainTextEdit->setPlainText(model->hostConsoleOutput(index));
    connect(model, SIGNAL(consoleOutputChanged(int)), this, SLOT(consoleOutputChanged(int)));
}

void ConsoleOutputDialog::consoleOutputChanged(int row) {
    if (_index.row() == row) {
        _ui.plainTextEdit->setPlainText(_model->hostConsoleOutput(_index));
    }
}
