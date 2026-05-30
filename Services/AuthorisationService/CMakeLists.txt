cmake_minimum_required(VERSION 3.20)
project(AuthorisationService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(AuthorisationService OT_AUTHORISATION_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_disable_warnings(AuthorisationService 4996)

ot_add_dependency(AuthorisationService
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

ot_finalize_lib(AuthorisationService)
ot_add_test(AuthorisationService)
