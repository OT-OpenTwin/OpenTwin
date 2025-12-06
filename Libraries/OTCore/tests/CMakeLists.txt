# ------------------------------------------------------------
# Helper: read and normalize environment variables
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
# Environment: Zlib / cURL / OTSystem / RapidJSON
# ------------------------------------------------------------

# zlib root (e.g. ZLIB_LIB -> C:/3rdparty/zlib)
get_env_path(ZLIB_LIB_PATH         ZLIB_LIB)

# zlib direct library paths (like used in OTCore DLL)
get_env_path(ZLIB_LIBPATHD_PATH    ZLIB_LIBPATHD)
get_env_path(ZLIB_LIBPATHR_PATH    ZLIB_LIBPATHR)

# cURL root for tests (CURL_LIB -> dir containing libCurl.lib)
get_env_path(CURL_LIB_PATH         CURL_LIB)

# OTSystem root and include subdir
get_env_path(OT_SYSTEM_ROOT_PATH   OT_SYSTEM_ROOT)
get_env_path(OT_INC_PATH           OT_INC)

# RapidJSON include paths
get_env_path(R_JSON_INCD_PATH      R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH      R_JSON_INCR)

# OTSystem lib subdirs (e.g. $(OT_CLIBD) / $(OT_CLIBR))
get_env_path(OT_CLIBD_PATH         OT_CLIBD)
get_env_path(OT_CLIBR_PATH         OT_CLIBR)

# Build concrete OTSystem lib directories (Debug/Release)
join_paths(OT_SYSTEM_DEBUG_LIB_DIR   "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBD_PATH}")
join_paths(OT_SYSTEM_RELEASE_LIB_DIR "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBR_PATH}")

# ------------------------------------------------------------
# Collect all test sources from tests/src/
# ------------------------------------------------------------
file(GLOB_RECURSE OTCore_TEST_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ------------------------------------------------------------
# Test executable
# Reuse compiled object files from OTCore_core (same pattern as OTRandom)
# ------------------------------------------------------------
add_executable(OTCore_tests
    ${OTCore_TEST_SOURCES}
    $<TARGET_OBJECTS:OTCore_core>
)

# Export macro for OTCore
target_compile_definitions(OTCore_tests
    PRIVATE
        OPENTWINCORE_EXPORTS
)

# ------------------------------------------------------------
# Include directories for tests
# ------------------------------------------------------------
target_include_directories(OTCore_tests
    PRIVATE
        # Local test headers
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        # Public OTCore headers
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

# Add OTSystem include root so that "OTSystem/SystemTypes.h" is found
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTCore_tests PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# Add RapidJSON include paths (same pattern as main OTCore target)
if(R_JSON_INCD_PATH)
    target_include_directories(OTCore_tests PRIVATE "${R_JSON_INCD_PATH}")
endif()

if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(OTCore_tests PRIVATE "${R_JSON_INCR_PATH}")
endif()

# ------------------------------------------------------------
# Windows-specific linking (zlib, cURL, OTSystem, system libs)
# ------------------------------------------------------------
if(WIN32)

    # --- OTSystem lib directories (like in OTCore DLL) ---
    if(OT_SYSTEM_DEBUG_LIB_DIR)
        target_link_directories(OTCore_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(OTCore_tests PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    # --- zlib library directories ---
    # Variant 1: ZLIB_LIBPATHD / ZLIB_LIBPATHR (like main OTCore DLL)
    if(ZLIB_LIBPATHD_PATH)
        target_link_directories(OTCore_tests PRIVATE
            $<$<CONFIG:Debug>:${ZLIB_LIBPATHD_PATH}>
        )
    endif()

    if(ZLIB_LIBPATHR_PATH)
        target_link_directories(OTCore_tests PRIVATE
            $<$<CONFIG:Release>:${ZLIB_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${ZLIB_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${ZLIB_LIBPATHR_PATH}>
        )
    endif()

    # Variant 2 (fallback, like OTRandom_tests): $(ZLIB_LIB)\Debug\lib etc.
    if(ZLIB_LIB_PATH)
        target_link_directories(OTCore_tests PRIVATE
            $<$<CONFIG:Debug>:${ZLIB_LIB_PATH}/Debug/lib>
            $<$<CONFIG:RelWithDebInfo>:${ZLIB_LIB_PATH}/Release/lib>
            $<$<CONFIG:MinSizeRel>:${ZLIB_LIB_PATH}/Release/lib>
            $<$<CONFIG:Release>:${ZLIB_LIB_PATH}/Release/lib>
        )
    endif()

    # --- cURL library directory from CURL_LIB_PATH (if defined) ---
    if(CURL_LIB_PATH)
        target_link_directories(OTCore_tests PRIVATE
            ${CURL_LIB_PATH}
        )
    endif()

    # Always link these Windows system libraries
    target_link_libraries(OTCore_tests PRIVATE
        Userenv
        Bcrypt
    )

    # Link OTSystem (this is the library that lives in OT_SYSTEM_*_LIB_DIR)
    target_link_libraries(OTCore_tests PRIVATE
        OTSystem
    )

    # Conditional linking for cURL + zlib (mirrors OTRandom_tests)
    if(CURL_LIB_PATH)
        target_link_libraries(OTCore_tests PRIVATE
            $<$<CONFIG:Debug>:libCurl;zlibd>
            $<$<CONFIG:RelWithDebInfo>:libCurl;zlib>
            $<$<CONFIG:MinSizeRel>:libCurl;zlib>
            $<$<CONFIG:Release>:libCurl;zlib>
        )
    else()
        # Fallback: at least link zlib
        target_link_libraries(OTCore_tests PRIVATE
            $<$<CONFIG:Debug>:zlibd>
            $<$<CONFIG:RelWithDebInfo>:zlib>
            $<$<CONFIG:MinSizeRel>:zlib>
            $<$<CONFIG:Release>:zlib>
        )
    endif()

endif()

# ------------------------------------------------------------
# GoogleTest integration
# GTest::gtest_main is provided by the parent OTCore/CMakeLists.txt
# via FetchContent (if not already available in the project).
# ------------------------------------------------------------
target_link_libraries(OTCore_tests
    PRIVATE
        GTest::gtest_main
)

# ------------------------------------------------------------
# GoogleTest: link test executable against gtest_main
# (googletest itself is provided in the main OTCore CMakeLists)
# ------------------------------------------------------------
target_link_libraries(OTCore_tests
    PRIVATE
        GTest::gtest_main
)

# We do NOT use gtest_discover_tests here, because that would
# run OTCore_tests.exe during the build / configure step and
# fail with STATUS_DLL_NOT_FOUND (0xc0000135) if DLLs are not
# yet on the PATH.

# Instead we register a normal CTest test and control PATH via
# the ENVIRONMENT property. The test will only be executed
# when you actually run ctest (or invoke tests in VS).

include(GoogleTest)  # not strictly required here, but harmless

add_test(NAME OTCore_tests COMMAND OTCore_tests)

# Build up PATH so that Windows can find all required DLLs:
#  - directory of OTCore_tests.exe
#  - directory of OTCore.dll
#  - OT_ALL_DLLD / OT_ALL_DLLR (Debug / Release)
#  - ZLIB_DLLPATHD (Debug only)
#  - OPENTWIN_DEV_ROOT/Deployment
#  - original PATH
set_tests_properties(OTCore_tests PROPERTIES
    WORKING_DIRECTORY $<TARGET_FILE_DIR:OTCore_tests>
)
