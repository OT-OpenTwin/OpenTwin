cmake_minimum_required(VERSION 3.20)
project(UICore LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(UICore OT_UICORE_ROOT_PATH UICORE_EXPORT)

ot_add_dependency(UICore
    OTSystem
    OTCore
    OTGui
    OTWidgets
    RJSON
    QT_TT
    QtWidgets
    QtOpenGLWidgets
    OSLibs
)

ot_finalize_lib(UICore)
ot_add_test(UICore)
