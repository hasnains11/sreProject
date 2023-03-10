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

#include <chaos/common/global.h>
#include <chaos/common/rpc/zmq/ZMQClient.h>
#include <chaos/common/rpc/zmq/ZmqMemoryManagement.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <chaos/common/chaos_errors.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::pool;
#if CHAOS_PROMETHEUS
using namespace chaos::common::metric;
#endif
using namespace std;
using namespace boost;
using namespace boost::algorithm;

#define ZMQC_LAPP INFO_LOG(ZMQClient)
#define ZMQC_LDBG DBG_LOG(ZMQClient)
#define ZMQC_LERR ERR_LOG(ZMQClient)

#define ZMQ_DO_AGAIN(x) do{x}while(err == EAGAIN);
#define ZMQ_SOCKET_MAINTENANCE_TIMEOUT (1000 * 30)
#define ZMQ_SOCKET_LIFETIME_TIMEOUT (1000 * 60)
//-------------------------------------------------------
DEFINE_CLASS_FACTORY(ZMQClient, RpcClient);

static void my_free (void *data, void *hint) {
    if(hint) {
        MemoryManagement *tmp = static_cast<MemoryManagement*>(hint);
        delete(tmp);
    } else {free(data);};
}


ZMQClient::ZMQClient(const string& alias):
RpcClient(alias),
zmq_context(NULL),
zmq_timeout(RpcConfigurationKey::GlobalRPCTimeoutinMSec){    
    seq_id=0;

#if CHAOS_PROMETHEUS
    //add custom driver metric
    chaos::common::metric::MetricManager::getInstance()->createGaugeFamily("rpc_zmq_client_queue", "Element in queue that need to forwarded by the zmq client");
    chaos::common::metric::MetricManager::getInstance()->createGaugeFamily("rpc_zmq_client_errors", "ZMQ Client errors");
    counter_queuend_uptr = MetricManager::getInstance()->getNewGaugeFromFamily("rpc_zmq_client_queue");
    counter_zmqerror_uptr= MetricManager::getInstance()->getNewGaugeFromFamily("rpc_zmq_client_errors");
#else
    counter_zmqerror_uptr.reset(new uint32_t);
    *counter_zmqerror_uptr=0;
#endif
}

ZMQClient::~ZMQClient(){
//    #ifndef CHAOS_PROMETHEUS
//    delete counter_zmqerror_uptr;
//    #endif
}

/*
 Initialization method for output buffer
 */
void ZMQClient::init(void *init_data) {
    CDataWrapper *cfg = reinterpret_cast<CDataWrapper*>(init_data);
    seq_id=0;
    ZMQC_LAPP << "initialization";
    int32_t threadNumber = cfg->hasKey(InitOption::OPT_RPC_SERVER_THREAD_NUMBER)? cfg->getInt32Value(InitOption::OPT_RPC_SERVER_THREAD_NUMBER):1;
    ZMQC_LAPP << "ObjectProcessingQueue<CDataWrapper> initialization with "<< threadNumber <<" thread";
    CObjectProcessingQueue<NetworkForwardInfo>::init(threadNumber);
    ZMQC_LAPP << "ObjectProcessingQueue<NetworkForwardInfo> initialized";
    
    ZMQC_LAPP << "ConnectionPool initialization";
    zmq_context = zmq_ctx_new();
    CHAOS_ASSERT(zmq_context)
    
    //et the thread number
    zmq_ctx_set(zmq_context, ZMQ_IO_THREADS, threadNumber);
    
    if(GlobalConfiguration::getInstance()->getRpcImplKVParam().count("zmq_timeout")) {
        CHAOS_NOT_THROW(zmq_timeout = boost::lexical_cast<int>(GlobalConfiguration::getInstance()->getRpcImplKVParam()["zmq_timeout"]););
        
    }
    
    ZMQC_LAPP << "ConnectionPool initialized";
}

/*
 start the rpc adapter
 */
