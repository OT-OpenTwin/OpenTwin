cmake_minimum_required(VERSION 3.20)
project(CircuitExecution LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_bin(CircuitExecution OT_CIRCUIT_EXECUTION_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_set_subsystem(CircuitExecution CONSOLE)

ot_add_dependency(CircuitExecution
    OTSystem
    OTCore
    OTGui
    OTCommunication
    OTGuiAPI
    OTServiceFoundation
    OTModelEntities
    OTDataStorage
    OTModelAPI
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

ot_finalize_bin(CircuitExecution)
ot_add_test(CircuitExecution)
