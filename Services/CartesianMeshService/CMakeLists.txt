cmake_minimum_required(VERSION 3.20)
project(CartesianMeshService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(CartesianMeshService OT_CARTESIAN_MESH_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_disable_warnings(CartesianMeshService 4996)

ot_add_dependency(CartesianMeshService
    OTSystem
    OTCore
    OTGui
    OTCommunication
    OTGuiAPI
    OTServiceFoundation
    OTModelEntities
    OTDataStorage
    OTModelAPI
    OTCADEntities
    RJSON
    CURL
    MONGO_C
    MONGO_CXX
    BOOST
    EMBREE
    CGAL
    GMP
    OCCT
)

ot_finalize_lib(CartesianMeshService)
ot_add_test(CartesianMeshService)
