cmake_minimum_required(VERSION 3.20)
project(FileManagementProjectService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(FileManagementProjectService OT_FILEMANAGEMENT_PROJECT_SERVICE_ROOT_PATH FILEMANAGEMENTPROJECTSERVICE_EXPORTS)
ot_disable_warnings(FileManagementProjectService 4996)

ot_add_dependency(FileManagementProjectService
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
    BOOST
    MONGO_BOOST
)

ot_finalize_lib(FileManagementProjectService)
ot_add_test(FileManagementProjectService)
