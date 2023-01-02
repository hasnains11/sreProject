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

#include <limits>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/event/channel/InstrumentEventChannel.h>
#include <chaos/cu_toolkit/control_manager/RTAbstractControlUnit.h>

#include <boost/format.hpp>
#include <boost/thread.hpp>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::exception;
using namespace chaos::common::data::cache;

using namespace chaos::cu;
using namespace chaos::cu::control_manager;

using namespace boost;
using namespace boost::chrono;
#define RTCUL_HEAD "[Real Time Control Unit:"<<getCUInstance()<<"] - "
#define RTCULAPP_ LAPP_ << RTCUL_HEAD
#define RTCULDBG_ LDBG_ << RTCUL_HEAD
#define RTCULERR_ LERR_ << RTCUL_HEAD << __PRETTY_FUNCTION__ <<"(" << __LINE__ << ") - "

RTAbstractControlUnit::RTAbstractControlUnit(const std::string& _control_unit_id,
                                             const std::string& _control_unit_param):
AbstractControlUnit(CUType::RTCU,
                    _control_unit_id,
                    _control_unit_param),
schedule_delay(1000000),
scheduler_run(false){
    //allocate the handler engine
    // attributeHandlerEngine = new DSAttributeHandlerExecutionEngine(this);
    
    //associate the shared cache of the executor to the asbtract control unit one
    attribute_value_shared_cache = new AttributeValueSharedCache();
}

/*!
 Parametrized constructor
 \param _control_unit_id unique id for the control unit
 \param _control_unit_drivers driver information
 */
RTAbstractControlUnit::RTAbstractControlUnit(const std::string& _control_unit_id,
                                             const std::string& _control_unit_param,
                                             const ControlUnitDriverList& _control_unit_drivers):
AbstractControlUnit(CUType::RTCU,
                    _control_unit_id,
                    _control_unit_param,
                    _control_unit_drivers),
schedule_delay(1000000),
scheduler_run(false) {
    //allocate the handler engine
    //attributeHandlerEngine = new DSAttributeHandlerExecutionEngine(this);
    
    //associate the shared cache of the executor to the asbtract control unit one
    attribute_value_shared_cache = new AttributeValueSharedCache();
}


RTAbstractControlUnit::RTAbstractControlUnit(const std::string& _alternate_type,
                                             const std::string& _control_unit_id,
                                             const std::string& _control_unit_param):
AbstractControlUnit(_alternate_type,
                    _control_unit_id,
                    _control_unit_param),
schedule_delay(1000000),
scheduler_run(false) {
    //associate the shared cache of the executor to the asbtract control unit one
    attribute_value_shared_cache = new AttributeValueSharedCache();
    
}

RTAbstractControlUnit::RTAbstractControlUnit(const std::string& _alternate_type,
                                             const std::string& _control_unit_id,
                                             const std::string& _control_unit_param,
                                             const ControlUnitDriverList& _control_unit_drivers):
AbstractControlUnit(_alternate_type,
                    _control_unit_id,
                    _control_unit_param,
                    _control_unit_drivers),
schedule_delay(1000000),
scheduler_run(false) {
    //associate the shared cache of the executor to the asbtract control unit one
    attribute_value_shared_cache = new AttributeValueSharedCache();
    
}

RTAbstractControlUnit::~RTAbstractControlUnit() {
    //release attribute shared cache
    if(attribute_value_shared_cache) {
        delete(attribute_value_shared_cache);
    }
}

void RTAbstractControlUnit::setDefaultScheduleDelay(uint64_t _schedule_delay) {
    
    RTCULDBG_<<"setting default schedule to:"<<_schedule_delay<<" us";
    schedule_delay = _schedule_delay;
}

/*
 fill the CDataWrapper with AbstractCU system configuration, this method
 is called after getStartConfiguration directly by sandbox. in this method
 are defined the action for the input element of the dataset
 */
void RTAbstractControlUnit::_defineActionAndDataset(CDataWrapper& setup_configuration)  {
    AbstractControlUnit::_defineActionAndDataset(setup_configuration);
}

AbstractSharedDomainCache *RTAbstractControlUnit::_getAttributeCache() {
    return AbstractControlUnit::_getAttributeCache();
}

/*!
 Init the  RT Control Unit scheduling for device
 */
void RTAbstractControlUnit::init(void *initData) {
    //call parent impl
    AbstractControlUnit::init(initData);
    
    scheduler_run = false;
    
    RTCULAPP_ << "Initializing shared attribute cache " << DatasetDB::getDeviceID();
    InizializableService::initImplementation((AttributeValueSharedCache*)attribute_value_shared_cache, (void*)NULL, "attribute_value_shared_cache", __PRETTY_FUNCTION__);
}

/*!
 Starto the  Control Unit scheduling for device
 */
void RTAbstractControlUnit::start() {
    //call parent impl
    AbstractControlUnit::start();
    
    RTCULAPP_ << "Starting thread for device:" << DatasetDB::getDeviceID();
    threadStartStopManagment(true);
    RTCULAPP_ << "Thread started for device:" << DatasetDB::getDeviceID();
}

/*!
 Stop the Custom Control Unit scheduling for device
 */
void RTAbstractControlUnit::stop() {
    //manage the thread
    RTCULAPP_ << "Stopping thread for device:" << DatasetDB::getDeviceID();
    threadStartStopManagment(false);
    RTCULAPP_ << "Thread for device stopped:" << DatasetDB::getDeviceID();
    //call parent impl
    AbstractControlUnit::stop();
}

