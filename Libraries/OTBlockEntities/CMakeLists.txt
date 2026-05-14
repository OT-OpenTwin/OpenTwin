cmake_minimum_required(VERSION 3.20)
project(OTBlockEntities LANGUAGES CXX)

set(OT_BLOCKENTITIES_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTBlockEntities OT_BLOCKENTITIES_ROOT_PATH OTBLOCKENTITIES_EXPORTS)
ot_add_compile_options(OTBlockEntities /W3)
ot_add_compile_definitions(OTBlockEntities
    _CONSOLE
    _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
)

ot_add_dependency(OTBlockEntities
    OTSystem
    OTCore
    OTGui
    OTGuiAPI
    OTCommunication
    OTModelEntities
    OTDataStorage
    RJSON
    MONGO_CXX
    MONGO_C
    BOOST
)

ot_finalize_lib(OTBlockEntities)
ot_add_test(OTBlockEntities)
