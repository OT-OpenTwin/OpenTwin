cmake_minimum_required(VERSION 3.20)
project(OTSystemInformationTool LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_bin(OTSystemInformationTool OT_OTSYSTEMINFORMATIONTOOL_ROOT_PATH)
ot_set_subsystem(OTSystemInformationTool CONSOLE)

ot_add_dependency(OTSystemInformationTool
    OTSystem
    OTCore
    RJSON
)

ot_finalize_bin(OTSystemInformationTool)

ot_tool_bin_debug_launch(OTSystemInformationTool
    ARGSD "Output.txt"
    PATHD "@OT_SYSTEM_ROOT@\\@OT_CDLLD@;@OT_CORE_ROOT@\\@OT_CDLLD@;@PATH@"
)
