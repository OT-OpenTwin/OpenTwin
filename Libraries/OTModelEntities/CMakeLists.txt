cmake_minimum_required(VERSION 3.20)

project(OTModelEntities LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTModelEntities OT_MODELENTITIES_ROOT_PATH)
ot_add_export(OTModelEntities)
ot_disable_warnings(OTModelEntities 4996 4828)
ot_add_compile_definitions(OTModelEntities
    _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
    _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
)

ot_add_dependency(OTModelEntities
    OTSystem
    OTCore
    OTGui
    OTCommunication
    OTDataStorage
    RJSON
    MONGO_CXX
    MONGO_C
)

ot_finalize_lib(OTModelEntities)
ot_add_test(OTModelEntities)
