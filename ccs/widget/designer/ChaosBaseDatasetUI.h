#ifndef CHAOSUIBASE_H
#define CHAOSUIBASE_H

#include <QFrame>
#include <QVariant>

#include <chaos/common/chaos_constants.h>

#include "ChaosBaseUI.h"

class ChaosBaseDatasetUI:
        public ChaosBaseUI {
    Q_OBJECT
    Q_PROPERTY(QString deviceID READ deviceID WRITE setDeviceID)
    Q_PROPERTY(DatasetType datasetType READ datasetType WRITE setDatasetType)

public:
    typedef enum {
        OnlineStateNotFound,
        OnlineStateUnknown,
        OnlineStateON,
        OnlineStateOFF
    } OnlineState;

    enum DatasetType { Output = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT,
                       Input = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_INPUT,
                       Custom = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM,
                       System = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM,
                       Health = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_HEALTH,
                       DevAlarm = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM,
                       CUAlarm  = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM,
                       Unset};

    Q_ENUM(DatasetType)

    ChaosBaseDatasetUI(QWidget *parent = nullptr);

    void setDeviceID(const QString &new_device_id);
    QString deviceID() const;

    void setDatasetType(const DatasetType &new_dataset_type);
    DatasetType datasetType() const;

    QVariant value();

    QJsonObject serialize();
    void deserialize(QJsonObject& serialized_data);

    bool isInEditMode();
signals:
    void attributeValueChanged(QVariant old_value,
                               QVariant new_value);
public slots:
    void chaosWidgetEditMode(bool new_edit_mode);
    virtual void updateOnlineStateSlot(int state);
    virtual void updateData(int dataset_type,
                            QString attribute_name,
                            QVariant attribute_value);
protected:
    virtual void updateOnline(ChaosBaseDatasetUI::OnlineState state) = 0;
    virtual void updateValue(QVariant variant_value) = 0;

private:
    bool edit_mode;
    QVariant p_value;
    QString p_device_id;
    DatasetType p_dataset_type;
};

#endif // CHAOSUIBASE_H
