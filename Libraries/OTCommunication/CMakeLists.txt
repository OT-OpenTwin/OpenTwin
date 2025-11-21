cmake_minimum_required(VERSION 3.20)

project(OTCommunication LANGUAGES CXX)

# ============================================================
# Helper functions (same style as OTCore / DataStorage)
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
get_env_path(OT_SYSTEM_ROOT_PATH   OT_SYSTEM_ROOT)
get_env_path(OT_CORE_ROOT_PATH     OT_CORE_ROOT)
get_env_path(OT_INC_PATH           OT_INC)

# OT library subdirs
get_env_path(OT_CLIBD_PATH         OT_CLIBD)
get_env_path(OT_CLIBR_PATH         OT_CLIBR)
get_env_path(OT_LIBD_PATH          OT_LIBD)
get_env_path(OT_LIBR_PATH          OT_LIBR)

# RapidJSON include
get_env_path(R_JSON_INCD_PATH      R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH      R_JSON_INCR)

# cURL include and libraries
get_env_path(CURL_INCD_PATH        CURL_INCD)
get_env_path(CURL_INCR_PATH        CURL_INCR)
get_env_path(CURL_LIBPATHD_PATH    CURL_LIBPATHD)
get_env_path(CURL_LIBPATHR_PATH    CURL_LIBPATHR)
get_env_libname(CURL_LIBD_NAME     CURL_LIBD)   # e.g. "libCurl_debug.lib" or similar
get_env_libname(CURL_LIBR_NAME     CURL_LIBR)   # e.g. "libCurl.lib"

# Base64 / thirdparty root(s)
get_env_path(BASE64_ROOT_PATH      BASE64_ROOT)
get_env_path(THIRDPARTY_ROOT_PATH  OPENTWIN_THIRDPARTY_ROOT)

# Zlib (only used in DebugTest/ReleaseTest configs in the original, but harmless to expose here)
get_env_path(ZLIB_INCD_PATH        ZLIB_INCD)
get_env_path(ZLIB_INCR_PATH        ZLIB_INCR)
get_env_path(ZLIB_LIBPATHD_PATH    ZLIB_LIBPATHD)
get_env_path(ZLIB_LIBPATHR_PATH    ZLIB_LIBPATHR)
get_env_path(ZLIB_LIB_PATH         ZLIB_LIB)
get_env_path(ZLIB_ROOT_PATH        ZLIB_ROOT)

# ============================================================
# C++ standard
# ============================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# Third-party base64.c / base64.h (from vcxproj)
# ============================================================
set(OTCOMM_THIRDPARTY_SOURCE "")
set(OTCOMM_THIRDPARTY_HEADER "")

# Preferred: OPENTWIN_THIRDPARTY_ROOT/base64/base64.*
if(THIRDPARTY_ROOT_PATH)
    set(OTCOMM_THIRDPARTY_SOURCE "${THIRDPARTY_ROOT_PATH}/base64/base64.c")
    set(OTCOMM_THIRDPARTY_HEADER "${THIRDPARTY_ROOT_PATH}/base64/base64.h")
elseif(BASE64_ROOT_PATH)
    # Alternative: BASE64_ROOT points directly to base64.[ch] or its directory
    if(EXISTS "${BASE64_ROOT_PATH}/base64.c")
        set(OTCOMM_THIRDPARTY_SOURCE "${BASE64_ROOT_PATH}/base64.c")
        set(OTCOMM_THIRDPARTY_HEADER "${BASE64_ROOT_PATH}/base64.h")
    else()
        # If BASE64_ROOT is a parent dir, assume BASE64_ROOT/base64/base64.c
        set(OTCOMM_THIRDPARTY_SOURCE "${BASE64_ROOT_PATH}/base64/base64.c")
        set(OTCOMM_THIRDPARTY_HEADER "${BASE64_ROOT_PATH}/base64/base64.h")
    endif()
else()
    # Fallback: original relative path from vcxproj (may or may not work, depending on tree layout)
    set(OTCOMM_THIRDPARTY_SOURCE "../../ThirdParty/base64/base64.c")
    set(OTCOMM_THIRDPARTY_HEADER "../../ThirdParty/base64/base64.h")
