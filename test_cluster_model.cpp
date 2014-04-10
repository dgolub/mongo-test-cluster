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
    return QVariant();
}

QVariant TestClusterModel::headerData(int section, Qt::Orientation orientation, int role) const {
    switch (role) {
    case Qt::DisplayRole:
        if (section < 0 || section >= COLUMN_MAX) {
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
    return 0;
}