void ZMQClient::start() {
    int err = 0;
    //start timere after and repeat every one minut
    if((err = chaos::common::async_central::AsyncCentralManager::getInstance()->addTimer(this, ZMQ_SOCKET_MAINTENANCE_TIMEOUT, ZMQ_SOCKET_MAINTENANCE_TIMEOUT))) {LOG_AND_TROW(ZMQC_LERR, err, "Error adding timer")}
}

/*
 start the rpc adapter
 */
void ZMQClient::stop() {
    int err = 0;
    if((err = chaos::common::async_central::AsyncCentralManager::getInstance()->removeTimer(this))) {LOG_AND_TROW(ZMQC_LERR, err, "Error removing timer")}
}

/*
 Deinitialization method for output buffer
 */
void ZMQClient::deinit() {
    ZMQC_LAPP << "ObjectProcessingQueue<NetworkForwardInfo> stopping";
    CObjectProcessingQueue<NetworkForwardInfo>::clear();
    CObjectProcessingQueue<NetworkForwardInfo>::deinit();
    ZMQC_LAPP << "ObjectProcessingQueue<NetworkForwardInfo> stopped";

    ZMQC_LAPP << "deinitialization";
    map_socket.clear();
    
    //destroy the zmq context
    zmq_ctx_shutdown(zmq_context);
    zmq_ctx_destroy(zmq_context);
    ZMQC_LAPP << "ZMQ Destroyed";
}

/*
 
 */
bool ZMQClient::submitMessage(NFISharedPtr forwardInfo,
                              bool onThisThread) {
    CHAOS_ASSERT(forwardInfo);
    ElementManagingPolicy ePolicy;
    try{
        if(!forwardInfo->destinationAddr.size())
            throw CException(0, "No destination ip in message description", __PRETTY_FUNCTION__);
        if(!forwardInfo->hasMessage())
            throw CException(0, "No message in description", __PRETTY_FUNCTION__);
        //allocate new forward info
        //submit action
        #if CHAOS_PROMETHEUS
            (*counter_queuend_uptr)++;
        #endif
        if(onThisThread){
            ePolicy.elementHasBeenDetached = false;
            processBufferElement(MOVE(forwardInfo));
        } else {
            CObjectProcessingQueue<NetworkForwardInfo>::push(MOVE(forwardInfo));
        }
    } catch(CException& ex){
        //in this case i need to delete the memory
        //in this case i need to delete te memory allocated by message
        DECODE_CHAOS_EXCEPTION(ex)
    }
    return true;
}

ZMQSocketPool::ResourceSlot *ZMQClient::getSocketForNFI(NetworkForwardInfo *nfi) {
    boost::unique_lock<boost::shared_mutex> lock_socket_map(map_socket_mutex);
    SocketMapIterator it = map_socket.find(nfi->destinationAddr);
    if(it != map_socket.end()){
        return it->second->getNewResource();
    } else {
        ChaosSharedPtr< ZMQSocketPool > socket_pool(new ZMQSocketPool(nfi->destinationAddr, this));
        map_socket.insert(make_pair(nfi->destinationAddr, socket_pool));
        return socket_pool->getNewResource();
    }
    ZMQC_LERR<<"cannot create socket pool, for :"<<nfi->destinationAddr;
    return NULL;
}

void ZMQClient::releaseSocket(ZMQSocketPool::ResourceSlot *socket_slot_to_release) {
    boost::unique_lock<boost::shared_mutex> lock_socket_map(map_socket_mutex);
    if(socket_slot_to_release && map_socket[socket_slot_to_release->pool_identification].get())
        map_socket[socket_slot_to_release->pool_identification]->releaseResource(socket_slot_to_release);
}

void ZMQClient::deleteSocket(ZMQSocketPool::ResourceSlot *socket_slot_to_release) {
    boost::unique_lock<boost::shared_mutex> lock_socket_map(map_socket_mutex);
    if(socket_slot_to_release && map_socket[socket_slot_to_release->pool_identification].get())
        map_socket[socket_slot_to_release->pool_identification]->releaseResource(socket_slot_to_release,
                                                                                 true);
}

