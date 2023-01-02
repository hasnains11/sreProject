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

#include <chaos/common/batch_command/AbstractSandbox.h>

using namespace chaos::common::batch_command;

//! Functor implementation

void AcquireFunctor::operator()() {
    try {
        if (cmd_instance && (cmd_instance->runningProperty < RunningPropertyType::RP_END)) {cmd_instance->acquireHandler();}
    } catch (chaos::CFatalException&ex) {
        SET_FAULT(ERR_LOG(AcquireFunctor), ex.errorCode, ex.errorMessage, ex.errorDomain,RunningPropertyType::RP_FATAL_FAULT)
    } catch (chaos::CException& ex) {
        SET_FAULT(ERR_LOG(AcquireFunctor), ex.errorCode, ex.errorMessage, ex.errorDomain,RunningPropertyType::RP_FAULT)
    } catch (std::exception& ex) {
        SET_FAULT(ERR_LOG(AcquireFunctor), -1, ex.what(), "Acquisition Handler:"+cmd_instance->getAlias(),RunningPropertyType::RP_FATAL_FAULT);
    } catch (...) {
        SET_FAULT(ERR_LOG(AcquireFunctor), -2, "Unmanaged exception", "Acquisition Handler:"+cmd_instance->getAlias(),RunningPropertyType::RP_FATAL_FAULT);
    }
}

void CorrelationFunctor::operator()() {
    try {
        if (cmd_instance && (cmd_instance->runningProperty < RunningPropertyType::RP_END)) {cmd_instance->ccHandler();};
    } catch (chaos::CFatalException&ex) {
        SET_FAULT(ERR_LOG(AcquireFunctor), ex.errorCode, ex.errorMessage, ex.errorDomain,RunningPropertyType::RP_FATAL_FAULT)
    } catch (chaos::CException& ex) {
        SET_FAULT(ERR_LOG(AcquireFunctor), ex.errorCode, ex.errorMessage, ex.errorDomain,RunningPropertyType::RP_FAULT)
    } catch (std::exception& ex) {
        SET_FAULT(ERR_LOG(AcquireFunctor), -1, ex.what(), "Correlation Handler",RunningPropertyType::RP_FATAL_FAULT);
    } catch (...) {
        SET_FAULT(ERR_LOG(AcquireFunctor), -2, "Unmanaged exception", "Correlation Handler",RunningPropertyType::RP_FATAL_FAULT);
    }
}

void EndFunctor::operator()() {
    try {
        if (cmd_instance && (cmd_instance->runningProperty >= RunningPropertyType::RP_END)) {/*cmd_instance->endHandler()*/};
    } catch (chaos::CFatalException&ex) {
        SET_FAULT(ERR_LOG(AcquireFunctor), ex.errorCode, ex.errorMessage, ex.errorDomain,RunningPropertyType::RP_FATAL_FAULT)
    } catch (chaos::CException& ex) {
        SET_FAULT(ERR_LOG(AcquireFunctor), ex.errorCode, ex.errorMessage, ex.errorDomain,RunningPropertyType::RP_FAULT)
    } catch (std::exception& ex) {
        SET_FAULT(ERR_LOG(AcquireFunctor), -1, ex.what(), "Correlation Handler",RunningPropertyType::RP_FATAL_FAULT);
    } catch (...) {
        SET_FAULT(ERR_LOG(AcquireFunctor), -2, "Unmanaged exception", "Correlation Handler",RunningPropertyType::RP_FATAL_FAULT);
    }
}
CommandInfoAndImplementation::CommandInfoAndImplementation(chaos::common::data::CDataWrapper *_cmdInfo, BatchCommand *_cmdImpl) {
    cmdInfo = _cmdInfo;
    cmdImpl = _cmdImpl;
}

CommandInfoAndImplementation::~CommandInfoAndImplementation() {
    deleteInfo();
    deleteImpl();
}

void CommandInfoAndImplementation::deleteInfo() {
    delete(cmdInfo);
    cmdInfo = NULL;
}

void CommandInfoAndImplementation::deleteImpl() {
    delete(cmdImpl);
    cmdImpl = NULL;
}

AbstractSandbox::AbstractSandbox():
command_sequence_id(0),
event_handler(NULL){}

AbstractSandbox::~AbstractSandbox(){}

uint64_t AbstractSandbox::addCommandID(BatchCommand *command_impl) {
    if(command_impl == NULL) return 0;
    return (command_impl->unique_id = ++command_sequence_id);
}
