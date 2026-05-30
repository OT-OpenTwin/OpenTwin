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

ot_finalize_lib(LocalSessionService)
ot_add_test(LocalSessionService)
