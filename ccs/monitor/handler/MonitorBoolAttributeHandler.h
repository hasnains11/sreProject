#ifndef MONITORBOOLATTRIBUTEHANDLER
#define MONITORBOOLATTRIBUTEHANDLER

#include "AbstractAttributeHandler.h"

class MonitorBoolAttributeHandler:
        public AbstractAttributeHandler,
        public chaos::metadata_service_client::monitor_system::QuantumKeyAttributeBoolHandler {
    Q_OBJECT
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const bool value) {
        //emit new value
        emit valueUpdated(QString::fromStdString(key),
                          QString::fromStdString(attribute),
                          QVariant::fromValue<bool>(value));
    }
    void consumeValueNotFound(const std::string& key,
                              const std::string& attribute) {
        //emit value not foud
        emit valueNotFound(QString::fromStdString(key),
                           QString::fromStdString(attribute));
    }
public:
    MonitorBoolAttributeHandler(const QString& attribute_name,
                                bool event_on_change = false):
        AbstractAttributeHandler(this),
        chaos::metadata_service_client::monitor_system::QuantumKeyAttributeBoolHandler(attribute_name.toStdString(),
                                                                                       event_on_change){}
};

#endif // MONITORBOOLATTRIBUTEHANDLER

