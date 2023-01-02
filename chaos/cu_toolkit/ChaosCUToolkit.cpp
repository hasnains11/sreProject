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
#include <chaos/common/external_unit/external_unit.h>
#include <chaos/common/external_unit/external_unit_constants.h>
#include <chaos/common/healt_system/HealtManager.h>
#include <chaos/common/io/SharedManagedDirecIoDataDriver.h>
#include <chaos/common/log/LogManager.h>
#include <chaos/common/metadata_logging/MetadataLoggingManager.h>
#include <chaos/cu_toolkit/ChaosCUToolkit.h>
#include <chaos/cu_toolkit/command_manager/CommandManager.h>
#include <chaos/cu_toolkit/control_manager/script/api/api.h>
#include <chaos/cu_toolkit/data_manager/DataManager.h>
#include "windowsCompliant.h"
using namespace std;
using namespace chaos::cu;
using namespace chaos::cu::data_manager;
using namespace chaos::cu::driver_manager;
using namespace chaos::cu::command_manager;
using namespace chaos::cu::control_manager;

using namespace chaos::common::io;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::healt_system;
using namespace chaos::common::metadata_logging;

//boost::mutex ChaosCUToolkit::monitor;
//boost::condition ChaosCUToolkit::endWaithCondition;
chaos::WaitSemaphore ChaosCUToolkit::waitCloseSemaphore;

ChaosCUToolkit::ChaosCUToolkit() {
  GlobalConfiguration::getInstance()->addOption<bool>(CU_OPT_IN_MEMORY_DATABASE,
                                                      "Specify when to use in memory or on disc contorl unit internal database",
                                                      true);
  //
  GlobalConfiguration::getInstance()->addOptionZeroToken<bool>(InitOption::OPT_LOG_ON_MDS,
                                                               "Specify when log need to be redirect to metadata server");

  GlobalConfiguration::getInstance()->addOption<bool>(CONTROL_MANAGER_UNIT_SERVER_ENABLE,
                                                      CONTROL_MANAGER_UNIT_SERVER_ENABLE_desc,
                                                      true);

  GlobalConfiguration::getInstance()->addOption<std::string>(CONTROL_MANAGER_UNIT_SERVER_ALIAS,
                                                             CONTROL_MANAGER_UNIT_SERVER_ALIAS_desc);

  GlobalConfiguration::getInstance()->addOption<std::string>(CONTROL_MANAGER_UNIT_SERVER_KEY,
                                                             CONTROL_MANAGER_UNIT_SERVER_KEY_desc);

  GlobalConfiguration::getInstance()->addOption<uint64_t>(CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC,
                                                          CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC_desc,
                                                          CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC_DEFAULT);

  GlobalConfiguration::getInstance()->addOption<std::vector<std::string> >(CONTROL_MANAGER_EXECUTION_POOLS,
                                                                           CONTROL_MANAGER_EXECUTION_POOLS_DESC);

  GlobalConfiguration::getInstance()->addOption<double>(CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP,
                                                        CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP_DESC,
                                                        CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP_DEFAULT);
}

ChaosCUToolkit::~ChaosCUToolkit() {
}

void ChaosCUToolkit::closeUIToolkit() {
  signalHanlder(SIGTERM);
}

//! C and C++ attribute parser
/*!
 Specialized option for startup c and cpp program main options parameter
 */
void ChaosCUToolkit::init(int argc, const char* argv[]) {
  ChaosCommon<ChaosCUToolkit>::init(argc, argv);
}
//!stringbuffer parser
/*
 specialized option for string stream buffer with boost semantics
 */
void ChaosCUToolkit::init(istringstream& initStringStream) {
  ChaosCommon<ChaosCUToolkit>::init(initStringStream);
}

/*
 *
 */
