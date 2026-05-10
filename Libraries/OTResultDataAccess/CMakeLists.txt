cmake_minimum_required(VERSION 3.20)
project(OTResultDataAccess LANGUAGES CXX)

set(OT_RESULTDATAACCESS_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTResultDataAccess OT_RESULTDATAACCESS_ROOT_PATH)
ot_add_export(OTResultDataAccess OT_RESULTDATAACCESS_EXPORTS)
ot_add_compile_options(OTResultDataAccess /utf-8 4996)
ot_add_compile_definitions(OTResultDataAccess
    _CONSOLE
    _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
    _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
)

ot_add_dependency(OTResultDataAccess
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

ot_finalize_lib(OTResultDataAccess)
ot_add_test(OTResultDataAccess)
