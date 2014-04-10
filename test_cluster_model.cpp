/* (C) Copyright 2014, MongoDB, Inc. */

#include "test_cluster_model.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

const QString TestClusterModel::_columnHeaders[TestClusterModel::COLUMN_MAX] = {
    "Port",
    "DB Path",
    "Type",
    "State"
};

TestClusterModel::TestClusterModel(QObject* parent)
    : QAbstractItemModel(parent) {}

int TestClusterModel::columnCount(const QModelIndex& parent) const {
    if (parent != QModelIndex()) {
        return 0;
    }
    return COLUMN_MAX;
}

QVariant TestClusterModel::data(const QModelIndex& index, int role) const {
    if (role != Qt::DisplayRole || index.row() < 0 || index.row() >= _hosts.size()) {
        return QVariant();
    }
    const HostInfo& info = _hosts[index.row()];
    switch (index.column()) {
    case COLUMN_PORT:
        return info.port;
    case COLUMN_DBPATH:
        return info.dbPath;
    case COLUMN_TYPE:
        return getHostTypeName(info.type);
    case COLUMN_STATE:
        switch (info.state) {
        case QProcess::NotRunning:
            return "Stopped";
        case QProcess::Starting:
            return "Starting";
        case QProcess::Running:
            return "Running";
        default:
            return "Unknown";
        }
    default:
        return QVariant();
    }
}

QVariant TestClusterModel::headerData(int section, Qt::Orientation orientation, int role) const {
    switch (role) {
    case Qt::DisplayRole:
        if (section < 0 || section >= COLUMN_MAX || orientation != Qt::Horizontal) {
            return QVariant();
        }
        return _columnHeaders[section];
    case Qt::TextAlignmentRole:
        return Qt::AlignLeft;
    default:
        return QAbstractItemModel::headerData(section, orientation, role);
    }
}

QModelIndex TestClusterModel::index(int row, int column, const QModelIndex& parent) const {
    return createIndex(row, column);
}

QModelIndex TestClusterModel::parent(const QModelIndex& index) const {
    return QModelIndex();
}

int TestClusterModel::rowCount(const QModelIndex& parent) const {
    if (parent != QModelIndex()) {
        return 0;
    }
    return _hosts.size();
}

void TestClusterModel::addHost(int port, const QString& dbPath, HostType type) {
    beginInsertRows(QModelIndex(), _hosts.size(), _hosts.size());
    HostInfo info;
    info.port = port;
    info.dbPath = dbPath;
    info.type = type;
    info.process = new QProcess(this);
    info.state = QProcess::Starting;
    _hosts.append(info);
    endInsertRows();
}

void TestClusterModel::startHost(const QModelIndex& index) {
    if (index.row() < 0 || index.row() >= _hosts.size()) {
        return;
    }
    const HostInfo& info = _hosts[index.row()];
    QString program = (info.type == HOST_TYPE_MONGOS) ? "mongos" : "mongod";
    QStringList arguments;
    arguments.append("--port");
    arguments.append(QString::number(info.port));
    if (info.type != HOST_TYPE_MONGOS) {
        arguments.append("--dbpath");
        arguments.append(info.dbPath);
    }
    if (info.type == HOST_TYPE_SHARD) {
        arguments.append("--shardsvr");
    } else if (info.type == HOST_TYPE_CONFIG) {
        arguments.append("--configsvr");
    }
    info.process->start(program, arguments);
}

void TestClusterModel::stopHost(const QModelIndex& index) {
    if (index.row() < 0 || index.row() >= _hosts.size()) {
        return;
    }
    const HostInfo& info = _hosts[index.row()];
#ifdef _WIN32
    // On Windows, QProcess::terminate uses an implementation appropriate for GUI applications but not for console
    // applications, so we need to do this manually using Win32.
    ::AttachConsole(info.process->pid()->dwProcessId);
    ::SetConsoleCtrlHandler(NULL, TRUE);
    ::GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
    ::SetConsoleCtrlHandler(NULL, FALSE);
    ::FreeConsole();
#else
    info.process->terminate();
#endif
}

void TestClusterModel::updateHostStates() {
    for (int i = 0; i < _hosts.size(); i++) {
        HostInfo& info = _hosts[i];
        if (info.state != info.process->state()) {
            info.state = info.process->state();
            QModelIndex index = createIndex(i, COLUMN_STATE);
            emit dataChanged(index, index);
        }
    }
}
