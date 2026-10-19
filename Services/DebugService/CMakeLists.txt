cmake_minimum_required(VERSION 3.20)
project(DebugService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(DebugService OT_DEBUGSERVICE_ROOT_PATH DEBUGSERVICE_EXPORTS)
ot_disable_warnings(DebugService 4996)

ot_add_dependency(DebugService
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
    RJSON
    CURL
    MONGO_C
    MONGO_CXX
    BOOST
    MONGO_BOOST
)

ot_finalize_lib(DebugService)
ot_add_test(DebugService)
