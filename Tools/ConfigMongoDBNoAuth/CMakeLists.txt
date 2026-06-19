cmake_minimum_required(VERSION 3.20)
project(ConfigMongoDBNoAuth LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_bin(ConfigMongoDBNoAuth OT_CONFIGMONGODBNOAUTH_ROOT_PATH)
ot_set_subsystem(ConfigMongoDBNoAuth CONSOLE)
ot_set_runtime_static_release(ConfigMongoDBNoAuth)

ot_add_dependency(ConfigMongoDBNoAuth
    OTSystem
    OTCore
    OTServiceFoundation
    WINLIB:bcrypt
)

ot_finalize_bin(ConfigMongoDBNoAuth)
