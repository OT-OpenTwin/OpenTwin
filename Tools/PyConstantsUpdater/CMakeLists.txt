cmake_minimum_required(VERSION 3.20)
project(PyConstantsUpdater LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_bin(PyConstantsUpdater OT_PYCONSTANTSUPDATER_ROOT_PATH)
ot_set_subsystem(PyConstantsUpdater CONSOLE)

ot_add_dependency(PyConstantsUpdater
    OSLibs
)

ot_finalize_bin(PyConstantsUpdater)
