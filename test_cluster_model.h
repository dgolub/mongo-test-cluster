/* (C) Copyright 2014, MongoDB, Inc. */

#pragma once

#include <QAbstractItemModel>
#include <QProcess>

#include "host_types.h"

class TestClusterModel : public QAbstractItemModel {
    Q_OBJECT
public:
    TestClusterModel(QObject* parent = nullptr);

    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& index) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    void addHost(HostType type, int port, const QString& dbPath, const QString& replicaSet);
    void startHost(const QModelIndex& index);
    void stopHost(const QModelIndex& index);
    void updateHosts();

    int hostPort(const QModelIndex& index) const;
    QString hostConsoleOutput(const QModelIndex& index) const;
    bool anyStarted() const;
    bool anyStopped() const;
    bool isDirty() const;
    bool hasFileName() const;

    void startAllHosts();
    void stopAllHosts();

    void clearCluster();
    void saveToFile();
    void saveToFile(const QString& fileName);
    bool loadFromFile(const QString& fileName);

signals:
    void consoleOutputChanged(int row);

private:
    enum Column {
        COLUMN_TYPE,
        COLUMN_PORT,
        COLUMN_DBPATH,
        COLUMN_REPLICASET,
        COLUMN_STATE,
        COLUMN_MAX
    };

    struct HostInfo {
        HostType type;
        int port;
        QString dbPath;
        QString replicaSet;
        QProcess* process;
        QProcess::ProcessState state;
        QString consoleOutput;
    };

    static const QString _columnHeaders[COLUMN_MAX];

    QList<HostInfo> _hosts;
    bool _anyStarted;
    bool _anyStopped;
    bool _dirty;
    QString _fileName;
};
