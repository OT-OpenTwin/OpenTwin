cmake_minimum_required(VERSION 3.20)
project(FDTDService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(FDTDService OT_FDTD_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_disable_warnings(FDTDService 4996)

ot_add_dependency(FDTDService
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
    RJSON
    CURL
    MONGO_C
    MONGO_CXX
    TINYXML2
    ZLIB
    WINLIB:Advapi32
)

ot_finalize_lib(FDTDService)
ot_add_test(FDTDService)
