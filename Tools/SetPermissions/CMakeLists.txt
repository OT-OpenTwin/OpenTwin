cmake_minimum_required(VERSION 3.20)
project(SetPermissions LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_bin(SetPermissions OT_SETPERMISSIONS_ROOT_PATH)
ot_set_subsystem(SetPermissions CONSOLE)
ot_set_runtime_static_release(SetPermissions)

ot_add_dependency(SetPermissions
    OTSystem
    OTCore
    OTRandom
    WINLIB:bcrypt
)

ot_finalize_bin(SetPermissions)
