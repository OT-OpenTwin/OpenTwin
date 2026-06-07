cmake_minimum_required(VERSION 3.20)
project(ConfigMongoDBWithAuth LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_bin(ConfigMongoDBWithAuth OT_CONFIGMONGODBWITHAUTH_ROOT_PATH)
ot_set_subsystem(ConfigMongoDBWithAuth CONSOLE)
ot_set_runtime_static_release(ConfigMongoDBWithAuth)

ot_add_dependency(ConfigMongoDBWithAuth
    OTSystem
    OTCore
    OTRandom
    WINLIB:bcrypt
)

ot_finalize_bin(ConfigMongoDBWithAuth)