endif()

# ============================================================
# Source and header lists (from .vcxproj)
# ============================================================
set(OTCOMM_SOURCE_FILES
    src/ActionDispatcher.cpp
    src/ActionDispatcherBase.cpp
    src/ActionDispatchProfiler.cpp
    src/ActionHandleConnector.cpp
    src/ActionHandleConnectorContainer.cpp
    src/CommunicationTypes.cpp
    src/dllmain.cpp
    src/GDSDebugInfo.cpp
    src/GSSDebugInfo.cpp
    src/IpConverter.cpp
    src/LDSDebugInfo.cpp
    src/LSSDebugInfo.cpp
    src/Msg.cpp
    src/RelayedMessageHandler.cpp
    src/ServiceInitData.cpp
    src/ServiceLogNotifier.cpp
    src/ServiceRunData.cpp
)

set(OTCOMM_HEADER_FILES
    include/OTCommunication/ActionDispatcher.h
    include/OTCommunication/ActionDispatcherBase.h
    include/OTCommunication/ActionDispatchProfiler.h
    include/OTCommunication/ActionHandleConnectorContainer.h
    include/OTCommunication/ActionHandleConnectorManager.hpp
    include/OTCommunication/ActionHandleConnector.h
    include/OTCommunication/ActionHandleConnectorManager.h
    include/OTCommunication/ActionHandleInlineConnector.h
    include/OTCommunication/ActionHandleInlineConnector.hpp
    include/OTCommunication/ActionHandler.h
    include/OTCommunication/ActionHandler.hpp
    include/OTCommunication/ActionTypes.h
    include/OTCommunication/CommunicationAPIExport.h
    include/OTCommunication/CommunicationTypes.h
    include/OTCommunication/GDSDebugInfo.h
    include/OTCommunication/GSSDebugInfo.h
    include/OTCommunication/IpConverter.h
    include/OTCommunication/LDSDebugInfo.h
    include/OTCommunication/LSSDebugInfo.h
    include/OTCommunication/Msg.h
    include/OTCommunication/RelayedMessageHandler.h
    include/OTCommunication/ServiceInitData.h
    include/OTCommunication/ServiceLogNotifier.h
    include/OTCommunication/ServiceRunData.h
)

# ============================================================
# Core OBJECT library (equivalent to the main .vcxproj compilation)
# ============================================================
add_library(OTCommunication_core OBJECT
    ${OTCOMM_SOURCE_FILES}
    ${OTCOMM_HEADER_FILES}
    ${OTCOMM_THIRDPARTY_SOURCE}
    ${OTCOMM_THIRDPARTY_HEADER}
)

