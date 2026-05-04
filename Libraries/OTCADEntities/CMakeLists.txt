cmake_minimum_required(VERSION 3.20)
project(OTCADEntities LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTCADEntities OT_CADMODELENTITIES_ROOT_PATH)
ot_add_export(OTCADEntities OPENTWINCADMODELENTITIES_EXPORTS)

ot_add_dependency(OTCADEntities
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
    OCCT
    TINYXML2
)

ot_finalize_lib(OTCADEntities)
ot_add_test(OTCADEntities)
