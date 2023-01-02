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

#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/direct_io/DirectIOClientConnection.h>
#include <chaos/common/utility/UUIDUtil.h>

#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using namespace chaos::common::utility;
using namespace chaos::common::direct_io;


#define DIOVCCLAPP_ INFO_LOG(DirectIOClientConnection)
#define DIOVCCLDBG_ DBG_LOG(DirectIOClientConnection)
#define DIOVCCLERR_ ERR_LOG(DirectIOClientConnection)

//// current client ip in string form
//std::string DirectIOClientConnection::my_str_ip;
//
//// current client ip in 64 bit form
//uint64_t DirectIOClientConnection::my_i64_ip = 0;

DirectIOClientConnection::DirectIOClientConnection(const std::string& _server_description,
												   uint16_t _endpoint):
server_description(_server_description),
endpoint(_endpoint),
map_client_channels(this),
event_handler(NULL),
current_state(DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected),
unique_uuid(UUIDUtil::generateUUIDLite()),
url(boost::str( boost::format("%1%|%2%") % _server_description % _endpoint)){}

DirectIOClientConnection::~DirectIOClientConnection() {
	
}

const char * DirectIOClientConnection::getServerDescription() {
	return server_description.c_str();
}

std::string DirectIOClientConnection::getURL() {
	return url;
}

std::string DirectIOClientConnection::getUniqueUUID() {
    return unique_uuid;
}

std::string	DirectIOClientConnection::getCustomStringIdentification() {
	return custom_string_identification;
}

void DirectIOClientConnection::setCustomStringIdentification(std::string _custom_string_identificaiton) {
	custom_string_identification = _custom_string_identificaiton;
}

void DirectIOClientConnection::setEventHandler(DirectIOClientConnectionEventHandler *_event_handler) {
	event_handler = _event_handler;
}

DirectIOClientConnectionStateType::DirectIOClientConnectionStateType DirectIOClientConnection::getState() {
	return current_state;
}

void DirectIOClientConnection::lowLevelManageEvent(DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event_state) {
	current_state = event_state;
	DEBUG_CODE( switch(current_state)  {
		case DirectIOClientConnectionStateType::DirectIOClientConnectionEventConnected:
			DIOVCCLDBG_ << "Set to state ->  DirectIOClientConnectionEventConnected";
			break;
		case DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected:
			DIOVCCLDBG_ << "Set to state ->  DirectIOClientConnectionEventDisconnected";
			break;
	});
	//forward event to the handler
	if(event_handler) event_handler->handleEvent(this, current_state);
}

// New channel allocation by name
channel::DirectIOVirtualClientChannel *DirectIOClientConnection::getNewChannelInstance(std::string channel_name) {
	channel::DirectIOVirtualClientChannel *channel = ObjectFactoryRegister<channel::DirectIOVirtualClientChannel>::getInstance()->getNewInstanceByName(channel_name);
    //sub class method for register the instance
    if(channel) {
		channel->client_instance() = this;
		map_client_channels.registerElement(channel->channel_route_index, channel);
    }
	return channel;
}

// New channel allocation by name
void DirectIOClientConnection::releaseChannelInstance(channel::DirectIOVirtualClientChannel *channel_instance) {
	if(channel_instance) {
        map_client_channels.deregisterElementKey(channel_instance->channel_route_index);
		channel_instance->client_instance() = NULL;
        delete(channel_instance);
    }
}

void DirectIOClientConnection::freeObject(const DICKeyObjectContainer::TKOCElement& element) {
	//releaseConnection(connection);
}
