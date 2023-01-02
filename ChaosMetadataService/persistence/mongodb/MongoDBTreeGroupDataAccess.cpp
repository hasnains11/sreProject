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

#include "mongo_db_constants.h"
#include "MongoDBTreeGroupDataAccess.h"

#include <chaos/common/global.h>

#include <mongo/client/dbclient.h>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#define MDBTGDA_INFO INFO_LOG(MongoDBTreeGroupDataAccess)
#define MDBTGDA_DBG  DBG_LOG(MongoDBTreeGroupDataAccess)
#define MDBTGDA_ERR  ERR_LOG(MongoDBTreeGroupDataAccess)

using namespace chaos::common::data;
using namespace chaos::service_common;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;

MongoDBTreeGroupDataAccess::MongoDBTreeGroupDataAccess(const ChaosSharedPtr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection),
TreeGroupDataAccess(){}

MongoDBTreeGroupDataAccess::~MongoDBTreeGroupDataAccess() {}

#pragma mark Prtoected Method

int MongoDBTreeGroupDataAccess::checkPathPresenceForDomain(const std::string& group_domain,
                                                           const std::string& tree_path,
                                                           bool& presence) {
    int err = 0;
    std::string node_name;
    std::string tree_path_tmp;
    unsigned long long count = 0;
    std::vector< std::string > tree_path_node;
    try {
        //estract node and parent path from full path
        if(!estractNodeFromPath(tree_path,
                                node_name,
                                tree_path_tmp)){
            return ErrorMDSCode::EC_MDS_NODE_BAD_PATH_EXTRACTION;
        }
        
        mongo::BSONObj q = BSON("node_name" << node_name <<
                                "node_parent_path" << tree_path_tmp <<
                                "node_domain"<< group_domain);
        
        
        DEBUG_CODE(MDBTGDA_ERR<<log_message("checkPathPresenceForDomain",
                                            "count",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        
        if((err = connection->count(count,
                                    MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_TREE_GROUP),
                                    q))){
            MDBTGDA_ERR << "Error checing the presence of node "<<node_name<<" of the path "<< tree_path_tmp << " with code " << err;
        } else {
            presence = (count == 1);
        }
    } catch (const mongo::DBException &e) {
        MDBTGDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBTGDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

/*!
 Check if a node is presente.
 */
int MongoDBTreeGroupDataAccess::checkNodePresenceForDomain(const std::string& group_domain,
                                                           const std::string& node_name,
                                                           const std::string& tree_path,
                                                           bool& presence) {
    int err = 0;
    unsigned long long count = 0;
    try {
        //we first need to fetch all node uid attacched to the snapshot
        mongo::BSONObj q = BSON("node_name" << node_name <<
                                "node_parent_path" << tree_path <<
                                "node_domain"<< group_domain);
        
        
        DEBUG_CODE(MDBTGDA_ERR<<log_message("checkNodePresenceForDomain",
                                            "count",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        
        if((err = connection->count(count,
                                    MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_TREE_GROUP),
                                    q))){
            MDBTGDA_ERR << "Error checing presence for node "<<node_name<<"["<<tree_path<<"]"<<" in domain "<<group_domain << " with code " << err;
        } else {
            presence = (count == 1);
        }
    } catch (const mongo::DBException &e) {
        MDBTGDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBTGDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

#pragma mark Public Method
//! Inherited method
int MongoDBTreeGroupDataAccess::deleteGroupDomain(const std::string& group_domain)  {
    int err = 0;
    std::vector< std::string > tree_path_node;
    try {
        //we first need to fetch all node uid attacched to the snapshot
        mongo::BSONObj q = BSON("node_domain"<< group_domain);
        
        
        DEBUG_CODE(MDBTGDA_ERR<<log_message("deleteGroupDomain",
                                            "remove",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        
        if((err = connection->remove(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_TREE_GROUP),
                                     q))){
            MDBTGDA_ERR << "Error removing all node group in domain "<< group_domain << " with code " << err;
        }
    } catch (const mongo::DBException &e) {
        MDBTGDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBTGDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBTreeGroupDataAccess::getAllGroupDomain(std::vector<std::string>& group_domain_list) {
    int err = 0;
    std::vector<std::string> distinct_values;
    mongo::BSONObj distinct_result;
    try {
        
        DEBUG_CODE(MDBTGDA_ERR<<log_message("getAllGroupDomain",
                                            "query",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    "no query"));)
        
        distinct_result = connection->distinct(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_TREE_GROUP), "node_domain");
        
        if(!distinct_result.isEmpty() && distinct_result.couldBeArray()){
            distinct_result.Vals<std::string>(group_domain_list);
        }
    } catch (const mongo::DBException &e) {
        MDBTGDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBTGDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

//! Inherited method
int MongoDBTreeGroupDataAccess::addNewNodeGroupToDomain(const std::string& group_domain,
                                                        const std::string& node_name,
                                                        const std::string& parent_path) {
    int err = 0;
    bool presence = false;
    try {
        //first i need to check if the node is already present
        if((err = checkNodePresenceForDomain(group_domain,
                                             node_name,
                                             parent_path,
                                             presence))) {
            return err;
        } else if(presence == false) {
            //if is not present i need to check if the parent is present
            if((err = checkPathPresenceForDomain(group_domain,
                                                 parent_path,
                                                 presence))) {
                return err;
            } else if(presence == false){
                MDBTGDA_ERR << "The parent for node "<<node_name<<"["<<parent_path<<"]in domain "<< group_domain << " is node present present";
                return ErrorMDSCode::EC_MDS_PARENT_NODE_NOT_IN_DOMAIN;
            }
        } else {
            MDBTGDA_ERR << "The group node "<<node_name<<"["<<parent_path<<"]in domain "<< group_domain << " is already present";
            return ErrorMDSCode::EC_MDS_NODE_DUPLICATION;
        }
        
        if(!checkNodeNameSintax(node_name)) {
            return ErrorMDSCode::EC_MDS_NODE_BAD_NAME;
        }
        
        //create the bson element that identify the node
        mongo::BSONObj q = BSON("node_name" << node_name <<
                                "node_parent_path" << parent_path <<
                                "node_domain"<< group_domain);
        
        
        DEBUG_CODE(MDBTGDA_ERR<<log_message("addNewNodeGroupToDomain",
                                            "insert",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        
        if((err = connection->insert(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_TREE_GROUP),
                                     q))){
            MDBTGDA_ERR << "Error creating a new group node "<<node_name<<"["<<parent_path<<"]in domain "<< group_domain << " with code " << err;
        }
    } catch (const mongo::DBException &e) {
        MDBTGDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBTGDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

//! inherited method
int MongoDBTreeGroupDataAccess::addNewNodeGroupToDomain(const std::string& group_domain,
                                                        const std::string& node_group_name) {
    int err = 0;
    try {
        if(!checkNodeNameSintax(node_group_name)) {
            return ErrorMDSCode::EC_MDS_NODE_BAD_NAME;
        }
        //create the bson element that identify the node
        mongo::BSONObj q = BSON("node_name" << node_group_name <<
                                "node_parent_path" << "" <<
                                "node_domain"<< group_domain);
        
        
        DEBUG_CODE(MDBTGDA_ERR<<log_message("addNewNodeGroupToDomain",
                                            "insert",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        
        if((err = connection->insert(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_TREE_GROUP),
                                     q))){
            MDBTGDA_ERR << "Error creating a new root group node "<<node_group_name<<" in domain "<< group_domain << " with code " << err;
        }
    } catch (const mongo::DBException &e) {
        MDBTGDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBTGDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

//! Inherited method
int MongoDBTreeGroupDataAccess::deleteNodeGroupToDomain(const std::string& group_domain,
                                                        const std::string& tree_path) {
    int err = 0;
    std::string node_name;
    std::string parent_path;
    try {
        //get node and paretn path from full path description
        if(estractNodeFromPath(tree_path,
                               node_name,
                               parent_path)){
            // call api specialized with node name
            err = deleteNodeGroupToDomain(group_domain,
                                          node_name,
                                          parent_path);
        }
    } catch (const mongo::DBException &e) {
        MDBTGDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBTGDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

//! Inherited method
int MongoDBTreeGroupDataAccess::deleteNodeGroupToDomain(const std::string& group_domain,
                                                        const std::string& node_group_name,
                                                        const std::string& parent_path) {
    int err = 0;
    bool presence = false;
    try {
        if((err = checkNodePresenceForDomain(group_domain,
                                             node_group_name,
                                             parent_path,
                                             presence))){
            return ErrorMDSCode::EC_MDS_PARENT_NODE_NOT_IN_DOMAIN;
        } else if(presence == false) {
            MDBTGDA_ERR << "The node "<<node_group_name<<" for parent path" << parent_path << " in the domain "<<group_domain<<" is not present";
            return 0;
        }
    
        //delete node and child that have node as parent
        mongo::BSONArrayBuilder or_builder;
        mongo::BSONArrayBuilder and_builder;
        //node query
        or_builder.append(BSON("node_name" << node_group_name <<
                                "node_parent_path" << parent_path));
        //child query
        const std::string path_reg_ex = parent_path + "/" + node_group_name +".*";
        or_builder.append(BSON("node_parent_path" << BSON("$regex" << path_reg_ex)));
        
//        mongo::BSONObj q = BSON("node_name" << node_group_name <<
//                                "node_parent_path" << parent_path <<
//                                "node_domain"<< group_domain);
        and_builder.append(BSON("node_domain"<< group_domain));
        and_builder.append(BSON("$or" << or_builder.arr()));
        mongo::BSONObj q = BSON("$and"<< and_builder.arr());
        
        DEBUG_CODE(MDBTGDA_ERR<<log_message("addNewNodeGroupToDomain",
                                            "delete",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        
        if((err = connection->remove(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_TREE_GROUP),
                                     q))){
            MDBTGDA_ERR << "Error deleting the group node "<<node_group_name<<"["<<parent_path<<"]in domain "<< group_domain << " with code " << err;
        }
    } catch (const mongo::DBException &e) {
        MDBTGDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBTGDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

//! Inherited method
int MongoDBTreeGroupDataAccess::addChaosNodeToGroupDomain(const std::string& group_domain,
                                                          const std::string& tree_path,
                                                          const std::string& chaos_node_type,
                                                          const std::string& chaos_node_uid) {
    int err = 0;
    std::string node_name;
    std::string parent_path;
    bool presence = false;
    try {
        //get node and paretn path from full path description
        if((err = checkPathPresenceForDomain(group_domain,
                                             tree_path,
                                             presence))){
            return ErrorMDSCode::EC_MDS_PARENT_NODE_NOT_IN_DOMAIN;
        }
        if(presence == false) {
            MDBTGDA_ERR << "The node for tree path" << tree_path << " is not present";
            return 0;
        }
        
        
        //create the bson element that identify the node
        mongo::BSONObj q = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_name <<
                                chaos::NodeDefinitionKey::NODE_TYPE << chaos_node_type);
        mongo::BSONObj u = BSON("$addToSet" << BSON_ARRAY("parent_tree_group"<<BSON("tree_domain"<<group_domain<<"tree_path"<<tree_path)));
        
        DEBUG_CODE(MDBTGDA_ERR<<log_message("addChaosNodeToGroupDomain",
                                            "update",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "Update",
                                                                    q.jsonString(),
                                                                    u.jsonString()));)
        
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     q,
                                     u))){
            MDBTGDA_ERR << "Error Updating the chaos node "<<chaos_node_uid<<" for add it to the tree path" << tree_path;
        }
    } catch (const mongo::DBException &e) {
        MDBTGDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBTGDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

//! Inherited method
int MongoDBTreeGroupDataAccess::getNodeChildFromPath(const std::string& group_domain,
                                                     const std::string& tree_path,
                                                     std::vector<std::string>& node_child) {
    int err = 0;
    bool presence = false;
    ChaosUniquePtr<mongo::DBClientCursor> query_result;
    try {
        
        //get node and paretn path from full path description
        if((err = checkPathPresenceForDomain(group_domain,
                                             tree_path,
                                             presence))){
            return ErrorMDSCode::EC_MDS_PARENT_NODE_NOT_IN_DOMAIN;
        }
        if(presence == false) {
            MDBTGDA_ERR << "The node for tree path" << tree_path << " is not present";
            return 0;
        }
        
        //create the bson element that identify the node
        mongo::BSONObj q = BSON("node_parent_path" << tree_path <<
                                "node_domain"<< group_domain);
        
        DEBUG_CODE(MDBTGDA_ERR<<log_message("getNodeChildFromPath",
                                            "query",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        
        query_result = connection->query(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_TREE_GROUP), q);
        while(query_result->more()){
            mongo::BSONObj element = query_result->next();
            node_child.push_back(element.getStringField("node_name"));
        }
    } catch (const mongo::DBException &e) {
        MDBTGDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBTGDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

//! Inherited method
int MongoDBTreeGroupDataAccess::getNodeRootFromDomain(const std::string& group_domain,
                                                      std::vector<std::string>& node_child) {
    int err = 0;
    ChaosUniquePtr<mongo::DBClientCursor> query_result;
    try {
        
        //create the bson element that identify the node
        mongo::BSONObj q = BSON("node_parent_path" << "" <<
                                "node_domain"<< group_domain);
        
        DEBUG_CODE(MDBTGDA_ERR<<log_message("getNodeChildFromPath",
                                            "query",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        
        query_result = connection->query(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_TREE_GROUP), q);
        while(query_result->more()){
            mongo::BSONObj element = query_result->next();
            node_child.push_back(element.getStringField("node_name"));
        }
    } catch (const mongo::DBException &e) {
        MDBTGDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBTGDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

//! Inherited method
int MongoDBTreeGroupDataAccess::removeChaosNodeFromGroupDomain(const std::string& group_domain,
                                                               const std::string& tree_path,
                                                               const std::string& chaos_node_type,
                                                               const std::string& chaos_node_uid) {
    int err = 0;
    std::string node_name;
    std::string parent_path;
    bool presence = false;
    try {
        //get node and paretn path from full path description
        if((err = checkPathPresenceForDomain(group_domain,
                                             tree_path,
                                             presence))){
            return ErrorMDSCode::EC_MDS_PARENT_NODE_NOT_IN_DOMAIN;
        }
        if(presence == false) {
            MDBTGDA_ERR << "The node for tree path" << tree_path << " is not present";
            return 0;
        }
        
        
        //create the bson element that identify the node
        mongo::BSONObj q = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_name <<
                                chaos::NodeDefinitionKey::NODE_TYPE << chaos_node_type);
        mongo::BSONObj u = BSON("$pop" << BSON_ARRAY("parent_tree_group"<<BSON("tree_domain"<<group_domain<<"tree_path"<<tree_path)));
        
        DEBUG_CODE(MDBTGDA_ERR<<log_message("removeChaosNodeFromGroupDomain",
                                            "update",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "Update",
                                                                    q.jsonString(),
                                                                    u.jsonString()));)
        
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     q,
                                     u))){
            MDBTGDA_ERR << "Error Updating the chaos node "<<chaos_node_uid<<" for add it to the tree path" << tree_path;
        }
    } catch (const mongo::DBException &e) {
        MDBTGDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBTGDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

//! Inherited method
int MongoDBTreeGroupDataAccess::getChaosNodeFromGroupDomain(const std::string& group_domain,
                                                            const std::string& tree_path,
                                                            data_access::TreeGroupChaosNodeList& chaos_node_list_in_group,
                                                            const unsigned int page_size,
                                                            const std::string& last_node_uid) {
    int err = 0;
    std::string node_name;
    std::string parent_path;
    ChaosUniquePtr<mongo::DBClientCursor> query_result;
    try {
        
        //create the bson element that identify the node
        mongo::BSONObj q = BSON("parent_tree_group.tree_domain"<<group_domain<<
                                "parent_tree_group.tree_path"<<tree_path);
        
        DEBUG_CODE(MDBTGDA_ERR<<log_message("getNodeChildFromPath",
                                            "update",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        
        query_result = connection->query(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES), q);
        while(query_result->more()){
            mongo::BSONObj element = query_result->next();
            chaos_node_list_in_group.push_back(data_access::ChaosNodeInGroupElement(element.getStringField(chaos::NodeDefinitionKey::NODE_UNIQUE_ID),
                                                                                    element.getStringField(chaos::NodeDefinitionKey::NODE_TYPE)));
        }
    } catch (const mongo::DBException &e) {
        MDBTGDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBTGDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}
