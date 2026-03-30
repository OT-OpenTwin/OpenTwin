cmake_minimum_required(VERSION 3.20)
project(OTCore LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTEnvironment.cmake")
include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTCore OT_CORE_ROOT_PATH)
ot_add_export(OTCore)

ot_add_dependency(OTCore
    OTSystem
    ZLIB
    RJSON
    BASE64
    WINLIB:Userenv
)

ot_finalize_lib(OTCore)

ot_add_test(OTCore)
