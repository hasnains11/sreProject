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

#ifndef __CHAOSFramework_CB7E9739_B802_4DF0_BCFC_B98DAE7E0669_MongoDBAgentDataAccess_h
#define __CHAOSFramework_CB7E9739_B802_4DF0_BCFC_B98DAE7E0669_MongoDBAgentDataAccess_h

#include "../data_access/AgentDataAccess.h"
#include "MongoDBUtilityDataAccess.h"
#include "MongoDBNodeDataAccess.h"
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace mongodb {
                //forward declaration
                class MongoDBPersistenceDriver;
                
                //! Data Access for manage the log
                /*!
                 */
                class MongoDBAgentDataAccess:
                public data_access::AgentDataAccess,
                protected service_common::persistence::mongodb::MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                    
                    MongoDBUtilityDataAccess *utility_data_access = NULL;
                    MongoDBNodeDataAccess *node_data_access = NULL;
                    //return the query for a page
                    mongo::Query getNextPagedQuery(uint64_t last_sequence_before_this_page,
                                                   const std::string& search_string);
                    
                    mongo::Query getNextPagedQueryForInstance(uint64_t last_sequence_before_this_page,
                                                              const std::string& script_name,
                                                              const std::string& search_string);
                    
                protected:
                    MongoDBAgentDataAccess(const ChaosSharedPtr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    ~MongoDBAgentDataAccess();
                public:
                    //! inherited by data_access::AgentDataAccess
                    int insertUpdateAgentDescription(chaos::common::data::CDataWrapper& agent_description);

                    //!load full agent description
                    int loadAgentDescription(const std::string& agent_uid,
                                             const bool load_related_data,
                                             chaos::service_common::data::agent::AgentInstance& agent_description);
                    
                    //! inherited by data_access::AgentDataAccess
                    int getAgentForNode(const std::string& associated_node_uid,
                                        std::string& agent_uid);
                    
                    //! inherited by data_access::AgentDataAccess
                    int saveNodeAssociationForAgent(const std::string& agent_uid,
                                                    chaos::service_common::data::agent::AgentAssociation& node_association);
                    //! inherited by data_access::AgentDataAccess
                    int loadNodeAssociationForAgent(const std::string& agent_uid,
                                                    const std::string& associated_node_uid,
                                                    chaos::service_common::data::agent::AgentAssociation& node_association);
                    //! inherited by data_access::AgentDataAccess
                    int removeNodeAssociationForAgent(const std::string& agent_uid,
                                                      const std::string& associated_node_uid);
                    //! inherited by data_access::AgentDataAccess
                    int setNodeAssociationStatus(const std::string& agent_uid,
                                                 const chaos::service_common::data::agent::AgentAssociationStatus& status);
                    
                    //! inherited by data_access::AgentDataAccess
                    int getNodeListStatusForAgent(const std::string& agent_uid,
                                                  chaos::service_common::data::agent::VectorAgentAssociationStatus& node_status_vec);
                    
                    //! inherited by data_access::AgentDataAccess
                    int pushLogEntry(const std::string& node_uid,
                                     const std::string& node_log_entry);
                    
                    //! inherited by data_access::AgentDataAccess
                    int getLogEntry(const std::string& node_uid,
                                    const int32_t number_of_entries,
                                    const bool asc,
                                    const uint64_t start_seq,
                                    chaos::service_common::data::agent::VectorAgentLogEntry& found_entries);
                    
                    //! inherited by data_access::AgentDataAccess
                    int setLogEntryExpiration(const bool expiration_active,
                                              const uint32_t expiration_in_seconds);
                    
                    //! inherited by data_access::AgentDataAccess
                    int getLogEntryExpiration(bool& expiration_active,
                                              uint32_t& expiration_in_seconds);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_CB7E9739_B802_4DF0_BCFC_B98DAE7E0669_MongoDBAgentDataAccess_h */
