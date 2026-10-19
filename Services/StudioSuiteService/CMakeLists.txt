cmake_minimum_required(VERSION 3.20)
project(StudioSuiteService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(StudioSuiteService OT_STUDIOSUITE_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_disable_warnings(StudioSuiteService 4996)

ot_add_dependency(StudioSuiteService
    OTSystem
    OTCore
    OTGui
    OTCommunication
    OTGuiAPI
    OTServiceFoundation
    OTModelEntities
    OTDataStorage
    OTModelAPI
    OTCADEntities
    OTResultDataAccess
    RJSON
    CURL
    MONGO_C
    MONGO_CXX
    BOOST
    MONGO_BOOST
    OCCT
    ZLIB
    BASE64
    TINYXML2
    WINLIB:Advapi32
)

ot_finalize_lib(StudioSuiteService)
ot_add_test(StudioSuiteService)
