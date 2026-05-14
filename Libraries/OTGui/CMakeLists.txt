cmake_minimum_required(VERSION 3.20)
project(OTGui LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTGui OT_GUI_ROOT_PATH OTGUI_EXPORTS)

ot_add_dependency(OTGui
    OTSystem
    OTCore
    RJSON
    OSLibs
)

ot_finalize_lib(OTGui)
ot_add_test(OTGui)
