cmake_minimum_required(VERSION 3.20)
project(OToolkitAPI LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OToolkitAPI OT_OTOOLKITAPI_ROOT_PATH OTOOLKITAPI_LIB)

ot_add_dependency(OToolkitAPI
    OTSystem
    OTCore
    OTGui
    OTWidgets
    RJSON
    ADS
    QtCore
    QtGui
    QtWidgets
)

ot_finalize_lib(OToolkitAPI)
