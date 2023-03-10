cmake_minimum_required(VERSION 3.0.0)

project(ccs)

option(CHAOS_PROMETHEUS "Enable Prometheus metrics support" OFF)

MESSAGE(">>Find the Qt5 library")
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Sql REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5PrintSupport REQUIRED)
FIND_PACKAGE(Qt5Qml REQUIRED)
FIND_PACKAGE(Qt5UiTools REQUIRED)
FIND_PACKAGE(Qt5Designer REQUIRED)

# Get Qt root directory
get_property(_filepath TARGET "Qt5::Core" PROPERTY LOCATION_RELEASE)
get_filename_component(_dir ${_filepath} PATH)
IF(UNIX AND NOT APPLE)
    set(QT_ROOT_DIR "${_dir}/..")
ELSE()
    set(QT_ROOT_DIR "${_dir}/../..")    
ENDIF()
SET(QT_PLUGINS_DIR ${QT_ROOT_DIR}/plugins)

# set(QT_USE_QTSQL 1)
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include precomp_header.h -w")

MESSAGE(">>Find includes in corresponding build directories")
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set the output folder where your program will be created
SET(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

MESSAGE(">>Instruct CMake to run moc automatically when needed.")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


MESSAGE(">>QT Directory: " ${CMAKE_PREFIX_PATH})
MESSAGE(">>QT Version: " ${Qt5Widgets_VERSION})
MESSAGE(">>QT WIDGET DIR: " ${Qt5Widgets_INCLUDE_DIRS})
MESSAGE(">>QT GUI DIR: " ${Qt5Gui_INCLUDE_DIRS})
MESSAGE(">>QT SQL DIR: " ${Qt5Sql_INCLUDE_DIRS})
MESSAGE(">>QT SQL DIR: " ${Qt5PrintSupport_INCLUDE_DIRS})
MESSAGE(">>QT LIB DIR: " ${Qt5Widgets_LIBRARIES})
MESSAGE(">>QT QML DIR: " ${Qt5Qml_LIBRARIES})
MESSAGE(">>QT UITOOLS DIR: " ${Qt5UiTools_LIBRARIES})
MESSAGE(">>QT Plugins DIR: " ${QT_PLUGINS_DIR})

SET(QT_QMAKE_EXECUTABLE ${QT_ROOT_DIR}/gcc_64/bin/qmake)
MESSAGE(STATUS "QT_QMAKE_EXECUTABLE: " ${QT_QMAKE_EXECUTABLE})

#INCLUDE_DIRECTORIES(${QT_LIB_DIR})
INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Sql_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5PrintSupport_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Qml_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5UiTools_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Designer_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR_QtCore})
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/../usr/local/include)
LINK_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/../usr/local/lib)

MESSAGE(">>Tell CMake to create the ccs executable")

QT5_WRAP_CPP(HEADERS_MOC ${QT_HEADERS})
QT5_ADD_RESOURCES(RESOURCES_RCC ${RESOURCES})

