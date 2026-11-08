cmake_minimum_required(VERSION 3.20)
project(PalaceService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(PalaceService OT_PALACE_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)

ot_add_dependency(PalaceService
    OTSystem
    OTCore
    OTGui
    OTCommunication
    OTGuiAPI
    OTServiceFoundation
    OTModelEntities
    OTCADEntities
    OTDataStorage
    OTModelAPI
    OTResultDataAccess
    RJSON
    CURL
    MONGO_C
    MONGO_CXX
    BOOST
    ZLIB
    WINLIB:Advapi32
)

ot_finalize_lib(PalaceService)
ot_add_test(PalaceService)
