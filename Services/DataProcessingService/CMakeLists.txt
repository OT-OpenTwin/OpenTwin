cmake_minimum_required(VERSION 3.20)
project(DataProcessingService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(DataProcessingService OT_DATA_PROCESSING_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_disable_warnings(DataProcessingService 4996)

ot_add_dependency(DataProcessingService
    OTSystem
    OTCore
    OTGui
    OTCommunication
    OTGuiAPI
    OTServiceFoundation
    OTModelEntities
    OTDataStorage
    OTModelAPI
    OTResultDataAccess
    OTBlockEntities
    RJSON
    CURL
    MONGO_C
    MONGO_CXX
    MONGO_BOOST
)

ot_finalize_lib(DataProcessingService)
ot_add_test(DataProcessingService)
