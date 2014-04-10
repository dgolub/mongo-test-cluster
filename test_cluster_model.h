/* (C) Copyright 2014, MongoDB, Inc. */

#pragma once

#include <QAbstractTableModel>

#include "host_types.h"

class TestClusterModel : public QAbstractTableModel {
public:
    TestClusterModel(QObject* parent = nullptr);

    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    void addHost(int port, const QString& dbPath, HostType type);

private:
    enum Column {
        COLUMN_PORT,
        COLUMN_DBPATH,
        COLUMN_TYPE,
        COLUMN_MAX
    };

    struct HostInfo {
        int port;
        QString dbPath;
        HostType type;
    };

    static const QString _columnHeaders[COLUMN_MAX];

    QList<HostInfo> _hosts;
};
