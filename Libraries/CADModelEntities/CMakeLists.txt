cmake_minimum_required(VERSION 3.20)

project(CADModelEntities LANGUAGES CXX)

# ============================================================
# Helper functions
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

# ============================================================
# Environment configuration (mirrors original .vcxproj)
# ============================================================

# OT roots
get_env_path(OT_SYSTEM_ROOT_PATH         OT_SYSTEM_ROOT)
get_env_path(OT_CORE_ROOT_PATH           OT_CORE_ROOT)
get_env_path(OT_GUI_ROOT_PATH            OT_GUI_ROOT)
get_env_path(OT_GUIAPI_ROOT_PATH         OT_GUIAPI_ROOT)
get_env_path(OT_COMMUNICATION_ROOT_PATH  OT_COMMUNICATION_ROOT)
get_env_path(OT_MODELENTITIES_ROOT_PATH  OT_MODELENTITIES_ROOT)
get_env_path(OT_DATASTORAGE_ROOT_PATH    OT_DATASTORAGE_ROOT)
get_env_path(OT_INC_PATH                 OT_INC)
get_env_path(OT_CLIBD_PATH               OT_CLIBD)
get_env_path(OT_CLIBR_PATH               OT_CLIBR)

# RapidJSON
get_env_path(R_JSON_INCD_PATH            R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH            R_JSON_INCR)

# Mongo C++ / C / Boost
get_env_path(MONGO_CXX_INC_PATH          MONGO_CXX_INC)
get_env_path(MONGO_C_INC_PATH            MONGO_C_INC)
get_env_path(MONGO_CXX_INCD_PATH         MONGO_CXX_INCD)
get_env_path(MONGO_C_INCD_PATH           MONGO_C_INCD)
get_env_path(MONGO_BOOST_ROOT_PATH       MONGO_BOOST_ROOT)

get_env_path(MONGO_CXX_LIBPATHD_PATH     MONGO_CXX_LIBPATHD)
get_env_path(MONGO_CXX_LIBPATHR_PATH     MONGO_CXX_LIBPATHR)
get_env_path(MONGO_C_LIBPATHD_PATH       MONGO_C_LIBPATHD)
get_env_path(MONGO_C_LIBPATHR_PATH       MONGO_C_LIBPATHR)

get_env_libname(MONGO_CXX_LIBS_NAME      MONGO_CXX_LIB)   # "bsoncxx.lib;mongocxx.lib"
get_env_libname(MONGO_C_LIBS_NAME        MONGO_C_LIB)     # "bson-1.0.lib;mongoc-1.0.lib"

# OpenCascade (OC)
get_env_path(OC_INCD_PATH                OC_INCD)
get_env_path(OC_INCR_PATH                OC_INCR)
get_env_path(OC_LIBPATHD_PATH            OC_LIBPATHD)
get_env_path(OC_LIBPATHR_PATH            OC_LIBPATHR)
get_env_libname(OC_LIBS_NAME             CFG_OC_LIBS)      # many TK*.lib etc.

