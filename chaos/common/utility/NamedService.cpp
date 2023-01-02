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

#include "NamedService.h"
#include <chaos/common/global.h>

using namespace chaos::common::utility;
using namespace std;

#define NamedService_LDBG_ DBG_LOG(NamedService)

NamedService::NamedService(const string & _service_name):
service_name(_service_name){
	NamedService_LDBG_ << "Allocated Named Service for " << service_name;
}

/*
 Return the adapter alias
 */
const string & NamedService::getName() const {
    return service_name;
}
