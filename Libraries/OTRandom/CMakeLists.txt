cmake_minimum_required(VERSION 3.20)

project(OTRandom LANGUAGES CXX)

# ------------------------------------------------------------
# Helper functions for environment variables and paths
# ------------------------------------------------------------
function(get_env_path OUT_VAR ENV_NAME)
    if(DEFINED ENV{${ENV_NAME}} AND NOT "$ENV{${ENV_NAME}}" STREQUAL "")
        set(_val "$ENV{${ENV_NAME}}")
        string(REPLACE "\"" "" _val "${_val}")   # strip double quotes
        file(TO_CMAKE_PATH "${_val}" _val)       # normalize slashes
        set(${OUT_VAR} "${_val}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

function(get_env_libname OUT_VAR ENV_NAME)
    if(DEFINED ENV{${ENV_NAME}} AND NOT "$ENV{${ENV_NAME}}" STREQUAL "")
        set(_val "$ENV{${ENV_NAME}}")
        string(REPLACE "\"" "" _val "${_val}")   # strip double quotes
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

# ------------------------------------------------------------
# Import environment configuration (mirrors original .vcxproj)
# ------------------------------------------------------------
# Roots and include subdirectories
get_env_path(OT_SYSTEM_ROOT_PATH  OT_SYSTEM_ROOT)
get_env_path(OT_CORE_ROOT_PATH    OT_CORE_ROOT)
get_env_path(OT_INC_PATH          OT_INC)

# RapidJSON include dirs
get_env_path(R_JSON_INCD_PATH     R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH     R_JSON_INCR)

# cURL includes and library search paths
get_env_path(CURL_INCD_PATH       CURL_INCD)
get_env_path(CURL_INCR_PATH       CURL_INCR)
get_env_path(CURL_LIBPATHD_PATH   CURL_LIBPATHD)
get_env_path(CURL_LIBPATHR_PATH   CURL_LIBPATHR)

# OTSystem / OTCore lib subdir names (e.g. build-vs2022-debug/Debug)
get_env_path(OT_CLIBD_PATH        OT_CLIBD)
get_env_path(OT_CLIBR_PATH        OT_CLIBR)
get_env_path(OT_LIBD_PATH         OT_LIBD)
get_env_path(OT_LIBR_PATH         OT_LIBR)

# cURL library names (not paths)
get_env_libname(CURL_LIBD_NAME    CURL_LIBD)
get_env_libname(CURL_LIBR_NAME    CURL_LIBR)

# ------------------------------------------------------------
# C++ standard configuration
# ------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------
# Core OBJECT library with all implementation sources.
# Both the DLL (OTRandom) and tests (OTRandom_tests) reuse
# these object files directly.
# ------------------------------------------------------------
add_library(OTRandom_core OBJECT
    src/RandomNumberGeneratorCryptoSecure.cpp
)

# Project-local includes
target_include_directories(OTRandom_core
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# $(OT_SYSTEM_ROOT)\$(OT_INC)
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTRandom_core PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# $(OT_CORE_ROOT)\$(OT_INC)
if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTRandom_core PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# RapidJSON includes
if(R_JSON_INCD_PATH)
    target_include_directories(OTRandom_core PRIVATE "${R_JSON_INCD_PATH}")
endif()
if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(OTRandom_core PRIVATE "${R_JSON_INCR_PATH}")
endif()

# cURL includes
if(CURL_INCD_PATH)
    target_include_directories(OTRandom_core PRIVATE "${CURL_INCD_PATH}")
endif()
if(CURL_INCR_PATH AND NOT CURL_INCR_PATH STREQUAL CURL_INCD_PATH)
    target_include_directories(OTRandom_core PRIVATE "${CURL_INCR_PATH}")
endif()

target_compile_definitions(OTRandom_core
    PRIVATE
        UNICODE
        _UNICODE
        _WINDOWS
        _USRDLL
        OPENTWINRANDOM_EXPORTS
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        $<$<CONFIG:RelWithDebInfo>:NDEBUG>
        $<$<CONFIG:MinSizeRel>:NDEBUG>
)

# ------------------------------------------------------------
# MSVC compiler options (mapped from Visual Studio)
# ------------------------------------------------------------
if(MSVC)
    # /MD (Release), /MDd (Debug)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    target_compile_options(OTRandom_core PRIVATE
        /W3
        /sdl
        /MP
        $<$<CXX_COMPILER_ID:MSVC>:/permissive- /Zc:__cplusplus>
    )
endif()

# ------------------------------------------------------------
# Optional Visual Studio source grouping
# ------------------------------------------------------------
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES
    src/RandomNumberGeneratorCryptoSecure.cpp

    include/OTRandom/OTRandomAPIExport.h
    include/OTRandom/RandomNumberGenerator.h
    include/OTRandom/RandomNumberGeneratorCryptoSecure.h
)

