cmake_minimum_required(VERSION 3.20)
project(HierarchicalProjectService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(HierarchicalProjectService OT_HIERARCHICAL_PROJECT_SERVICE_ROOT_PATH HIERARCHICALPROJECTSERVICE_EXPORTS)
ot_disable_warnings(HierarchicalProjectService 4996)

ot_add_dependency(HierarchicalProjectService
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

ot_finalize_lib(HierarchicalProjectService)
ot_add_test(HierarchicalProjectService)
