cmake_minimum_required(VERSION 3.20)

project(OTGuiAPI LANGUAGES CXX)

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
get_env_path(OT_GUIAPI_ROOT_PATH        OT_GUIAPI_ROOT)
get_env_path(OT_COMMUNICATION_ROOT_PATH OT_COMMUNICATION_ROOT)
get_env_path(OT_INC_PATH                OT_INC)

# OT library subdirs
get_env_path(OT_CLIBD_PATH              OT_CLIBD)
get_env_path(OT_CLIBR_PATH              OT_CLIBR)

# RapidJSON include
get_env_path(R_JSON_INCD_PATH           R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH           R_JSON_INCR)

# cURL include and libraries
get_env_path(CURL_INCD_PATH             CURL_INCD)
get_env_path(CURL_INCR_PATH             CURL_INCR)
get_env_path(CURL_LIBPATHD_PATH         CURL_LIBPATHD)
get_env_path(CURL_LIBPATHR_PATH         CURL_LIBPATHR)
get_env_libname(CURL_LIBD_NAME          CURL_LIBD)   # e.g. "libCurl_debug.lib"
get_env_libname(CURL_LIBR_NAME          CURL_LIBR)   # e.g. "libCurl.lib"

# Optional generic CURL_LIB root (used only as fallback)
get_env_path(CURL_LIB_PATH              CURL_LIB)

# ============================================================
# C++ standard
# ============================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# Source and header lists (from .vcxproj)
# ============================================================

set(OTGUIAPI_HEADER_FILES
    include/OTGuiAPI/ButtonHandler.h
    include/OTGuiAPI/CheckBoxHandler.h
    include/OTGuiAPI/FrontendActionHandler.h
    include/OTGuiAPI/Frontend.h
    include/OTGuiAPI/GraphicsFlowItem.h
    include/OTGuiAPI/GraphicsActionHandler.h
    include/OTGuiAPI/GuiAPIManager.h
    include/OTGuiAPI/LineEditHandler.h
    include/OTGuiAPI/OTGuiAPIAPIExport.h
    include/OTGuiAPI/TableActionHandler.h
    include/OTGuiAPI/TextEditorActionHandler.h
)

set(OTGUIAPI_SOURCE_FILES
    src/ButtonHandler.cpp
    src/CheckBoxHandler.cpp
    src/dllmain.cpp
    src/FrontendActionHandler.cpp
    src/Frontend.cpp
    src/GraphicsFlowItem.cpp
    src/GraphicsActionHandler.cpp
    src/GuiAPIManager.cpp
    src/LineEditHandler.cpp
    src/TableActionHandler.cpp
    src/TextEditorActionHandler.cpp
)

# ============================================================
# Core OBJECT library (equivalent to the main .vcxproj compilation)
# ============================================================
add_library(OTGuiAPI_core OBJECT
    ${OTGUIAPI_SOURCE_FILES}
    ${OTGUIAPI_HEADER_FILES}
)

