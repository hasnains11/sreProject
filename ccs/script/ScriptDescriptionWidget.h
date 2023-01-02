#ifndef SCRIPTDESCRIPTIONWIDGET_H
#define SCRIPTDESCRIPTIONWIDGET_H

#include <QWidget>
#include <QVector>
#include <QTextEdit>
#include <QItemSelection>
#include <QStringListModel>
#include <QSharedPointer>
#include "../language_editor/LanguageEditorSupport.h"
#include "../api_async_processor/ApiSubmitter.h"
#include "../data/EditableDatasetTableModel.h"
#include "../data/EditableScriptVariableTableModel.h"
#include "../utility/WidgetUtility.h"
namespace Ui {
class ScriptDescriptionWidget;
}

class ScriptDescriptionWidget :
        public QWidget,
        public ApiHandler,
        public WidgetUtilityhandler {
    Q_OBJECT

    void updateTextEditorFeatures();
    void fillScriptWithGUIValues();
    void updateLanguages();
public:
    explicit ScriptDescriptionWidget(QWidget *parent = 0);
    explicit ScriptDescriptionWidget(const chaos::service_common::data::script::Script& _script, QWidget *parent = 0);
    ~ScriptDescriptionWidget();
    QString getScriptName();

protected:

    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);

    void cmActionTrigger(const QString& cm_title,
                                     const QVariant& cm_data);
private slots:
    void updateScripUI();
    void on_comboBoxsScirptLanguage_currentIndexChanged(int index);

    void on_pushButtonSaveScript_clicked();

    void on_pushButtonUpdateAll_clicked();

    void on_pushButtonAddAttributeToDataset_clicked();

    void on_tableViewDataset_doubleClicked(const QModelIndex &index);

    void handleModelSelectionChanged(const QItemSelection& selected,const QItemSelection& deselected);

    void on_pushButtonremoveAttributeToDataset_clicked();

    void on_pushButtonSelectClass_clicked();

    void selectedGroupPath(const QString& selection_tag,
                           const QStringList& selected_groups);

    void on_pushButtonRemoveSelectedClass_clicked();

    void on_pushButtonSelectExecutionPools_clicked();

    void on_pushButtonRemoveSelectedExecutionPools_clicked();

private:
    WidgetUtility widget_utility;
    Ui::ScriptDescriptionWidget *ui;
    QStringListModel execution_pool_model;
    QStringListModel classification_model;
    EditableDatasetTableModel editable_dataset_table_model;
    QSharedPointer<LanguageEditorSupport> language_support;
    QMap< QString, QSharedPointer<LanguageEditorSupport> > languages;
    ApiSubmitter api_submitter;
    //declre the script serialization wrapper
    chaos::service_common::data::script::ScriptSDWrapper script_wrapper;
};

#endif // SCRIPTDESCRIPTIONWIDGET_H
