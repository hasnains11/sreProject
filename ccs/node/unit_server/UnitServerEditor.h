#ifndef UNITSERVEREDITOR_H
#define UNITSERVEREDITOR_H

#include "../../search/SearchNodeResult.h"
#include "../../presenter/PresenterWidget.h"
#include "../../widget/CLedIndicatorHealt.h"
#include "../../data/LogProcessEntriesTableModel.h"

#include <QItemSelection>
#include <QStringListModel>
#include <QStandardItemModel>

namespace Ui {
class UnitServerEditor;
}

class UnitServerEditor :
        public PresenterWidget
{
    Q_OBJECT
    const QString node_unique_id;
    //list model
    QStringListModel *list_model_cu_type;

    //! hel p to colelct the information for the api
    chaos::metadata_service_client::api_proxy::control_unit::SetInstanceDescriptionHelper instance_description_helper;

    //the lis tof the isntances setuped for the unit server
    std::vector<QSharedPointer<chaos::common::data::CDataWrapper> > instance_list;

    //add an instance to the table
    void fillTableWithInstance(QSharedPointer<chaos::common::data::CDataWrapper> cu_instance);
public:
    explicit UnitServerEditor(const QString& _node_unique_id);
    ~UnitServerEditor();

private slots:
    void customMenuRequested(QPoint pos);

    void updateAll();

    void on_pushButtonCreateNewInstance_clicked();

    void handleSelectionChanged(const QItemSelection& selection);

    void on_pushButtonUpdateAllInfo_clicked();

    void on_pushButtonRemoveInstance_clicked();

    void on_pushButtonEditInstance_clicked();

    void tableCurrentChanged(const QModelIndex &current,
                             const QModelIndex &previous);
    void tableSelectionChanged(const QItemSelection& selected,
                               const QItemSelection& unselected);
//----------control unti slot----------------
    void cuInstanceLoadSelected();
    void cuInstanceUnloadSelected();
    void cuInstanceInitSelected();
    void cuInstanceDeinitSelected();
    void cuInstanceStartSelected();
    void cuInstanceStopSelected();

    void duplicateInstance();
    void moveToUnitServer();
    void copyToUnitServer();

    void on_pushButtonUpdateControlUnitType_clicked();
    void on_pushButtonAddNewCUType_clicked();
    void on_pushButtonRemoveCUType_clicked();

    void changedNodeOnlineStatus(const QString& node_uid,
                                chaos::metadata_service_client::node_monitor::OnlineState alive_state);
    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_pushButtonStartSearchEntry_clicked();

    void on_pushButtonLoadMoreEntries_clicked();

protected:
    void initUI();
    bool isClosing();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
protected slots:
        void selectedUnitServer(const QString& tag, const QVector<QPair<QString,QString> >& selected_item);
private:
    LogProcessEntriesTableModel us_log_entries_tab_model;
    SearchNodeResult    *move_copy_search_instance;
    QStandardItemModel *table_model;
    Ui::UnitServerEditor *ui;
};

#endif // UNITSERVEREDITOR_H
