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

#ifndef __CHAOSFramework__AsioImplEventServer__
#define __CHAOSFramework__AsioImplEventServer__

#include <chaos/common/event/AsioEventHandler.h>
#include <chaos/common/event/EventServer.h>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace common {
        namespace event{
            using namespace std;
            using namespace boost;
            
            const short multicast_port = 30001;
            class AsioEventHandler;
            
            //! Asio impleentation for the !CHAOS event server
            /*!
             
             */
            DECLARE_CLASS_FACTORY(AsioImplEventServer, EventServer) {
                friend class AsioEventHandler;
                
                //! alert socket
                AsioEventHandler *alertHandler;
                
                //! istrument socket
                AsioEventHandler *instrumentHandler;
                
                //! command socket
                AsioEventHandler *commandHandler;
                
                //! custom socket
                AsioEventHandler *customHandler;
                
            protected:
                /*
                 init the event adapter
                 */
                void init(void*);
                
                /*
                 start the event adapter
                 */
                void start();
                
                /*
                 stop the event adapter
                 */
                void stop();
                
                /*
                 register or deregister for a kynd of event
                 */
                virtual void listeForEventType(event::EventType type,  bool listen);
                
                /*
                 deinit the event adapter
                 */
                void deinit();
                
                void sendEventDataToRootHandler(unsigned char * buff, uint16_t length);
            public:
                AsioImplEventServer(const string& alias);
                
            private:
                boost::asio::io_service io_service;
                boost::thread_group serviceThreads;
                //vector< ChaosSharedPtr<boost::thread> > serviceThread;
                uint8_t threadNumber;
                
            };
        }
    }
}
#endif /* defined(__CHAOSFramework__AsioImplEventServer__) */
