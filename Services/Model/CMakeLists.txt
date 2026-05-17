cmake_minimum_required(VERSION 3.20)
project(Model LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(Model OT_MODEL_SERVICE_ROOT_PATH MODEL_EXPORTS)
ot_add_compile_definitions(Model
    _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
)

ot_add_dependency(Model
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
    OTBlockEntities
    OTResultDataAccess
    RJSON
    CURL
    MONGO_C
    MONGO_CXX
    BOOST
    OCCT
    ZLIB
    BASE64
    EXPREVAL
)

ot_finalize_lib(Model)
ot_add_test(Model)
