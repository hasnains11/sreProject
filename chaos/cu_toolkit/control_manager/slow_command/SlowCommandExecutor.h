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


#ifndef __CHAOSFramework__SlowCommandExecutor__
#define __CHAOSFramework__SlowCommandExecutor__

#include <map>
#include <deque>
#include <memory>
#include <stdint.h>


#include <boost/thread.hpp>
#include <boost/heap/priority_queue.hpp>
#include <boost/atomic.hpp>

#include <chaos/common/data/DatasetDB.h>
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/data/cache/AttributeCache.h>
#include <chaos/common/batch_command/BatchCommand.h>
#include <chaos/common/metadata_logging/metadata_logging.h>
#include <chaos/common/batch_command/BatchCommandExecutor.h>

#include <chaos/cu_toolkit/data_manager/KeyDataStorage.h>
#include <chaos/cu_toolkit/driver_manager/DriverErogatorInterface.h>

namespace chaos {
    namespace cu {

        //forward declaration
        namespace dm {
            namespace driver {
                class DriverAccessor;
            }
        }

        namespace control_manager {
			//forward declaration
			class AbstractControlUnit;
			class SCAbstractControlUnit;

            namespace slow_command {

				namespace boost_cont = boost::container;
				namespace chaos_data = chaos::common::data;
				namespace chaos_cache = chaos::common::data::cache;
				namespace chaos_batch = chaos::common::batch_command;

                //forward declaration
                class SlowCommand;

#define SLOWCOMMAND_INSTANCER(SlowCommandClass)\
new chaos::common::utility::NestedObjectInstancer<chaos::cu::control_manager::slow_command::SlowCommand, chaos::common::batch_command::BatchCommand>(\
new chaos::common::utility::TypedObjectInstancer<SlowCommandClass, chaos::cu::control_manager::slow_command::SlowCommand>())

                //! Slow command execution sand box
                /*!
                    This class is the environment where the exeecution of the slow command handlers take place.
                 */
                class SlowCommandExecutor:
                private chaos_batch::BatchCommandExecutor {

                    friend class chaos::cu::control_manager::SCAbstractControlUnit;

					//! The driver erogator
					chaos::cu::driver_manager::DriverErogatorInterface *driverAccessorsErogator;

					//! attribute cache wrapper
					AttributeSharedCacheWrapper * attribute_cache;

					AbstractControlUnit *control_unit_instance;
					//fast unit last id cache value
					AttributeValue *last_ru_id_cache;
					//fast cache error variable accessor
					AttributeValue *last_error_code;
					AttributeValue *last_error_message;
					AttributeValue *last_error_domain;
                    
                    //------mds logging---------
                    chaos::common::metadata_logging::ErrorLoggingChannel        *error_logging_channel;
                    chaos::common::metadata_logging::BatchCommandLoggingChannel *command_logging_channel;
                protected:

                    //! Private constructor
                    SlowCommandExecutor(const std::string& _executorID,
										SCAbstractControlUnit *_control_unit_instance);

                    //! Private deconstructor
                    ~SlowCommandExecutor();

                    //allocate a new command
                    chaos_batch::BatchCommand *instanceCommandInfo(const std::string& command_alias,
                                                                   uint32_t submission_rule,
                                                                   uint32_t submission_retry_delay,
                                                                   uint64_t scheduler_step_delay);

					//overlodaed command event handler
					void handleCommandEvent(const std::string& command_alias,
                                            uint64_t command_seq,
											chaos_batch::BatchCommandEventType::BatchCommandEventType type,
                                            chaos::common::batch_command::CommandInfoAndImplementation *command_info,
                                            const chaos_batch::BatchCommandStat& commands_stats);

					//! general sandbox event handler
					void handleSandboxEvent(const std::string& sandbox_id,
											chaos_batch::BatchSandboxEventType::BatchSandboxEventType type,
											void* type_value_ptr,
											uint32_t type_value_size);
                public:

                    // Initialize instance
                    void init(void*);

					// Start the implementation
					void start();

					// Start the implementation
					void stop();

					// Deinit instance
					void deinit();

					//! Install a command associated with a type
                    void installCommand(const std::string& alias,
										chaos::common::utility::NestedObjectInstancer<SlowCommand, chaos_batch::BatchCommand> *instancer);
                    //! Install a command
                    void installCommand(ChaosSharedPtr<common::batch_command::BatchCommandDescription> command_description);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SlowCommandExecutor__) */
