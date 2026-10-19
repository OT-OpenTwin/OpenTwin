cmake_minimum_required(VERSION 3.20)
project(LTSpiceService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(LTSpiceService OT_LTSPICE_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_disable_warnings(LTSpiceService 4996)

ot_add_dependency(LTSpiceService
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
    WINLIB:Advapi32
)

ot_finalize_lib(LTSpiceService)
ot_add_test(LTSpiceService)
