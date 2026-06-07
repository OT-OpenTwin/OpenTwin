cmake_minimum_required(VERSION 3.20)
project(WindowsSSO LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_bin(WindowsSSO OT_WINDOWSSSO_ROOT_PATH)
ot_set_subsystem(WindowsSSO CONSOLE)

ot_add_dependency(WindowsSSO
    BOOST
    WINLIB:Crypt32
)

ot_finalize_bin(WindowsSSO)
