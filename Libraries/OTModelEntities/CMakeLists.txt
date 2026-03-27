cmake_minimum_required(VERSION 3.20)

project(OTModelEntities LANGUAGES CXX)

# ============================================================
# Helper functions (same style as your other libs)
# ============================================================
function(get_env_path OUT_VAR ENV_NAME)
    if(DEFINED ENV{${ENV_NAME}} AND NOT "$ENV{${ENV_NAME}}" STREQUAL "")
        set(_val "$ENV{${ENV_NAME}}")
        string(REPLACE "\"" "" _val "${_val}")   # strip quotes
        file(TO_CMAKE_PATH "${_val}" _val)       # normalize slashes
        set(${OUT_VAR} "${_val}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

function(get_env_libname OUT_VAR ENV_NAME)
    if(DEFINED ENV{${ENV_NAME}} AND NOT "$ENV{${ENV_NAME}}" STREQUAL "")
        set(_val "$ENV{${ENV_NAME}}")
        string(REPLACE "\"" "" _val "${_val}")   # strip quotes
        set(${OUT_VAR} "${_val}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

function(join_paths OUT_VAR BASE SUBDIR)
    if(NOT "${BASE}" STREQUAL "" AND NOT "${SUBDIR}" STREQUAL "")
        set(_p "${BASE}/${SUBDIR}")
        file(TO_CMAKE_PATH "${_p}" _p)
        set(${OUT_VAR} "${_p}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

# Small helper for Visual Studio source_group mapping
function(assign_filter FILTER_NAME)
    foreach(FILE IN LISTS ARGN)
        set(_full_path "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}")
        if(EXISTS "${_full_path}")
            source_group("${FILTER_NAME}" FILES "${FILE}")
        endif()
    endforeach()
endfunction()

# ============================================================
# Environment configuration (mirrors .vcxproj usage)
# ============================================================

# OT roots and include subdir
get_env_path(OT_SYSTEM_ROOT_PATH        OT_SYSTEM_ROOT)
get_env_path(OT_CORE_ROOT_PATH          OT_CORE_ROOT)
get_env_path(OT_GUI_ROOT_PATH           OT_GUI_ROOT)
get_env_path(OT_COMMUNICATION_ROOT_PATH OT_COMMUNICATION_ROOT)
get_env_path(OT_DATASTORAGE_ROOT_PATH   OT_DATASTORAGE_ROOT)
get_env_path(OT_MODELENTITIES_ROOT_PATH OT_MODELENTITIES_ROOT)
get_env_path(OT_INC_PATH                OT_INC)

# OT library subdirs
get_env_path(OT_CLIBD_PATH              OT_CLIBD)
get_env_path(OT_CLIBR_PATH              OT_CLIBR)

# RapidJSON include
get_env_path(R_JSON_INCD_PATH           R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH           R_JSON_INCR)

# Mongo C++ / C / Boost
get_env_path(MONGO_CXX_INC_PATH         MONGO_CXX_INC)
get_env_path(MONGO_C_INC_PATH           MONGO_C_INC)

get_env_path(MONGO_BOOST_INCD_PATH      MONGO_BOOST_INCD)
get_env_path(MONGO_BOOST_INCR_PATH      MONGO_BOOST_INCR)

get_env_path(MONGO_CXX_LIBPATHD_PATH    MONGO_CXX_LIBPATHD)
get_env_path(MONGO_CXX_LIBPATHR_PATH    MONGO_CXX_LIBPATHR)
get_env_path(MONGO_C_LIBPATHD_PATH      MONGO_C_LIBPATHD)
get_env_path(MONGO_C_LIBPATHR_PATH      MONGO_C_LIBPATHR)

get_env_libname(MONGO_CXX_LIBS_NAME     MONGO_CXX_LIB)  # "bsoncxx.lib;mongocxx.lib"
get_env_libname(MONGO_C_LIBS_NAME       MONGO_C_LIB)    # "bson-1.0.lib;mongoc-1.0.lib"

# ============================================================
# C++ standard
# ============================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# Source and header lists (from .vcxproj)
# ============================================================

file(GLOB_RECURSE OTMODELENTITIESSOURCE_FILES
    CONFIGURE_DEPENDS
    "${OT_MODELENTITIES_ROOT_PATH}/src/*.cpp"
)

file(GLOB_RECURSE OTMODELENTITIES_HEADER_FILES
    CONFIGURE_DEPENDS
    "${OT_MODELENTITIES_ROOT_PATH}/include/*.h"
    "${OT_MODELENTITIES_ROOT_PATH}/include/*.hpp"
)

# ============================================================
# Core OBJECT library
# ============================================================
add_library(OTModelEntities_core OBJECT
    ${OTMODELENTITIESSOURCE_FILES}
    ${OTMODELENTITIESHEADER_FILES}
)

target_compile_definitions(OTModelEntities_core
    PRIVATE
        UNICODE
        _UNICODE
        _WINDOWS
        _USRDLL
        OPENTWINMODELENTITIES_EXPORTS
        _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
        _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

# ============================================================
# Include directories
# Mirrors AdditionalIncludeDirectories from the .vcxproj
# ============================================================
target_include_directories(OTModelEntities_core
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# $(OT_*_ROOT)\$(OT_INC)
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelEntities_core PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelEntities_core PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_GUI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelEntities_core PRIVATE
        "${OT_GUI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_COMMUNICATION_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelEntities_core PRIVATE
        "${OT_COMMUNICATION_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_DATASTORAGE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelEntities_core PRIVATE
        "${OT_DATASTORAGE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# RapidJSON
if(R_JSON_INCD_PATH)
    target_include_directories(OTModelEntities_core PRIVATE "${R_JSON_INCD_PATH}")
endif()
if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(OTModelEntities_core PRIVATE "${R_JSON_INCR_PATH}")
endif()

# Mongo C++ / C / Boost includes
if(MONGO_CXX_INC_PATH)
    target_include_directories(OTModelEntities_core PRIVATE "${MONGO_CXX_INC_PATH}")
endif()

if(MONGO_C_INC_PATH)
    target_include_directories(OTModelEntities_core PRIVATE "${MONGO_C_INC_PATH}")
endif()

if(MONGO_BOOST_INCD_PATH)
    target_include_directories(OTModelEntities_core PRIVATE "${MONGO_BOOST_INCD_PATH}")
endif()

if(MONGO_BOOST_INCR_PATH AND NOT MONGO_BOOST_INCR_PATH STREQUAL MONGO_BOOST_INCD_PATH)
    target_include_directories(OTModelEntities_core PRIVATE "${MONGO_BOOST_INCR_PATH}")
endif()

# ============================================================
# MSVC compiler flags
# ============================================================
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    target_compile_options(OTModelEntities_core PRIVATE
        /W3
        /sdl-
        /MP
        /permissive-
        /Zc:__cplusplus
        /wd4996
        /wd4828
    )
endif()

# ============================================================
# Visual Studio filters (simple grouping)
# ============================================================
assign_filter("Source Files"
    ${OTMODELENTITIESSOURCE_FILES}
)

assign_filter("Header Files"
    ${OTMODELENTITIESHEADER_FILES}
)

# ============================================================
# Shared Library (DLL) OTModelEntities
# ============================================================
add_library(OTModelEntities SHARED
    $<TARGET_OBJECTS:OTModelEntities_core>
)

target_include_directories(OTModelEntities
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(WIN32)
    target_compile_definitions(OTModelEntities
        PRIVATE
            UNICODE
            _UNICODE
            _WINDOWS
            _USRDLL
            OPENTWINOTMODELENTITIESEXPORTS
    )

    # OTSystem / OTCore / OTGui / OTCommunication / OTDataStorage library directories
    join_paths(OT_SYSTEM_DEBUG_LIB_DIR        "${OT_SYSTEM_ROOT_PATH}"        "${OT_CLIBD_PATH}")
    join_paths(OT_SYSTEM_RELEASE_LIB_DIR      "${OT_SYSTEM_ROOT_PATH}"        "${OT_CLIBR_PATH}")
    join_paths(OT_CORE_DEBUG_LIB_DIR          "${OT_CORE_ROOT_PATH}"          "${OT_CLIBD_PATH}")
    join_paths(OT_CORE_RELEASE_LIB_DIR        "${OT_CORE_ROOT_PATH}"          "${OT_CLIBR_PATH}")
    join_paths(OT_GUI_DEBUG_LIB_DIR           "${OT_GUI_ROOT_PATH}"           "${OT_CLIBD_PATH}")
    join_paths(OT_GUI_RELEASE_LIB_DIR         "${OT_GUI_ROOT_PATH}"           "${OT_CLIBR_PATH}")
    join_paths(OT_COMM_DEBUG_LIB_DIR          "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBD_PATH}")
    join_paths(OT_COMM_RELEASE_LIB_DIR        "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBR_PATH}")
    join_paths(OT_DATASTORAGE_DEBUG_LIB_DIR   "${OT_DATASTORAGE_ROOT_PATH}"   "${OT_CLIBD_PATH}")
    join_paths(OT_DATASTORAGE_RELEASE_LIB_DIR "${OT_DATASTORAGE_ROOT_PATH}"   "${OT_CLIBR_PATH}")

    if(OT_SYSTEM_DEBUG_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_CORE_DEBUG_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_CORE_RELEASE_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_GUI_DEBUG_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_GUI_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_GUI_RELEASE_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_GUI_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_COMM_DEBUG_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_COMM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_COMM_RELEASE_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_COMM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_DATASTORAGE_DEBUG_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_DATASTORAGE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_DATASTORAGE_RELEASE_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
        )
    endif()

    # Mongo C++ / C library directories
    if(MONGO_CXX_LIBPATHD_PATH)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Debug>:${MONGO_CXX_LIBPATHD_PATH}>
        )
    endif()

    if(MONGO_CXX_LIBPATHR_PATH)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Release>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${MONGO_CXX_LIBPATHR_PATH}>
        )
    endif()

    if(MONGO_C_LIBPATHD_PATH)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Debug>:${MONGO_C_LIBPATHD_PATH}>
        )
    endif()

    if(MONGO_C_LIBPATHR_PATH)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Release>:${MONGO_C_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${MONGO_C_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${MONGO_C_LIBPATHR_PATH}>
        )
    endif()

    # Libraries (equivalent to AdditionalDependencies in the vcxproj)
    target_link_libraries(OTModelEntities
        PRIVATE
            OTSystem
            OTCore
            OTGui
            OTCommunication
            OTDataStorage
            ${MONGO_CXX_LIBS_NAME}
            ${MONGO_C_LIBS_NAME}
    )

    if(MSVC)
        target_link_options(OTModelEntities PRIVATE
            /SUBSYSTEM:WINDOWS
            /MANIFESTUAC:NO
            $<$<CONFIG:Release>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:RelWithDebInfo>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:MinSizeRel>:/OPT:REF /OPT:ICF>
        )
    endif()
endif()

# ============================================================
# GoogleTest + test integration (OTModelEntities_tests in tests/)
# ============================================================
include(CTest)

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
