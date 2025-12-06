cmake_minimum_required(VERSION 3.20)

project(OTModelAPI LANGUAGES CXX)

# ============================================================
# Helper functions
# ============================================================

# Read a path from an environment variable and normalize it
function(get_env_path OUT_VAR ENV_NAME)
    if(DEFINED ENV{${ENV_NAME}} AND NOT "$ENV{${ENV_NAME}}" STREQUAL "")
        set(_val "$ENV{${ENV_NAME}}")
        string(REPLACE "\"" "" _val "${_val}")
        file(TO_CMAKE_PATH "${_val}" _val)
        set(${OUT_VAR} "${_val}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

# Read a semi-colon separated list of library names from an env var
function(get_env_libname OUT_VAR ENV_NAME)
    if(DEFINED ENV{${ENV_NAME}} AND NOT "$ENV{${ENV_NAME}}" STREQUAL "")
        set(_val "$ENV{${ENV_NAME}}")
        string(REPLACE "\"" "" _val "${_val}")
        # no path normalization – these are pure library names
        set(${OUT_VAR} "${_val}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

# Join ROOT + SUBDIR (used for $(OT_SYSTEM_ROOT)\$(OT_CLIBD) etc.)
function(join_paths OUT_VAR BASE SUBDIR)
    if(NOT "${BASE}" STREQUAL "" AND NOT "${SUBDIR}" STREQUAL "")
        set(_p "${BASE}/${SUBDIR}")
        file(TO_CMAKE_PATH "${_p}" _p)
        set(${OUT_VAR} "${_p}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

# ============================================================
# Read environment variables (paths & library names)
# Mirrors the original .vcxproj setup, but stays generic.
# ============================================================

# OT roots + include & lib subfolders
get_env_path(OT_SYSTEM_ROOT_PATH        OT_SYSTEM_ROOT)
get_env_path(OT_CORE_ROOT_PATH          OT_CORE_ROOT)
get_env_path(OT_GUI_ROOT_PATH           OT_GUI_ROOT)
get_env_path(OT_COMMUNICATION_ROOT_PATH OT_COMMUNICATION_ROOT)
get_env_path(OT_MODELENTITIES_ROOT_PATH OT_MODELENTITIES_ROOT)
get_env_path(OT_DATASTORAGE_ROOT_PATH   OT_DATASTORAGE_ROOT)

get_env_path(OT_INC_PATH                OT_INC)
get_env_path(OT_CLIBD_PATH              OT_CLIBD)
get_env_path(OT_CLIBR_PATH              OT_CLIBR)

# RapidJSON
get_env_path(R_JSON_INCD_PATH           R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH           R_JSON_INCR)

# Mongo C++ & Boost
get_env_path(MONGO_CXX_INC_PATH         MONGO_CXX_INC)
get_env_path(MONGO_BOOST_ROOT_PATH      MONGO_BOOST_ROOT)

get_env_path(MONGO_C_LIBPATHD_PATH      MONGO_C_LIBPATHD)
get_env_path(MONGO_C_LIBPATHR_PATH      MONGO_C_LIBPATHR)
get_env_path(MONGO_CXX_LIBPATHD_PATH    MONGO_CXX_LIBPATHD)
get_env_path(MONGO_CXX_LIBPATHR_PATH    MONGO_CXX_LIBPATHR)

get_env_libname(MONGO_CXX_LIBS_NAME     MONGO_CXX_LIB)    # "bsoncxx.lib;mongocxx.lib"

# Build concrete OT library directories
join_paths(OT_SYSTEM_DEBUG_LIB_DIR      "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBD_PATH}")
join_paths(OT_SYSTEM_RELEASE_LIB_DIR    "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBR_PATH}")
join_paths(OT_CORE_DEBUG_LIB_DIR        "${OT_CORE_ROOT_PATH}"   "${OT_CLIBD_PATH}")
join_paths(OT_CORE_RELEASE_LIB_DIR      "${OT_CORE_ROOT_PATH}"   "${OT_CLIBR_PATH}")
join_paths(OT_GUI_DEBUG_LIB_DIR         "${OT_GUI_ROOT_PATH}"    "${OT_CLIBD_PATH}")
join_paths(OT_GUI_RELEASE_LIB_DIR       "${OT_GUI_ROOT_PATH}"    "${OT_CLIBR_PATH}")
join_paths(OT_COMM_DEBUG_LIB_DIR        "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBD_PATH}")
join_paths(OT_COMM_RELEASE_LIB_DIR      "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBR_PATH}")
join_paths(OT_MODELENT_DEBUG_LIB_DIR    "${OT_MODELENTITIES_ROOT_PATH}" "${OT_CLIBD_PATH}")
join_paths(OT_MODELENT_RELEASE_LIB_DIR  "${OT_MODELENTITIES_ROOT_PATH}" "${OT_CLIBR_PATH}")
join_paths(OT_DATASTORAGE_DEBUG_LIB_DIR "${OT_DATASTORAGE_ROOT_PATH}"   "${OT_CLIBD_PATH}")
join_paths(OT_DATASTORAGE_RELEASE_LIB_DIR "${OT_DATASTORAGE_ROOT_PATH}" "${OT_CLIBR_PATH}")

# ============================================================
# Sources
# ============================================================

file(GLOB_RECURSE OTModelAPI_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

file(GLOB_RECURSE OTModelAPI_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Object library with all core sources (reused by tests)
add_library(OTModelAPI_core OBJECT
    ${OTModelAPI_HEADERS}
    ${OTModelAPI_SOURCES}
)

target_include_directories(OTModelAPI_core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# OT include trees (e.g. $(OT_SYSTEM_ROOT)\$(OT_INC))
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelAPI_core PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelAPI_core PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_GUI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelAPI_core PRIVATE
        "${OT_GUI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_COMMUNICATION_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelAPI_core PRIVATE
        "${OT_COMMUNICATION_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_MODELENTITIES_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelAPI_core PRIVATE
        "${OT_MODELENTITIES_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_DATASTORAGE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelAPI_core PRIVATE
        "${OT_DATASTORAGE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# RapidJSON includes
if(R_JSON_INCD_PATH)
    target_include_directories(OTModelAPI_core PRIVATE "${R_JSON_INCD_PATH}")
endif()

if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(OTModelAPI_core PRIVATE "${R_JSON_INCR_PATH}")
endif()

# Mongo C++ / Boost includes
if(MONGO_CXX_INC_PATH)
    target_include_directories(OTModelAPI_core PRIVATE "${MONGO_CXX_INC_PATH}")
endif()

if(MONGO_BOOST_ROOT_PATH)
    target_include_directories(OTModelAPI_core PRIVATE "${MONGO_BOOST_ROOT_PATH}")
endif()

# C++20, MSVC warning tweaks etc.
target_compile_features(OTModelAPI_core PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(OTModelAPI_core PRIVATE /MP /wd4996)
endif()

target_compile_definitions(OTModelAPI_core
    PRIVATE
        OTMODELAPI_EXPORTS
        _WINDOWS
        _USRDLL
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:DebugTest>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        $<$<CONFIG:ReleaseTest>:NDEBUG>
)

# ============================================================
# Shared library OTModelAPI (DLL)
# ============================================================

add_library(OTModelAPI SHARED)
add_library(OT::ModelAPI ALIAS OTModelAPI)

target_sources(OTModelAPI
    PRIVATE
        $<TARGET_OBJECTS:OTModelAPI_core>
)

target_include_directories(OTModelAPI
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_features(OTModelAPI PRIVATE cxx_std_20)

target_compile_definitions(OTModelAPI
    PRIVATE
        OTMODELAPI_EXPORTS
        _WINDOWS
        _USRDLL
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:DebugTest>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        $<$<CONFIG:ReleaseTest>:NDEBUG>
)

set_target_properties(OTModelAPI PROPERTIES
    OUTPUT_NAME "OTModelAPI"
)

# ============================================================
# Windows-specific linking (OTSystem, OTCore, Mongo, etc.)
# ============================================================

if(WIN32)
    # --- Library directories ---

    # OTSystem
    if(OT_SYSTEM_DEBUG_LIB_DIR)
        target_link_directories(OTModelAPI PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
            $<$<CONFIG:DebugTest>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(OTModelAPI PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:ReleaseTest>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    # OTCore
    if(OT_CORE_DEBUG_LIB_DIR)
        target_link_directories(OTModelAPI PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
            $<$<CONFIG:DebugTest>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_CORE_RELEASE_LIB_DIR)
        target_link_directories(OTModelAPI PRIVATE
            $<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:ReleaseTest>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    # OTGui
    if(OT_GUI_DEBUG_LIB_DIR)
        target_link_directories(OTModelAPI PRIVATE
            $<$<CONFIG:Debug>:${OT_GUI_DEBUG_LIB_DIR}>
            $<$<CONFIG:DebugTest>:${OT_GUI_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_GUI_RELEASE_LIB_DIR)
        target_link_directories(OTModelAPI PRIVATE
            $<$<CONFIG:Release>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:ReleaseTest>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_GUI_RELEASE_LIB_DIR}>
        )
    endif()

    # OTCommunication
    if(OT_COMM_DEBUG_LIB_DIR)
        target_link_directories(OTModelAPI PRIVATE
            $<$<CONFIG:Debug>:${OT_COMM_DEBUG_LIB_DIR}>
            $<$<CONFIG:DebugTest>:${OT_COMM_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_COMM_RELEASE_LIB_DIR)
        target_link_directories(OTModelAPI PRIVATE
            $<$<CONFIG:Release>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:ReleaseTest>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_COMM_RELEASE_LIB_DIR}>
        )
    endif()

    # ModelEntities
    if(OT_MODELENT_DEBUG_LIB_DIR)
        target_link_directories(OTModelAPI PRIVATE
            $<$<CONFIG:Debug>:${OT_MODELENT_DEBUG_LIB_DIR}>
            $<$<CONFIG:DebugTest>:${OT_MODELENT_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_MODELENT_RELEASE_LIB_DIR)
        target_link_directories(OTModelAPI PRIVATE
            $<$<CONFIG:Release>:${OT_MODELENT_RELEASE_LIB_DIR}>
            $<$<CONFIG:ReleaseTest>:${OT_MODELENT_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_MODELENT_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_MODELENT_RELEASE_LIB_DIR}>
        )
    endif()

    # DataStorage
    if(OT_DATASTORAGE_DEBUG_LIB_DIR)
        target_link_directories(OTModelAPI PRIVATE
            $<$<CONFIG:Debug>:${OT_DATASTORAGE_DEBUG_LIB_DIR}>
            $<$<CONFIG:DebugTest>:${OT_DATASTORAGE_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_DATASTORAGE_RELEASE_LIB_DIR)
        target_link_directories(OTModelAPI PRIVATE
            $<$<CONFIG:Release>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
            $<$<CONFIG:ReleaseTest>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
        )
    endif()

    # Mongo C / C++ lib paths (full paths from env)
    if(MONGO_C_LIBPATHD_PATH)
        target_link_directories(OTModelAPI PRIVATE
            $<$<CONFIG:Debug>:${MONGO_C_LIBPATHD_PATH}>
            $<$<CONFIG:DebugTest>:${MONGO_C_LIBPATHD_PATH}>
        )
    endif()
    if(MONGO_C_LIBPATHR_PATH)
        target_link_directories(OTModelAPI PRIVATE
            $<$<CONFIG:Release>:${MONGO_C_LIBPATHR_PATH}>
            $<$<CONFIG:ReleaseTest>:${MONGO_C_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${MONGO_C_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${MONGO_C_LIBPATHR_PATH}>
        )
    endif()

    if(MONGO_CXX_LIBPATHD_PATH)
        target_link_directories(OTModelAPI PRIVATE
            $<$<CONFIG:Debug>:${MONGO_CXX_LIBPATHD_PATH}>
            $<$<CONFIG:DebugTest>:${MONGO_CXX_LIBPATHD_PATH}>
        )
    endif()
    if(MONGO_CXX_LIBPATHR_PATH)
        target_link_directories(OTModelAPI PRIVATE
            $<$<CONFIG:Release>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:ReleaseTest>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${MONGO_CXX_LIBPATHR_PATH}>
        )
    endif()

    # --- Link libraries ---

    target_link_libraries(OTModelAPI
        PRIVATE
            OTSystem
            OTCore
            OTGui
            OTCommunication
            ModelEntities
            DataStorage
            Userenv
            wsock32
            Advapi32
            ${MONGO_CXX_LIBS_NAME}   # e.g. "bsoncxx.lib;mongocxx.lib"
    )
endif()

# ============================================================
# Tests (optional)
# ============================================================

option(BUILD_TESTING "Build OTModelAPI tests" ON)

if(BUILD_TESTING)
    include(FetchContent)

    if(NOT TARGET GTest::gtest_main)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.15.2
        )

        set(BUILD_GTEST ON  CACHE BOOL "" FORCE)
        set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

        FetchContent_MakeAvailable(googletest)
    endif()

    enable_testing()
    add_subdirectory(tests)
endif()
