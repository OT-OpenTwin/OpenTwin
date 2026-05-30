cmake_minimum_required(VERSION 3.20)
project(RelayService_tests LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_test(RelayService_tests RelayService)