//----resource pool handler-----
ZMQSocketPoolDef* ZMQClient::allocateResource(const std::string& pool_identification,
                                              uint32_t& alive_for_ms) {
    int err = 0;
    int linger = 0;
    int water_mark = 2;
    std::string new_id;
    ChaosUniquePtr<ZMQSocketPoolDef> socket_def(new ZMQSocketPoolDef());
    //set the alive time to one minute
    alive_for_ms = ZMQ_SOCKET_LIFETIME_TIMEOUT;
    //create zmq socket
    socket_def->socket = zmq_socket (zmq_context, ZMQ_DEALER);
    if(!socket_def->socket) {
        return NULL;
    } else if ((err = zmq_setsockopt(socket_def->socket, ZMQ_LINGER, &linger, sizeof(int)))) {
    } else if ((err = zmq_setsockopt(socket_def->socket, ZMQ_RCVHWM, &water_mark, sizeof(int)))) {
    } else if ((err = zmq_setsockopt(socket_def->socket, ZMQ_SNDHWM, &water_mark, sizeof(int)))) {
    } else if ((err = zmq_setsockopt(socket_def->socket, ZMQ_SNDTIMEO, &zmq_timeout, sizeof(int)))) {
    } else if ((err = zmq_setsockopt(socket_def->socket, ZMQ_RCVTIMEO, &zmq_timeout, sizeof(int)))) {
    } else {
        string url = "tcp://";
        url.append(pool_identification);
        if((err = zmq_connect(socket_def->socket, url.c_str()))) {
            ZMQC_LERR << "Error "<< err <<" connecting socket to " <<pool_identification;
             (*counter_zmqerror_uptr)++;
        } else {
            DEBUG_CODE(ZMQC_LAPP << "New socket for "<<pool_identification;)
            socket_def->identity = common::utility::UUIDUtil::generateUUIDLite();
            if((err = zmq_setsockopt(socket_def->socket, ZMQ_IDENTITY, socket_def->identity.c_str(), socket_def->identity.size())) != 0){
                DEBUG_CODE(ZMQC_LAPP << "Error setting socket identity for "<<pool_identification;)
            }
        }
    }
    
    if(err) {
        if(socket_def->socket) {
            ZMQC_LERR << "Error during configuration of the socket for "<<pool_identification;
             (*counter_zmqerror_uptr)++;
            zmq_close(socket_def->socket);
            //reset socket
            socket_def->socket = NULL;
        }
        socket_def.reset();
    }
    return socket_def.release();
}

void ZMQClient::deallocateResource(const std::string& pool_identification, ZMQSocketPoolDef* resource_to_deallocate) {
    CHAOS_ASSERT(resource_to_deallocate)
    DEBUG_CODE(ZMQC_LAPP << "delete socket for "<<pool_identification;)
    zmq_close(resource_to_deallocate->socket);
    delete(resource_to_deallocate);
}

//-----timer handler------
void ZMQClient::timeout() {
    boost::unique_lock<boost::shared_mutex> lock_socket_map(map_socket_mutex);
    SocketMapIterator it = map_socket.begin();
    while(it != map_socket.end()){
        SocketMapIterator tmp_it = it++;
        tmp_it->second->maintenance();
        if( tmp_it->second->getSize() == 0 ) {
            DEBUG_CODE(ZMQC_LAPP << "Delete socket pool for:" << tmp_it->first;)
            map_socket.erase( tmp_it );
        }
    }
}

