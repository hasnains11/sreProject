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

#ifndef CHAOSFramework_EventServer_h
#define CHAOSFramework_EventServer_h
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/event/EventHandler.h>
#include <chaos/common/utility/StartableService.h>


namespace chaos {
    namespace common {
        namespace network {
            class NetworkBroker;
        }
        
        namespace event{
            
            class EventServer:
            public common::utility::NamedService,
            public chaos::common::utility::StartableService {
                friend class chaos::common::network::NetworkBroker;
                //! handler that can manage the event reception
                EventHandler *rootEventHandler;
            protected:
                //Default Server Constructor
                EventServer(const std::string& alias);
                
                /*
                 init the event adapter
                 */
                virtual void init(void*) = 0;
                
                /*
                 start the event adapter
                 */
                virtual void start() = 0;
                
                virtual void stop() = 0;
                
                /*
                 start the event adapter
                 */
                virtual void listeForEventType(event::EventType type,  bool listen) = 0;
                
                
                /*
                 deinit the event adapter
                 */
                virtual void deinit() = 0;
                
                void dispatchEventToHandler(const unsigned char * const serializedEvent, uint16_t length);
                
            public:
                
                void setEventHanlder(EventHandler *eventHanlder);
            };
        }
    }
}

#endif
