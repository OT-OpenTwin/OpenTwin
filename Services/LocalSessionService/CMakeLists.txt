cmake_minimum_required(VERSION 3.20)
project(LocalSessionService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(LocalSessionService OT_LOCAL_SESSION_SERVICE_ROOT_PATH LOCALSESSIONSERVICE_EXPORTS)

ot_add_dependency(LocalSessionService
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

ot_service_debug_launch(LocalSessionService
    ARGS "@OPEN_TWIN_LOGGING_URL@"
         "@OPEN_TWIN_SERVICES_ADDRESS@:@OPEN_TWIN_LSS_PORT@"
         "@OPEN_TWIN_SERVICES_ADDRESS@:@OPEN_TWIN_GSS_PORT@"
         "@OPEN_TWIN_AUTH_PORT@"
)

ot_finalize_lib(LocalSessionService)
ot_add_test(LocalSessionService)
