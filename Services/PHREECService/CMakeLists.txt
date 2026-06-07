cmake_minimum_required(VERSION 3.20)
project(PHREECService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(PHREECService OT_PHREEC_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_disable_warnings(PHREECService 4996)

ot_add_dependency(PHREECService
    OTSystem
    OTCore
    OTGui
    OTCommunication
    OTGuiAPI
    OTServiceFoundation
    OTModelEntities
    OTDataStorage
    OTModelAPI
    RJSON
    CURL
    MONGO_C
    MONGO_CXX
    MONGO_BOOST
    ZLIB
    WINLIB:Advapi32
)

ot_finalize_lib(PHREECService)
ot_add_test(PHREECService)
