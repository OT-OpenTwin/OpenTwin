cmake_minimum_required(VERSION 3.20)
project(OTRandom LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTRandom OT_RANDOM_ROOT_PATH)
ot_add_export(OTRandom)

ot_add_dependency(OTRandom
    OTSystem
    OTCore
    RJSON
    CURL
    WINLIB:Bcrypt
)

ot_finalize_lib(OTRandom)
ot_add_test(OTRandom)
