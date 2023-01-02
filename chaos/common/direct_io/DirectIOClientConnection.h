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

#ifndef __CHAOSFramework__DirectIOClientStartPoint__
#define __CHAOSFramework__DirectIOClientStartPoint__

#include <map>
#include <string>

#include <boost/thread.hpp>

#include <chaos/common/utility/TemplatedKeyObjectContainer.h>

#include <chaos/common/direct_io/DirectIOTypes.h>

#include <chaos/common/direct_io/DirectIOForwarder.h>
#include <chaos/common/direct_io/DirectIOURLManagment.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualClientChannel.h>

namespace chaos {
	
	namespace common {
		
		namespace network {
			class NetworkBroker;
		}
		
		namespace direct_io {
            //forward delcarations
            class DirectIOClient;
            class DirectIOClientConnection;
			namespace channel{
				class DirectIOVirtualClientChannel;
			}
 
			//--------------connection event handler-----------------------
			//! Connection event enumeration
			namespace DirectIOClientConnectionStateType {
				typedef enum DirectIOClientConnectionStateType {
					DirectIOClientConnectionEventConnected,
					DirectIOClientConnectionEventDisconnected
				} DirectIOClientConnectionStateType;
            }

            //! Direct io client connection event handler
            class DirectIOClientConnectionEventHandler {
				friend class DirectIOClientConnection;
            protected:
                //! han
                virtual void handleEvent(DirectIOClientConnection *client_connection, DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event) = 0;
            };
			
			typedef  utility::TemplatedKeyObjectContainer< unsigned int, channel::DirectIOVirtualClientChannel*> DICKeyObjectContainer;
			
			
            //! Represent the start point of a messaget towards an endpoint
            /*!
                This class is the start point for a comunication winth a server endpoint,
                In definitive it can be viwed as a socket connected to a server::port that
                need to send the data towards an endpoint
             */
            class DirectIOClientConnection  :
			public DirectIOForwarder,
			public DirectIOURLManagment,
            protected DICKeyObjectContainer::FreeHandler {
                friend class DirectIOClient;
				friend class chaos::common::network::NetworkBroker;
								
			protected:
				std::string		url;
				std::string     server_description;
				std::string     custom_string_identification;
                uint16_t        endpoint;
				std::string     unique_uuid;
                DICKeyObjectContainer map_client_channels;
				DirectIOClientConnectionStateType::DirectIOClientConnectionStateType current_state;

//                //! current client ip in string form
//                static std::string my_str_ip;
//                
//                //! current client ip in 32 bit form
//                static uint64_t my_i64_ip;

                DirectIOClientConnectionEventHandler *event_handler;
				
				//overriding ofr free object fuunction for the tempalted key object container superclass
                void freeObject(const DICKeyObjectContainer::TKOCElement& element);
				
				//! callend by client or by implementation when some event occour from socket conenction
				void lowLevelManageEvent(DirectIOClientConnectionStateType::DirectIOClientConnectionStateType state_type);
                
                // prepare header for defaut connection data
                inline void completeDataPack(chaos::common::direct_io::DirectIODataPack& data_pack) {
                    data_pack.header.dispatcher_header.fields.route_addr = endpoint;
                }
				
            public:
				
				DirectIOClientConnection(const std::string& _server_description,
                                         uint16_t _endpoint);
                virtual ~DirectIOClientConnection();

				std::string	getUniqueUUID();
				
				std::string	getCustomStringIdentification();
				void		setCustomStringIdentification(std::string _custom_string_identificaiton);
				
				void setEventHandler(DirectIOClientConnectionEventHandler *_event_handler);
				
				const char * getServerDescription();
				
				std::string getURL();
				
				//! return the state of the connection
				DirectIOClientConnectionStateType::DirectIOClientConnectionStateType getState();
				
				//! New channel allocation by name
				/*!
				 Allocate a new channel and initialize it
				 */
				channel::DirectIOVirtualClientChannel *getNewChannelInstance(std::string channel_name);
				
				//! New channel allocation by name
				/*!
				 Allocate a new channel and initialize it
				 */
				void releaseChannelInstance(channel::DirectIOVirtualClientChannel *channel_instance);
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIOClientStartPoint__) */
