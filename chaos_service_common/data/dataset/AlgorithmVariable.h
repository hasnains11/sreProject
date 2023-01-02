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

#ifndef __CHAOSFramework_B52AA82F_9020_4D5F_9289_ED8EBDDBCD42_AlgorithmVariable_h
#define __CHAOSFramework_B52AA82F_9020_4D5F_9289_ED8EBDDBCD42_AlgorithmVariable_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/TemplatedDataSDWrapper.h>

namespace chaos {
    namespace service_common {
        namespace data {
            namespace dataset {
                
#define CHAOS_ALGO_NAME          "algo_variable_name"
#define CHAOS_ALGO_DESCRIPTION   "algo_variable_description"
#define CHAOS_ALGO_DIRECTION     "algo_variable_direction"
#define CHAOS_ALGO_TYPE          "algo_variable_type"
#define CHAOS_ALGO_BIN_SUB_TYPE  "algo_variable_bin_sub_type"
#define CHAOS_ALGO_MANDATORY     "algo_variable_mandatory"
#define CHAOS_ALGO_NODE_DS_ATTR  "algo_variable_node_ds_attr"
#define CHAOS_ALGO_NODE_UID      "algo_variable_node_uid"
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(unsigned int, AlgorithmSubtypeList)
                
                //! The description of a n attribute of a CHAOS dataset
                struct AlgorithmVariable {
                    //! is the alias associate to the variable
                    std::string name;
                    
                    //! is the description that explaint the usage of the variable whitin the execution unit
                    std::string description;
                    
                    //! node class
                    chaos::DataType::DataType type;
                    
                    //! direction
                    DataType::DataSetAttributeIOAttribute direction;
                    
                    //!is the list of the subtye in case the type is binary
                    AlgorithmSubtypeList                            subtype_list;
                    
                    //! determinae if the variabl eis mandatory
                    bool                                            mandatory;
                    
                    //! node dataset attribute that has been attacched to variable
                    std::string                                     node_dataset_attribute;
                    
                    //! node unique that is used to fetch the dataset for variable
                    std::string                                     node_uid;
                    
                    AlgorithmVariable():
                    name(),
                    description(),
                    direction(chaos::DataType::Bidirectional),
                    type(chaos::DataType::TYPE_UNDEFINED),
                    subtype_list(),
                    mandatory(false){}
                    
                    AlgorithmVariable(const std::string& _name,
                                      const std::string& _description,
                                      const chaos::DataType::DataType _type,
                                      const DataType::DataSetAttributeIOAttribute _direction,
                                      const AlgorithmSubtypeList& _subtype_list,
                                      const bool _mandatory):
                    name(_name),
                    description(_description),
                    direction(_direction),
                    type(_type),
                    subtype_list(_subtype_list),
                    mandatory(_mandatory){}
                    
                    AlgorithmVariable(const AlgorithmVariable& copy_src):
                    name(copy_src.name),
                    description(copy_src.description),
                    direction(copy_src.direction),
                    type(copy_src.type),
                    subtype_list(copy_src.subtype_list),
                    mandatory(copy_src.mandatory){}
                    
                    AlgorithmVariable& operator=(AlgorithmVariable const &rhs) {
                        name = rhs.name;
                        description = rhs.description;
                        direction = rhs.direction;
                        type = rhs.type;
                        subtype_list = rhs.subtype_list;
                        mandatory = rhs.mandatory;
                        return *this;
                    };
                    
                };
                
                //!helper for create or read the script description
                CHAOS_OPEN_SDWRAPPER(AlgorithmVariable)
                void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                    if(serialized_data == NULL) return;
                    dataWrapped().subtype_list.clear();
                    dataWrapped().name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, CHAOS_ALGO_NAME, "");
                    dataWrapped().description = CDW_GET_SRT_WITH_DEFAULT(serialized_data, CHAOS_ALGO_DESCRIPTION, "");
                    dataWrapped().type = static_cast<chaos::DataType::DataType>(CDW_GET_INT32_WITH_DEFAULT(serialized_data, CHAOS_ALGO_TYPE, chaos::DataType::TYPE_UNDEFINED));
                    dataWrapped().node_dataset_attribute = CDW_GET_SRT_WITH_DEFAULT(serialized_data, CHAOS_ALGO_NODE_DS_ATTR, "");
                    dataWrapped().node_uid = CDW_GET_SRT_WITH_DEFAULT(serialized_data, CHAOS_ALGO_NODE_UID, "");
                    dataWrapped().mandatory = CDW_GET_BOOL_WITH_DEFAULT(serialized_data, CHAOS_ALGO_MANDATORY, false);
                    
                    //we can deserialize data
                    if(serialized_data->hasKey(CHAOS_ALGO_BIN_SUB_TYPE) &&
                       serialized_data->isVectorValue(CHAOS_ALGO_BIN_SUB_TYPE)) {
                        chaos::common::data::CMultiTypeDataArrayWrapperSPtr serialized_array = serialized_data->getVectorValue(CHAOS_ALGO_BIN_SUB_TYPE);
                        for(int idx = 0;
                            idx < serialized_array->size();
                            idx++) {
                            chaos::DataType::BinarySubtype sub_type = static_cast<chaos::DataType::BinarySubtype>(serialized_array->getInt32ElementAtIndex(idx));
                            dataWrapped().subtype_list.push_back(sub_type);
                        }
                    }
                }
                
                ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                    data_serialized->addStringValue(CHAOS_ALGO_NAME, dataWrapped().name);
                    data_serialized->addStringValue(CHAOS_ALGO_DESCRIPTION, dataWrapped().description);
                    data_serialized->addInt32Value(CHAOS_ALGO_TYPE, dataWrapped().type);
                    data_serialized->addStringValue(CHAOS_ALGO_NODE_DS_ATTR, dataWrapped().node_dataset_attribute);
                    data_serialized->addStringValue(CHAOS_ALGO_NODE_UID, dataWrapped().node_uid);
                    data_serialized->addBoolValue(CHAOS_ALGO_MANDATORY, dataWrapped().mandatory);
                    
                    for(AlgorithmSubtypeListIterator it = dataWrapped().subtype_list.begin(),
                        end = dataWrapped().subtype_list.end();
                        it != end;
                        it++) {
                        data_serialized->appendInt32ToArray(*it);
                    }
                    data_serialized->finalizeArrayForKey(CHAOS_ALGO_BIN_SUB_TYPE);
                    return data_serialized;
                }
                CHAOS_CLOSE_SDWRAPPER()
                
                //!a list of a script base information usefullt for search operation
                CHAOS_DEFINE_TYPE_FOR_SD_LIST_WRAPPER(AlgorithmVariable,
                                                      AlgorithmVariableSDWrapper,
                                                      AlgorithmVariableListWrapper);
                
                
                //Definition of script variable list
                CHAOS_DEFINE_VECTOR_FOR_TYPE(chaos::service_common::data::dataset::AlgorithmVariable,
                                             AlgorithmVariableList);
            }
        }
    }
}

#endif /* __CHAOSFramework_B52AA82F_9020_4D5F_9289_ED8EBDDBCD42_AlgorithmVariable_h */
