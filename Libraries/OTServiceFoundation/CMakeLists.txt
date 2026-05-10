cmake_minimum_required(VERSION 3.20)
project(OTServiceFoundation LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTServiceFoundation OT_FOUNDATION_ROOT_PATH)
ot_add_export(OTServiceFoundation OPENTWINFOUNDATION_EXPORTS)
ot_add_compile_options(OTServiceFoundation /wd4996 /wd4828)
ot_add_dependency(OTServiceFoundation
    OTSystem
    OTCore
    OTGui
    OTGuiAPI
    OTCommunication
    OTModelAPI
    OTModelEntities
    OTDataStorage
    OTCADEntities
    OTEncryptionKey
    RJSON
    MONGO_CXX
    CURL
    ZLIB
    OCCT
    BASE64
)

ot_finalize_lib(OTServiceFoundation)
ot_add_test(OTServiceFoundation)
