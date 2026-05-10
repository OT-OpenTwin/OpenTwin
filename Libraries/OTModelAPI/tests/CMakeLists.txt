cmake_minimum_required(VERSION 3.20)
project(OTModelAPI_tests LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_test(OTModelAPI_tests OTModelAPI)
