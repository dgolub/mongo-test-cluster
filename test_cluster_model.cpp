/* (C) Copyright 2014, MongoDB, Inc. */

#include "test_cluster_model.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <QBrush>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

const QString TestClusterModel::_columnHeaders[TestClusterModel::COLUMN_MAX] = {
    "Type",
    "Port",
    "DB Path",
    "Replica Set",
    "State"
};

TestClusterModel::TestClusterModel(QObject* parent)
    : QAbstractItemModel(parent), _dirty(false) {}

int TestClusterModel::columnCount(const QModelIndex& parent) const {
    if (parent != QModelIndex()) {
        return 0;
    }
    return COLUMN_MAX;
}

QVariant TestClusterModel::data(const QModelIndex& index, int role) const {
    if (index.row() < 0 || index.row() >= _hosts.size()) {
        return QVariant();
    }
    const HostInfo& info = _hosts[index.row()];
    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case COLUMN_TYPE:
            return getHostTypeName(info.type);
        case COLUMN_PORT:
            return info.port;
        case COLUMN_DBPATH:
            return info.dbPath;
        case COLUMN_REPLICASET:
            return info.replicaSet;
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
    case Qt::ForegroundRole:
        if (index.column() == COLUMN_STATE) {
            switch (info.state) {
            case QProcess::NotRunning:
                return QBrush(Qt::red);
            case QProcess::Starting:
                return QBrush(Qt::darkYellow);
            case QProcess::Running:
                return QBrush(Qt::darkGreen);
            }
        }
        return QVariant();
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

void TestClusterModel::addHost(HostType type, int port, const QString& dbPath, const QString& replicaSet) {
    beginInsertRows(QModelIndex(), _hosts.size(), _hosts.size());
    HostInfo info;
    info.type = type;
    info.port = port;
    info.dbPath = dbPath;
    info.replicaSet = replicaSet;
    info.process = new QProcess(this);
    info.state = QProcess::Starting;
    _hosts.append(info);
    endInsertRows();
    _dirty = true;
}

void TestClusterModel::startHost(const QModelIndex& index) {
    if (index.row() < 0 || index.row() >= _hosts.size()) {
        return;
    }
    HostInfo& info = _hosts[index.row()];
    if (info.process->state() != QProcess::NotRunning) {
        return;
    }
    QString program = (info.type == HOST_TYPE_MONGOS) ? "mongos" : "mongod";
    QStringList arguments;
    arguments.append("--port");
    arguments.append(QString::number(info.port));
    if (info.type != HOST_TYPE_MONGOS) {
        arguments.append("--dbpath");
        arguments.append(info.dbPath);
    }
    if (info.type != HOST_TYPE_MONGOS && info.type != HOST_TYPE_CONFIG && !info.replicaSet.isEmpty()) {
        arguments.append("--replSet");
        arguments.append(info.replicaSet);
    }
    if (info.type == HOST_TYPE_SHARD) {
        arguments.append("--shardsvr");
    } else if (info.type == HOST_TYPE_CONFIG) {
        arguments.append("--configsvr");
    }
    info.consoleOutput.clear();
#ifdef _WIN32
    // On Windows, ensure that signals are enabled so that the process being spawned can be stopped using CTRL+C.
    ::SetConsoleCtrlHandler(NULL, FALSE);
#endif
    info.process->start(program, arguments);
}

void TestClusterModel::stopHost(const QModelIndex& index) {
    if (index.row() < 0 || index.row() >= _hosts.size()) {
        return;
    }
    const HostInfo& info = _hosts[index.row()];
    if (info.process->state() == QProcess::NotRunning) {
        return;
    }
#ifdef _WIN32
    // On Windows, QProcess::terminate uses an implementation appropriate for GUI applications but not for console
    // applications, so we need to do this manually using Win32.
    ::SetConsoleCtrlHandler(NULL, TRUE);
    ::AttachConsole(info.process->pid()->dwProcessId);
    ::GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
    ::FreeConsole();
#else
    info.process->terminate();
#endif
}

void TestClusterModel::updateHosts() {
    _anyStarted = false;
    _anyStopped = false;
    for (int i = 0; i < _hosts.size(); i++) {
        HostInfo& info = _hosts[i];
        if (info.state != info.process->state()) {
            info.state = info.process->state();
            QModelIndex index = createIndex(i, COLUMN_STATE);
            emit dataChanged(index, index);
        }
        _anyStarted = _anyStarted || info.state != QProcess::NotRunning;
        _anyStopped = _anyStopped || info.state == QProcess::NotRunning;
        if (info.state != QProcess::NotRunning) {
            QByteArray newConsoleOutput = info.process->readAll();
            if (!newConsoleOutput.isEmpty()) {
                info.consoleOutput += newConsoleOutput;
                emit consoleOutputChanged(i);
            }
        }
    }
}

int TestClusterModel::hostPort(const QModelIndex& index) const {
    if (index.row() < 0 || index.row() >= _hosts.size()) {
        return 0;
    }
    return _hosts[index.row()].port;
}

QString TestClusterModel::hostConsoleOutput(const QModelIndex& index) const {
    if (index.row() < 0 || index.row() >= _hosts.size()) {
        return QString();
    }
    return _hosts[index.row()].consoleOutput;
}

bool TestClusterModel::anyStarted() const {
    return _anyStarted;
}

bool TestClusterModel::anyStopped() const {
    return _anyStopped;
}

bool TestClusterModel::isDirty() const {
    return _dirty;
}

bool TestClusterModel::hasFileName() const {
    return !_fileName.isEmpty();
}

void TestClusterModel::startAllHosts() {
    for (int i = 0; i < _hosts.size(); i++) {
        startHost(createIndex(i, 0));
    }
}

void TestClusterModel::stopAllHosts() {
    for (int i = 0; i < _hosts.size(); i++) {
        stopHost(createIndex(i, 0));
    }
}

void TestClusterModel::clearCluster() {
    for (const HostInfo& info : _hosts) {
        if (info.process != nullptr) {
            delete info.process;
        }
    }
    if (!_hosts.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, _hosts.size() - 1);
        _hosts.clear();
        endRemoveRows();
    }
    _fileName.clear();
    _dirty = false;
}

