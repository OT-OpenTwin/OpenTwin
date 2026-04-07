cmake_minimum_required(VERSION 3.20)
project(UICore LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(UICore OT_UICORE_ROOT_PATH)
ot_add_export(UICore UICORE_EXPORT)

ot_add_dependency(UICore
    OTSystem
    OTCore
    OTGui
    OTWidgets
    RJSON
    QT_TT
    QtWidgets
    QtOpenGLWidgets
    WINLIB:advapi32
    WINLIB:odbc32
)

ot_finalize_lib(UICore)
ot_add_test(UICore)
