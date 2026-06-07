cmake_minimum_required(VERSION 3.20)
project(PasswordEncryption LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_bin(PasswordEncryption OT_PASSWORDENCRYPTION_ROOT_PATH)
ot_set_subsystem(PasswordEncryption CONSOLE)
ot_set_runtime_static_release(PasswordEncryption)

ot_add_dependency(PasswordEncryption
    OTSystem
    OTCore
    OTServiceFoundation
)

ot_finalize_bin(PasswordEncryption)
