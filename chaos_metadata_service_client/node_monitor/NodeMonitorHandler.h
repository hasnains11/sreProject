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

#ifndef __CHAOSFramework__NodeMonitorHandler_h
#define __CHAOSFramework__NodeMonitorHandler_h

#include <chaos_metadata_service_client/node_monitor/node_monitor_types.h>

namespace chaos {
    namespace metadata_service_client {
        namespace node_monitor {
            class NodeFetcher;
            struct NodeMonitorHandlerComparator;
            
            //! node monitor handler
            class NodeMonitorHandler {
                friend class NodeMonitorHandlerComparator;
                friend class NodeFetcher;
                const std::string handler_uuid;
                bool signal_on_change;
            public:
                static const char * const MAP_KEY_ONLINE_STATE;
                NodeMonitorHandler();
                virtual ~NodeMonitorHandler();
            
                virtual void nodeHasBeenRestarted(const std::string& node_uid);
                
                //! called when an online state has changed
                virtual void nodeChangedOnlineState(const std::string& node_uid,
                                                    OnlineState old_state,
                                                    OnlineState new_state);
                
                virtual void nodeChangedInternalState(const std::string& node_uid,
                                                      const std::string& old_state,
                                                      const std::string& new_state);
                
                virtual void nodeChangedProcessResource(const std::string& node_uid,
                                                        const ProcessResource& old_proc_res,
                                                        const ProcessResource& new_proc_res);
                
                virtual void nodeChangedErrorInformation(const std::string& node_uid,
                                                         const ErrorInformation& old_error_information,
                                                         const ErrorInformation& new_error_information);
                
                virtual void nodeChangedHealthDataset(const std::string& node_uid,
                                                      MapDatasetKeyValues& map_health_dataset);
                
                void setSignalOnChange(bool new_change_on_signal);
                
                bool signalOnChange();
            };
            
            struct NodeMonitorHandlerComparator {
                bool operator() (const NodeMonitorHandler* h1, const NodeMonitorHandler* h2) const {
                    return h1->handler_uuid.compare(h2->handler_uuid) < 0;
                }
            };
        }
    }
}

#endif /* __CHAOSFramework__NodeMonitorHandler_h */
