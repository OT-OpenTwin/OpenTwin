# ============================================================
# OTGuiAPI test CMake
# Reuses OTGuiAPI_core object files and links against
# OTSystem, OTCore, OTGui, OTCommunication, cURL, and system libs.
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
# Environment: OTSystem / OTCore / OTGui / OTCommunication / RapidJSON / cURL
# ------------------------------------------------------------

# OT roots and include subdir
get_env_path(OT_SYSTEM_ROOT_PATH        OT_SYSTEM_ROOT)
get_env_path(OT_CORE_ROOT_PATH          OT_CORE_ROOT)
get_env_path(OT_GUI_ROOT_PATH           OT_GUI_ROOT)
get_env_path(OT_GUIAPI_ROOT_PATH        OT_GUIAPI_ROOT)
get_env_path(OT_COMMUNICATION_ROOT_PATH OT_COMMUNICATION_ROOT)
get_env_path(OT_INC_PATH                OT_INC)

# RapidJSON
get_env_path(R_JSON_INCD_PATH           R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH           R_JSON_INCR)

# cURL
get_env_path(CURL_LIBPATHD_PATH         CURL_LIBPATHD)
get_env_path(CURL_LIBPATHR_PATH         CURL_LIBPATHR)
get_env_libname(CURL_LIBD_NAME          CURL_LIBD)
get_env_libname(CURL_LIBR_NAME          CURL_LIBR)
get_env_path(CURL_LIB_PATH              CURL_LIB)

# OT library subdirs
get_env_path(OT_CLIBD_PATH              OT_CLIBD)
get_env_path(OT_CLIBR_PATH              OT_CLIBR)

# Build concrete library directories
join_paths(OT_SYSTEM_DEBUG_LIB_DIR        "${OT_SYSTEM_ROOT_PATH}"        "${OT_CLIBD_PATH}")
join_paths(OT_SYSTEM_RELEASE_LIB_DIR      "${OT_SYSTEM_ROOT_PATH}"        "${OT_CLIBR_PATH}")
join_paths(OT_CORE_DEBUG_LIB_DIR          "${OT_CORE_ROOT_PATH}"          "${OT_CLIBD_PATH}")
join_paths(OT_CORE_RELEASE_LIB_DIR        "${OT_CORE_ROOT_PATH}"          "${OT_CLIBR_PATH}")
join_paths(OT_GUI_DEBUG_LIB_DIR           "${OT_GUI_ROOT_PATH}"           "${OT_CLIBD_PATH}")
join_paths(OT_GUI_RELEASE_LIB_DIR         "${OT_GUI_ROOT_PATH}"           "${OT_CLIBR_PATH}")
join_paths(OT_COMM_DEBUG_LIB_DIR          "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBD_PATH}")
join_paths(OT_COMM_RELEASE_LIB_DIR        "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBR_PATH}")

