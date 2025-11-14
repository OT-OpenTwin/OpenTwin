cmake_minimum_required(VERSION 3.20)

project(OTSystem LANGUAGES CXX)

# ------------------------------------------------------------
# C++ standard configuration
# ------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------
# Core OBJECT library with all implementation sources.
# Both the DLL (OTSystem) and tests (OTSystem_tests) use
# these object files directly.
# ------------------------------------------------------------
add_library(OTSystem_core OBJECT
    src/AppExitCodes.cpp
    src/DateTime.cpp
    src/FileSystem.cpp
    src/SystemProcess.cpp
    src/dllmain.cpp
    src/Network.cpp
    src/OperatingSystem.cpp
    src/PortManager.cpp
    src/SystemInformation.cpp
    src/UrlEncoding.cpp
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
# Optional Visual Studio source grouping (mirrors the folder layout)
# ------------------------------------------------------------
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES
    src/AppExitCodes.cpp
    src/DateTime.cpp
    src/FileSystem.cpp
    src/SystemProcess.cpp
    src/dllmain.cpp
    src/Network.cpp
    src/OperatingSystem.cpp
    src/PortManager.cpp
    src/SystemInformation.cpp
    src/UrlEncoding.cpp

    include/OTSystem/AppExitCodes.h
    include/OTSystem/DateTime.h
    include/OTSystem/Exception.h
    include/OTSystem/FileSystem.h
    include/OTSystem/Flags.h
    include/OTSystem/OTAssert.h
    include/OTSystem/SystemProcess.h
    include/OTSystem/ArchitectureInfo.h
    include/OTSystem/Network.h
    include/OTSystem/OperatingSystem.h
    include/OTSystem/PortManager.h
    include/OTSystem/RunResult.h
    include/OTSystem/SystemAPIExport.h
    include/OTSystem/SystemInformation.h
    include/OTSystem/SystemTypes.h
    include/OTSystem/UrlEncoding.h
)

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