target_compile_definitions(OTCommunication_core
    PRIVATE
        UNICODE
        _UNICODE
        _WINDOWS
        _USRDLL
        OPENTWINCOMMUNICATION_EXPORTS
        _CRT_SECURE_NO_WARNINGS
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

# ============================================================
# Include directories
# Mirrors AdditionalIncludeDirectories from the .vcxproj
# ============================================================
target_include_directories(OTCommunication_core
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# $(OT_SYSTEM_ROOT)\$(OT_INC)
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTCommunication_core PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# $(OT_CORE_ROOT)\$(OT_INC)
if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTCommunication_core PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# $(ProjectDir)include is already added above; no need for OT_INC again here.

# RapidJSON
if(R_JSON_INCD_PATH)
    target_include_directories(OTCommunication_core PRIVATE "${R_JSON_INCD_PATH}")
endif()
if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(OTCommunication_core PRIVATE "${R_JSON_INCR_PATH}")
endif()

# cURL includes
if(CURL_INCD_PATH)
    target_include_directories(OTCommunication_core PRIVATE "${CURL_INCD_PATH}")
endif()
if(CURL_INCR_PATH AND NOT CURL_INCR_PATH STREQUAL CURL_INCD_PATH)
    target_include_directories(OTCommunication_core PRIVATE "${CURL_INCR_PATH}")
endif()

# base64 include directory (from third-party file location)
if(OTCOMM_THIRDPARTY_HEADER)
    get_filename_component(_thirdparty_dir "${OTCOMM_THIRDPARTY_HEADER}" DIRECTORY)
    target_include_directories(OTCommunication_core PRIVATE "${_thirdparty_dir}")
endif()

# Optional: zlib includes (used in Test configs in original vcxproj)
if(ZLIB_INCD_PATH)
    target_include_directories(OTCommunication_core PRIVATE "${ZLIB_INCD_PATH}")
endif()
if(ZLIB_INCR_PATH AND NOT ZLIB_INCR_PATH STREQUAL ZLIB_INCD_PATH)
    target_include_directories(OTCommunication_core PRIVATE "${ZLIB_INCR_PATH}")
endif()

# ============================================================
# MSVC compiler flags
# ============================================================
if(MSVC)
    # Use dynamic runtime (same pattern as OTCore / DataStorage)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    target_compile_options(OTCommunication_core PRIVATE
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
    ${OTCOMM_SOURCE_FILES}
)

assign_filter("Header Files"
    ${OTCOMM_HEADER_FILES}
)

assign_filter("ThirdParty"
    ${OTCOMM_THIRDPARTY_SOURCE}
    ${OTCOMM_THIRDPARTY_HEADER}
)

# ============================================================
# Shared Library (DLL) OTCommunication
# Matches DynamicLibrary configurations from the vcxproj
# ============================================================
add_library(OTCommunication SHARED
    $<TARGET_OBJECTS:OTCommunication_core>
)

target_include_directories(OTCommunication
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(WIN32)
    target_compile_definitions(OTCommunication
        PRIVATE
            UNICODE
            _UNICODE
            _WINDOWS
            _USRDLL
            OPENTWINCOMMUNICATION_EXPORTS
    )

    # OTSystem / OTCore library directories
    join_paths(OT_SYSTEM_DEBUG_LIB_DIR   "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBD_PATH}")
    join_paths(OT_SYSTEM_RELEASE_LIB_DIR "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBR_PATH}")
    join_paths(OT_CORE_DEBUG_LIB_DIR     "${OT_CORE_ROOT_PATH}"   "${OT_CLIBD_PATH}")
    join_paths(OT_CORE_RELEASE_LIB_DIR   "${OT_CORE_ROOT_PATH}"   "${OT_CLIBR_PATH}")

    if(OT_SYSTEM_DEBUG_LIB_DIR)
        target_link_directories(OTCommunication PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(OTCommunication PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_CORE_DEBUG_LIB_DIR)
        target_link_directories(OTCommunication PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_CORE_RELEASE_LIB_DIR)
        target_link_directories(OTCommunication PRIVATE
            $<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    # cURL library directories (CURL_LIBPATHD / CURL_LIBPATHR)
    if(CURL_LIBPATHD_PATH)
        target_link_directories(OTCommunication PRIVATE
            $<$<CONFIG:Debug>:${CURL_LIBPATHD_PATH}>
        )
    endif()

    if(CURL_LIBPATHR_PATH)
        target_link_directories(OTCommunication PRIVATE
            $<$<CONFIG:Release>:${CURL_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${CURL_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${CURL_LIBPATHR_PATH}>
        )
    endif()

    # Libraries (equivalent to AdditionalDependencies in the vcxproj)
    target_link_libraries(OTCommunication
        PRIVATE
            OTSystem
            OTCore
            $<$<CONFIG:Debug>:${CURL_LIBD_NAME}>
            $<$<NOT:$<CONFIG:Debug>>:${CURL_LIBR_NAME}>
    )

    if(MSVC)
        target_link_options(OTCommunication PRIVATE
            /SUBSYSTEM:WINDOWS
            /MANIFESTUAC:NO
            $<$<CONFIG:Release>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:RelWithDebInfo>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:MinSizeRel>:/OPT:REF /OPT:ICF>
        )
    endif()
endif()

# ============================================================
# GoogleTest + test integration (OTCommunication_tests in tests/)
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
