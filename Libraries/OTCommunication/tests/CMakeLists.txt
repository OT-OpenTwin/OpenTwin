# ============================================================
# OTCommunication test CMake
# Reuses OTCommunication_core object files and links against
# OTSystem, OTCore, cURL, Zlib, and system libs.
# ============================================================

# ------------------------------------------------------------
# Helper functions
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

# ------------------------------------------------------------
# Environment: Zlib / cURL / OTSystem / OTCore / RapidJSON
# ------------------------------------------------------------

# Zlib
get_env_path(ZLIB_LIB_PATH          ZLIB_LIB)
get_env_path(ZLIB_LIBPATHD_PATH     ZLIB_LIBPATHD)
get_env_path(ZLIB_LIBPATHR_PATH     ZLIB_LIBPATHR)
get_env_path(ZLIB_ROOT_PATH         ZLIB_ROOT)

# cURL (root + specific libpaths)
get_env_path(CURL_LIB_PATH          CURL_LIB)
get_env_path(CURL_LIBPATHD_PATH     CURL_LIBPATHD)
get_env_path(CURL_LIBPATHR_PATH     CURL_LIBPATHR)
get_env_libname(CURL_LIBD_NAME      CURL_LIBD)
get_env_libname(CURL_LIBR_NAME      CURL_LIBR)

# OTSystem / OTCore roots and include subdir
get_env_path(OT_SYSTEM_ROOT_PATH    OT_SYSTEM_ROOT)
get_env_path(OT_CORE_ROOT_PATH      OT_CORE_ROOT)
get_env_path(OT_INC_PATH            OT_INC)

# RapidJSON
get_env_path(R_JSON_INCD_PATH       R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH       R_JSON_INCR)

# OTSystem / OTCore library subdirs
get_env_path(OT_CLIBD_PATH          OT_CLIBD)
get_env_path(OT_CLIBR_PATH          OT_CLIBR)

# Build concrete OTSystem / OTCore library directories
join_paths(OT_SYSTEM_DEBUG_LIB_DIR   "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBD_PATH}")
join_paths(OT_SYSTEM_RELEASE_LIB_DIR "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBR_PATH}")
join_paths(OT_CORE_DEBUG_LIB_DIR     "${OT_CORE_ROOT_PATH}"   "${OT_CLIBD_PATH}")
join_paths(OT_CORE_RELEASE_LIB_DIR   "${OT_CORE_ROOT_PATH}"   "${OT_CLIBR_PATH}")