set(CCS_SOURCE
    main.cpp
    MainController.cpp
    GlobalServices.cpp
    api_async_processor/ApiAsyncProcessor.cpp
    api_async_processor/ApiAsyncRunnable.cpp
    api_async_processor/ApiHandler.cpp
    api_async_processor/ApiSubmitter.cpp
    data/AgentNodeAssociatedListModel.cpp
    data/ApplicationErrorLoggingTableModel.cpp
    data/AttributeReader.cpp
    data/AttributeValueChangeSet.cpp
    data/ChaosAbstractDataSetTableModel.cpp
    data/ChaosAbstractListModel.cpp
    data/ChaosAbstractTableModel.cpp
    data/ChaosByteArray.cpp
    data/ChaosTypedAttributeValueSetter.cpp
    data/CommandListModel.cpp
    data/CommandParameterReader.cpp
    data/CommandParameterTableModel.cpp
    data/CommandReader.cpp
    data/CommandTemplateListModel.cpp
    data/ControlUnitStateVaribleListModel.cpp
    data/DatasetAttributeListModel.cpp
    data/DatasetAttributeReader.cpp
    data/DatasetReader.cpp
    data/EditableDatasetTableModel.cpp
    data/EditableScriptVariableTableModel.cpp
    data/FixedInputChannelDatasetTableModel.cpp
    data/FixedOutputChannelDatasetTableModel.cpp
    data/LiveDatasetTableModel.cpp
    data/LogDataTableModel.cpp
    data/LogDomainListModel.cpp
    data/LogEntryAdvancedSearchTableModel.cpp
    data/LogEntryTableModel.cpp
    data/LogProcessEntriesTableModel.cpp
    data/ScriptInstanceListModel.cpp
    data/ScriptListModel.cpp
    data/SearchNodeListModel.cpp
    data/SnapshotDatasetNodeListModel.cpp
    data/delegate/ComboBoxDelegate.cpp
    data/delegate/TwoLineInformationItem.cpp
    data/delegate/TwoLineInformationListItemDelegate.cpp
    data/editor/common/EditableSubtypeListTableModel.cpp
    data/editor/common/EditableSubtypeListWidget.cpp
    data/editor/common/EditableSubtypeListWidget.ui
    data/editor/dataset/EditableDatasetTableModelEditDialog.cpp
    data/editor/dataset/EditableDatasetTableModelEditDialog.ui
    data/editor/script/EditableScriptVariableTableModelEditDialog.cpp
    data/editor/script/EditableScriptVariableTableModelEditDialog.ui
    data/json/JsonTableModel.cpp
    error/ErrorManager.cpp
    external_lib/qcustomplot.cpp
    external_lib/QtNodeEditor/Connection.cpp
    external_lib/QtNodeEditor/ConnectionBlurEffect.cpp
    external_lib/QtNodeEditor/ConnectionGeometry.cpp
    external_lib/QtNodeEditor/ConnectionGraphicsObject.cpp
    external_lib/QtNodeEditor/ConnectionPainter.cpp
    external_lib/QtNodeEditor/ConnectionState.cpp
    external_lib/QtNodeEditor/ConnectionStyle.cpp
    external_lib/QtNodeEditor/DataModelRegistry.cpp
    external_lib/QtNodeEditor/FlowItemInterface.cpp
    external_lib/QtNodeEditor/FlowScene.cpp
    external_lib/QtNodeEditor/FlowView.cpp
    external_lib/QtNodeEditor/FlowViewStyle.cpp
    external_lib/QtNodeEditor/Node.cpp
    external_lib/QtNodeEditor/NodeConnectionInteraction.cpp
    external_lib/QtNodeEditor/NodeDataModel.cpp
    external_lib/QtNodeEditor/NodeGeometry.cpp
    external_lib/QtNodeEditor/NodeGraphicsObject.cpp
    external_lib/QtNodeEditor/NodePainter.cpp
    external_lib/QtNodeEditor/NodeState.cpp
    external_lib/QtNodeEditor/NodeStyle.cpp
    external_lib/QtNodeEditor/Properties.cpp
    external_lib/QtNodeEditor/StyleCollection.cpp
    language_editor/CLINGLanguageSupport.cpp
    language_editor/JsonEditor.cpp
    language_editor/LuaLanguageSupport.cpp
    language_editor/ScriptEditor.cpp
    log_browser/ApplicationErrorLogging.cpp
    log_browser/LogBrowser.cpp
    logic/property_switch/MultiPropertyLogicSwitch.cpp
    logic/property_switch/SwitchAggregator.cpp
    node/agent/AgentEditor.cpp
    node/agent/AgentNodeAssociationEditor.cpp
    node/agent/AgentSetting.cpp
    node/connection_manager/ConnectionEditor.cpp
    node/connection_manager/model/ControlUnitNodeDataModel.cpp
    node/control_unit/AttributeValueRangeDialog.cpp
    node/control_unit/CommandTemplateInstanceEditor.cpp
    node/control_unit/ControlUnitCommandTemplateEditor.cpp
    node/control_unit/ControlUnitEditor.cpp
    node/control_unit/ControUnitInstanceEditor.cpp
    node/control_unit/DriverDescriptionInputDialog.cpp
    node/data_service/CreateEditDataService.cpp
    node/data_service/DataServiceEditor.cpp
    node/unit_server/UnitServerEditor.cpp
    plot/AbstractBinaryPlotAdapter.cpp
    plot/BufferPlot.cpp
    plot/NodeAttributePlotting.cpp
    preference/PreferenceDialog.cpp
    preference/PreferenceManager.cpp
    preference/PreferenceMDSHostListModel.cpp
    preference/SelectNetworkDomain.cpp
    presenter/PresenterWidget.cpp
    script/CreateNewScriptDialog.cpp
    script/ScriptDescriptionWidget.cpp
    script/ScriptInstanceManagerWidget.cpp
    script/ScriptManager.cpp
    script/editor/CodeEditor.cpp
    search/SearchNodeResult.cpp
    snapshot/NewSnapshot.cpp
    snapshot/NodeInSnapshotTableModel.cpp
    snapshot/NodeListModel.cpp
    snapshot/SnapshotManager.cpp
    snapshot/SnapshotTableModel.cpp
    status_manager/StatusManager.cpp
    tree_group/AddNewDomain.cpp
    tree_group/DomainListModel.cpp
    tree_group/GroupTreeItem.cpp
    tree_group/GroupTreeModel.cpp
    tree_group/TreeGroupManager.cpp
    monitor/healt/HealtMonitorWidget.cpp
    monitor/healt/HealtPresenterWidget.cpp
    monitor/healt/HealtWidgetsListPresenteWidget.cpp
    monitor/healt/HighlighterForChaosType.cpp
    monitor/handler/healt/HealtHeartbeatHandler.h
    monitor/handler/healt/HealtStatusHandler.h
    monitor/handler/system/SystemControlUnitRunScheduleDelay.h
    monitor/handler/AbstractAttributeHandler.h
    monitor/handler/AbstractTSTaggedAttributeHandler.h
    monitor/handler/MonitorBinaryAttributeHandler.h
    monitor/handler/MonitorBoolAttributeHandler.h
    monitor/handler/MonitorDoubleAttributeHandler.h
    monitor/handler/MonitorInt32AttributeHandler.h
    monitor/handler/MonitorInt64AttributeHandler.h
    monitor/handler/MonitorStringAttributeHandler.h
    monitor/handler/MonitorTSTaggedBinaryAttributeHandler.h
    monitor/handler/MonitorTSTaggedDoubleAttributeHandler.h
    monitor/handler/MonitorTSTaggedInt32AttributeHandler.h
    monitor/handler/MonitorTSTaggedInt64AttributeHandler.h
    monitor/handler/MonitorTSTaggedStringAttributeHandler.h
    monitor/handler/MonitorTSTaggetBoolAttributeHandler.h
    utility/MessageUtility.cpp
    utility/WidgetUtility.cpp
    widget/CCommandStatisticWidget.cpp
    widget/CControlUnitDatasetLabel.cpp
    widget/CDSAttrQLineEdit.cpp
    widget/ChaosMonitorWidgetCompanion.cpp
    widget/ChaosStorageTypeWidget.cpp
    widget/ChaosWidgetCompanion.cpp
    widget/CLedIndicatorHealt.cpp
    widget/CNodeHealthLabel.cpp
    widget/CNodeLogWidget.cpp
    widget/CNodeResourceWidget.cpp
    widget/CPopupWidgetContainer.cpp
    widget/CPropertyPushButton.cpp
    widget/CPropertyTextEdit.cpp
    widget/CQListView.cpp
    widget/CQTableView.cpp
    widget/CStateVisiblePushButton.cpp
    widget/OnOffImageIndicator.cpp
    widget/StateImageIndicatorWidget.cpp
    widget/WidgetDynamicList.cpp
    widget/StorageBurst.cpp
    widget/CLedIndicatorCUBusy.cpp
    widget/designer/ChaosUISynopticLoaderWindow.cpp
    widget/designer/ScriptSignalDialog.cpp
    widget/designer/CUNodeRoot.cpp
    widget/designer/ChaosBaseUI.cpp
    widget/designer/ChaosBaseDatasetUI.cpp
    widget/designer/ChaosBaseDatasetAttributeUI.cpp
    widget/designer/ChaosBaseCommandButtonUI.cpp
    theme/theme.qrc
    # ${ccs_include_moc}
)

