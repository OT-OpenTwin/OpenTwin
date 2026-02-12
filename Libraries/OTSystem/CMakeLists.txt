cmake_minimum_required(VERSION 3.20)

project(OTSystem LANGUAGES CXX)

# ------------------------------------------------------------
# C++ standard configuration
# ------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# Source and header lists (from .vcxproj)
# ============================================================

set(OTSYSTEM_HEADER_FILES
    include/OTSystem/FileSystem/AdvancedDirectoryIterator.h
    include/OTSystem/FileSystem/DirectoryIterator.h
    include/OTSystem/FileSystem/FileInformation.h
    include/OTSystem/FileSystem/FileSystem.h
    include/OTSystem/SingleSignOn/SingleSignOn_Client.h
    include/OTSystem/AppExitCodes.h
    include/OTSystem/Application.h
    include/OTSystem/ArchitectureInfo.h
    include/OTSystem/DateTime.h
    include/OTSystem/Exception.h
    include/OTSystem/Flags.h
    include/OTSystem/IgnoreRules.h
    include/OTSystem/IgnoreRules.hpp
    include/OTSystem/Network.h
    include/OTSystem/OperatingSystem.h
    include/OTSystem/OTAssert.h
    include/OTSystem/PortManager.h
    include/OTSystem/RunResult.h
    include/OTSystem/SystemAPIExport.h
    include/OTSystem/SystemInformation.h
    include/OTSystem/SystemProcess.h
    include/OTSystem/SystemTypes.h
    include/OTSystem/UrlEncoding.h
)

set(OTSYSTEM_SOURCE_FILES
    src/FileSystem/AdvancedDirectoryIterator.cpp
    src/FileSystem/DirectoryIterator.cpp
    src/FileSystem/FileInformation.cpp
    src/FileSystem/FileSystem.cpp
    src/AppExitCodes.cpp
    src/DateTime.cpp
    src/dllmain.cpp
    src/IgnoreRules.cpp
    src/Network.cpp
    src/OperatingSystem.cpp
    src/PortManager.cpp
    src/SystemInformation.cpp
    src/SystemProcess.cpp
    src/UrlEncoding.cpp
    src/SingleSignOn_Client.cpp
)

# ------------------------------------------------------------
# Core OBJECT library with all implementation sources.
# Both the DLL (OTSystem) and tests (OTSystem_tests) use
# these object files directly.
# ------------------------------------------------------------

add_library(OTSystem_core OBJECT
    ${OTSYSTEM_SOURCE_FILES}
    ${OTSYSTEM_HEADER_FILES}
)

target_include_directories(OTSystem_core
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions(OTSystem_core
    PRIVATE
        UNICODE
        _UNICODE
        _WINDOWS
        OPENTWINSYSTEM_EXPORTS
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        $<$<CONFIG:RelWithDebInfo>:NDEBUG>
        $<$<CONFIG:MinSizeRel>:NDEBUG>
)

# ------------------------------------------------------------
# MSVC compiler options mapped from Visual Studio settings
# ------------------------------------------------------------
if(MSVC)
    # Use multithreaded DLL runtime: /MD (Release) and /MDd (Debug)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    target_compile_options(OTSystem_core PRIVATE
        /W3                     # Warning level 3
        /sdl                    # SDL checks enabled
        /MP                     # Multi-processor compilation
        /wd4996                 # Disable warning 4996
        $<$<CXX_COMPILER_ID:MSVC>:/permissive- /Zc:__cplusplus>
    )
endif()

# ------------------------------------------------------------
# Shared library (DLL) – main production artifact.
# Uses all object files from OTSystem_core.
# ------------------------------------------------------------
add_library(OTSystem SHARED
    $<TARGET_OBJECTS:OTSystem_core>
)

target_include_directories(OTSystem
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(WIN32)
    target_link_libraries(OTSystem PRIVATE
        Secur32
        Advapi32
        Userenv
        ws2_32
        pdh
    )

    if(MSVC)
        target_link_options(OTSystem PRIVATE
            /SUBSYSTEM:WINDOWS
            /MANIFESTUAC:NO
            $<$<CONFIG:Release>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:RelWithDebInfo>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:MinSizeRel>:/OPT:REF /OPT:ICF>
        )
    endif()
endif()

# ------------------------------------------------------------
# GoogleTest integration (fetched automatically via FetchContent)
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
# Testing setup – tests live in the tests/ subdirectory
# ------------------------------------------------------------
include(CTest)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
