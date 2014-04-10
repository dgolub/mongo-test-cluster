/* (C) Copyright 2014, MongoDB, Inc. */

#include "add_host_dialog.h"
#include "host_types.h"

#include <QDir>
#include <QFileDialog>
#include <QIntValidator>
#include <QMessageBox>

AddHostDialog::AddHostDialog(QWidget* parent)
    : QDialog(parent)
{
    _ui.setupUi(this);

    connect(_ui.toolButtonDBPath, SIGNAL(clicked()), this, SLOT(browseForDBPath()));

    // Give the dialog a fixed border.
    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

    // Force the port to be an integer in the correct range.
    _ui.lineEditPort->setValidator(new QIntValidator(1, 0xFFFF, this));

    // Add all host types to the combo box.
    for (int i = 0; i < HOST_TYPE_MAX; i++) {
        _ui.comboBoxType->addItem(getHostTypeName(i));
    }
}

HostType AddHostDialog::type() const {
    return (HostType)_ui.comboBoxType->currentIndex();
}

int AddHostDialog::port() const {
    return _ui.lineEditPort->text().toInt();
}

QString AddHostDialog::dbPath() const {
    return _ui.lineEditDBPath->text();
}

QString AddHostDialog::replicaSet() const {
    return _ui.lineEditReplicaSet->text();
}

void AddHostDialog::accept() {
    // Verify that the required fields are specified.
    if (_ui.lineEditPort->text().isEmpty()) {
        QMessageBox::critical(this, QString(), "Please enter a port number.");
        return;
    }
    if (_ui.lineEditDBPath->text().isEmpty()) {
        QMessageBox::critical(this, QString(), "Please enter a database path.");
        return;
    }
    
    // Prompt to create the DB path if it doesn't exist.
    QDir dbPath(_ui.lineEditDBPath->text());
    if (!dbPath.exists()) {
        int result = QMessageBox::question(
            this,
            QString(),
            "The specified database path does not exist.  Do you want to create it?");
        if (result != QMessageBox::Yes) {
            return;
        }
        if (!dbPath.mkpath(".")) {
            QMessageBox::critical(this, QString(), "Failed to create the database path!");
            return;
        }
    }

    QDialog::accept();
}

void AddHostDialog::browseForDBPath() {
    QString newDBPath = QFileDialog::getExistingDirectory(
        this,
        "Browse for DB Path",
        _ui.lineEditDBPath->text());
    if (newDBPath.isEmpty()) {
        return;
    }
    _ui.lineEditDBPath->setText(newDBPath);
}
