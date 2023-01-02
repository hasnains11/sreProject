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

#include <chaos_metadata_service_client/node_monitor/NodeFetcher.h>
#include <chaos_metadata_service_client/node_monitor/ControlUnitController.h>

using namespace chaos::metadata_service_client::node_monitor;
using namespace chaos::metadata_service_client::monitor_system;

#define NF_INFO INFO_LOG(chaos::metadata_service_client::monitor_system::NodeFetcher)
#define NF_DBG DBG_LOG(chaos::metadata_service_client::monitor_system::NodeFetcher)
#define NF_ERR ERR_LOG(chaos::metadata_service_client::monitor_system::NodeFetcher)


NodeFetcher::NodeFetcher(chaos::metadata_service_client::monitor_system::MonitorManager *_monitor_manager,
                         const std::string& _node_uid):
node_uid(_node_uid),
monitor_manager(_monitor_manager){}

NodeFetcher::~NodeFetcher() {
    //deregister for receive key for controller not removed by user
    for(NodeTypeControllerMapIterator it = map_type_controller.begin(),
        end = map_type_controller.end();
        it!=end;
        it++) {
        //unlisten on controller key
        managmentKeyToMonitor(it->second->getMonitorKeyList(), false);
    }
}

void NodeFetcher::quantumSlotHasData(const std::string& key,
                                     const KeyValue& value) {
    for(NodeTypeControllerMapIterator it = map_type_controller.begin(),
        end = map_type_controller.end();
        it!=end;
        it++) {
        it->second->quantumSlotHasData(key,
                                       value);
    }
}

void NodeFetcher::quantumSlotHasNoData(const std::string& key) {
    for(NodeTypeControllerMapIterator it = map_type_controller.begin(),
        end = map_type_controller.end();
        it!=end;
        it++) {
        it->second->quantumSlotHasNoData(key);
    }
}

ChaosSharedPtr<NodeController>  NodeFetcher::getControllerForType(ControllerType type) {
    ChaosSharedPtr<NodeController> controller;
    
    if(map_type_controller.count(type) == 0) {
        switch(type){
            case ControllerTypeNode:{
                NF_INFO << "Create new node controller["<<type<<"]";
                
                controller.reset(new NodeController(node_uid));
                break;
            }
                
            case ControllerTypeNodeControlUnit:{
                NF_INFO << "Create new control unit controller["<<type<<"]";
                controller.reset(new ControlUnitController(node_uid));
                break;
            }
        }
        
        //registe controller for receive update for his dataset
        map_type_controller.insert(make_pair(type, controller));
        
        //liste on controller key
        managmentKeyToMonitor(map_type_controller[type]->getMonitorKeyList(), true);
        
    } else {
        NF_INFO << "Controller already created for type:" << type;
        controller = map_type_controller[type];
    }
    return controller;
}

bool NodeFetcher::removeHandlerFromController(ControllerType type,
                                              NodeMonitorHandler *handler) {
    bool result = false;
    //remove handler from controller
    if((result = map_type_controller[type]->removeHandler(handler)) ){
        
        //check if we need to purge the controller
        if(map_type_controller[type]->getHandlerListSise() == 0) {
            //unlisten on controller key
            managmentKeyToMonitor(map_type_controller[type]->getMonitorKeyList(), false);
            NF_INFO << "Erase controller for type:" << type;
            //we can remove the controller
            map_type_controller.erase(type);
        }
    }
    return result;
}

void NodeFetcher::managmentKeyToMonitor(const MonitorKeyList& key_to_listen,
                                        bool monitor) {
    //register key to monitor system
    for(MonitorKeyListConstIterator it = key_to_listen.begin(),
        it_end = key_to_listen.end();
        it != it_end;
        it++) {
        const std::string key = *it;
        if(monitor) {
            map_monitor_key_registered_times[key]++;
            NF_INFO << "Add key <<" << key <<" to monitoring";
            if(map_monitor_key_registered_times[key] == 1) {
                monitor_manager->addKeyConsumer(key, 10, this);
            } else {
                NF_INFO << "Key <<" << key <<" already monitored with instance "<< map_monitor_key_registered_times[key];
            }
        } else {
            map_monitor_key_registered_times[key]--;
            if(map_monitor_key_registered_times[key] == 0) {
                NF_INFO << "Remove key <<" << key <<" from monitoring";
                monitor_manager->removeKeyConsumer(key, 10, this);
            } else {
                 NF_INFO << "Key <<" << key <<" still needed by instance "<< map_monitor_key_registered_times[key];
            }
        }
    }
}

bool NodeFetcher::addHanlderForControllerType(ControllerType type,
                                              NodeMonitorHandler *handler) {
    boost::unique_lock<boost::mutex> wl(maps_mutex);
    ChaosSharedPtr<NodeController>   controller = getControllerForType(type);
    return controller->addHandler(handler);
}

bool NodeFetcher::removeHandler(ControllerType type,
                                NodeMonitorHandler *handler) {
    boost::unique_lock<boost::mutex> wl(maps_mutex);
    if(map_type_controller.count(type) == 0) return false;
    
    return removeHandlerFromController(type, handler);
}
