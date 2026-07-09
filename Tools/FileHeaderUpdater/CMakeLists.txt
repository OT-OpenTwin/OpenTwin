cmake_minimum_required(VERSION 3.20)
project(FileHeaderUpdater LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_bin(FileHeaderUpdater OT_FILEHEADERUPDATER_ROOT_PATH)
ot_set_subsystem(FileHeaderUpdater CONSOLE)

ot_add_dependency(FileHeaderUpdater
    RJSON
)

ot_finalize_bin(FileHeaderUpdater)

ot_tool_bin_debug_launch(FileHeaderUpdater
    ARGSD "--dry" "--config" "@OPENTWIN_DEV_ROOT@\\Tools\\FileHeaderUpdater\\OT_FHU_Config.json"
)
