cmake_minimum_required(VERSION 3.20)
project(OTRubberband LANGUAGES CXX)

set(OT_RUBBERBAND_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTRubberband OT_RUBBERBAND_ROOT_PATH)
ot_add_export(OTRubberband OT_RUBBERBAND_EXPORTS)
ot_add_dependency(OTRubberband
    OTSystem
    OTCore
    RJSON
)

ot_finalize_lib(OTRubberband)
ot_add_test(OTRubberband)
