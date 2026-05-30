cmake_minimum_required(VERSION 3.20)
project(LocalDirectoryService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(LocalDirectoryService OT_LOCAL_DIRECTORY_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)

ot_disable_warnings(LocalDirectoryService 4996)

ot_add_dependency(LocalDirectoryService
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
    BOOST
)

ot_finalize_lib(LocalDirectoryService)
ot_add_test(LocalDirectoryService)
