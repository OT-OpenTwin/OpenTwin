cmake_minimum_required(VERSION 3.20)
project(GlobalSessionService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(GlobalSessionService OT_GLOBAL_SESSION_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)

ot_add_dependency(GlobalSessionService
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
    BASE64
    ZLIB
)

ot_service_debug_launch(GlobalSessionService
    ARGS "@OPEN_TWIN_LOGGING_URL@"
         "@OPEN_TWIN_SERVICES_ADDRESS@:@OPEN_TWIN_GSS_PORT@"
         "@OPEN_TWIN_MONGODB_ADDRESS@"
         "@OPEN_TWIN_SERVICES_ADDRESS@:@OPEN_TWIN_AUTH_PORT@"
         "@OPEN_TWIN_DOWNLOAD_PORT@"
)

ot_finalize_lib(GlobalSessionService)
ot_add_test(GlobalSessionService)
