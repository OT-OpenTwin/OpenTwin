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

ot_service_debug_launch(LocalDirectoryService
    ARGS "@OPEN_TWIN_LOGGING_URL@"
         "@OPEN_TWIN_SERVICES_ADDRESS@:@OPEN_TWIN_LDS_PORT@"
         "@OPEN_TWIN_SERVICES_ADDRESS@:@OPEN_TWIN_GDS_PORT@"
         "@OPEN_TWIN_AUTH_PORT@"
)

ot_finalize_lib(LocalDirectoryService)
ot_add_test(LocalDirectoryService)
