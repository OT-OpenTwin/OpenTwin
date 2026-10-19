cmake_minimum_required(VERSION 3.20)
project(GlobalDirectoryService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(GlobalDirectoryService OT_GLOBAL_DIRECTORY_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)

ot_disable_warnings(GlobalDirectoryService 4996)

ot_add_dependency(GlobalDirectoryService
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

ot_service_debug_launch(GlobalDirectoryService
    ARGS "@OPEN_TWIN_LOGGING_URL@"
         "@OPEN_TWIN_SERVICES_ADDRESS@:@OPEN_TWIN_GDS_PORT@"
         "@OPEN_TWIN_SERVICES_ADDRESS@:@OPEN_TWIN_GSS_PORT@"
         "unused"
)

ot_finalize_lib(GlobalDirectoryService)
ot_add_test(GlobalDirectoryService)
