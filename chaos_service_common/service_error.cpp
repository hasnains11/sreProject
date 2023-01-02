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

#include <chaos_service_common/service_error.h>

using namespace chaos::service_common::ErrorMDSCode;

CHAOS_DEFINE_ERROR_CODE_MAPPING(EC_MDS_NODE_BAD_NAME,               -10000, "Bad name format for node");
CHAOS_DEFINE_ERROR_CODE_MAPPING(EC_MDS_NODE_BAD_PATH_EXTRACTION,    -10001, "Node name cannot be extract from parent path");
CHAOS_DEFINE_ERROR_CODE_MAPPING(EC_MDS_PARENT_NODE_NOT_IN_DOMAIN,   -10002, "The parent path has not been found on domain");
CHAOS_DEFINE_ERROR_CODE_MAPPING(EC_MDS_NODE_DUPLICATION,            -10002,  "There another node with same name");

