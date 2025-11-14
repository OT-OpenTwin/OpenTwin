# tests/CMakeLists.txt

# ------------------------------------------------------------
# Helper: get and sanitize env paths
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

get_env_path(ZLIB_LIB_PATH ZLIB_LIB)
get_env_path(CURL_LIB_PATH CURL_LIB)

# ------------------------------------------------------------
# Collect all test sources recursively
# ------------------------------------------------------------
file(GLOB_RECURSE OTRandom_TEST_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Test executable reusing core object files (same compiled objects as the DLL)
add_executable(OTRandom_tests
    ${OTRandom_TEST_SOURCES}
    $<TARGET_OBJECTS:OTRandom_core>
)

target_compile_definitions(OTRandom_tests
    PRIVATE
        OPENTWINRANDOM_EXPORTS
)

target_include_directories(OTRandom_tests
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

if(WIN32)
    # Zlib link directories from ZLIB_LIB_PATH (if defined)
    if(ZLIB_LIB_PATH)
        target_link_directories(OTRandom_tests PRIVATE
            $<$<CONFIG:Debug>:${ZLIB_LIB_PATH}/Debug/lib>
            $<$<CONFIG:RelWithDebInfo>:${ZLIB_LIB_PATH}/Release/lib>
            $<$<CONFIG:MinSizeRel>:${ZLIB_LIB_PATH}/Release/lib>
            $<$<CONFIG:Release>:${ZLIB_LIB_PATH}/Release/lib>
        )
    endif()

    # cURL lib directory from CURL_LIB_PATH (if defined)
    if(CURL_LIB_PATH)
        target_link_directories(OTRandom_tests PRIVATE
            ${CURL_LIB_PATH}
        )
    endif()

    target_link_libraries(OTRandom_tests PRIVATE
        Userenv
        Bcrypt
    )

    if(CURL_LIB_PATH)
        target_link_libraries(OTRandom_tests PRIVATE
            $<$<CONFIG:Debug>:libCurl;zlibd>
            $<$<CONFIG:RelWithDebInfo>:libCurl;zlib>
            $<$<CONFIG:MinSizeRel>:libCurl;zlib>
            $<$<CONFIG:Release>:libCurl;zlib>
        )
    endif()
endif()

# Link GoogleTest main entry point
target_link_libraries(OTRandom_tests
    PRIVATE
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(OTRandom_tests
    DISCOVERY_TIMEOUT 60
)
