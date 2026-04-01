cmake_minimum_required(VERSION 3.20)
project(OTSystem LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTSystem OT_SYSTEM_ROOT_PATH)
ot_add_export(OTSystem)

target_compile_options(OTSystem_core PRIVATE
    /W3
    /wd4996
    /sdl
)

ot_add_dependency(OTSystem
    WINLIB:secur32
    WINLIB:advapi32
    WINLIB:userenv
    WINLIB:ws2_32
    WINLIB:pdh
)

ot_finalize_lib(OTSystem)
ot_add_test(OTSystem)
