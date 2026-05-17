cmake_minimum_required(VERSION 3.20)
project(Model_tests LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_test(Model_tests Model)
