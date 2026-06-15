cmake_minimum_required(VERSION 3.20)
project(LoggerService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(LoggerService OT_LOGGER_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)

ot_add_dependency(LoggerService
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

ot_service_debug_launch(LoggerService
    ARGS "unused"
         "@OPEN_TWIN_SERVICES_ADDRESS@:@OPEN_TWIN_LOG_PORT@"
         "unused"
         "unused"
)

ot_finalize_lib(LoggerService)
ot_add_test(LoggerService)
