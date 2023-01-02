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

#ifndef __CHAOSFramework__GetLogForSourceUID_h
#define __CHAOSFramework__GetLogForSourceUID_h

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>
#include <chaos_metadata_service_client/api_proxy/logging/logging_types.h>

#include <chaos/common/chaos_types.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace logging {
                
                class GetLogForSourceUIDHelper {
                    LogEntryList log_entry_list;
                public:
                    GetLogForSourceUIDHelper(chaos::common::data::CDataWrapper *api_result);
                    virtual ~GetLogForSourceUIDHelper();
                    size_t getLogEntryListSize();
                    const LogEntryList& getLogEntryList();
                };
                
                //! get log entries for a source
                class GetLogForSourceUID:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(GetLogForSourceUID)
                protected:
                    API_PROXY_CD_DECLARATION(GetLogForSourceUID)
                public:
                    //! find all log entry for a source id specifying the domain and other set
                    /*!
                     \param source node that has emitted the log entries
                     \param domain if is not and empty string it will be used to filter domain in log entries
                     \param last_sequence_id is the id of the last returned entries in the past query
                     \param page_length is the length of the returned element
                     */
                    ApiProxyResult execute(const std::string& source,
                                           const std::string& domain = std::string(),
                                           const uint64_t last_sequence_id = 0,
                                           const uint32_t page_length = 30);
                    
                    //! find all log entry for a source id specifying a list of domain and other set
                    /*!
                     \param source node that has emitted the log entries
                     \param domain is the lis tof domain to wich need to belog the result entry
                     \param last_sequence_id is the id of the last returned entries in the past query
                     \param page_length is the length of the returned element
                     */
                    ApiProxyResult execute(const std::string& source,
                                           const LogDomainList& domain_list,
                                           const uint64_t last_sequence_id,
                                           const uint32_t page_length = 30);

                    static ChaosUniquePtr<GetLogForSourceUIDHelper> getHelper(chaos::common::data::CDataWrapper *api_result);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__GetLogForSourceUID_h */
