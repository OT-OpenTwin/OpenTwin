# tests/CMakeLists.txt

# Collect all test source files from the tests/ folder.
file(GLOB_RECURSE OTSystem_TEST_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# NOTE: EXCLUDE_FROM_ALL ensures that the tests are NOT built as part of
# the default "all" target. They will only be built when OTSystem_tests
# is explicitly built (or when a test preset is used).
add_executable(OTSystem_tests 
    ${OTSystem_TEST_SOURCES}
    $<TARGET_OBJECTS:OTSystem_core>
 )

target_compile_definitions(OTSystem_tests
    PRIVATE
        OPENTWINSYSTEM_EXPORTS
)

target_include_directories(OTSystem_tests
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

if(WIN32)
    target_link_libraries(OTSystem_tests PRIVATE
        Advapi32
        Userenv
        ws2_32
        pdh
    )
endif()

target_link_libraries(OTSystem_tests
    PRIVATE
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(OTSystem_tests
    DISCOVERY_TIMEOUT 60
)
