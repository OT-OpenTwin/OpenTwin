cmake_minimum_required(VERSION 3.20)
project(EndpointDocParser LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_bin(EndpointDocParser OT_ENDPOINTDOCPARSER_ROOT_PATH)
ot_set_subsystem(EndpointDocParser CONSOLE)

ot_add_dependency(EndpointDocParser
    OTCore
    OTSystem
    RJSON
    ZLIB
)

ot_finalize_bin(EndpointDocParser)
ot_add_test(EndpointDocParser)
