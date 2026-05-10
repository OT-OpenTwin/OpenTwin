cmake_minimum_required(VERSION 3.20)
project(OTModelAPI LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTModelAPI OT_MODELAPI_ROOT_PATH)
ot_add_export(OTModelAPI OTMODELAPI_EXPORTS)

ot_add_dependency(OTModelAPI
    OTSystem
    OTCore
    OTGui
    OTCommunication
    OTModelEntities
    OTDataStorage
    RJSON
    MONGO_CXX
    OSLibs
)

ot_finalize_lib(OTModelAPI)
ot_add_test(OTModelAPI)
