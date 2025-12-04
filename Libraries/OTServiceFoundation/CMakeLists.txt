cmake_minimum_required(VERSION 3.20)

# ============================================================
# OTServiceFoundation library (DLL)
# - Uses environment variables for include and library paths
# - Builds a core object library and one shared library
#   (Debug/Release). "Test" configs are handled via presets.
# ============================================================

project(OTServiceFoundation LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------
# Helper functions
# ------------------------------------------------------------
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
        string(REPLACE "\"" "" _val "${_val}")
        # Semicolon-separated list becomes a proper CMake list
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
# Read environment variables for OT roots, third party, etc.
# ------------------------------------------------------------

# OT roots
get_env_path(OT_SYSTEM_ROOT_PATH           OT_SYSTEM_ROOT)
get_env_path(OT_CORE_ROOT_PATH             OT_CORE_ROOT)
get_env_path(OT_GUI_ROOT_PATH              OT_GUI_ROOT)
get_env_path(OT_GUIAPI_ROOT_PATH           OT_GUIAPI_ROOT)
get_env_path(OT_COMMUNICATION_ROOT_PATH    OT_COMMUNICATION_ROOT)
get_env_path(OT_MODELENTITIES_ROOT_PATH    OT_MODELENTITIES_ROOT)
get_env_path(OT_MODELAPI_ROOT_PATH         OT_MODELAPI_ROOT)
get_env_path(OT_DATASTORAGE_ROOT_PATH      OT_DATASTORAGE_ROOT)
get_env_path(OT_CADMODELENTITIES_ROOT_PATH OT_CADMODELENTITIES_ROOT)
get_env_path(OT_RESULT_DATA_ACCESS_ROOT_PATH OT_RESULT_DATA_ACCESS_ROOT)
get_env_path(OT_ENCRYPTIONKEY_ROOT_PATH    OT_ENCRYPTIONKEY_ROOT)

get_env_path(OT_INC_PATH                   OT_INC)
get_env_path(OT_CLIBD_PATH                 OT_CLIBD)
get_env_path(OT_CLIBR_PATH                 OT_CLIBR)
get_env_path(OT_LIBD_PATH                  OT_LIBD)
get_env_path(OT_LIBR_PATH                  OT_LIBR)

# Third-party: RapidJSON
get_env_path(R_JSON_INCD_PATH              R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH              R_JSON_INCR)

# Third-party: cURL
get_env_path(CURL_INCD_PATH                CURL_INCD)
get_env_path(CURL_INCR_PATH                CURL_INCR)
get_env_path(CURL_LIBPATHD_PATH            CURL_LIBPATHD)
get_env_path(CURL_LIBPATHR_PATH            CURL_LIBPATHR)
get_env_libname(CURL_LIBD_NAME             CURL_LIBD)   # e.g. libcurl_debug.lib
get_env_libname(CURL_LIBR_NAME             CURL_LIBR)   # e.g. libcurl.lib

# Third-party: Mongo C / C++ / Boost
get_env_path(MONGO_CXX_INC_PATH            MONGO_CXX_INC)
get_env_path(MONGO_C_INC_PATH              MONGO_C_INC)
get_env_path(MONGO_BOOST_ROOT_PATH         MONGO_BOOST_ROOT)

get_env_path(MONGO_C_LIBPATHD_PATH         MONGO_C_LIBPATHD)
get_env_path(MONGO_C_LIBPATHR_PATH         MONGO_C_LIBPATHR)
get_env_path(MONGO_CXX_LIBPATHD_PATH       MONGO_CXX_LIBPATHD)
get_env_path(MONGO_CXX_LIBPATHR_PATH       MONGO_CXX_LIBPATHR)

get_env_libname(MONGO_CXX_LIBS_NAME        MONGO_CXX_LIB)   # "bsoncxx.lib;mongocxx.lib"
get_env_libname(MONGO_C_LIBS_NAME          MONGO_C_LIB)     # "bson-1.0.lib;mongoc-1.0.lib"

# Third-party: Zlib
get_env_path(ZLIB_INCD_PATH                ZLIB_INCD)
get_env_path(ZLIB_INCR_PATH                ZLIB_INCR)
get_env_path(ZLIB_LIBPATHD_PATH            ZLIB_LIBPATHD)
get_env_path(ZLIB_LIBPATHR_PATH            ZLIB_LIBPATHR)

# Third-party: OpenCascade (OC)
get_env_path(OC_INCD_PATH                  OC_INCD)
get_env_path(OC_INCR_PATH                  OC_INCR)
get_env_path(OC_LIBPATHD_PATH              OC_LIBPATHD)
get_env_path(OC_LIBPATHR_PATH              OC_LIBPATHR)
get_env_libname(OC_LIBS_NAME               CFG_OC_LIBS)     # from CFG_OC_LIBS

# base64 (header + c-file)
get_env_path(BASE64_ROOT_PATH              BASE64_ROOT)

# ------------------------------------------------------------
# Sources
# ------------------------------------------------------------

file(GLOB_RECURSE OTServiceFoundation_SRC_CPP
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

set(OTServiceFoundation_SOURCES ${OTServiceFoundation_SRC_CPP})

# Add base64.c if BASE64_ROOT is defined
if(BASE64_ROOT_PATH)
    list(APPEND OTServiceFoundation_SOURCES
        "${BASE64_ROOT_PATH}/base64.c"
    )
endif()

# Split out dllmain.cpp so tests can reuse core object files
set(OTServiceFoundation_DLLMAIN "")
set(OTServiceFoundation_CORE_SOURCES ${OTServiceFoundation_SOURCES})

foreach(_src IN LISTS OTServiceFoundation_SOURCES)
    if(_src MATCHES "dllmain\\.cpp$")
        set(OTServiceFoundation_DLLMAIN "${_src}")
        list(REMOVE_ITEM OTServiceFoundation_CORE_SOURCES "${_src}")
    endif()
endforeach()

# ------------------------------------------------------------
# Core object library
# ------------------------------------------------------------
add_library(OTServiceFoundation_core OBJECT
    ${OTServiceFoundation_CORE_SOURCES}
)

target_compile_definitions(OTServiceFoundation_core
    PRIVATE
        OPENTWINFOUNDATION_EXPORTS
)

target_include_directories(OTServiceFoundation_core
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# OT includes
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_GUI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE
        "${OT_GUI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_GUIAPI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE
        "${OT_GUIAPI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_COMMUNICATION_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE
        "${OT_COMMUNICATION_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_MODELENTITIES_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE
        "${OT_MODELENTITIES_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_MODELAPI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE
        "${OT_MODELAPI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_DATASTORAGE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE
        "${OT_DATASTORAGE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_CADMODELENTITIES_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE
        "${OT_CADMODELENTITIES_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_RESULT_DATA_ACCESS_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE
        "${OT_RESULT_DATA_ACCESS_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_ENCRYPTIONKEY_ROOT_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE
        "${OT_ENCRYPTIONKEY_ROOT_PATH}"
    )
endif()

# RapidJSON
if(R_JSON_INCD_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE "${R_JSON_INCD_PATH}")
endif()
if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE "${R_JSON_INCR_PATH}")
endif()

# cURL
if(CURL_INCD_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE "${CURL_INCD_PATH}")
endif()
if(CURL_INCR_PATH AND NOT CURL_INCR_PATH STREQUAL CURL_INCD_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE "${CURL_INCR_PATH}")
endif()

# Mongo C++ / Boost
if(MONGO_CXX_INC_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE "${MONGO_CXX_INC_PATH}")
endif()
if(MONGO_BOOST_ROOT_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE "${MONGO_BOOST_ROOT_PATH}")
endif()

# Mongo C (for some configs / code paths)
if(MONGO_C_INC_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE "${MONGO_C_INC_PATH}")
endif()

# Zlib
if(ZLIB_INCD_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE "${ZLIB_INCD_PATH}")
endif()
if(ZLIB_INCR_PATH AND NOT ZLIB_INCR_PATH STREQUAL ZLIB_INCD_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE "${ZLIB_INCR_PATH}")
endif()

# OpenCascade includes
if(OC_INCD_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE "${OC_INCD_PATH}")
endif()
if(OC_INCR_PATH AND NOT OC_INCR_PATH STREQUAL OC_INCD_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE "${OC_INCR_PATH}")
endif()

# base64 header
if(BASE64_ROOT_PATH)
    target_include_directories(OTServiceFoundation_core PRIVATE "${BASE64_ROOT_PATH}")
endif()

target_compile_options(OTServiceFoundation_core PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/wd4996 /wd4828>
)

# ------------------------------------------------------------
# Shared library that reuses the core object files
# ------------------------------------------------------------
add_library(OTServiceFoundation SHARED
    $<TARGET_OBJECTS:OTServiceFoundation_core>
    ${OTServiceFoundation_DLLMAIN}
)

target_link_libraries(OTServiceFoundation
    PRIVATE
        OTSystem
        OTCore
        OTGui
        OTGuiAPI
        OTCommunication
        OTModelAPI
        ModelEntities
        DataStorage
        CADModelEntities
        ${MONGO_CXX_LIBS_NAME}
        ${OC_LIBS_NAME}
)

# ------------------------------------------------------------
# Windows-specific link directories and libs
# ------------------------------------------------------------
if(WIN32)
    # OTSystem / OTCore / OTGui / OTGuiAPI / OTCommunication / OTModelAPI / ModelEntities / DataStorage / CADModelEntities
    if(OT_SYSTEM_ROOT_PATH AND OT_CLIBD_PATH)
        join_paths(OT_SYSTEM_DEBUG_LIB_DIR "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBD_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_SYSTEM_ROOT_PATH AND OT_CLIBR_PATH)
        join_paths(OT_SYSTEM_RELEASE_LIB_DIR "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBR_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_CORE_ROOT_PATH AND OT_CLIBD_PATH)
        join_paths(OT_CORE_DEBUG_LIB_DIR "${OT_CORE_ROOT_PATH}" "${OT_CLIBD_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_CORE_ROOT_PATH AND OT_CLIBR_PATH)
        join_paths(OT_CORE_RELEASE_LIB_DIR "${OT_CORE_ROOT_PATH}" "${OT_CLIBR_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_GUI_ROOT_PATH AND OT_CLIBD_PATH)
        join_paths(OT_GUI_DEBUG_LIB_DIR "${OT_GUI_ROOT_PATH}" "${OT_CLIBD_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<CONFIG:Debug>:${OT_GUI_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_GUI_ROOT_PATH AND OT_CLIBR_PATH)
        join_paths(OT_GUI_RELEASE_LIB_DIR "${OT_GUI_ROOT_PATH}" "${OT_CLIBR_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_GUI_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_GUIAPI_ROOT_PATH AND OT_CLIBD_PATH)
        join_paths(OT_GUIAPI_DEBUG_LIB_DIR "${OT_GUIAPI_ROOT_PATH}" "${OT_CLIBD_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<CONFIG:Debug>:${OT_GUIAPI_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_GUIAPI_ROOT_PATH AND OT_CLIBR_PATH)
        join_paths(OT_GUIAPI_RELEASE_LIB_DIR "${OT_GUIAPI_ROOT_PATH}" "${OT_CLIBR_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_GUIAPI_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_COMMUNICATION_ROOT_PATH AND OT_CLIBD_PATH)
        join_paths(OT_COMM_DEBUG_LIB_DIR "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBD_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<CONFIG:Debug>:${OT_COMM_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_COMMUNICATION_ROOT_PATH AND OT_CLIBR_PATH)
        join_paths(OT_COMM_RELEASE_LIB_DIR "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBR_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_COMM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_MODELAPI_ROOT_PATH AND OT_LIBD_PATH)
        join_paths(OT_MODELAPI_DEBUG_LIB_DIR "${OT_MODELAPI_ROOT_PATH}" "${OT_LIBD_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<CONFIG:Debug>:${OT_MODELAPI_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_MODELAPI_ROOT_PATH AND OT_LIBR_PATH)
        join_paths(OT_MODELAPI_RELEASE_LIB_DIR "${OT_MODELAPI_ROOT_PATH}" "${OT_LIBR_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_MODELAPI_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_MODELENTITIES_ROOT_PATH AND OT_CLIBD_PATH)
        join_paths(OT_MODELENTITIES_DEBUG_LIB_DIR "${OT_MODELENTITIES_ROOT_PATH}" "${OT_CLIBD_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<CONFIG:Debug>:${OT_MODELENTITIES_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_MODELENTITIES_ROOT_PATH AND OT_CLIBR_PATH)
        join_paths(OT_MODELENTITIES_RELEASE_LIB_DIR "${OT_MODELENTITIES_ROOT_PATH}" "${OT_CLIBR_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_MODELENTITIES_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_DATASTORAGE_ROOT_PATH AND OT_CLIBD_PATH)
        join_paths(OT_DATASTORAGE_DEBUG_LIB_DIR "${OT_DATASTORAGE_ROOT_PATH}" "${OT_CLIBD_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<CONFIG:Debug>:${OT_DATASTORAGE_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_DATASTORAGE_ROOT_PATH AND OT_CLIBR_PATH)
        join_paths(OT_DATASTORAGE_RELEASE_LIB_DIR "${OT_DATASTORAGE_ROOT_PATH}" "${OT_CLIBR_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_CADMODELENTITIES_ROOT_PATH AND OT_CLIBD_PATH)
        join_paths(OT_CADMODELENTITIES_DEBUG_LIB_DIR "${OT_CADMODELENTITIES_ROOT_PATH}" "${OT_CLIBD_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<CONFIG:Debug>:${OT_CADMODELENTITIES_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_CADMODELENTITIES_ROOT_PATH AND OT_CLIBR_PATH)
        join_paths(OT_CADMODELENTITIES_RELEASE_LIB_DIR "${OT_CADMODELENTITIES_ROOT_PATH}" "${OT_CLIBR_PATH}")
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_CADMODELENTITIES_RELEASE_LIB_DIR}>
        )
    endif()

    # cURL
    if(CURL_LIBPATHD_PATH)
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<CONFIG:Debug>:${CURL_LIBPATHD_PATH}>
        )
    endif()
    if(CURL_LIBPATHR_PATH)
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${CURL_LIBPATHR_PATH}>
        )
    endif()

    # Mongo C / C++
    if(MONGO_C_LIBPATHD_PATH)
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<CONFIG:Debug>:${MONGO_C_LIBPATHD_PATH}>
        )
    endif()
    if(MONGO_C_LIBPATHR_PATH)
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${MONGO_C_LIBPATHR_PATH}>
        )
    endif()

    if(MONGO_CXX_LIBPATHD_PATH)
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<CONFIG:Debug>:${MONGO_CXX_LIBPATHD_PATH}>
        )
    endif()
    if(MONGO_CXX_LIBPATHR_PATH)
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${MONGO_CXX_LIBPATHR_PATH}>
        )
    endif()

    # Zlib
    if(ZLIB_LIBPATHD_PATH)
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<CONFIG:Debug>:${ZLIB_LIBPATHD_PATH}>
        )
    endif()
    if(ZLIB_LIBPATHR_PATH)
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${ZLIB_LIBPATHR_PATH}>
        )
    endif()

    # OpenCascade
    if(OC_LIBPATHD_PATH)
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<CONFIG:Debug>:${OC_LIBPATHD_PATH}>
        )
    endif()
    if(OC_LIBPATHR_PATH)
        target_link_directories(OTServiceFoundation PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OC_LIBPATHR_PATH}>
        )
    endif()

    # Config-specific third-party libs
    target_link_libraries(OTServiceFoundation PRIVATE
        $<$<CONFIG:Debug>:${CURL_LIBD_NAME}>
        $<$<NOT:$<CONFIG:Debug>>:${CURL_LIBR_NAME}>
        $<$<CONFIG:Debug>:zlibd>
        $<$<NOT:$<CONFIG:Debug>>:zlib>
    )
endif()

# ------------------------------------------------------------
# Tests (optional, enabled via BUILD_TESTING)
# ------------------------------------------------------------
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
