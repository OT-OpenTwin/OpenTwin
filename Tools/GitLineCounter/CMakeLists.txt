cmake_minimum_required(VERSION 3.20)
project(GitLineCounter LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_bin(GitLineCounter OT_GITLINECOUNTER_ROOT_PATH)
ot_set_subsystem(GitLineCounter CONSOLE)

ot_finalize_bin(GitLineCounter)