MESSAGE(">>Tell CMake to create the chaos custom widget library")
set(CUSTOM_WIDGET_SOURCE
    designerplugin/ChaosCustomDesignerWidget/CDatasetAttributeLabelMenu.cpp
    designerplugin/ChaosCustomDesignerWidget/CChangeSetGlobalCommitPlugin.cpp
    designerplugin/ChaosCustomDesignerWidget/CDatasetAttributeHealthLabelPlugin.cpp
    designerplugin/ChaosCustomDesignerWidget/CDatasetAttributeImagePlugin.cpp
    designerplugin/ChaosCustomDesignerWidget/CDatasetAttributeLabelDialog.cpp
    designerplugin/ChaosCustomDesignerWidget/CDatasetAttributeLabelPlugin.cpp
    designerplugin/ChaosCustomDesignerWidget/CDatasetAttributeSetValueComboBoxDialog.cpp
    designerplugin/ChaosCustomDesignerWidget/CDatasetAttributeSetValueComboBoxMenuExtension.cpp
    designerplugin/ChaosCustomDesignerWidget/CDatasetAttributeSetValueComboBoxPlugin.cpp
    designerplugin/ChaosCustomDesignerWidget/CDatasetAttributeSetValueLineEditPlugin.cpp
    designerplugin/ChaosCustomDesignerWidget/CUOnlineLedIndicatorPlugin.cpp
    designerplugin/ChaosCustomDesignerWidget/ChaosWidgets.cpp
    theme/theme.qrc)

