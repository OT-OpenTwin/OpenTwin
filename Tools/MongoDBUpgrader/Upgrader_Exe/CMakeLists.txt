cmake_minimum_required(VERSION 3.20)
project(MongoDBUpgradeManager LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_bin(MongoDBUpgradeManager OT_MONGODBUPGRADEMANAGER_ROOT_PATH)
ot_set_subsystem(MongoDBUpgradeManager CONSOLE)
ot_set_runtime_static_release(MongoDBUpgradeManager)

ot_add_dependency(MongoDBUpgradeManager
    OTSystem
    OTCore
    RJSON
    MONGO_C
    MONGO_CXX
    BOOST_FILESYSTEM
)

ot_finalize_bin(MongoDBUpgradeManager)
