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
#ifndef __CHAOSFramework__DriverManager__
#define __CHAOSFramework__DriverManager__

#include <map>
#include <vector>

#include <chaos/common/utility/Singleton.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/plugin/PluginInspector.h>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriver.h>
#include <chaos/cu_toolkit/driver_manager/driver/DriverAccessor.h>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace chaos {
	namespace cu {
		namespace control_manager {
			//forward declaration
			class AbstractControlUnit;
		}
		namespace driver_manager {
			
			#define MATERIALIZE_INSTANCE_AND_INSPECTOR(_DriverClass_) \
			ChaosSharedPtr< chaos::common::plugin::PluginInspector> _DriverClass_ ## Inspector(GET_PLUGIN_CLASS_DEFINITION(_DriverClass_)); \
			ChaosSharedPtr< chaos::common::utility::ObjectInstancer< chaos::cu::driver_manager::driver::AbstractDriver > > _DriverClass_ ## Instancer(CU_DRIVER_INSTANCER(_DriverClass_));
			
#define MATERIALIZE_INSTANCE_AND_INSPECTOR_WITH_NS(_n_, _DriverClass_)	\
			ChaosSharedPtr< chaos::common::plugin::PluginInspector> _DriverClass_ ## Inspector(GET_PLUGIN_CLASS_DEFINITION(_DriverClass_)); \
			  ChaosSharedPtr< chaos::common::utility::ObjectInstancer< chaos::cu::driver_manager::driver::AbstractDriver > > _DriverClass_ ## Instancer(CU_DRIVER_INSTANCER(_n_::_DriverClass_));
			
#define CU_DRIVER_INSTANCER(_DriverClass_) new chaos::common::utility::TypedObjectInstancer< _DriverClass_, chaos::cu::driver_manager::driver::AbstractDriver >()
		  
#define REGISTER_DRIVER(_n_,_driver_)				\
		  MATERIALIZE_INSTANCE_AND_INSPECTOR_WITH_NS(_n_,_driver_); \
		  chaos::cu::driver_manager::DriverManager::getInstance()->registerDriver(_driver_ ##Instancer, _driver_ ##Inspector);


			typedef struct DriverPluginInfo {
				ChaosSharedPtr< chaos::common::plugin::PluginInspector > sp_inspector;
				ChaosSharedPtr< chaos::common::utility::ObjectInstancer<chaos::cu::driver_manager::driver::AbstractDriver> > sp_instancer;
			} DriverPluginInfo;
			
			//! !CHAOS Driver infrastructure managment
			class DriverManager:
			public common::utility::StartableService,
			public chaos::common::utility::Singleton<DriverManager> {
				friend class chaos::common::utility::Singleton<DriverManager>;
				friend class chaos::cu::control_manager::AbstractControlUnit;
				
				//! this map correlate the alias to the object instancer
				std::map<std::string, ChaosSharedPtr< DriverPluginInfo > > mapDriverAliasVersionInstancer;
				
				//! this map correlate the param md5 to a live driver instance
				/*!
					the param hashing identify the CDataWrapper with the initilization param for the init pahse of the
					device driver. So in this way when a new initialization wil be requested, before a new instance of the
					is created, the hashing of the input parameter is checked if it is contained in the map. In case
					of successfull test a live instance is returned.
				 */
                std::map<std::string, chaos::cu::driver_manager::driver::AbstractDriver*> mapParameterLiveInstance;
				
				
				std::map<std::string, chaos::cu::driver_manager::driver::AbstractDriver*> mapDriverUUIDHashLiveInstance;
				
				//!Mutex for priority queue managment
				boost::shared_mutex    mutextMapAccess;
			protected:
				
				/*
				 Constructor
				 */
				DriverManager();
				
				/*
				 Desctructor
				 */
				~DriverManager();
				
                // Initialize instance
				void init(void *initParameter);
				
                // Start the implementation
				void start();
				
                // Stop the implementation
				void stop();
				
				
                // Deinit the implementation
				void deinit();
				
				
				//! Get a new driver accessor for a driver instance
				/*!
				 checking the hashing of the input parameter is created (or got one) device driver instance,
				 from this a new driver accessor is created
				 */
				chaos::cu::driver_manager::driver::DriverAccessor *getNewAccessorForDriverInstance(chaos::cu::driver_manager::driver::DrvRequestInfo& request_info);
				
				//! release the accessor instance
				void releaseAccessor(chaos::cu::driver_manager::driver::DriverAccessor *accessor);
			public:

				
				//! Register a new driver
				void registerDriver(ChaosSharedPtr< chaos::common::utility::ObjectInstancer<chaos::cu::driver_manager::driver::AbstractDriver> > instancer,
									ChaosSharedPtr< chaos::common::plugin::PluginInspector > description);

			};
		}
	}
}


#endif /* defined(__CHAOSFramework__DriverManager__) */
