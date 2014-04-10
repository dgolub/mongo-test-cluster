#include "test_cluster_model.h"

const QString TestClusterModel::_columnHeaders[TestClusterModel::COLUMN_MAX] = {
    "Port",
    "DB Path",
    "Type"
};

TestClusterModel::TestClusterModel(QObject* parent)
    : QAbstractTableModel(parent) {}

int TestClusterModel::columnCount(const QModelIndex& parent) const {
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

int TestClusterModel::rowCount(const QModelIndex& parent) const {
    return _hosts.size();
}

void TestClusterModel::addHost(int port, const QString& dbPath, HostType type) {
    beginInsertRows(QModelIndex(), _hosts.size(), _hosts.size());
    HostInfo info;
    info.port = port;
    info.dbPath = dbPath;
    info.type = type;
    _hosts.append(info);
    endInsertRows();
}
