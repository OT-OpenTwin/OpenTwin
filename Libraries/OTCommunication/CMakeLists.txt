cmake_minimum_required(VERSION 3.20)
project(OTCommunication LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTCommunication OT_COMMUNICATION_ROOT_PATH)
ot_add_export(OTCommunication)

ot_add_dependency(OTCommunication
    OTSystem
    OTCore
    RJSON
    CURL
    ZLIB
    BASE64
)

ot_finalize_lib(OTCommunication)

ot_add_test(OTCommunication)
