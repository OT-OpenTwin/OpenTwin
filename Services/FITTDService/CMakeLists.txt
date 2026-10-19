cmake_minimum_required(VERSION 3.20)
project(FITTDService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(FITTDService OT_FITTD_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_disable_warnings(FITTDService 4996)

ot_add_dependency(FITTDService
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
)

ot_finalize_lib(FITTDService)
ot_add_test(FITTDService)
