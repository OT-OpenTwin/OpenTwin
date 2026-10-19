cmake_minimum_required(VERSION 3.20)
project(ModelingService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(ModelingService OT_MODELING_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_add_compile_options(ModelingService "$<$<CONFIG:Debug>:/source-charset:ascii>")
ot_disable_warnings(ModelingService 4996)

ot_add_dependency(ModelingService
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
    OTRubberband
    RJSON
    CURL
    MONGO_C
    MONGO_CXX
    MONGO_BOOST
    OCCT
    ZLIB
    BASE64
)

ot_finalize_lib(ModelingService)
ot_add_test(ModelingService)
