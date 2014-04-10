/* (C) Copyright 2014, MongoDB, Inc. */

#pragma once

#include <QDialog>

#include "ui_add_host_dialog.h"

class AddHostDialog : public QDialog {
    Q_OBJECT
public:
    AddHostDialog(QWidget* parent = nullptr);

    virtual void accept();

private slots:
    void browseForDBPath();

private:
    Ui::AddHostDialog _ui;
};
