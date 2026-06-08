cmake_minimum_required(VERSION 3.20)
project(OTSystem LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTSystem OT_SYSTEM_ROOT_PATH)
ot_add_compile_options(OTSystem /W3 /sdl 4996)

ot_add_dependency(OTSystem
    OSLibs
    WINLIB:Crypt32
)

ot_finalize_lib(OTSystem)
ot_add_test(OTSystem)
