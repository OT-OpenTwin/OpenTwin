cmake_minimum_required(VERSION 3.20)

project(OTRubberband
    VERSION 1.0.0
    LANGUAGES CXX
)

# ------------------------------------------------------------
# Helper functions
# ------------------------------------------------------------

# Reads an environment variable and converts it to a normalized CMake path
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

# Joins two paths into a fully normalized path
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
# Load relevant environment variables (OTSystem, OTCore, RapidJSON)
# ------------------------------------------------------------

get_env_path(OT_SYSTEM_ROOT_PATH OT_SYSTEM_ROOT)
get_env_path(OT_CORE_ROOT_PATH   OT_CORE_ROOT)
get_env_path(OT_INC_PATH         OT_INC)

# Convert relative OT_INC to absolute path
if(OT_INC_PATH AND NOT IS_ABSOLUTE "${OT_INC_PATH}")
    set(OT_INC_PATH "${CMAKE_SOURCE_DIR}/${OT_INC_PATH}")
endif()

get_env_path(OT_CLIBD_PATH OT_CLIBD)
get_env_path(OT_CLIBR_PATH OT_CLIBR)

# RapidJSON include paths
get_env_path(R_JSON_INCD_PATH R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH R_JSON_INCR)

# Build full include paths
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    join_paths(OT_SYSTEM_INC_DIR "${OT_SYSTEM_ROOT_PATH}" "${OT_INC_PATH}")
endif()

if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    join_paths(OT_CORE_INC_DIR "${OT_CORE_ROOT_PATH}" "${OT_INC_PATH}")
endif()

# Build full library paths
if(OT_SYSTEM_ROOT_PATH AND OT_CLIBD_PATH)
    join_paths(OT_SYSTEM_DEBUG_LIB_DIR "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBD_PATH}")
endif()

if(OT_SYSTEM_ROOT_PATH AND OT_CLIBR_PATH)
    join_paths(OT_SYSTEM_RELEASE_LIB_DIR "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBR_PATH}")
endif()

if(OT_CORE_ROOT_PATH AND OT_CLIBD_PATH)
    join_paths(OT_CORE_DEBUG_LIB_DIR "${OT_CORE_ROOT_PATH}" "${OT_CLIBD_PATH}")
endif()

if(OT_CORE_ROOT_PATH AND OT_CLIBR_PATH)
    join_paths(OT_CORE_RELEASE_LIB_DIR "${OT_CORE_ROOT_PATH}" "${OT_CLIBR_PATH}")
endif()

# ------------------------------------------------------------
# Collect sources
# ------------------------------------------------------------

set(OTRUBBERBAND_HEADERS
    include/OTRubberband/Calc/AbstractCalculationItem.h
    include/OTRubberband/Calc/AbstractOperator.h
    include/OTRubberband/Calc/OperatorAdd.h
    include/OTRubberband/Calc/OperatorDivide.h
    include/OTRubberband/Calc/OperatorMultiply.h
    include/OTRubberband/Calc/OperatorSubtract.h
    include/OTRubberband/Calc/ParserAPI.h
    include/OTRubberband/Calc/PointDistanceOperator.h
    include/OTRubberband/Calc/PointReference.h
    include/OTRubberband/Calc/VariableValue.h
    include/OTRubberband/Core/AbstractConnection.h
    include/OTRubberband/Core/AbstractPoint.h
    include/OTRubberband/Core/CircleConnection.h
    include/OTRubberband/Core/dataTypes.h
    include/OTRubberband/Core/HistoryConnection.h
    include/OTRubberband/Core/jsonMember.h
    include/OTRubberband/Core/Limit.h
    include/OTRubberband/Core/LineConnection.h
    include/OTRubberband/Core/NumericPoint.h
    include/OTRubberband/Core/Point.h
    include/OTRubberband/Core/rbeAssert.h
    include/OTRubberband/Core/RubberbandConfiguration.h
    include/OTRubberband/Core/RubberbandEngine.h
    include/OTRubberband/Core/Step.h
)

set(OTRUBBERBAND_SOURCES
    src/dllmain.cpp
    src/Calc/AbstractOperator.cpp
    src/Calc/OperatorAdd.cpp
    src/Calc/OperatorDivide.cpp
    src/Calc/OperatorMultiply.cpp
    src/Calc/OperatorSubtract.cpp
    src/Calc/ParserAPI.cpp
    src/Calc/PointDistanceOperator.cpp
    src/Calc/PointReference.cpp
    src/Core/AbstractConnection.cpp
    src/Core/AbstractPoint.cpp
    src/Core/CircleConnection.cpp
    src/Core/HistoryConnection.cpp
    src/Core/Limit.cpp
    src/Core/LineConnection.cpp
    src/Core/NumericPoint.cpp
    src/Core/Point.cpp
    src/Core/RubberbandConfiguration.cpp
    src/Core/RubberbandEngine.cpp
    src/Core/Step.cpp
)

# ------------------------------------------------------------
# Core object library (reused by DLL + tests)
# ------------------------------------------------------------
add_library(OTRubberband_core OBJECT
    ${OTRUBBERBAND_SOURCES}
    ${OTRUBBERBAND_HEADERS}
)

target_compile_features(OTRubberband_core PUBLIC cxx_std_17)

target_include_directories(OTRubberband_core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(OT_SYSTEM_INC_DIR)
    target_include_directories(OTRubberband_core PUBLIC "${OT_SYSTEM_INC_DIR}")
endif()

if(OT_CORE_INC_DIR)
    target_include_directories(OTRubberband_core PUBLIC "${OT_CORE_INC_DIR}")
endif()

if(R_JSON_INCD_PATH)
    target_include_directories(OTRubberband_core PUBLIC "${R_JSON_INCD_PATH}")
endif()

if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(OTRubberband_core PUBLIC "${R_JSON_INCR_PATH}")
endif()

# ------------------------------------------------------------
# Shared library (DLL) — final exported library
# ------------------------------------------------------------
add_library(OTRubberband SHARED
    $<TARGET_OBJECTS:OTRubberband_core>
)

target_compile_features(OTRubberband PUBLIC cxx_std_17)

# Export symbol macro
target_compile_definitions(OTRubberband_core
    PRIVATE
        OT_RUBBERBAND_EXPORTS
)

target_include_directories(OTRubberband
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(OT_SYSTEM_INC_DIR)
    target_include_directories(OTRubberband PUBLIC "${OT_SYSTEM_INC_DIR}")
endif()

if(OT_CORE_INC_DIR)
    target_include_directories(OTRubberband PUBLIC "${OT_CORE_INC_DIR}")
endif()

if(R_JSON_INCD_PATH)
    target_include_directories(OTRubberband PUBLIC "${R_JSON_INCD_PATH}")
endif()

if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(OTRubberband PUBLIC "${R_JSON_INCR_PATH}")
endif()

# ------------------------------------------------------------
# Link OTSystem + OTCore depending on configuration
# ------------------------------------------------------------
if(WIN32)
    if(OT_SYSTEM_DEBUG_LIB_DIR)
        target_link_directories(OTRubberband PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(OTRubberband PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_CORE_DEBUG_LIB_DIR)
        target_link_directories(OTRubberband PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_CORE_RELEASE_LIB_DIR)
        target_link_directories(OTRubberband PRIVATE
            $<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    target_link_libraries(OTRubberband
        PRIVATE
            OTSystem
            OTCore
    )
endif()

# ------------------------------------------------------------
# Optional tests
# ------------------------------------------------------------
option(BUILD_TESTING "Build OTRubberband tests" ON)

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
