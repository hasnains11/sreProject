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

#ifndef __CHAOSFramework__CUCommonUtility_hpp
#define __CHAOSFramework__CUCommonUtility_hpp

#include "../persistence/persistence.h"

#include <chaos/common/property/property.h>
#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    namespace metadata_service {
        namespace common {
            
            class CUCommonUtility {
                void mergeDefaultToProperty(chaos::common::property::PropertyGroupVector& src_group,
                                            chaos::common::property::PropertyGroupVector& dst_group);
                
                static chaos::common::data::CDWShrdPtr getConfigurationToUse(const std::string& cu_uid,
                                                                             const std::string& ds_attribute_name,
                                                                             chaos::metadata_service::persistence::data_access::NodeDataAccess *n_da,
                                                                             chaos::metadata_service::persistence::data_access::ControlUnitDataAccess *cu_da,
                                                                             chaos::common::property::PropertyGroup& control_unit_property_group);
                
            public:
                static ChaosUniquePtr<chaos::common::data::CDataWrapper> prepareRequestPackForLoadControlUnit(const std::string& cu_uid,
                                                                                                              chaos::metadata_service::persistence::data_access::ControlUnitDataAccess *cu_da);
                
                static void prepareAutoInitAndStartInAutoLoadControlUnit(const std::string& cu_uid,
                                                                         chaos::metadata_service::persistence::data_access::NodeDataAccess *n_da,
                                                                         chaos::metadata_service::persistence::data_access::ControlUnitDataAccess *cu_da,
                                                                         chaos::metadata_service::persistence::data_access::DataServiceDataAccess *ds_da,
                                                                         chaos::common::data::CDataWrapper *auto_load_pack);
                
                static ChaosUniquePtr<chaos::common::data::CDataWrapper> initDataPack(const std::string& cu_uid,
                                                                                      chaos::metadata_service::persistence::data_access::NodeDataAccess *n_da,
                                                                                      chaos::metadata_service::persistence::data_access::ControlUnitDataAccess *cu_da,
                                                                                      chaos::metadata_service::persistence::data_access::DataServiceDataAccess *ds_da);
                
                static ChaosUniquePtr<chaos::common::data::CDataWrapper> startDataPack(const std::string& cu_uid);
                
                static ChaosUniquePtr<chaos::common::data::CDataWrapper> stopDataPack(const std::string& cu_uid);
                
                static ChaosUniquePtr<chaos::common::data::CDataWrapper> deinitDataPack(const std::string& cu_uid);
                
                static ChaosUniquePtr<chaos::common::data::CDataWrapper> mergeDatasetAttributeWithSetup(chaos::common::data::CDataWrapper *element_in_dataset,
                                                                                                        chaos::common::data::CDataWrapper *element_in_setup);
            };
            
        }
    }
}

#endif /* CUCommonUtility_hpp */
