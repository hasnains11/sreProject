#include "ChaosAbstractTableModel.h"

#include <QDebug>

ChaosAbstractTableModel::ChaosAbstractTableModel(QObject *parent)
    :QAbstractTableModel(parent) {
}

int ChaosAbstractTableModel::rowCount(const QModelIndex& parent) const
{
    return getRowCount();
}

int ChaosAbstractTableModel::columnCount(const QModelIndex& parent) const
{
    return getColumnCount();
}

QVariant ChaosAbstractTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return getHeaderForColumn(section);
        }
    }
    return QVariant();
}

QVariant ChaosAbstractTableModel::getCellData(int row, int column) const {
    return QVariant();
}

QVariant ChaosAbstractTableModel::getCellUserData(int row, int column) const {
    return QVariant();
}

QVariant ChaosAbstractTableModel::getCheckeable(int row, int column) const {
    return QVariant();
}

QVariant ChaosAbstractTableModel::getFontForData(int row, int column) const {
    return QVariant();
}

QVariant ChaosAbstractTableModel::getBackgroundForData(int row, int column) const {
    return QVariant();
}

QVariant ChaosAbstractTableModel::getTextColorForData(int row, int column) const {
    return QVariant();
}

QVariant ChaosAbstractTableModel::getTextAlignForData(int row, int column) const {
    return QVariant();
}

QVariant ChaosAbstractTableModel::getCheckStateForData(int row, int column) const {
    return QVariant();
}

QVariant ChaosAbstractTableModel::getTooltipTextForData(int row, int column) const {
    return QVariant();
}

bool ChaosAbstractTableModel::setCellData(const QModelIndex& index, const QVariant& value) {
    return false;
}

QVariant ChaosAbstractTableModel::data(const QModelIndex& index, int role) const {
    int row = index.row();
    int col = index.column();

    QVariant result;
    if (!index.isValid())
        return QVariant();
    switch(role){
    case Qt::DisplayRole:
        result = getCellData(row, col);
        break;
    case Qt::UserRole:
        result = getCellUserData(row, col);
        break;
    case Qt::CheckStateRole:
        result = getCheckeable(row, col);
        break;
    case Qt::FontRole:
        result = getFontForData(row, col);
        break;
    case Qt::BackgroundRole:
        result = getBackgroundForData(row, col);
        break;
    case Qt::TextColorRole:
        result = getTextColorForData(row, col);
        break;
    case Qt::TextAlignmentRole:
        result = getTextAlignForData(row, col);
        break;
    case Qt::ToolTipRole:
        result = getTooltipTextForData(row, col);
        break;
    default:
        break;
    }
    return result;
}

bool ChaosAbstractTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    bool result = false;
    if (index.isValid() &&
            (role == Qt::EditRole || role == Qt::CheckStateRole)) {
        if(result = setCellData(index, value)){
            emit(dataChanged(index, index));
        }
    }
    return result;
}

bool ChaosAbstractTableModel::isCellEditable(const QModelIndex &index) const {
    return false;
}

bool ChaosAbstractTableModel::isCellCheckable(const QModelIndex &index) const {
    return false;
}

bool ChaosAbstractTableModel::isCellSelectable(const QModelIndex &index) const {
    return false;
}

Qt::ItemFlags ChaosAbstractTableModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = Qt::ItemIsEnabled;
    if (!index.isValid()) return flags;
    if(isCellCheckable(index)) {
        flags |= Qt::ItemIsUserCheckable;
    } else if(isCellEditable(index)) {
        flags |= Qt::ItemIsEditable;
    } else if(isCellSelectable(index)) {
        flags |= Qt::ItemIsSelectable;
    }
    return flags;
}
