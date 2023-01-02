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

#ifndef __CHAOSFramework__InizializableService__
#define __CHAOSFramework__InizializableService__
#ifdef _WIN32
#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__  __FUNCSIG__
#endif
#endif
#include <inttypes.h>

// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
// for And_ operator
#include <boost/msm/front/euml/operator.hpp>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/exception/exception.h>




namespace chaos {
	namespace common {
		namespace utility {
			
			namespace service_state_machine {
				
				//SM Event
				namespace EventType {
					struct init {};
					struct deinit {};
				}
				
				// The list of FSM states
				struct Deinit : public boost::msm::front::state<>{};
				
				struct Init : public boost::msm::front::state<> {};
				
				// front-end: define the FSM structure
				struct id_states_machine : public boost::msm::front::state_machine_def<id_states_machine> {
					
					// the initial state of the player SM. Must be defined
					typedef Deinit initial_state;
					
					typedef boost::msm::front::Row <  Deinit   ,  EventType::init    , Init, boost::msm::front::none   , boost::msm::front::none > deinit_init_row;
					typedef boost::msm::front::Row <  Init     ,  EventType::deinit  , Deinit, boost::msm::front::none , boost::msm::front::none > init_deinit_row;

					// Transition table for initialization services
					struct transition_table : boost::mpl::vector<
					deinit_init_row,
					init_deinit_row > {};
					
					template <class FSM,class Event>
					void no_transition(Event const& ,FSM&, int )
					{
					  //	std::cout << "no trnasition";
					}
				};
			}
			
			class InizializableService {
				boost::msm::back::state_machine< service_state_machine::id_states_machine > state_machine;
			protected:
				unsigned int serviceState;
				
				//! Initialize instance
				virtual void init(void*);
				
				//! Deinit the implementation
				virtual void deinit();
				
			public:
				InizializableService();
				virtual ~InizializableService();
				//! Return the state
				const uint8_t getServiceState() const;
				
				
				static bool initImplementation(InizializableService& impl, void *initData, const std::string & implName,  const std::string & domainString);
				static bool deinitImplementation(InizializableService& impl, const std::string & implName,  const std::string & domainString);
				
				static bool initImplementation(InizializableService *impl, void *initData, const std::string & implName,  const std::string & domainString);
				static bool deinitImplementation(InizializableService *impl, const std::string & implName,  const std::string & domainString);
			};
			
			template<typename T>
			class InizializableServiceContainer {
				bool delete_on_dispose;
				std::string service_name;
				T *startable_service_instance;
			public:
				InizializableServiceContainer():
				delete_on_dispose(true),
				startable_service_instance(NULL){}
				
				InizializableServiceContainer(bool _delete_on_dispose,
											  const std::string & instance_name):
				delete_on_dispose(_delete_on_dispose),
				startable_service_instance(new T()),
				service_name(instance_name) {}
				
				InizializableServiceContainer(T *instance,
											  bool _delete_on_dispose,
											  const std::string & instance_name):
				startable_service_instance(instance),
				delete_on_dispose(_delete_on_dispose),
				service_name(instance_name) {}
				
				~InizializableServiceContainer() {
					if(delete_on_dispose) delete(startable_service_instance);
				}
				
				bool init(void *init_data, const std::string & domainString) {
					return InizializableService::initImplementation(startable_service_instance, init_data, service_name, domainString);
				}
				
				bool deinit(const std::string & domainString) {
					return InizializableService::deinitImplementation(startable_service_instance, service_name, domainString);
				}
				
				bool isInstantiated() {
					return startable_service_instance != NULL;
				}
				
				T* get() {
					return startable_service_instance;
				}
				void reset(T *new_instance, const std::string & instance_name) {
					if(startable_service_instance) {
						delete startable_service_instance;
					}
					startable_service_instance = new_instance;
					service_name = instance_name;
				}
				
				T& operator*() {
					return *startable_service_instance;
				}
				
				T* operator->() {
					return startable_service_instance;
				}
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__InizializableService__) */
