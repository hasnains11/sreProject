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
#ifndef __CHAOSFramework__DirectIOVirtualServerChannel__
#define __CHAOSFramework__DirectIOVirtualServerChannel__

#include <chaos/common/direct_io/DirectIODataPack.h>
#include <chaos/common/direct_io/DirectIOEndpointHandler.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualChannel.h>

#include <boost/function.hpp>

namespace chaos {
	namespace common {
		namespace direct_io {
			
			class DirectIOServerEndpoint;
			
            namespace channel {
				
				class DirectIOVirtualServerChannel:
                public DirectIOVirtualChannel {
					friend class chaos::common::direct_io::DirectIOServerEndpoint;
				public:

					typedef DirectIOEndpointHandler* ServerChannelDelegate;
					
					void clearDelegate();
					uint16_t getEndpointRouteIndex();
				private:
					ServerChannelDelegate server_channel_delegate;
					uint16_t endpoint_route_index;
				protected:
					DirectIOVirtualServerChannel(std::string channel_name, uint8_t channel_route_index);
                    ~DirectIOVirtualServerChannel();
					

					void setDelegate(ServerChannelDelegate delegate_function);
				};
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIOVirtualServerChannel__) */