/*!
 Init the  RT Control Unit scheduling for device
 */
void RTAbstractControlUnit::deinit() {
    //call parent impl
    AbstractControlUnit::deinit();
    
    RTCULAPP_ << "Deinitializing shared attribute cache " << DatasetDB::getDeviceID();
    InizializableService::deinitImplementation((AttributeValueSharedCache*)attribute_value_shared_cache, "attribute_value_shared_cache", __PRETTY_FUNCTION__);
}

/*!
 return the appropriate thread for the device
 */
void RTAbstractControlUnit::threadStartStopManagment(bool startAction) {
    try{
        if(startAction) {
            if(scheduler_thread.get() && scheduler_run){
                RTCULAPP_ << "thread already running";
                return;
            }
            scheduler_run = true;
            scheduler_thread.reset(new boost::thread(boost::bind(&RTAbstractControlUnit::executeOnThread, this)));
#if defined(__linux__) || defined(__APPLE__)
            int retcode;
            int policy;
            struct sched_param param;
            pthread_t threadID = (pthread_t) scheduler_thread->native_handle();
            if ((retcode = pthread_getschedparam(threadID, &policy, &param)) != 0)  {
                return;
            }
            
            policy = SCHED_RR;
            param.sched_priority = sched_get_priority_max(SCHED_RR);
            if ((retcode = pthread_setschedparam(threadID, policy, &param)) != 0) {
                errno = retcode;
            }
            
            if ((retcode = pthread_getschedparam(threadID, &policy, &param)) != 0) {
                errno = retcode;
                return;
            }
#endif
        } else {
            if(!scheduler_run){
                RTCULAPP_ << "thread already stopped";
            }
            RTCULAPP_ << "Stopping and joining scheduling thread";
            scheduler_run = false;
            scheduler_thread->join();
            RTCULAPP_ << "Thread stopped";
        }
    } catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::thread_resource_error> >& exc) {
        RTCULERR_ << exc.what();
        throw MetadataLoggingCException(getCUID(), -1, exc.what(), std::string(__PRETTY_FUNCTION__));
    }
}

//!inherited method by @AbstractControlUnit
void RTAbstractControlUnit::propertyUpdatedHandler(const std::string& group_name,
                                                   const std::string& property_name,
                                                   const chaos::common::data::CDataVariant& old_value,
                                                   const chaos::common::data::CDataVariant& new_value) {
    if(group_name.compare(chaos::ControlUnitPropertyKey::P_GROUP_NAME) == 0) {
        key_data_storage->updateConfiguration(property_name, new_value);
        //is my group
        if(property_name.compare(ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY) == 0) {
            uint64_t u_secdelay = new_value.asUInt64();
            if(u_secdelay != schedule_delay){
                RTCULAPP_ << "Update schedule delay in:" << u_secdelay << " microsecond";
                schedule_delay = u_secdelay;
                _updateRunScheduleDelay(schedule_delay);
            }
        }
    }
    //call superclass
    AbstractControlUnit::propertyUpdatedHandler(group_name,
                                                property_name,
                                                old_value,
                                                new_value);
}

/*
 Execute the scehduling for the device
 */
void RTAbstractControlUnit::executeOnThread() {
    uint64_t start_execution = 0;
    int64_t time_to_sleep = 0;
    uint64_t next_predicted_run = 0;
    int64_t next_prediction_error = 0;
    uint64_t duration;
    RTCULAPP_ << "Run Start, schedule delay:"<<schedule_delay<<" us";
    while(scheduler_run) {
        start_execution = TimingUtil::getTimeStampInMicroseconds();
        /*
         if(next_predicted_run) {
         //are onthe second
         if((next_prediction_error = (start_execution - next_predicted_run)) < 0){
         next_prediction_error = 0;
         }
         }
         */
        
        //udpate output dataset timestamp tag
        _updateAcquistionTimestamp((uint64_t)start_execution);
        try{
            //! exec the control unit step
            unitRun();
        } catch(CException& ex) {
            //go in recoverable error
            boost::thread(boost::bind(&AbstractControlUnit::_goInRecoverableError, this, ex)).detach();
            boost::this_thread::sleep_for(boost::chrono::seconds(2));
        }  catch(...) {
            CException ex(-1, "undefined error", __PRETTY_FUNCTION__);
            //go in recoverable error
            boost::thread(boost::bind(&AbstractControlUnit::_goInRecoverableError, this, ex)).detach();
            boost::this_thread::sleep_for(boost::chrono::seconds(2));
        }
        
        // check if the output dataset need to be pushed
        pushOutputDataset();
        //calculate the number of microsencods to wait
        //time_to_sleep = schedule_delay - (((next_predicted_run = TimingUtil::getTimeStampInMicroseconds()) - start_execution)+next_prediction_error);
        
        duration = TimingUtil::getTimeStampInMicroseconds() - start_execution;
        time_to_sleep = schedule_delay - duration;
        if(time_to_sleep>0){
            //next_predicted_run = time_to_sleep;
            RTCULDBG_<<" schedule:"<<schedule_delay<<" us wait "<<time_to_sleep<<" us cycle duration:"<<duration<<" us";
            boost::this_thread::sleep_for(boost::chrono::microseconds(time_to_sleep));
        } else {
            next_predicted_run = 0;
            next_prediction_error = 0;
        }
    }
    RTCULAPP_ << "Run Exit";
}