int ZMQClient::sendMessage(void *socket,
                           void *message_data,
                           size_t message_size,
                           bool more_to_send) {
    int err = 0;
    zmq_msg_t message;
    
    if((err = zmq_msg_init_size(&message,
                                message_size)) == -1){
        //error creating the message
        err = zmq_errno();
        ZMQC_LERR << "Error initializing message with error:" << zmq_strerror(err);
         (*counter_zmqerror_uptr)++;
    } else {
        //copy content into message
        memcpy(zmq_msg_data(&message),
               message_data,
               message_size);
        //send data
        if((err = zmq_msg_send(&message, socket, more_to_send?ZMQ_SNDMORE:ZMQ_DONTWAIT)) == -1){
            err = zmq_errno();
            ZMQC_LERR << "Error sending message with error:" << zmq_strerror(err);
             (*counter_zmqerror_uptr)++;
        } else {
            //reset the error
            err = 0;
        }
        //close the message
        zmq_msg_close(&message);
    }
    return err;
}

/*
 process the element action to be executed
 */
void ZMQClient::processBufferElement(NFISharedPtr messageInfo) {
    //the domain is securely the same is is mandatory for submition so i need to get the name of the action
    int			err = 0;
    uint64_t    loc_seq_id = 0;
    zmq_msg_t	reply;
    zmq_msg_t	message;
    zmq_msg_init (&reply);
    
#if CHAOS_PROMETHEUS
    (*counter_queuend_uptr)--;
#endif
    
    //get remote ip
    //serialize the call packet
    ZMQSocketPool::ResourceSlot *socket_info = NULL;
    messageInfo->message->addBoolValue("syncrhonous_call", RpcClient::syncrhonous_call);
    messageInfo->message->addInt64Value("seq_id", (loc_seq_id = ++seq_id));
    CDWShrdPtr message_data = CDWShrdPtr(messageInfo->message.release());
    try{
        socket_info = getSocketForNFI(messageInfo.get());
        if(socket_info == NULL){
            ZMQC_LERR << "GetSocketForNFI failed";
            forwadSubmissionResultError(MOVE(messageInfo),
                                        ErrorRpcCoce::EC_RPC_NO_SOCKET,
                                        "GetSocketForNFI failed",
                                        __PRETTY_FUNCTION__);
            (*counter_zmqerror_uptr)++;
            return;
        }
        
        if(!socket_info->resource_pooled) {
            ZMQC_LERR << "Socket creation error";
            forwadSubmissionResultError(MOVE(messageInfo),
                                        ErrorRpcCoce::EC_RPC_NO_SOCKET,
                                        "Socket creation error",
                                        __PRETTY_FUNCTION__);
            deleteSocket(socket_info);
            (*counter_zmqerror_uptr)++;
            return;
        }
        
        //send data
        if((err = zmq_msg_init_data(&message,
                                    (void*)message_data->getBSONRawData(),
                                    message_data->getBSONRawSize(),
                                    my_free,
                                    new MemoryManagement(message_data))) == -1) {
            int32_t sent_error = zmq_errno();
            std::string error_message =zmq_strerror(sent_error);
            (*counter_zmqerror_uptr)++;
            ZMQC_LERR << "Error allocating zmq messagecode:" << sent_error << " message:" <<error_message;
             (*counter_zmqerror_uptr)++;
            if(messageInfo->is_request) {
                forwadSubmissionResultError(MOVE(messageInfo),
                                            (ErrorRpcCoce::EC_RPC_IMPL_ERR-1),
                                            "Error initializiend rcp message",
                                            __PRETTY_FUNCTION__);
            }
        } else {
            ZMQC_LDBG << "Try to send message seq_id:"<<loc_seq_id;
            err = zmq_msg_send(&message,
                               socket_info->resource_pooled->socket,
                               ZMQ_DONTWAIT);
            if(err == -1) {
                int32_t sent_error = zmq_errno();
                std::string error_message = zmq_strerror(sent_error);
                ZMQC_LERR << "Error sending message seq_id:"<<loc_seq_id<<" with code:" << sent_error << " message:" <<error_message<<" @"<<messageInfo->destinationAddr;
                 (*counter_zmqerror_uptr)++;
                if(messageInfo->is_request) {
                    forwadSubmissionResultError(MOVE(messageInfo),
                                                ErrorRpcCoce::EC_RPC_SENDING_DATA,
                                                error_message,
                                                __PRETTY_FUNCTION__);
                }
                
                //delete socket
                deleteSocket(socket_info);
                socket_info = NULL;
            }else{
                ZMQC_LDBG << "Message seq_id:"<<loc_seq_id<<" sent now wait for ack";
                //ok get the answer
                err = zmq_msg_recv(&reply,
                                   socket_info->resource_pooled->socket,
                                   0);
                if(err == -1) {
                    int32_t sent_error = zmq_errno();
                    std::string error_message = zmq_strerror(sent_error);
                    ZMQC_LERR << "Error receiving ack for message seq_id:"<<loc_seq_id<<" with code:" << sent_error << " message:" <<error_message<<" @"<<messageInfo->destinationAddr;
                     (*counter_zmqerror_uptr)++;
                    if(messageInfo->is_request) {
                        forwadSubmissionResultError(MOVE(messageInfo),
                                                    ErrorRpcCoce::EC_RPC_GETTING_ACK_DATA,
                                                    CHAOS_FORMAT("%1%[%2%]",%error_message%sent_error),
                                                    __PRETTY_FUNCTION__);
                    }
                    //delete socket
                    deleteSocket(socket_info);
                    socket_info = NULL;
                } else {
                    CDWUniquePtr tmp;
                    uint64_t rid_ack=0;
                    //decode result of the posting message operation
                    if(zmq_msg_size(&reply)>0){
                        tmp.reset(new CDataWrapper(static_cast<const char *>(zmq_msg_data(&reply))));
                        if(tmp->hasKey("seq_id")){
                            rid_ack=tmp->getInt64Value("seq_id");
                            if(rid_ack!=loc_seq_id){
                                ZMQC_LERR<<"MISMATCH request id:"<<loc_seq_id<<" to:@"<<messageInfo->destinationAddr<<" ack id:"<<rid_ack <<" from @"<<messageInfo->sender_node_id;

                                (*counter_zmqerror_uptr)++;
                            }
                        }
                    }
                    if(messageInfo->is_request) {
                        if(tmp.get()){
                            if(RpcClient::syncrhonous_call) {
                                forwadSubmissionResult(MOVE(messageInfo),MOVE(tmp));
                            } else {
                                ZMQC_LDBG << "ACK id:"<<rid_ack<<" Received for request:"<<loc_seq_id;
                                //there is a reply so we need to check if all ok or in case answer to request
                                forwadSubmissionResultError(messageInfo->sender_node_id,
                                                            messageInfo->sender_request_id,
                                                            MOVE(tmp));
                            }
                        } else {
                            ZMQC_LDBG << "Bad ACK received for request:"<<loc_seq_id<<" @"<<messageInfo->sender_node_id;
                            forwadSubmissionResultError(MOVE(messageInfo),
                                                        ErrorRpcCoce::EC_RPC_GETTING_ACK_DATA,
                                                        "bad ack received",
                                                        __PRETTY_FUNCTION__);
                        }
                    } else {
                        ZMQC_LDBG << "ACK id:"<<rid_ack<<" Received for message:"<<loc_seq_id;
                    }
                }
            }
        }
        //return socket to the pool
        if(socket_info) {releaseSocket(socket_info);}
    }catch (std::exception& e) {
        ZMQC_LERR << "Error during message forwarding:"<< e.what();
         (*counter_zmqerror_uptr)++;
    } catch (...) {
        ZMQC_LERR << "General error during message forwarding:";
         (*counter_zmqerror_uptr)++;
    }
    zmq_msg_close (&message);
    zmq_msg_close (&reply);
}

uint64_t ZMQClient::getMessageQueueSize() {
    return CObjectProcessingQueue::queueSize();
}