# ------------------------------------------------------------
# Shared library (DLL) – main production artifact.
# ------------------------------------------------------------
add_library(OTRandom SHARED
    $<TARGET_OBJECTS:OTRandom_core>
)

target_include_directories(OTRandom
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(WIN32)
    target_compile_definitions(OTRandom
        PRIVATE
            UNICODE
            _UNICODE
            _WINDOWS
            _USRDLL
            OPENTWINRANDOM_EXPORTS
    )

    # ---------- Library search paths for OTSystem / OTCore ----------
    join_paths(OT_SYSTEM_DEBUG_LIB_DIR   "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBD_PATH}")
    join_paths(OT_SYSTEM_RELEASE_LIB_DIR "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBR_PATH}")
    join_paths(OT_CORE_DEBUG_LIB_DIR     "${OT_CORE_ROOT_PATH}"   "${OT_LIBD_PATH}")
    join_paths(OT_CORE_RELEASE_LIB_DIR   "${OT_CORE_ROOT_PATH}"   "${OT_LIBR_PATH}")

    if(OT_SYSTEM_DEBUG_LIB_DIR)
        target_link_directories(OTRandom PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(OTRandom PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_CORE_DEBUG_LIB_DIR)
        target_link_directories(OTRandom PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_CORE_RELEASE_LIB_DIR)
        target_link_directories(OTRandom PRIVATE
            $<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    # ---------- cURL lib paths ----------
    if(CURL_LIBPATHD_PATH)
        target_link_directories(OTRandom PRIVATE
            $<$<CONFIG:Debug>:${CURL_LIBPATHD_PATH}>
        )
    endif()

    if(CURL_LIBPATHR_PATH)
        target_link_directories(OTRandom PRIVATE
            $<$<CONFIG:Release>:${CURL_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${CURL_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${CURL_LIBPATHR_PATH}>
        )
    endif()

    # ---------- Libraries to link ----------
    target_link_libraries(OTRandom
        PRIVATE
            OTSystem
            OTCore
            Bcrypt
    )

    if(CURL_LIBD_NAME)
        target_link_libraries(OTRandom PRIVATE
            $<$<CONFIG:Debug>:${CURL_LIBD_NAME}>
        )
    endif()

    if(CURL_LIBR_NAME)
        target_link_libraries(OTRandom PRIVATE
            $<$<CONFIG:Release>:${CURL_LIBR_NAME}>
            $<$<CONFIG:RelWithDebInfo>:${CURL_LIBR_NAME}>
            $<$<CONFIG:MinSizeRel>:${CURL_LIBR_NAME}>
        )
    endif()

    if(MSVC)
        target_link_options(OTRandom PRIVATE
            /SUBSYSTEM:WINDOWS
            /MANIFESTUAC:NO
            $<$<CONFIG:Release>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:RelWithDebInfo>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:MinSizeRel>:/OPT:REF /OPT:ICF>
        )
    endif()
endif()

# ------------------------------------------------------------
# GoogleTest integration (fetched via FetchContent)
# ------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
)

set(BUILD_GTEST ON  CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# ------------------------------------------------------------
# Testing setup – tests live in tests/ subdirectory
# ------------------------------------------------------------
include(CTest)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
