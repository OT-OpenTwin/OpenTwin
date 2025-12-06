cmake_minimum_required(VERSION 3.20)

# ============================================================
# Helper functions (same as main CMakeLists)
# ============================================================

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

function(get_env_libname OUT_VAR ENV_NAME)
    if(DEFINED ENV{${ENV_NAME}} AND NOT "$ENV{${ENV_NAME}}" STREQUAL "")
        set(_val "$ENV{${ENV_NAME}}")
        string(REPLACE "\"" "" _val "${_val}")
        set(${OUT_VAR} "${_val}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

# ============================================================
# Environment (same variables as in main CMakeLists)
# ============================================================

get_env_path(OT_GLOBAL_INC_ROOT              OT_INC)

if(OT_GLOBAL_INC_ROOT)
    if(NOT IS_ABSOLUTE "${OT_GLOBAL_INC_ROOT}")
        set(OT_GLOBAL_INC_ROOT "${CMAKE_SOURCE_DIR}/${OT_GLOBAL_INC_ROOT}")
        file(TO_CMAKE_PATH "${OT_GLOBAL_INC_ROOT}" OT_GLOBAL_INC_ROOT)
    endif()
endif()

get_env_path(OT_DEFAULT_SERVICE_INCD_PATH    OT_DEFAULT_SERVICE_INCD)
get_env_path(OT_DEFAULT_SERVICE_INCR_PATH    OT_DEFAULT_SERVICE_INCR)
get_env_path(OT_DEFAULT_SERVICE_LIBPATHD_PATH OT_DEFAULT_SERVICE_LIBPATHD)
get_env_path(OT_DEFAULT_SERVICE_LIBPATHR_PATH OT_DEFAULT_SERVICE_LIBPATHR)

get_env_path(MONGO_C_INCD_PATH               MONGO_C_INCD)
get_env_path(MONGO_C_INCR_PATH               MONGO_C_INCR)
get_env_path(MONGO_C_LIBPATHD_PATH           MONGO_C_LIBPATHD)
get_env_path(MONGO_C_LIBPATHR_PATH           MONGO_C_LIBPATHR)

get_env_path(MONGO_CXX_LIPPATHD_PATH         MONGO_CXX_LIPPATHD)
get_env_path(MONGO_CXX_LIPPATHR_PATH         MONGO_CXX_LIPPATHR)

get_env_path(BOOST_INCD_PATH                 BOOST_INCD)
get_env_path(BOOST_INCR_PATH                 BOOST_INCR)
get_env_path(BOOST_LIBPATHD_PATH             BOOST_LIBPATHD)
get_env_path(BOOST_LIBPATHR_PATH             BOOST_LIBPATHR)

get_env_libname(MONGO_CXX_LIBD_NAME          MONGO_CXX_LIBD)
get_env_libname(MONGO_CXX_LIBR_NAME          MONGO_CXX_LIBR)
get_env_libname(OT_DEFAULT_SERVICE_LIBD_NAME OT_DEFAULT_SERVICE_LIBD)
get_env_libname(OT_DEFAULT_SERVICE_LIBR_NAME OT_DEFAULT_SERVICE_LIBR)
get_env_libname(MONGO_C_LIB_NAME             MONGO_C_LIB)

# ============================================================
# Collect test sources
# ============================================================

file(GLOB_RECURSE ResultDataAccess_TEST_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(ResultDataAccess_tests
    ${ResultDataAccess_TEST_SOURCES}
    $<TARGET_OBJECTS:ResultDataAccess_core>
)

target_compile_features(ResultDataAccess_tests PRIVATE cxx_std_17)

target_compile_definitions(ResultDataAccess_tests
    PRIVATE
        _CONSOLE
        _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>:_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS>
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:_DEBUG>
        $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:NDEBUG>
)

target_include_directories(ResultDataAccess_tests
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

# Includes wie im Hauptprojekt
if(OT_GLOBAL_INC_ROOT)
    target_include_directories(ResultDataAccess_tests PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>,$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${OT_GLOBAL_INC_ROOT}>
    )
endif()

if(OT_DEFAULT_SERVICE_INCD_PATH)
    target_include_directories(ResultDataAccess_tests PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${OT_DEFAULT_SERVICE_INCD_PATH}>
    )
endif()
if(OT_DEFAULT_SERVICE_INCR_PATH)
    target_include_directories(ResultDataAccess_tests PRIVATE
        $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${OT_DEFAULT_SERVICE_INCR_PATH}>
    )
endif()

if(MONGO_C_INCD_PATH)
    target_include_directories(ResultDataAccess_tests PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${MONGO_C_INCD_PATH}>
    )
endif()
if(MONGO_C_INCR_PATH)
    target_include_directories(ResultDataAccess_tests PRIVATE
        $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${MONGO_C_INCR_PATH}>
    )
endif()

if(BOOST_INCD_PATH)
    target_include_directories(ResultDataAccess_tests PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${BOOST_INCD_PATH}>
    )
endif()
if(BOOST_INCR_PATH)
    target_include_directories(ResultDataAccess_tests PRIVATE
        $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${BOOST_INCR_PATH}>
    )
endif()

# Linker-Einstellungen analog zur DLL
if(WIN32)
    target_link_directories(ResultDataAccess_tests PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${OT_DEFAULT_SERVICE_LIBPATHD_PATH}>
        $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${OT_DEFAULT_SERVICE_LIBPATHR_PATH}>
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${MONGO_CXX_LIPPATHD_PATH}>
        $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${MONGO_CXX_LIPPATHR_PATH}>
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${BOOST_LIBPATHD_PATH}>
        $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${BOOST_LIBPATHR_PATH}>
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${MONGO_C_LIBPATHD_PATH}>
        $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${MONGO_C_LIBPATHR_PATH}>
    )

    target_link_libraries(ResultDataAccess_tests
        PRIVATE
            # dieselben Third-Party Libs wie im Hauptziel
            $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${MONGO_CXX_LIBD_NAME}>
            $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${OT_DEFAULT_SERVICE_LIBD_NAME}>
            $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${MONGO_C_LIB_NAME}>

            $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${MONGO_CXX_LIBR_NAME}>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${OT_DEFAULT_SERVICE_LIBR_NAME}>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${MONGO_C_LIB_NAME}>
    )
endif()

# ============================================================
# Optional GoogleTest integration
# ============================================================

if(TARGET GTest::gtest_main)
    target_link_libraries(ResultDataAccess_tests
        PRIVATE
            GTest::gtest_main
    )

    include(GoogleTest)
    add_test(NAME ResultDataAccess_tests COMMAND ResultDataAccess_tests)

    set_tests_properties(ResultDataAccess_tests PROPERTIES
        WORKING_DIRECTORY $<TARGET_FILE_DIR:ResultDataAccess_tests>
    )
else()
    message(WARNING
        "GTest::gtest_main not found. "
        "ResultDataAccess_tests will be built, but not registered as GoogleTest/CTest tests."
    )
endif()
