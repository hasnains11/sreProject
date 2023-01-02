#ifndef CDATASETATTRIBUTESETVALUECOMBOBOX_H
#define CDATASETATTRIBUTESETVALUECOMBOBOX_H


#include "../ChaosBaseDatasetAttributeUI.h"

#include <QMap>
#include <QSize>
#include <QVariant>
#include <QPalette>
#include <QComboBox>

class CDatasetAttributeSetValueComboBox:
        public ChaosBaseDatasetAttributeUI {
    Q_OBJECT
    Q_PROPERTY(QString setup READ setup WRITE setSetup)
public:
    explicit CDatasetAttributeSetValueComboBox(QWidget *parent = nullptr);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    void setSetup(QString new_combo_label_value);
    QString setup();
private slots:
    void changeSetCommitted();
    void changeSetRollback();
    void currentIndexChanged(int index);
public slots:
    virtual void reset();
protected:
    void updateOnline(ChaosBaseDatasetUI::OnlineState state);
    void updateValue(QVariant variant_value);
private:
    bool value_committed;
    QComboBox *combo_box;
    QString p_setup;
    QMap<QVariant, QString> map_value_label;
};

#endif // CDATASETATTRIBUTESETVALUECOMBOBOX_H