# Build OT library directories (Debug/Release)
join_paths(OT_SYSTEM_DEBUG_LIB_DIR       "${OT_SYSTEM_ROOT_PATH}"        "${OT_CLIBD_PATH}")
join_paths(OT_SYSTEM_RELEASE_LIB_DIR     "${OT_SYSTEM_ROOT_PATH}"        "${OT_CLIBR_PATH}")
join_paths(OT_CORE_DEBUG_LIB_DIR         "${OT_CORE_ROOT_PATH}"          "${OT_CLIBD_PATH}")
join_paths(OT_CORE_RELEASE_LIB_DIR       "${OT_CORE_ROOT_PATH}"          "${OT_CLIBR_PATH}")
join_paths(OT_GUI_DEBUG_LIB_DIR          "${OT_GUI_ROOT_PATH}"           "${OT_CLIBD_PATH}")
join_paths(OT_GUI_RELEASE_LIB_DIR        "${OT_GUI_ROOT_PATH}"           "${OT_CLIBR_PATH}")
join_paths(OT_GUIAPI_DEBUG_LIB_DIR       "${OT_GUIAPI_ROOT_PATH}"        "${OT_CLIBD_PATH}")
join_paths(OT_GUIAPI_RELEASE_LIB_DIR     "${OT_GUIAPI_ROOT_PATH}"        "${OT_CLIBR_PATH}")
join_paths(OT_COMM_DEBUG_LIB_DIR         "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBD_PATH}")
join_paths(OT_COMM_RELEASE_LIB_DIR       "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBR_PATH}")
join_paths(OT_MODELENTITIES_DEBUG_LIB_DIR "${OT_MODELENTITIES_ROOT_PATH}" "${OT_CLIBD_PATH}")
join_paths(OT_MODELENTITIES_RELEASE_LIB_DIR "${OT_MODELENTITIES_ROOT_PATH}" "${OT_CLIBR_PATH}")
join_paths(OT_DATASTORAGE_DEBUG_LIB_DIR  "${OT_DATASTORAGE_ROOT_PATH}"   "${OT_CLIBD_PATH}")
join_paths(OT_DATASTORAGE_RELEASE_LIB_DIR "${OT_DATASTORAGE_ROOT_PATH}"  "${OT_CLIBR_PATH}")

# ============================================================
# C++ standard
# ============================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# Sources and headers
# ============================================================

set(CADMODELENTITIES_SOURCE_FILES
    src/CheckGeometry.cpp
    src/EntityBrep.cpp
    src/EntityFaceAnnotation.cpp
    src/EntityGeometry.cpp
    src/GeometryOperations.cpp
)

