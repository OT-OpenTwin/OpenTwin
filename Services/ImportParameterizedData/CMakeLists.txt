cmake_minimum_required(VERSION 3.20)
project(ImportParameterizedDataService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(ImportParameterizedDataService OT_IMPORT_PARAMETERIZED_DATA_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_disable_warnings(ImportParameterizedDataService 4996)
ot_add_compile_options(ImportParameterizedDataService "/utf-8")

ot_add_dependency(ImportParameterizedDataService
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
    RJSON
    CURL
    MONGO_C
    MONGO_CXX
    BOOST
    ZLIB
    BASE64
)

ot_finalize_lib(ImportParameterizedDataService)
ot_add_test(ImportParameterizedDataService)