set(CUSTOM_WIDGET_SOURCE ${CUSTOM_WIDGET_SOURCE}
    widget/designer/ChaosBaseUI.cpp
    widget/designer/ChaosBaseDatasetUI.cpp
    widget/designer/ChaosBaseDatasetAttributeUI.cpp
    widget/designer/ChaosBaseCommandButtonUI.cpp
    widget/designer/ui/CChangeSetGlobalCommit.cpp
    widget/designer/ui/CDatasetAttributeHealthLabel.cpp
    widget/designer/ui/CDatasetAttributeImage.cpp
    widget/designer/ui/CDatasetAttributeLabel.cpp
    widget/designer/ui/CDatasetAttributeSetValueComboBox.cpp
    widget/designer/ui/CDatasetAttributeSetValueLineEdit.cpp
    widget/designer/ui/CUOnlineLedIndicator.cpp)

IF(UNIX AND NOT APPLE)
        MESSAGE(">>Add Linux Exacutable")
        LINK_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/../usr/local/lib64)
        ADD_EXECUTABLE(ccs
                        ${CCS_SOURCE}
                        # ${ccs_include_moc}
                        ${UIS_HDRS})
        # ADD_LIBRARY(ChaosCustomDesignerWidget SHARED ${CUSTOM_WIDGET_SOURCE})
        install (TARGETS ccs RUNTIME DESTINATION RUNTIME DESTINATION bin)
        # install(TARGETS ChaosCustomDesignerWidget DESTINATION ${QT_PLUGINS_DIR}/designer)
ELSE()
        #get deployer
        get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
        find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
        
        MESSAGE(">>Add OSX Exacutable")
        MESSAGE(">>Mac deployer ${MACDEPLOYQT_EXECUTABLE}")
        #Bundle (OSX)
        # the property added to Info.plist
        SET(MACOSX_BUNDLE_ICON_FILE chaos.icns)

        # And this part tells CMake where to find and install the file itself
        set(CHAOS_ICON ${CMAKE_CURRENT_SOURCE_DIR}/chaos.icns)
        set_source_files_properties(${CHAOS_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        file(GLOB_RECURSE RES_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/theme/*")
        SET_SOURCE_FILES_PROPERTIES(${RES_SOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        ADD_EXECUTABLE(ccs
                        MACOSX_BUNDLE
                        ${CHAOS_ICON}
                        ${CCS_SOURCE}
                        # ${ccs_include_moc}
                        ${RES_SOURCES}
                        ${UIS_HDRS})
        INSTALL( TARGETS ccs DESTINATION ${CMAKE_SOURCE_DIR})

        ADD_CUSTOM_COMMAND(TARGET ccs POST_BUILD
                            COMMAND "${MACDEPLOYQT_EXECUTABLE}"
                                    "$<TARGET_FILE_DIR:ccs>/../.."
                                    "-libpath=${CMAKE_CURRENT_LIST_DIR}/../usr/local/lib"
                                    -always-overwrite
                            COMMENT "Running macdeployqt..."
)
ENDIF()

# set(CP_LINK_LIB   Qt5::Core Qt5::Designer)

set(CCS_LINK_LIB    Qt5::Core
                    Qt5::Gui
                    Qt5::Sql
                    Qt5::Widgets
                    Qt5::PrintSupport
                    Qt5::Qml
                    Qt5::UiTools
                    chaos_common
                    chaos_metadata_service_client
                    zmq)
IF (CHAOS_PROMETHEUS)
    MESSAGE(">>Enable prometheus supports")
    set(CCS_LINK_LIB    ${CCS_LINK_LIB}
                        prometheus-cpp-core
                        prometheus-cpp-pull)
ENDIF()

MESSAGE(">>Target libraries")
target_link_libraries(ccs ${CCS_LINK_LIB})
# target_link_libraries(ChaosCustomDesignerWidget ${CP_LINK_LIB})
add_subdirectory(designerplugin/ChaosCustomDesignerWidget)
MESSAGE(">>END")