void ChaosCUToolkit::init(void* init_data) {
  try {
    ChaosCommon<ChaosCUToolkit>::init(init_data);

    if (signal((int)SIGINT, ChaosCUToolkit::signalHanlder) == SIG_ERR) {
      LERR_ << "SIGINT Signal handler registration error";
    }
#ifndef _WIN32
    if (signal((int)SIGQUIT, ChaosCUToolkit::signalHanlder) == SIG_ERR) {
      LERR_ << "SIGQUIT Signal handler registration error";
    }
#endif
    if (signal((int)SIGTERM, ChaosCUToolkit::signalHanlder) == SIG_ERR) {
      LERR_ << "SIGTERM Signal handler registration error";
    }

    if (signal((int)SIGABRT, ChaosCUToolkit::signalHanlder) == SIG_ERR) {
      LERR_ << "SIGABRT Signal handler registration error";
    }

    InizializableService::initImplementation(SharedManagedDirecIoDataDriver::getInstance(), NULL, "SharedManagedDirecIoDataDriver", __PRETTY_FUNCTION__);

    if (GlobalConfiguration::getInstance()->hasOption(InitOption::OPT_LOG_ON_MDS) &&
        GlobalConfiguration::getInstance()->hasOption(CONTROL_MANAGER_UNIT_SERVER_ALIAS)) {
      chaos::common::log::LogManager::getInstance()->addMDSLoggingBackend(GlobalConfiguration::getInstance()->getOption<std::string>(CONTROL_MANAGER_UNIT_SERVER_ALIAS));
    }

    //force first allocation of metadata logging
    if (GlobalConfiguration::getInstance()->getMetadataServerAddressList().size()) {
      //we can initilize the logging manager
      InizializableService::initImplementation(chaos::common::metadata_logging::MetadataLoggingManager::getInstance(), NULL, "MetadataLoggingManager", __PRETTY_FUNCTION__);
    }
    StartableService::initImplementation(HealtManager::getInstance(), NULL, "HealtManager", __PRETTY_FUNCTION__);
    StartableService::initImplementation(CommandManager::getInstance(), NULL, "CommandManager", "ChaosCUToolkit::init");
    CommandManager::getInstance()->server_handler = this;
    StartableService::initImplementation(DriverManager::getInstance(), NULL, "DriverManager", "ChaosCUToolkit::init");
    StartableService::initImplementation(DataManager::getInstance(), NULL, "DataManager", "ChaosCUToolkit::init");
    StartableService::initImplementation(ControlManager::getInstance(), NULL, "ControlManager", "ChaosCUToolkit::init");

    if (GlobalConfiguration::getInstance()->hasOption(chaos::common::external_unit::InitOption::OPT_UNIT_GATEWAY_ENABLE) &&
        GlobalConfiguration::getInstance()->getOption<bool>(chaos::common::external_unit::InitOption::OPT_UNIT_GATEWAY_ENABLE)) {
      //initilize unit gateway
      InizializableService::initImplementation(common::external_unit::ExternalUnitManager::getInstance(), NULL, "ExternalUnitManager", __PRETTY_FUNCTION__);
    }

    LAPP_ << "Control Manager Initialized";

    LAPP_ << "!CHAOS Control Unit System Initialized";

  } catch (CException& ex) {
    DECODE_CHAOS_EXCEPTION(ex)
    exit(1);
  }
  //start data manager
}

/*
 *
 */
void ChaosCUToolkit::start() {
  try {
    ChaosCommon<ChaosCUToolkit>::start();
    LAPP_ << "Starting !CHAOS Control Unit System";
    StartableService::startImplementation(HealtManager::getInstance(), "HealtManager", __PRETTY_FUNCTION__);
    StartableService::startImplementation(CommandManager::getInstance(), "CommandManager", "ChaosCUToolkit::start");
    StartableService::startImplementation(DriverManager::getInstance(), "DriverManager", "ChaosCUToolkit::start");
    StartableService::startImplementation(DataManager::getInstance(), "DataManager", "ChaosCUToolkit::start");
    StartableService::startImplementation(ControlManager::getInstance(), "ControlManager", "ChaosCUToolkit::start");
    LAPP_ << "-----------------------------------------";
    LAPP_ << "!CHAOS Control Unit System Started";
    LAPP_ << "RPC Server address: " << CommandManager::getInstance()->broker->getRPCUrl();
    LAPP_ << "DirectIO Server address: " << CommandManager::getInstance()->broker->getDirectIOUrl();
    LAPP_ << "-----------------------------------------";
    //at this point i must with for end signal
    waitCloseSemaphore.wait();
    LAPP_ << "------ ORDERED EXIT";

  } catch (CException& ex) {
    DECODE_CHAOS_EXCEPTION(ex);
  } catch (...) {
    LERR_ << "unexpected exeception";
  }
  //execute the stop and the deinitialization of the toolkit
  stop();
  deinit();
}

/*
 Stop the toolkit execution
 */
