#ifndef NODEINSNAPSHOTTABLEMODEL_H
#define NODEINSNAPSHOTTABLEMODEL_H

#include "../data/ChaosAbstractListModel.h"

#include <QSharedPointer>

class NodeInSnapshotTableModel:
        public ChaosAbstractListModel {
    Q_OBJECT
public:
    NodeInSnapshotTableModel(QObject *parent = 0);
    ~NodeInSnapshotTableModel();
    void updateSnapshotList(ChaosUniquePtr<chaos::metadata_service_client::api_proxy::service::GetNodesForSnapshotHelper> _node_in_snapshot_helper);
    void reset();
protected:
    int getRowCount() const;
    QVariant getRowData(int row) const;
    QVariant getUserData(int row) const;
private:
    ChaosUniquePtr<chaos::metadata_service_client::api_proxy::service::GetNodesForSnapshotHelper> node_in_snapshot_helper;
};

#endif // NODEINSNAPSHOTTABLEMODEL_H