# ------------------------------------------------------------
# Collect all test sources from tests/src/
# ------------------------------------------------------------
file(GLOB_RECURSE OTGuiAPI_TEST_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ------------------------------------------------------------
# Test executable
# Reuse compiled object files from OTGuiAPI_core
# ------------------------------------------------------------
add_executable(OTGuiAPI_tests
    ${OTGuiAPI_TEST_SOURCES}
    $<TARGET_OBJECTS:OTGuiAPI_core>
)

# Use same export macro as the DLL
target_compile_definitions(OTGuiAPI_tests
    PRIVATE
        OTGUIAPI_EXPORTS
)

# ------------------------------------------------------------
# Include directories for tests
# ------------------------------------------------------------
target_include_directories(OTGuiAPI_tests
    PRIVATE
        # Local test headers
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        # Public OTGuiAPI headers
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

# OTSystem / OTCore / OTGui / OTGuiAPI / OTCommunication include roots
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTGuiAPI_tests PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTGuiAPI_tests PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_GUI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTGuiAPI_tests PRIVATE
        "${OT_GUI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_GUIAPI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTGuiAPI_tests PRIVATE
        "${OT_GUIAPI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_COMMUNICATION_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTGuiAPI_tests PRIVATE
        "${OT_COMMUNICATION_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# RapidJSON includes
if(R_JSON_INCD_PATH)
    target_include_directories(OTGuiAPI_tests PRIVATE "${R_JSON_INCD_PATH}")
endif()
if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(OTGuiAPI_tests PRIVATE "${R_JSON_INCR_PATH}")
endif()

# ------------------------------------------------------------
# Windows-specific linking (OTSystem, OTCore, OTGui, OTCommunication, cURL)
# ------------------------------------------------------------
if(WIN32)

    # --- OTSystem library directories ---
    if(OT_SYSTEM_DEBUG_LIB_DIR)
        target_link_directories(OTGuiAPI_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(OTGuiAPI_tests PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    # --- OTCore library directories ---
    if(OT_CORE_DEBUG_LIB_DIR)
        target_link_directories(OTGuiAPI_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_CORE_RELEASE_LIB_DIR)
        target_link_directories(OTGuiAPI_tests PRIVATE
            $<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    # --- OTGui library directories ---
    if(OT_GUI_DEBUG_LIB_DIR)
        target_link_directories(OTGuiAPI_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_GUI_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_GUI_RELEASE_LIB_DIR)
        target_link_directories(OTGuiAPI_tests PRIVATE
            $<$<CONFIG:Release>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_GUI_RELEASE_LIB_DIR}>
        )
    endif()

    # --- OTCommunication library directories ---
    if(OT_COMM_DEBUG_LIB_DIR)
        target_link_directories(OTGuiAPI_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_COMM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_COMM_RELEASE_LIB_DIR)
        target_link_directories(OTGuiAPI_tests PRIVATE
            $<$<CONFIG:Release>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_COMM_RELEASE_LIB_DIR}>
        )
    endif()

    # --- cURL library directories ---
    if(CURL_LIBPATHD_PATH)
        target_link_directories(OTGuiAPI_tests PRIVATE
            $<$<CONFIG:Debug>:${CURL_LIBPATHD_PATH}>
        )
    endif()

    if(CURL_LIBPATHR_PATH)
        target_link_directories(OTGuiAPI_tests PRIVATE
            $<$<CONFIG:Release>:${CURL_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${CURL_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${CURL_LIBPATHR_PATH}>
        )
    endif()

    if(CURL_LIB_PATH)
        target_link_directories(OTGuiAPI_tests PRIVATE
            ${CURL_LIB_PATH}
        )
    endif()

    # --- Link libraries ---

    # Windows system libraries
    target_link_libraries(OTGuiAPI_tests PRIVATE
        Userenv
        wsock32
        Advapi32
    )

    # OTSystem / OTCore / OTGui / OTCommunication
    target_link_libraries(OTGuiAPI_tests PRIVATE
        OTSystem
        OTCore
        OTGui
        OTCommunication
    )

    # cURL (Debug vs non-Debug)
    if(CURL_LIBD_NAME OR CURL_LIBR_NAME)
        target_link_libraries(OTGuiAPI_tests PRIVATE
            $<$<CONFIG:Debug>:${CURL_LIBD_NAME}>
            $<$<NOT:$<CONFIG:Debug>>:${CURL_LIBR_NAME}>
        )
    else()
        # Fallback: common name if env vars are not set
        target_link_libraries(OTGuiAPI_tests PRIVATE
            $<$<CONFIG:Debug>:libCurl>
            $<$<NOT:$<CONFIG:Debug>>:libCurl>
        )
    endif()

endif()

# ------------------------------------------------------------
# GoogleTest integration
# GTest::gtest_main is provided by the parent CMake (OTGuiAPI).
# ------------------------------------------------------------
target_link_libraries(OTGuiAPI_tests
    PRIVATE
        GTest::gtest_main
)

include(GoogleTest)

add_test(NAME OTGuiAPI_tests COMMAND OTGuiAPI_tests)

# Use the executable directory as working directory (DLL lookup etc.)
set_tests_properties(OTGuiAPI_tests PROPERTIES
    WORKING_DIRECTORY $<TARGET_FILE_DIR:OTGuiAPI_tests>
)
