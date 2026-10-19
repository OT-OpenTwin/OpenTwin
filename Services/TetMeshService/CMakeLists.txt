cmake_minimum_required(VERSION 3.20)
project(TetMeshService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(TetMeshService OT_TET_MESH_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_disable_warnings(TetMeshService 4996)

ot_add_dependency(TetMeshService
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
    GMSH
    OCCT
    ZLIB
    BASE64
)

ot_finalize_lib(TetMeshService)
ot_add_test(TetMeshService)
