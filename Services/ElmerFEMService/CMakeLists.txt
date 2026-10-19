cmake_minimum_required(VERSION 3.20)
project(ElmerFEMService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(ElmerFEMService OT_ELMERFEM_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_disable_warnings(ElmerFEMService 4996)

ot_add_dependency(ElmerFEMService
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
    BOOST
    ZLIB
    WINLIB:Advapi32
)

ot_finalize_lib(ElmerFEMService)
ot_add_test(ElmerFEMService)
