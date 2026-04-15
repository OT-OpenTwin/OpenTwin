cmake_minimum_required(VERSION 3.20)
project(OTDataStorage LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTDataStorage OT_DATASTORAGE_ROOT_PATH)
ot_add_export(OTDataStorage DATASTORAGE_EXPORTS)
ot_disable_warnings(OTDataStorage 4996)

ot_add_dependency(OTDataStorage
    OTSystem
    OTCore
    RJSON
    MONGO_C
    MONGO_CXX
)

ot_finalize_lib(OTDataStorage)
ot_add_test(OTDataStorage)