# ------------------------------------------------------------
# Collect all test sources from tests/src/
# ------------------------------------------------------------
file(GLOB_RECURSE OTCommunication_TEST_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ------------------------------------------------------------
# Test executable
# Reuse compiled object files from OTCommunication_core
# ------------------------------------------------------------
add_executable(OTCommunication_tests
    ${OTCommunication_TEST_SOURCES}
    $<TARGET_OBJECTS:OTCommunication_core>
)

# Use same export macro as the DLL
target_compile_definitions(OTCommunication_tests
    PRIVATE
        OPENTWINCOMMUNICATION_EXPORTS
)

# ------------------------------------------------------------
# Include directories for tests
# ------------------------------------------------------------
target_include_directories(OTCommunication_tests
    PRIVATE
        # Local test headers
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        # Public OTCommunication headers
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

# OTSystem / OTCore include roots
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTCommunication_tests PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTCommunication_tests PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# RapidJSON includes
if(R_JSON_INCD_PATH)
    target_include_directories(OTCommunication_tests PRIVATE "${R_JSON_INCD_PATH}")
endif()
if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(OTCommunication_tests PRIVATE "${R_JSON_INCR_PATH}")
endif()

# ------------------------------------------------------------
# Windows-specific linking (Zlib, cURL, OTSystem, OTCore)
# ------------------------------------------------------------
if(WIN32)

    # --- OTSystem library directories ---
    if(OT_SYSTEM_DEBUG_LIB_DIR)
        target_link_directories(OTCommunication_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(OTCommunication_tests PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    # --- OTCore library directories ---
    if(OT_CORE_DEBUG_LIB_DIR)
        target_link_directories(OTCommunication_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_CORE_RELEASE_LIB_DIR)
        target_link_directories(OTCommunication_tests PRIVATE
            $<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    # --- Zlib library directories ---

    # Variant 1: ZLIB_LIBPATHD / ZLIB_LIBPATHR
    if(ZLIB_LIBPATHD_PATH)
        target_link_directories(OTCommunication_tests PRIVATE
            $<$<CONFIG:Debug>:${ZLIB_LIBPATHD_PATH}>
        )
    endif()

    if(ZLIB_LIBPATHR_PATH)
        target_link_directories(OTCommunication_tests PRIVATE
            $<$<CONFIG:Release>:${ZLIB_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${ZLIB_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${ZLIB_LIBPATHR_PATH}>
        )
    endif()

    # Variant 2: $(ZLIB_LIB)\$(Configuration)\lib
    if(ZLIB_LIB_PATH)
        target_link_directories(OTCommunication_tests PRIVATE
            $<$<CONFIG:Debug>:${ZLIB_LIB_PATH}/Debug/lib>
            $<$<CONFIG:RelWithDebInfo>:${ZLIB_LIB_PATH}/Release/lib>
            $<$<CONFIG:MinSizeRel>:${ZLIB_LIB_PATH}/Release/lib>
            $<$<CONFIG:Release>:${ZLIB_LIB_PATH}/Release/lib>
        )
    endif()

    # --- cURL library directories ---
    if(CURL_LIBPATHD_PATH)
        target_link_directories(OTCommunication_tests PRIVATE
            $<$<CONFIG:Debug>:${CURL_LIBPATHD_PATH}>
        )
    endif()

    if(CURL_LIBPATHR_PATH)
        target_link_directories(OTCommunication_tests PRIVATE
            $<$<CONFIG:Release>:${CURL_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${CURL_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${CURL_LIBPATHR_PATH}>
        )
    endif()

    # Also allow direct CURL_LIB root as a generic fallback
    if(CURL_LIB_PATH)
        target_link_directories(OTCommunication_tests PRIVATE
            ${CURL_LIB_PATH}
        )
    endif()

    # --- Link libraries ---

    # Windows system libraries
    target_link_libraries(OTCommunication_tests PRIVATE
        Userenv
        Bcrypt
    )

    # OTSystem and OTCore
    target_link_libraries(OTCommunication_tests PRIVATE
        OTSystem
        OTCore
    )

    # cURL (Debug vs non-Debug)
    if(CURL_LIBD_NAME OR CURL_LIBR_NAME)
        target_link_libraries(OTCommunication_tests PRIVATE
            $<$<CONFIG:Debug>:${CURL_LIBD_NAME}>
            $<$<NOT:$<CONFIG:Debug>>:${CURL_LIBR_NAME}>
        )
    else()
        # Fallback: common names if env-vars are not set
        target_link_libraries(OTCommunication_tests PRIVATE
            $<$<CONFIG:Debug>:libCurl>
            $<$<NOT:$<CONFIG:Debug>>:libCurl>
        )
    endif()

    # Zlib (Debug vs non-Debug)
    target_link_libraries(OTCommunication_tests PRIVATE
        $<$<CONFIG:Debug>:zlibd>
        $<$<NOT:$<CONFIG:Debug>>:zlib>
    )

endif()

# ------------------------------------------------------------
# GoogleTest integration
# GTest::gtest_main is provided by the parent CMake (OTCommunication).
# ------------------------------------------------------------
target_link_libraries(OTCommunication_tests
    PRIVATE
        GTest::gtest_main
)

include(GoogleTest)

add_test(NAME OTCommunication_tests COMMAND OTCommunication_tests)

# Use the executable directory as working directory (DLL lookup etc.)
set_tests_properties(OTCommunication_tests PROPERTIES
    WORKING_DIRECTORY $<TARGET_FILE_DIR:OTCommunication_tests>
)