void ChaosCUToolkit::stop() {
  LAPP_ << "Stopping !CHAOS Control Unit System";
  CHAOS_NOT_THROW(StartableService::stopImplementation(CommandManager::getInstance(), "CommandManager", "ChaosCUToolkit::stop"););
  CHAOS_NOT_THROW(StartableService::stopImplementation(ControlManager::getInstance(), "ControlManager", "ChaosCUToolkit::stop"););
  CHAOS_NOT_THROW(StartableService::stopImplementation(DataManager::getInstance(), "DataManager", "ChaosCUToolkit::stop"););
  CHAOS_NOT_THROW(StartableService::stopImplementation(DriverManager::getInstance(), "DriverManager", "ChaosCUToolkit::stop"););
  CHAOS_NOT_THROW(StartableService::stopImplementation(HealtManager::getInstance(), "HealtManager", __PRETTY_FUNCTION__););
  ChaosCommon<ChaosCUToolkit>::stop();
}

/*
 Deiniti all the manager
 */
void ChaosCUToolkit::deinit() {
  LAPP_ << "Deinitilizzating !CHAOS Control Unit System";
  if (GlobalConfiguration::getInstance()->hasOption(chaos::common::external_unit::InitOption::OPT_UNIT_GATEWAY_ENABLE) &&
      GlobalConfiguration::getInstance()->getOption<bool>(chaos::common::external_unit::InitOption::OPT_UNIT_GATEWAY_ENABLE)) {
    //initilize unit gateway
    InizializableService::deinitImplementation(common::external_unit::ExternalUnitManager::getInstance(), "ExternalUnitManager", __PRETTY_FUNCTION__);
  }
  CHAOS_NOT_THROW(StartableService::deinitImplementation(CommandManager::getInstance(), "CommandManager", "ChaosCUToolkit::stop"););
  CHAOS_NOT_THROW(StartableService::deinitImplementation(ControlManager::getInstance(), "ControlManager", "ChaosCUToolkit::stop"););
  CHAOS_NOT_THROW(StartableService::deinitImplementation(DataManager::getInstance(), "DataManager", "ChaosCUToolkit::deinit"););
  CHAOS_NOT_THROW(StartableService::deinitImplementation(DriverManager::getInstance(), "DriverManager", "ChaosCUToolkit::deinit"););
  CHAOS_NOT_THROW(InizializableService::deinitImplementation(MetadataLoggingManager::getInstance(), "MetadataLoggingManager", __PRETTY_FUNCTION__););
  CHAOS_NOT_THROW(StartableService::deinitImplementation(HealtManager::getInstance(), "HealtManager", __PRETTY_FUNCTION__););
  CHAOS_NOT_THROW(InizializableService::deinitImplementation(SharedManagedDirecIoDataDriver::getInstance(), "SharedManagedDirecIoDataDriver", __PRETTY_FUNCTION__););
  ChaosCommon<ChaosCUToolkit>::deinit();
  LAPP_ << "-----------------------------------------";
  LAPP_ << "!CHAOS Control Unit System deinitialized  ";
  LAPP_ << "-----------------------------------------";
}

/*
 * Add a new Control Unit Class for execution
 */
void ChaosCUToolkit::setProxyCreationHandler(ProxyLoadHandler load_handler) {
  //call command manager to submit Custom Control Unit
  return ControlManager::getInstance()->setProxyCreationHandler(load_handler);
}

/*
 *
 */
void ChaosCUToolkit::signalHanlder(int signalNumber) {
  //lock lk(monitor);
  //unlock the condition for end start method
  //endWaithCondition.notify_one();
  //waitCloseSemaphore.unlock();

  if ((signalNumber == SIGABRT) || (signalNumber == SIGSEGV)) {
    LAPP_ << "INTERNAL ERROR, please provide log, Catch SIGNAL: " << signalNumber;
    sigignore(signalNumber);
    waitCloseSemaphore.notifyAll();
    sleep(5);
    exit(1);
    //throw CFatalException(signalNumber,"trapped signal",__PRETTY_FUNCTION__);
  } else if ((signalNumber == SIGTERM) || (signalNumber == SIGINT)) {
    sigignore(signalNumber);
    LAPP_ << "CATCH Interrupting signal  "<<signalNumber<<" exiting ...";
    waitCloseSemaphore.notifyAll();
    sleep(5);
    exit(0);
  } else {
    LAPP_ << "CATCH SIGNAL " << signalNumber;

    //exit(0);
  }
  sleep(1);
}

CDWUniquePtr ChaosCUToolkit::nodeShutDown(chaos::common::data::CDWUniquePtr data) {
    waitCloseSemaphore.unlock();
    return ChaosCommon<ChaosCUToolkit>::nodeShutDown(MOVE(data));
}
