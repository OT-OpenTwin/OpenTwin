cmake_minimum_required(VERSION 3.20)
project(OTGui LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTGui OT_GUI_ROOT_PATH)
ot_add_export(OTGui OTGUI_EXPORTS)

ot_add_dependency(OTGui
    OTSystem
    OTCore
    RJSON

    WINLIB:userenv
    WINLIB:wsock32
    WINLIB:advapi32
)

ot_finalize_lib(OTGui)
ot_add_test(OTGui)
