cmake_minimum_required(VERSION 3.20)
project(KeyGenerator LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_bin(KeyGenerator OT_KEYGENERATOR_ROOT_PATH)
ot_set_subsystem(KeyGenerator CONSOLE)
ot_set_runtime_static_release(KeyGenerator)

ot_add_dependency(KeyGenerator
    OTSystem
    OTCore
    WINLIB:bcrypt
)

ot_finalize_bin(KeyGenerator)
