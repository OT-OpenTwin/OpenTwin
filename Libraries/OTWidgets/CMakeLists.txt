cmake_minimum_required(VERSION 3.20)
project(OTWidgets LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTWidgets OT_WIDGETS_ROOT_PATH OT_WIDGETS_EXPORTS)
ot_add_compile_definitions(OTWidgets QWT_DLL)

ot_add_dependency(OTWidgets
    OTSystem
    OTCore
    OTGui
    QtFull
    QWT
    ADS
    RJSON
    QT_TT
    OSLibs
)

ot_finalize_lib(OTWidgets)
ot_add_test(OTWidgets)
