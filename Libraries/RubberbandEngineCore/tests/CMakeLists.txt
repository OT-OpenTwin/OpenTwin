cmake_minimum_required(VERSION 3.20)

include(FetchContent)

# ------------------------------------------------------------
# Helper: env path reader
# ------------------------------------------------------------
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
# Locate or fetch GoogleTest
# ------------------------------------------------------------
set(_have_gtest OFF)
if(TARGET GTest::gtest_main)
    set(_have_gtest ON)
endif()

if(NOT _have_gtest)
    message(STATUS "GTest::gtest_main not found – fetching GoogleTest")

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# ------------------------------------------------------------
# Collect test sources
# ------------------------------------------------------------
file(GLOB_RECURSE RubberbandEngineCore_TEST_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

if(RubberbandEngineCore_TEST_SOURCES STREQUAL "")
    message(STATUS "No test sources found – skipping test target.")
    return()
endif()

# ------------------------------------------------------------
# Test executable
# ------------------------------------------------------------
add_executable(RubberbandEngineCore_tests
    ${RubberbandEngineCore_TEST_SOURCES}
    $<TARGET_OBJECTS:RubberbandEngineCore_core>
)

target_compile_features(RubberbandEngineCore_tests PRIVATE cxx_std_17)

# Export definitions same as DLL
target_compile_definitions(RubberbandEngineCore_tests PRIVATE RUBBERBANDENGINECORE_EXPORTS)

target_include_directories(RubberbandEngineCore_tests
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

# ------------------------------------------------------------
# Optional OT include paths
# ------------------------------------------------------------
get_env_path(OT_SYSTEM_ROOT_PATH OT_SYSTEM_ROOT)
get_env_path(OT_CORE_ROOT_PATH   OT_CORE_ROOT)
get_env_path(OT_INC_PATH         OT_INC)

if(OT_INC_PATH AND NOT IS_ABSOLUTE "${OT_INC_PATH}")
    set(OT_INC_PATH "${CMAKE_SOURCE_DIR}/${OT_INC_PATH}")
endif()

if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    join_paths(OT_SYSTEM_INC_DIR "${OT_SYSTEM_ROOT_PATH}" "${OT_INC_PATH}")
endif()

if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    join_paths(OT_CORE_INC_DIR "${OT_CORE_ROOT_PATH}" "${OT_INC_PATH}")
endif()

if(OT_SYSTEM_INC_DIR)
    target_include_directories(RubberbandEngineCore_tests PRIVATE "${OT_SYSTEM_INC_DIR}")
endif()

if(OT_CORE_INC_DIR)
    target_include_directories(RubberbandEngineCore_tests PRIVATE "${OT_CORE_INC_DIR}")
endif()

# ------------------------------------------------------------
# Linking
# ------------------------------------------------------------

target_link_libraries(RubberbandEngineCore_tests
    PRIVATE
        RubberbandEngineCore
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(RubberbandEngineCore_tests
    WORKING_DIRECTORY $<TARGET_FILE_DIR:RubberbandEngineCore_tests>
)
