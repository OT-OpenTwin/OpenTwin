cmake_minimum_required(VERSION 3.20)
project(OTRubberbandOSG LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTRubberbandOSG OT_RUBBERBAND_OSG_ROOT_PATH RBE_OSG_WRAPPER_PROJ_BUILD)

ot_add_dependency(OTRubberbandOSG
    OTSystem
    OTCore
    OTRubberband
    QtFull
    OSG
    RJSON
    OSLibs
)

ot_finalize_lib(OTRubberbandOSG)
ot_add_test(OTRubberbandOSG)
