cmake_minimum_required(VERSION 3.20)
project(OTWidgets LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTWidgets OT_WIDGETS_ROOT_PATH)
ot_add_export(OTWidgets OT_WIDGETS_EXPORTS)

target_compile_definitions(OTWidgets_core PRIVATE
    QWT_DLL
)

ot_add_dependency(OTWidgets
    OTSystem
    OTCore
    OTGui
    QtFull
    QWT
    ADS
    RJSON
    QT_TT
    WINLIB:userenv
    WINLIB:ws2_32
    WINLIB:advapi32
    WINLIB:shell32
    WINLIB:bcrypt
)

ot_finalize_lib(OTWidgets)
ot_add_test(OTWidgets)
