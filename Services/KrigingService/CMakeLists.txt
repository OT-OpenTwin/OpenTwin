cmake_minimum_required(VERSION 3.20)
project(KrigingService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(KrigingService OT_KRIGING_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)

ot_add_dependency(KrigingService
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
    MONGO_BOOST
    ZLIB
    PYTHON_LEGACY
)

ot_finalize_lib(KrigingService)