file(GLOB CADMODELENTITIES_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

# ============================================================
# Core OBJECT library
# ============================================================
add_library(CADModelEntities_core OBJECT
    ${CADMODELENTITIES_SOURCE_FILES}
    ${CADMODELENTITIES_HEADER_FILES}
)

target_compile_definitions(CADModelEntities_core
    PRIVATE
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

# ============================================================
# Include directories
# ============================================================
target_include_directories(CADModelEntities_core
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# OT includes (system, core, gui, guiapi, communication, modelentities, datastorage)
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(CADModelEntities_core PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(CADModelEntities_core PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_GUI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(CADModelEntities_core PRIVATE
        "${OT_GUI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_GUIAPI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(CADModelEntities_core PRIVATE
        "${OT_GUIAPI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_COMMUNICATION_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(CADModelEntities_core PRIVATE
        "${OT_COMMUNICATION_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_MODELENTITIES_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(CADModelEntities_core PRIVATE
        "${OT_MODELENTITIES_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_DATASTORAGE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(CADModelEntities_core PRIVATE
        "${OT_DATASTORAGE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# RapidJSON
if(R_JSON_INCD_PATH)
    target_include_directories(CADModelEntities_core PRIVATE "${R_JSON_INCD_PATH}")
endif()

if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(CADModelEntities_core PRIVATE "${R_JSON_INCR_PATH}")
endif()

# Mongo C++ / C / Boost
if(MONGO_CXX_INC_PATH)
    target_include_directories(CADModelEntities_core PRIVATE "${MONGO_CXX_INC_PATH}")
endif()

if(MONGO_C_INC_PATH)
    target_include_directories(CADModelEntities_core PRIVATE "${MONGO_C_INC_PATH}")
endif()

if(MONGO_CXX_INCD_PATH)
    target_include_directories(CADModelEntities_core PRIVATE "${MONGO_CXX_INCD_PATH}")
endif()

if(MONGO_C_INCD_PATH)
    target_include_directories(CADModelEntities_core PRIVATE "${MONGO_C_INCD_PATH}")
endif()

if(MONGO_BOOST_ROOT_PATH)
    target_include_directories(CADModelEntities_core PRIVATE "${MONGO_BOOST_ROOT_PATH}")
endif()

# OpenCascade includes
if(OC_INCD_PATH)
    target_include_directories(CADModelEntities_core PRIVATE "${OC_INCD_PATH}")
endif()

if(OC_INCR_PATH AND NOT OC_INCR_PATH STREQUAL OC_INCD_PATH)
    target_include_directories(CADModelEntities_core PRIVATE "${OC_INCR_PATH}")
endif()

# ============================================================
# MSVC compiler flags
# ============================================================
if(MSVC)
    # Match /MD and /MDd usage
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    target_compile_options(CADModelEntities_core PRIVATE
        /W3
        /MP
        /permissive-
        /Zc:__cplusplus
    )
endif()

# ============================================================
# Shared Library (DLL)
# ============================================================
add_library(CADModelEntities SHARED
    $<TARGET_OBJECTS:CADModelEntities_core>
)

target_include_directories(CADModelEntities
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(WIN32)
    target_compile_definitions(CADModelEntities
        PRIVATE
            _WINDOWS
            _USRDLL
    )

    # --- OT library directories ---
    if(OT_SYSTEM_DEBUG_LIB_DIR)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_CORE_DEBUG_LIB_DIR)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_CORE_RELEASE_LIB_DIR)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_GUI_DEBUG_LIB_DIR)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_GUI_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_GUI_RELEASE_LIB_DIR)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_GUI_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_GUIAPI_DEBUG_LIB_DIR)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_GUIAPI_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_GUIAPI_RELEASE_LIB_DIR)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_GUIAPI_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_GUIAPI_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_GUIAPI_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_COMM_DEBUG_LIB_DIR)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_COMM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_COMM_RELEASE_LIB_DIR)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_COMM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_MODELENTITIES_DEBUG_LIB_DIR)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_MODELENTITIES_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_MODELENTITIES_RELEASE_LIB_DIR)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_MODELENTITIES_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_MODELENTITIES_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_MODELENTITIES_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_DATASTORAGE_DEBUG_LIB_DIR)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_DATASTORAGE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_DATASTORAGE_RELEASE_LIB_DIR)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
        )
    endif()

    # --- Mongo C++ / C library directories ---
    if(MONGO_CXX_LIBPATHD_PATH)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Debug>:${MONGO_CXX_LIBPATHD_PATH}>
        )
    endif()

    if(MONGO_CXX_LIBPATHR_PATH)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Release>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${MONGO_CXX_LIBPATHR_PATH}>
        )
    endif()

    if(MONGO_C_LIBPATHD_PATH)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Debug>:${MONGO_C_LIBPATHD_PATH}>
        )
    endif()

    if(MONGO_C_LIBPATHR_PATH)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Release>:${MONGO_C_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${MONGO_C_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${MONGO_C_LIBPATHR_PATH}>
        )
    endif()

    # --- OpenCascade library directories ---
    if(OC_LIBPATHD_PATH)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OC_LIBPATHD_PATH}>
        )
    endif()

    if(OC_LIBPATHR_PATH)
        target_link_directories(CADModelEntities PRIVATE
            $<$<CONFIG:Release>:${OC_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${OC_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${OC_LIBPATHR_PATH}>
        )
    endif()

    # --- Libraries to link ---
    target_link_libraries(CADModelEntities
        PRIVATE
            OTSystem
            OTCore
            OTGui
            OTGuiAPI
            OTCommunication
            ModelEntities
            DataStorage
            ${MONGO_CXX_LIBS_NAME}
            # Mongo C libs are usually pulled transitively, but keep optional:
            ${MONGO_C_LIBS_NAME}
            ${OC_LIBS_NAME}
    )

    if(MSVC)
        target_link_options(CADModelEntities PRIVATE
            /SUBSYSTEM:WINDOWS
            /MANIFESTUAC:NO
            $<$<CONFIG:Release>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:RelWithDebInfo>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:MinSizeRel>:/OPT:REF /OPT:ICF>
        )
    endif()
endif()

# ============================================================
# GoogleTest + tests integration (CADModelEntities_tests)
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
