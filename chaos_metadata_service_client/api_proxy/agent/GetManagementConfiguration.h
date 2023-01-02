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

#ifndef __CHAOSFramework__4990BF5_1D2A_4E32_96D1_D01CD1B6A6D9_GetManagementConfiguration_h
#define __CHAOSFramework__4990BF5_1D2A_4E32_96D1_D01CD1B6A6D9_GetManagementConfiguration_h

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace agent {
                
                //! Set the managment configuration for agent
                class GetManagementConfiguration:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(GetManagementConfiguration)
                protected:
                    API_PROXY_CD_DECLARATION(GetManagementConfiguration)
                public:
                    ApiProxyResult execute();
                    static chaos::service_common::data::agent::AgentManagementSetting deserialize(chaos::common::data::CDataWrapper& api_result);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__4990BF5_1D2A_4E32_96D1_D01CD1B6A6D9_GetManagementConfiguration_h */
