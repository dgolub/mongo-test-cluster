/* (C) Copyright 2014, MongoDB, Inc. */

#include "add_host_dialog.h"
#include "host_types.h"

#include <QCompleter>
#include <QDir>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QIntValidator>
#include <QMessageBox>
#include <QStringListModel>

AddHostDialog::AddHostDialog(const QStringList& replicaSets, QWidget* parent)
    : QDialog(parent)
{
    _ui.setupUi(this);

    connect(_ui.toolButtonDBPath, SIGNAL(clicked()), this, SLOT(browseForDBPath()));
    connect(_ui.comboBoxType, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChanged(int)));

    // Give the dialog a fixed border.
    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

    // Force the port to be an integer in the correct range.
    _ui.lineEditPort->setValidator(new QIntValidator(1, 0xFFFF, this));

    // Enable path auto-completion.
    QCompleter* completer = new QCompleter(this);
    QFileSystemModel* model = new QFileSystemModel(completer);
    model->setRootPath(QDir::currentPath());
    completer->setModel(model);
    _ui.lineEditDBPath->setCompleter(completer);

    // Enable replica set auto-completion.
    if (!replicaSets.isEmpty()) {
        QCompleter* rsCompleter = new QCompleter(this);
        rsCompleter->setModel(new QStringListModel(replicaSets, rsCompleter));
        _ui.lineEditReplicaSet->setCompleter(rsCompleter);
    }

    // Add all host types to the combo box.
    for (int i = 0; i < HOST_TYPE_MAX; i++) {
        _ui.comboBoxType->addItem(getHostTypeName(i));
    }

    // Default to a standard host.
    _ui.comboBoxType->setCurrentIndex(HOST_TYPE_STANDARD);
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

QString AddHostDialog::configDB() const {
    return _ui.lineEditConfigDB->text();
}

void AddHostDialog::accept() {
    // Verify that the required fields are specified.
    int type = _ui.comboBoxType->currentIndex();
    if (_ui.lineEditPort->text().isEmpty()) {
        QMessageBox::critical(this, QString(), "Please enter a port number.");
        return;
    }
    if (_ui.lineEditDBPath->text().isEmpty() && type != HOST_TYPE_MONGOS) {
        QMessageBox::critical(this, QString(), "Please enter a database path.");
        return;
    }
    if (_ui.lineEditConfigDB->text().isEmpty() && type == HOST_TYPE_MONGOS) {
        QMessageBox::critical(this, QString(), "Please enter a configuration database.");
        return;
    }
    
    // Prompt to create the DB path if it doesn't exist.
    if (!_ui.labelDBPath->text().isEmpty()) {
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

void AddHostDialog::typeChanged(int index) {
    if (index == HOST_TYPE_MONGOS) {
        _ui.lineEditDBPath->clear();
    }
    _ui.lineEditDBPath->setEnabled(index != HOST_TYPE_MONGOS);
    _ui.toolButtonDBPath->setEnabled(index != HOST_TYPE_MONGOS);
    if (index == HOST_TYPE_MONGOS || index == HOST_TYPE_CONFIG) {
        _ui.lineEditReplicaSet->clear();
    }
    _ui.lineEditReplicaSet->setEnabled(index != HOST_TYPE_MONGOS && index != HOST_TYPE_CONFIG);
    _ui.lineEditConfigDB->setEnabled(index == HOST_TYPE_MONGOS);
}
