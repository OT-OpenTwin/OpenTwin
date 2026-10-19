cmake_minimum_required(VERSION 3.20)
project(CircuitSimulatorService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(CircuitSimulatorService OT_CIRCUIT_SIMULATOR_SERVICE_ROOT_PATH CIRCUITSIMULATORSERVICE_EXPORTS)
ot_disable_warnings(CircuitSimulatorService 4996)

ot_add_dependency(CircuitSimulatorService
    OTSystem
    OTCore
    OTGui
    OTCommunication
    OTGuiAPI
    OTServiceFoundation
    OTModelEntities
    OTDataStorage
    OTModelAPI
    OTResultDataAccess
    OTBlockEntities
    RJSON
    CURL
    MONGO_C
    MONGO_CXX
    MONGO_BOOST
    NGSPICE
    QtCore
    QtNetwork
    QtWidgets
)

ot_finalize_lib(CircuitSimulatorService)
ot_add_test(CircuitSimulatorService)
