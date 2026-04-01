cmake_minimum_required(VERSION 3.20)
project(OTGuiAPI LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTGuiAPI OT_GUIAPI_ROOT_PATH)
ot_add_export(OTGuiAPI OTGUIAPI_EXPORTS)

ot_add_dependency(OTGuiAPI
    OTSystem
    OTCore
    OTGui
    OTCommunication
    RJSON
    CURL
    WINLIB:userenv
    WINLIB:wsock32
    WINLIB:advapi32
)

ot_finalize_lib(OTGuiAPI)
ot_add_test(OTGuiAPI)
