cmake_minimum_required(VERSION 3.20)
project(GetDPService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(GetDPService OT_GETDP_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_disable_warnings(GetDPService 4996)

ot_add_dependency(GetDPService
    OTSystem
    OTCore
    OTGui
    OTCommunication
    OTGuiAPI
    OTServiceFoundation
    OTModelEntities
    OTDataStorage
    OTModelAPI
    OTCADEntities
    RJSON
    CURL
    MONGO_C
    MONGO_CXX
    BOOST
    ZLIB
    WINLIB:Advapi32
)

ot_finalize_lib(GetDPService)
ot_add_test(GetDPService)
