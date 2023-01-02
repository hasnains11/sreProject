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
#ifndef CHAOSFramework_api_types_h
#define CHAOSFramework_api_types_h

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/data/CDataWrapperKeyValueSetter.h>
#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace node {
                //create list types for cdatawrapper setter
                CHAOS_DEFINE_VECTOR_FOR_TYPE(ChaosSharedPtr<chaos::common::data::CDataWrapperKeyValueSetter>, ParameterSetterList)
                /*!
                 wrap the command template information
                 */
                struct CommandTemplate {
                    //! the representative name of the template (unique)
                    std::string template_name;
                    //! the unique id of the command
                    std::string command_unique_id;
                    //!the setter for the paramter
                    ParameterSetterList parameter_value_list;
                    //!the submission rule
                    uint32_t submission_rule;
                    //! the priority of the command within the whait queue
                    uint32_t submission_priority;
                    //! the run schedule delay between a runa and the next(in microseconds)
                    uint64_t schedule_step_delay;
                    //! the delay between the submission retry, waithing a favorable current command state(in milliseconds)
                    uint32_t submission_retry_delay;
                    //! the execution channel where run the command
                    uint32_t execution_channel;
                };

                CHAOS_DEFINE_VECTOR_FOR_TYPE(ChaosSharedPtr<CommandTemplate>, TemplateList)
                
                struct TemplateSubmission {
                    //! unique id of the node that need to receive the command
                    std::string node_unique_id;
                    //! the representative name of the template (unique)
                    std::string template_name;
                    //! the unique id of the command
                    std::string command_unique_id;
                    //! value for parametrized attribute in tempalte
                    ParameterSetterList parametrized_attribute_value;
                };
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(ChaosSharedPtr<TemplateSubmission>, TemplateSubmissionList)
                
                //define the list, iterator and cost iterator
                CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, CommandUIDList)
                
                //!group of property
                struct NodePropertyGroup {
                    std::string           group_name;
                    ParameterSetterList   group_property_list;
                };
                CHAOS_DEFINE_VECTOR_FOR_TYPE(ChaosSharedPtr<NodePropertyGroup>, NodePropertyGroupList)
            }
        }
    }
}


#endif