target_compile_definitions(OTGuiAPI_core
    PRIVATE
        UNICODE
        _UNICODE
        _WINDOWS
        _USRDLL
        OTGUIAPI_EXPORTS
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

# ============================================================
# Include directories
# Mirrors AdditionalIncludeDirectories from the .vcxproj
# ============================================================
target_include_directories(OTGuiAPI_core
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# $(OT_SYSTEM_ROOT)\$(OT_INC)
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTGuiAPI_core PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# $(OT_CORE_ROOT)\$(OT_INC)
if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTGuiAPI_core PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# $(OT_GUI_ROOT)\$(OT_INC)
if(OT_GUI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTGuiAPI_core PRIVATE
        "${OT_GUI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# $(OT_GUIAPI_ROOT)\$(OT_INC) - mostly redundant but mirrors vcxproj
if(OT_GUIAPI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTGuiAPI_core PRIVATE
        "${OT_GUIAPI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# $(OT_COMMUNICATION_ROOT)\$(OT_INC)
if(OT_COMMUNICATION_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTGuiAPI_core PRIVATE
        "${OT_COMMUNICATION_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# RapidJSON
if(R_JSON_INCD_PATH)
    target_include_directories(OTGuiAPI_core PRIVATE "${R_JSON_INCD_PATH}")
endif()
if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(OTGuiAPI_core PRIVATE "${R_JSON_INCR_PATH}")
endif()

# cURL includes
if(CURL_INCD_PATH)
    target_include_directories(OTGuiAPI_core PRIVATE "${CURL_INCD_PATH}")
endif()
if(CURL_INCR_PATH AND NOT CURL_INCR_PATH STREQUAL CURL_INCD_PATH)
    target_include_directories(OTGuiAPI_core PRIVATE "${CURL_INCR_PATH}")
endif()

# ============================================================
# MSVC compiler flags
# ============================================================
if(MSVC)
    # Use dynamic runtime (same pattern as your other libs)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    target_compile_options(OTGuiAPI_core PRIVATE
        /W3
        /sdl
        /MP
        /permissive-
        /Zc:__cplusplus
    )
endif()

# ============================================================
# Visual Studio filters (simple grouping)
# ============================================================
assign_filter("Source Files"
    ${OTGUIAPI_SOURCE_FILES}
)

assign_filter("Header Files"
    ${OTGUIAPI_HEADER_FILES}
)

# ============================================================
# Shared Library (DLL) OTGuiAPI
# Matches DynamicLibrary configurations from the vcxproj
# ============================================================
add_library(OTGuiAPI SHARED
    $<TARGET_OBJECTS:OTGuiAPI_core>
)

target_include_directories(OTGuiAPI
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(WIN32)
    target_compile_definitions(OTGuiAPI
        PRIVATE
            UNICODE
            _UNICODE
            _WINDOWS
            _USRDLL
            OTGUIAPI_EXPORTS
    )

    # OTSystem / OTCore / OTGui / OTCommunication library directories
    join_paths(OT_SYSTEM_DEBUG_LIB_DIR        "${OT_SYSTEM_ROOT_PATH}"        "${OT_CLIBD_PATH}")
    join_paths(OT_SYSTEM_RELEASE_LIB_DIR      "${OT_SYSTEM_ROOT_PATH}"        "${OT_CLIBR_PATH}")
    join_paths(OT_CORE_DEBUG_LIB_DIR          "${OT_CORE_ROOT_PATH}"          "${OT_CLIBD_PATH}")
    join_paths(OT_CORE_RELEASE_LIB_DIR        "${OT_CORE_ROOT_PATH}"          "${OT_CLIBR_PATH}")
    join_paths(OT_GUI_DEBUG_LIB_DIR           "${OT_GUI_ROOT_PATH}"           "${OT_CLIBD_PATH}")
    join_paths(OT_GUI_RELEASE_LIB_DIR         "${OT_GUI_ROOT_PATH}"           "${OT_CLIBR_PATH}")
    join_paths(OT_COMM_DEBUG_LIB_DIR          "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBD_PATH}")
    join_paths(OT_COMM_RELEASE_LIB_DIR        "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBR_PATH}")

    if(OT_SYSTEM_DEBUG_LIB_DIR)
        target_link_directories(OTGuiAPI PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(OTGuiAPI PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_CORE_DEBUG_LIB_DIR)
        target_link_directories(OTGuiAPI PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_CORE_RELEASE_LIB_DIR)
        target_link_directories(OTGuiAPI PRIVATE
            $<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_GUI_DEBUG_LIB_DIR)
        target_link_directories(OTGuiAPI PRIVATE
            $<$<CONFIG:Debug>:${OT_GUI_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_GUI_RELEASE_LIB_DIR)
        target_link_directories(OTGuiAPI PRIVATE
            $<$<CONFIG:Release>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_GUI_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_COMM_DEBUG_LIB_DIR)
        target_link_directories(OTGuiAPI PRIVATE
            $<$<CONFIG:Debug>:${OT_COMM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_COMM_RELEASE_LIB_DIR)
        target_link_directories(OTGuiAPI PRIVATE
            $<$<CONFIG:Release>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_COMM_RELEASE_LIB_DIR}>
        )
    endif()

    # cURL library directories
    if(CURL_LIBPATHD_PATH)
        target_link_directories(OTGuiAPI PRIVATE
            $<$<CONFIG:Debug>:${CURL_LIBPATHD_PATH}>
        )
    endif()

    if(CURL_LIBPATHR_PATH)
        target_link_directories(OTGuiAPI PRIVATE
            $<$<CONFIG:Release>:${CURL_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${CURL_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${CURL_LIBPATHR_PATH}>
        )
    endif()

    # Also allow generic CURL_LIB root as fallback
    if(CURL_LIB_PATH)
        target_link_directories(OTGuiAPI PRIVATE
            ${CURL_LIB_PATH}
        )
    endif()

    # Libraries (equivalent to AdditionalDependencies in the vcxproj)
    target_link_libraries(OTGuiAPI
        PRIVATE
            OTSystem
            OTCore
            OTGui
            OTCommunication
            Userenv
            wsock32
            Advapi32
            $<$<CONFIG:Debug>:${CURL_LIBD_NAME}>
            $<$<NOT:$<CONFIG:Debug>>:${CURL_LIBR_NAME}>
    )

    if(MSVC)
        target_link_options(OTGuiAPI PRIVATE
            /SUBSYSTEM:WINDOWS
            /MANIFESTUAC:NO
            $<$<CONFIG:Release>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:RelWithDebInfo>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:MinSizeRel>:/OPT:REF /OPT:ICF>
        )
    endif()
endif()

# ============================================================
# GoogleTest + test integration (OTGuiAPI_tests in tests/)
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
