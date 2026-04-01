cmake_minimum_required(VERSION 3.20)
project(OTCore_tests LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_test(OTCore_tests OTCore)

