cmake_minimum_required(VERSION 3.20)
project(VisualizationService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(VisualizationService OT_VISUALIZATION_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_disable_warnings(VisualizationService 4996)

ot_add_dependency(VisualizationService
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
    VTK
    OSG
)

ot_finalize_lib(VisualizationService)
ot_add_test(VisualizationService)
