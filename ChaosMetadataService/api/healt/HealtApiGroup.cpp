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

#include "HealtApiGroup.h"
#include "ProcessHello.h"
#include "ProcessBye.h"

using namespace chaos::metadata_service::api::healt;

DEFINE_CLASS_FACTORY_NO_ALIAS(HealtApiGroup,
                              chaos::metadata_service::api::AbstractApiGroup);

HealtApiGroup::HealtApiGroup():
AbstractApiGroup(chaos::HealtProcessDomainAndActionRPC::RPC_DOMAIN){
    addApi<ProcessHello>();
    addApi<ProcessBye>();
}

HealtApiGroup::~HealtApiGroup() {
    
}