void TestClusterModel::saveToFile() {
    saveToFile(_fileName);
}

void TestClusterModel::saveToFile(const QString& fileName) {
    QJsonArray array;
    for (const HostInfo& info : _hosts) {
        QJsonObject jsonInfo;
        jsonInfo.insert("type", QString(getHostTypeInternalName(info.type)));
        jsonInfo.insert("port", info.port);
        if (!info.dbPath.isEmpty()) {
            jsonInfo.insert("dbPath", info.dbPath);
        }
        if (!info.replicaSet.isEmpty()) {
            jsonInfo.insert("replicaSet", info.replicaSet);
        }
        array.append(jsonInfo);
    }
    QJsonDocument doc(array);
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(doc.toJson());
    file.close();
    _fileName = fileName;
    _dirty = false;
}

bool TestClusterModel::loadFromFile(const QString& fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QByteArray bytes = file.readAll();
    file.close();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(bytes, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return false;
    }
    if (!doc.isArray()) {
        return false;
    }
    QList<HostInfo> newHosts;
    QJsonArray array = doc.array();
    for (const QJsonValue& value : array) {
        HostInfo info;
        QJsonObject object = value.toObject();
        QJsonValue typeVal = object["type"];
        if (!typeVal.isString()) {
            return false;
        }
        int type = findHostTypeByInternalName(typeVal.toString().toUtf8().data());
        if (type < 0) {
            return false;
        }
        info.type = (HostType)type;
        QJsonValue portVal = object["port"];
        if (!portVal.isDouble()) {
            return false;
        }
        info.port = static_cast<int>(portVal.toDouble());
        QJsonValue dbPathVal = object["dbPath"];
        if (dbPathVal.isString()) {
            info.dbPath = dbPathVal.toString();
        }
        QJsonValue replicaSetVal = object["replicaSet"];
        if (replicaSetVal.isString()) {
            info.replicaSet = replicaSetVal.toString();
        }
        newHosts.append(info);
    }
    for (HostInfo& info : newHosts) {
        info.process = new QProcess(this);
        info.state = QProcess::NotRunning;
    }
    beginInsertRows(QModelIndex(), 0, newHosts.size() - 1);
    _hosts = newHosts;
    endInsertRows();
    _fileName = fileName;
    _dirty = false;
    return true;
}
