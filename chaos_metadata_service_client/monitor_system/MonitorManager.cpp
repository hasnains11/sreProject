/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include <chaos_metadata_service_client/monitor_system/MonitorManager.h>

using namespace chaos::common::async_central;
using namespace chaos::metadata_service_client::monitor_system;

#define MM_INFO   INFO_LOG(MonitorManager)
#define MM_DBG    INFO_LOG(MonitorManager)
#define MM_ERR    INFO_LOG(MonitorManager)

MonitorManager::MonitorManager(chaos::common::network::NetworkBroker *_network_broker,
                               chaos::metadata_service_client::ClientSetting *_setting):
setting(_setting),
slot_scheduler(NULL),
network_broker(_network_broker){}

MonitorManager::~MonitorManager() {}

void MonitorManager::init(void *init_data)  {
    CHAOS_LASSERT_EXCEPTION(network_broker, MM_ERR, -1, "No network broker instance found")
    if(slot_scheduler==NULL){
    	slot_scheduler = new QuantumSlotScheduler(network_broker);
    }
    CHAOS_LASSERT_EXCEPTION(slot_scheduler, MM_ERR, -2, "Error allcaoting quantum slot scheduler")
    
    StartableService::initImplementation(slot_scheduler, init_data, "QuantumSlotScheduler", __PRETTY_FUNCTION__);
}

void MonitorManager::start()  {
    StartableService::startImplementation(slot_scheduler, "QuantumSlotScheduler", __PRETTY_FUNCTION__);
    
    //add timer for purge operation
    AsyncCentralManager::getInstance()->addTimer(this,
                                                 5000,
                                                 5000);
}

void MonitorManager::stop()  {
    //remove time for purge oepration
    AsyncCentralManager::getInstance()->removeTimer(this);
    
    //remove all append consumer
    purgeKeyConsumer(true);
    
    
    StartableService::stopImplementation(slot_scheduler, "QuantumSlotScheduler", __PRETTY_FUNCTION__);
}

void MonitorManager::deinit()  {
    StartableService::deinitImplementation(slot_scheduler, "QuantumSlotScheduler", __PRETTY_FUNCTION__);
    if(slot_scheduler){
    	delete slot_scheduler;
    	slot_scheduler=NULL;
    }
}

void MonitorManager::resetEndpointList(std::vector<std::string> new_server_list) {
    slot_scheduler->setDataDriverEndpoints(new_server_list);
}

void MonitorManager::addKeyConsumer(const std::string& key_to_monitor,
                                    int quantum_multiplier,
                                    QuantumSlotConsumer *consumer,
                                    unsigned int consumer_priority) {
    //ad consumer to the scheduler
    slot_scheduler->addKeyConsumer(key_to_monitor,
                                   quantum_multiplier,
                                   consumer,
                                   consumer_priority);
}

//! add a new handler for a single attribute for a key
void MonitorManager::addKeyAttributeHandler(const std::string& key_to_monitor,
                                            int quantum_multiplier,
                                            AbstractQuantumKeyAttributeHandler *attribute_handler,
                                            unsigned int consumer_priority) {
    if(attribute_handler == NULL) return;
    boost::unique_lock<boost::mutex>   lock(map_mutex);
    
    QuantumKeyConsumer *consumer = NULL;
    
    //create unique key
    std::string unique_slot_key = CHAOS_QSS_COMPOSE_QUANTUM_SLOT_KEY(key_to_monitor, quantum_multiplier);
    
    //chec if we already have the consumer allocated
    if(map_quantum_key_consumer.count(unique_slot_key)) {
        //we already have the key consumer
        consumer = map_quantum_key_consumer[unique_slot_key];
    }else {
        //we need to allocate the key consumer
        map_quantum_key_consumer.insert(make_pair(unique_slot_key, (consumer = new QuantumKeyConsumer(key_to_monitor))));
        
        MM_INFO << boost::str(boost::format("Created new consumer slot for %1% with pointer %2%")%unique_slot_key%consumer);
        
        //add consuemr to the scheduler
        addKeyConsumer(key_to_monitor,
                       quantum_multiplier,
                       consumer);
    }
    
    consumer->addAttributeHandler(attribute_handler);
}

bool MonitorManager::removeKeyConsumer(const std::string& key_to_monitor,
                                       int quantum_multiplier,
                                       QuantumSlotConsumer *consumer,
                                       bool wait_completion) {
    return slot_scheduler->removeKeyConsumer(key_to_monitor,
                                             quantum_multiplier,
                                             consumer,
                                             wait_completion);
}

//! remove an handler associated to ans attirbute of a key
void MonitorManager::removeKeyAttributeHandler(const std::string& key_to_monitor,
                                               int quantum_multiplier,
                                               AbstractQuantumKeyAttributeHandler *attribute_handler) {
    if(attribute_handler == NULL) return;
    boost::unique_lock<boost::mutex>   lock(map_mutex);
    
    //create unique key
    std::string unique_slot_key = CHAOS_QSS_COMPOSE_QUANTUM_SLOT_KEY(key_to_monitor, quantum_multiplier);
    
    //we already have the key consumer
    QuantuKeyConsumerMapIterator it = map_quantum_key_consumer.find(unique_slot_key);
    if(it == map_quantum_key_consumer.end()) return;
    
    //remove the attribute within the consumer
    it->second->removeAttributeHandler(attribute_handler);
    
    //check if the attribute is the last, in this case we eed to remove all the consumer
    if(it->second->size() == 0) {
        //remove consumer from the scheduler without wait the completion
        removeKeyConsumer(key_to_monitor,
                          quantum_multiplier,
                          it->second,
                          false);
        
        //add key consumer to the queue of to purge
        //delete(it->second);
        boost::unique_lock<boost::mutex> wl(mutex_queue_to_purge);
        queue_to_purge.push(it->second);
        
        //remove elemento form the map of used key consumer
        map_quantum_key_consumer.erase(it);
    }
}

void MonitorManager::timeout() {
    purgeKeyConsumer(false);
}

void MonitorManager::purgeKeyConsumer(bool all) {
    
    bool end_purge_operation = false;
    int max_to_purge = 3;
    QuantumKeyConsumer *consumer = NULL;
    boost::unique_lock<boost::mutex> wl(mutex_queue_to_purge);
    while(queue_to_purge.size() &&
          !end_purge_operation ) {
        consumer = queue_to_purge.front(); queue_to_purge.pop();
        MM_INFO << boost::str(boost::format("Auto purged key handler consumer slot for pointer %1% queue left size %2%")%consumer%queue_to_purge.size());
        consumer->waitForCompletion();
        delete(consumer);
        if(!all){
            //end when we have processed max number of element
            end_purge_operation = (--max_to_purge>=0);
        }
    }
}
