/* (C) Copyright 2014, MongoDB, Inc. */

#pragma once

#include <QDialog>

#include "host_types.h"
#include "ui_add_host_dialog.h"

class AddHostDialog : public QDialog {
    Q_OBJECT
public:
    AddHostDialog(const QStringList& replicaSets, QWidget* parent = nullptr);

    HostType type() const;
    int port() const;
    QString dbPath() const;
    QString replicaSet() const;
    QString configDB() const;

    virtual void accept();

private slots:
    void browseForDBPath();
    void typeChanged(int index);

private:
    Ui::AddHostDialog _ui;
};
