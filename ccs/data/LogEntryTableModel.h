#ifndef LOGENTRYTABLEMODEL_H
#define LOGENTRYTABLEMODEL_H

#include "ChaosAbstractTableModel.h"
#include "../api_async_processor/ApiSubmitter.h"

#include <string>
#include <vector>

class LogEntryTableModel:
        public ChaosAbstractTableModel,
        public ApiHandler {
public:
    LogEntryTableModel(QObject *parent = 0);

    //!update log entries for node uid as emitter and log domain list to inclue
    void updateEntriesList(const QString& node_uid,
                            const chaos::metadata_service_client::api_proxy::logging::LogDomainList& domain_list);
    ChaosSharedPtr<chaos::metadata_service_client::api_proxy::logging::LogEntry> getLogEntryForRow(unsigned int row);
    virtual void clear();
    void setMaxResultItem(uint32_t _number_of_max_result);
protected:
    int getRowCount() const;
    int getColumnCount() const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;
    QVariant getCellUserData(int row, int column) const;
    QVariant getTooltipTextForData(int row, int column) const;
    QVariant getTextAlignForData(int row, int column) const;
    bool isCellSelectable(const QModelIndex &index) const;

    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
protected:
    bool show_source_column;
    uint32_t number_of_max_result;
    ApiSubmitter api_submitter;
    chaos::metadata_service_client::api_proxy::logging::LogDomainList domain_list;
    ChaosUniquePtr<chaos::metadata_service_client::api_proxy::logging::GetLogForSourceUIDHelper> helper;
};

#endif // LOGENTRYTABLEMODEL_H
