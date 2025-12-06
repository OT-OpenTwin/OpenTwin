cmake_minimum_required(VERSION 3.20)

project(DataStorage LANGUAGES CXX)

# ============================================================
# Helper functions (compatible with OTCore)
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

# Small helper for Visual Studio filters (source_group)
function(assign_filter FILTER_NAME)
    foreach(FILE IN LISTS ARGN)
        set(_full_path "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}")
        if(EXISTS "${_full_path}")
            # Use the same relative path string as in add_library()
            source_group("${FILTER_NAME}" FILES "${FILE}")
        endif()
    endforeach()
endfunction()

# ============================================================
# Environment configuration
# Mirrors .vcxproj environment usage via variables
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

# Zlib
get_env_path(ZLIB_INCD_PATH        ZLIB_INCD)
get_env_path(ZLIB_INCR_PATH        ZLIB_INCR)
get_env_path(ZLIB_LIBPATHD_PATH    ZLIB_LIBPATHD)
get_env_path(ZLIB_LIBPATHR_PATH    ZLIB_LIBPATHR)
get_env_path(ZLIB_LIB_PATH         ZLIB_LIB)
get_env_path(ZLIB_ROOT_PATH        ZLIB_ROOT)

# Mongo C / C++ include and libraries
get_env_path(MONGO_CXX_INC_PATH        MONGO_CXX_INC)
get_env_path(MONGO_C_INC_PATH          MONGO_C_INC)

get_env_path(MONGO_BOOST_INCD_PATH     MONGO_BOOST_INCD)
get_env_path(MONGO_BOOST_INCR_PATH     MONGO_BOOST_INCR)

get_env_path(MONGO_CXX_LIBPATHD_PATH   MONGO_CXX_LIBPATHD)
get_env_path(MONGO_CXX_LIBPATHR_PATH   MONGO_CXX_LIBPATHR)
get_env_path(MONGO_C_LIBPATHD_PATH     MONGO_C_LIBPATHD)
get_env_path(MONGO_C_LIBPATHR_PATH     MONGO_C_LIBPATHR)

get_env_libname(MONGO_CXX_LIBS_NAME    MONGO_CXX_LIB)   # e.g. "bsoncxx.lib;mongocxx.lib"
get_env_libname(MONGO_C_LIBS_NAME      MONGO_C_LIB)     # e.g. "bson-1.0.lib;mongoc-1.0.lib"

# FTP / cURL / OpenTwinCommunication (used mainly in tests)
get_env_path(FTP_LIBCURL_ROOT_PATH     FTP_LIBCURL_ROOT)
get_env_path(FTP_CLIENT_ROOT_PATH      FTP_CLIENT_ROOT)

# ============================================================
# C++ standard
# ============================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# Source and header lists (from DataStorage.vcxproj)
# ============================================================

set(DATASTORAGE_SOURCE_FILES
    src/AdvancedQueryBuilder.cpp
    src/BsonValuesHelper.cpp
    src/DataStorageResponse.cpp
    src/ConnectionAPI.cpp
    src/DataStorageAPI.cpp
    src/DataStorageLogger.cpp
    src/dllmain.cpp
    src/DocumentAccess.cpp
    src/DocumentAPI.cpp
    src/DocumentAccessBase.cpp
    src/DocumentManager.cpp
    src/MongoDBAssertions.cpp
    src/QueryBuilder.cpp
    src/ResultDataStorageAPI.cpp
    src/stdafx.cpp
    src/SystemDataAccess.cpp
    src/ThreadLocalClientInitialiser.cpp
    src/UniqueFileName.cpp
    src/UniqueUIDGenerator.cpp
    src/UnittestDocumentAccess.cpp
    src/UnittestDocumentAccessBase.cpp
    src/UserAPI.cpp
)

set(DATASTORAGE_HEADER_FILES
    include/AdvancedQueryBuilder.h
    include/ArrayBinaryConverter.h
    include/Connection/ConnectionAPI.h
    include/DataStorageAPI.h
    include/DataStorageLogger.h
    include/GridFSFeature.h
    include/Helper/BsonValuesHelper.h
    include/Helper/QueryBuilder.h
    include/MongoDBAssertions.h
    include/Response/DataStorageResponse.h
    include/Document/DocumentAccess.h
    include/DocumentAPI.h
    include/Document/DocumentAccessBase.h
    include/Document/DocumentManager.h
    include/ResultDataStorageAPI.h
    include/stdafx.h
    include/System/SystemDataAccess.h
    include/targetver.h
    include/Helper/UniqueFileName.h
    include/ThreadLocalClientInitialiser.h
    include/UniqueUIDGenerator.h
    include/Unittest/UnittestDocumentAccess.h
    include/Unittest/UnittestDocumentAccessBase.h
    include/UserAPI.h
    include/Enums/InsertTypeEnum.h
)

# ============================================================
# Core OBJECT library (equivalent of the main .vcxproj compilation)
# ============================================================
add_library(DataStorage_core OBJECT
    ${DATASTORAGE_SOURCE_FILES}
    ${DATASTORAGE_HEADER_FILES}
)

target_compile_definitions(DataStorage_core
    PRIVATE
        UNICODE
        _UNICODE
        _WINDOWS
        _USRDLL
        DATASTORAGE_EXPORTS
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

# ============================================================
# Include directories
# Mirrors AdditionalIncludeDirectories from the .vcxproj
# ============================================================
target_include_directories(DataStorage_core
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# $(OT_SYSTEM_ROOT)\$(OT_INC)
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(DataStorage_core PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# $(OT_CORE_ROOT)\$(OT_INC)
if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(DataStorage_core PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# $(ProjectDir)$(OT_INC)
if(OT_INC_PATH)
    target_include_directories(DataStorage_core PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/${OT_INC_PATH}"
    )
endif()

# RapidJSON
if(R_JSON_INCD_PATH)
    target_include_directories(DataStorage_core PRIVATE "${R_JSON_INCD_PATH}")
endif()
if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(DataStorage_core PRIVATE "${R_JSON_INCR_PATH}")
endif()

# Mongo C++ headers (this must provide bsoncxx/... and mongocxx/...)
if(MONGO_CXX_INC_PATH)
    message(STATUS "Using MONGO_CXX_INC: ${MONGO_CXX_INC_PATH}")
    target_include_directories(DataStorage_core PRIVATE "${MONGO_CXX_INC_PATH}")
else()
    message(WARNING "MONGO_CXX_INC is not set - bsoncxx/mongocxx headers may not be found!")
endif()

# Mongo C headers
if(MONGO_C_INC_PATH)
    target_include_directories(DataStorage_core PRIVATE "${MONGO_C_INC_PATH}")
endif()

# Boost includes used by Mongo
if(MONGO_BOOST_INCD_PATH)
    target_include_directories(DataStorage_core PRIVATE "${MONGO_BOOST_INCD_PATH}")
endif()
if(MONGO_BOOST_INCR_PATH AND NOT MONGO_BOOST_INCR_PATH STREQUAL MONGO_BOOST_INCD_PATH)
    target_include_directories(DataStorage_core PRIVATE "${MONGO_BOOST_INCR_PATH}")
endif()

# Optional zlib includes (not strictly required by the vcxproj, but symmetric)
if(ZLIB_INCD_PATH)
    target_include_directories(DataStorage_core PRIVATE "${ZLIB_INCD_PATH}")
endif()
if(ZLIB_INCR_PATH AND NOT ZLIB_INCR_PATH STREQUAL ZLIB_INCD_PATH)
    target_include_directories(DataStorage_core PRIVATE "${ZLIB_INCR_PATH}")
endif()

# ============================================================
# MSVC compiler flags
# ============================================================
if(MSVC)
    # Use dynamic runtime (same pattern as OTCore)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    target_compile_options(DataStorage_core PRIVATE
        /W3
        /sdl
        /MP
        /permissive-
        /Zc:__cplusplus
        /wd4996          # matches DisableSpecificWarnings=4996
    )
endif()

# ============================================================
# Visual Studio filters (simple: group by source/header)
# ============================================================
assign_filter("Source Files"
    ${DATASTORAGE_SOURCE_FILES}
)

assign_filter("Header Files"
    ${DATASTORAGE_HEADER_FILES}
)

# ============================================================
# Shared Library (DLL) DataStorage
# Equivalent to the DynamicLibrary configuration in the vcxproj
# ============================================================
add_library(DataStorage SHARED
    $<TARGET_OBJECTS:DataStorage_core>
)

target_include_directories(DataStorage
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(WIN32)
    target_compile_definitions(DataStorage
        PRIVATE
            UNICODE
            _UNICODE
            _WINDOWS
            _USRDLL
            DATASTORAGE_EXPORTS
    )

    # OTSystem / OTCore library directories (Debug/Release)
    join_paths(OT_SYSTEM_DEBUG_LIB_DIR   "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBD_PATH}")
    join_paths(OT_SYSTEM_RELEASE_LIB_DIR "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBR_PATH}")
    join_paths(OT_CORE_DEBUG_LIB_DIR     "${OT_CORE_ROOT_PATH}"   "${OT_CLIBD_PATH}")
    join_paths(OT_CORE_RELEASE_LIB_DIR   "${OT_CORE_ROOT_PATH}"   "${OT_CLIBR_PATH}")

    if(OT_SYSTEM_DEBUG_LIB_DIR)
        target_link_directories(DataStorage PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(DataStorage PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_CORE_DEBUG_LIB_DIR)
        target_link_directories(DataStorage PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_CORE_RELEASE_LIB_DIR)
        target_link_directories(DataStorage PRIVATE
            $<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    # Mongo C++ library directories (from MONGO_CXX_LIBPATHD/R)
    if(MONGO_CXX_LIBPATHD_PATH)
        target_link_directories(DataStorage PRIVATE
            $<$<CONFIG:Debug>:${MONGO_CXX_LIBPATHD_PATH}>
        )
    endif()

    if(MONGO_CXX_LIBPATHR_PATH)
        target_link_directories(DataStorage PRIVATE
            $<$<CONFIG:Release>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${MONGO_CXX_LIBPATHR_PATH}>
        )
    endif()

    # Link libraries (equivalent to AdditionalDependencies in .vcxproj)
    target_link_libraries(DataStorage
        PRIVATE
            OTSystem
            OTCore
            # From $(MONGO_CXX_LIB) = "bsoncxx.lib;mongocxx.lib"
            ${MONGO_CXX_LIBS_NAME}
    )

    if(MSVC)
        target_link_options(DataStorage PRIVATE
            /SUBSYSTEM:WINDOWS
            /MANIFESTUAC:NO
            $<$<CONFIG:Release>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:RelWithDebInfo>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:MinSizeRel>:/OPT:REF /OPT:ICF>
        )
    endif()
endif()

# ============================================================
# GoogleTest + test integration (DataStorage_tests in tests/)
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
