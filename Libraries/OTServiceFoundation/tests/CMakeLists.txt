# ============================================================
# OTServiceFoundation tests
# Reuses OTServiceFoundation_core object files and links
# against OTSystem / OTCore / OTGui / OTGuiAPI /
# OTCommunication / OTModelAPI / ModelEntities / DataStorage /
# CADModelEntities / Mongo / Zlib / cURL / OpenCascade, ...
# ============================================================

# Helper functions (same as in top-level CMake)
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
# Environment: OT, Mongo, cURL, Zlib, OC, RapidJSON, etc.
# ------------------------------------------------------------

# OT roots
get_env_path(OT_SYSTEM_ROOT_PATH           OT_SYSTEM_ROOT)
get_env_path(OT_CORE_ROOT_PATH             OT_CORE_ROOT)
get_env_path(OT_GUI_ROOT_PATH              OT_GUI_ROOT)
get_env_path(OT_GUIAPI_ROOT_PATH           OT_GUIAPI_ROOT)
get_env_path(OT_COMMUNICATION_ROOT_PATH    OT_COMMUNICATION_ROOT)
get_env_path(OT_MODELENTITIES_ROOT_PATH    OT_MODELENTITIES_ROOT)
get_env_path(OT_MODELAPI_ROOT_PATH         OT_MODELAPI_ROOT)
get_env_path(OT_DATASTORAGE_ROOT_PATH      OT_DATASTORAGE_ROOT)
get_env_path(OT_CADMODELENTITIES_ROOT_PATH OT_CADMODELENTITIES_ROOT)
get_env_path(OT_RESULT_DATA_ACCESS_ROOT_PATH OT_RESULT_DATA_ACCESS_ROOT)
get_env_path(OT_ENCRYPTIONKEY_ROOT_PATH    OT_ENCRYPTIONKEY_ROOT)

get_env_path(OT_INC_PATH                   OT_INC)
get_env_path(OT_CLIBD_PATH                 OT_CLIBD)
get_env_path(OT_CLIBR_PATH                 OT_CLIBR)
get_env_path(OT_LIBD_PATH                  OT_LIBD)
get_env_path(OT_LIBR_PATH                  OT_LIBR)

# RapidJSON
get_env_path(R_JSON_INCD_PATH              R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH              R_JSON_INCR)

# cURL
get_env_path(CURL_INCD_PATH                CURL_INCD)
get_env_path(CURL_INCR_PATH                CURL_INCR)
get_env_path(CURL_LIBPATHD_PATH            CURL_LIBPATHD)
get_env_path(CURL_LIBPATHR_PATH            CURL_LIBPATHR)
get_env_libname(CURL_LIBD_NAME             CURL_LIBD)
get_env_libname(CURL_LIBR_NAME             CURL_LIBR)

# Mongo C / C++
get_env_path(MONGO_CXX_INC_PATH            MONGO_CXX_INC)
get_env_path(MONGO_C_INC_PATH              MONGO_C_INC)
get_env_path(MONGO_BOOST_ROOT_PATH         MONGO_BOOST_ROOT)

get_env_path(MONGO_C_LIBPATHD_PATH         MONGO_C_LIBPATHD)
get_env_path(MONGO_C_LIBPATHR_PATH         MONGO_C_LIBPATHR)
get_env_path(MONGO_CXX_LIBPATHD_PATH       MONGO_CXX_LIBPATHD)
get_env_path(MONGO_CXX_LIBPATHR_PATH       MONGO_CXX_LIBPATHR)

get_env_libname(MONGO_CXX_LIBS_NAME        MONGO_CXX_LIB)
get_env_libname(MONGO_C_LIBS_NAME          MONGO_C_LIB)

# Zlib
get_env_path(ZLIB_INCD_PATH                ZLIB_INCD)
get_env_path(ZLIB_INCR_PATH                ZLIB_INCR)
get_env_path(ZLIB_LIBPATHD_PATH            ZLIB_LIBPATHD)
get_env_path(ZLIB_LIBPATHR_PATH            ZLIB_LIBPATHR)

# OpenCascade
get_env_path(OC_INCD_PATH                  OC_INCD)
get_env_path(OC_INCR_PATH                  OC_INCR)
get_env_path(OC_LIBPATHD_PATH              OC_LIBPATHD)
get_env_path(OC_LIBPATHR_PATH              OC_LIBPATHR)
get_env_libname(OC_LIBS_NAME               CFG_OC_LIBS)

# base64
get_env_path(BASE64_ROOT_PATH              BASE64_ROOT)

# ------------------------------------------------------------
# Collect test sources
# ------------------------------------------------------------
file(GLOB_RECURSE OTServiceFoundation_TEST_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ------------------------------------------------------------
# Test executable reusing OTServiceFoundation_core
# ------------------------------------------------------------
add_executable(OTServiceFoundation_tests
    ${OTServiceFoundation_TEST_SOURCES}
    $<TARGET_OBJECTS:OTServiceFoundation_core>
)

target_compile_definitions(OTServiceFoundation_tests
    PRIVATE
        OPENTWINFOUNDATION_EXPORTS
)

# ------------------------------------------------------------
# Include directories for tests
# ------------------------------------------------------------
target_include_directories(OTServiceFoundation_tests
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

# OT includes
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()
if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()
if(OT_GUI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE
        "${OT_GUI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()
if(OT_GUIAPI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE
        "${OT_GUIAPI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()
if(OT_COMMUNICATION_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE
        "${OT_COMMUNICATION_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()
if(OT_MODELENTITIES_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE
        "${OT_MODELENTITIES_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()
if(OT_MODELAPI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE
        "${OT_MODELAPI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()
if(OT_DATASTORAGE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE
        "${OT_DATASTORAGE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()
if(OT_CADMODELENTITIES_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE
        "${OT_CADMODELENTITIES_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()
if(OT_RESULT_DATA_ACCESS_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE
        "${OT_RESULT_DATA_ACCESS_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()
if(OT_ENCRYPTIONKEY_ROOT_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE
        "${OT_ENCRYPTIONKEY_ROOT_PATH}"
    )
endif()

# RapidJSON
if(R_JSON_INCD_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE "${R_JSON_INCD_PATH}")
endif()
if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE "${R_JSON_INCR_PATH}")
endif()

# cURL
if(CURL_INCD_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE "${CURL_INCD_PATH}")
endif()
if(CURL_INCR_PATH AND NOT CURL_INCR_PATH STREQUAL CURL_INCD_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE "${CURL_INCR_PATH}")
endif()

# Mongo C++ / C
if(MONGO_CXX_INC_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE "${MONGO_CXX_INC_PATH}")
endif()
if(MONGO_C_INC_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE "${MONGO_C_INC_PATH}")
endif()
if(MONGO_BOOST_ROOT_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE "${MONGO_BOOST_ROOT_PATH}")
endif()

# Zlib
if(ZLIB_INCD_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE "${ZLIB_INCD_PATH}")
endif()
if(ZLIB_INCR_PATH AND NOT ZLIB_INCR_PATH STREQUAL ZLIB_INCD_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE "${ZLIB_INCR_PATH}")
endif()

# OpenCascade
if(OC_INCD_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE "${OC_INCD_PATH}")
endif()
if(OC_INCR_PATH AND NOT OC_INCR_PATH STREQUAL OC_INCD_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE "${OC_INCR_PATH}")
endif()

# base64
if(BASE64_ROOT_PATH)
    target_include_directories(OTServiceFoundation_tests PRIVATE "${BASE64_ROOT_PATH}")
endif()

# ------------------------------------------------------------
# Windows-specific linking
# ------------------------------------------------------------
if(WIN32)
    # OT libraries
    if(OT_SYSTEM_ROOT_PATH AND OT_CLIBD_PATH)
        join_paths(OT_SYSTEM_DEBUG_LIB_DIR "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBD_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_SYSTEM_ROOT_PATH AND OT_CLIBR_PATH)
        join_paths(OT_SYSTEM_RELEASE_LIB_DIR "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBR_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_CORE_ROOT_PATH AND OT_CLIBD_PATH)
        join_paths(OT_CORE_DEBUG_LIB_DIR "${OT_CORE_ROOT_PATH}" "${OT_CLIBD_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_CORE_ROOT_PATH AND OT_CLIBR_PATH)
        join_paths(OT_CORE_RELEASE_LIB_DIR "${OT_CORE_ROOT_PATH}" "${OT_CLIBR_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_GUI_ROOT_PATH AND OT_CLIBD_PATH)
        join_paths(OT_GUI_DEBUG_LIB_DIR "${OT_GUI_ROOT_PATH}" "${OT_CLIBD_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_GUI_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_GUI_ROOT_PATH AND OT_CLIBR_PATH)
        join_paths(OT_GUI_RELEASE_LIB_DIR "${OT_GUI_ROOT_PATH}" "${OT_CLIBR_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_GUI_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_GUIAPI_ROOT_PATH AND OT_CLIBD_PATH)
        join_paths(OT_GUIAPI_DEBUG_LIB_DIR "${OT_GUIAPI_ROOT_PATH}" "${OT_CLIBD_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_GUIAPI_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_GUIAPI_ROOT_PATH AND OT_CLIBR_PATH)
        join_paths(OT_GUIAPI_RELEASE_LIB_DIR "${OT_GUIAPI_ROOT_PATH}" "${OT_CLIBR_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_GUIAPI_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_COMMUNICATION_ROOT_PATH AND OT_CLIBD_PATH)
        join_paths(OT_COMM_DEBUG_LIB_DIR "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBD_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_COMM_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_COMMUNICATION_ROOT_PATH AND OT_CLIBR_PATH)
        join_paths(OT_COMM_RELEASE_LIB_DIR "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBR_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_COMM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_MODELAPI_ROOT_PATH AND OT_LIBD_PATH)
        join_paths(OT_MODELAPI_DEBUG_LIB_DIR "${OT_MODELAPI_ROOT_PATH}" "${OT_LIBD_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_MODELAPI_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_MODELAPI_ROOT_PATH AND OT_LIBR_PATH)
        join_paths(OT_MODELAPI_RELEASE_LIB_DIR "${OT_MODELAPI_ROOT_PATH}" "${OT_LIBR_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_MODELAPI_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_MODELENTITIES_ROOT_PATH AND OT_CLIBD_PATH)
        join_paths(OT_MODELENTITIES_DEBUG_LIB_DIR "${OT_MODELENTITIES_ROOT_PATH}" "${OT_CLIBD_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_MODELENTITIES_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_MODELENTITIES_ROOT_PATH AND OT_CLIBR_PATH)
        join_paths(OT_MODELENTITIES_RELEASE_LIB_DIR "${OT_MODELENTITIES_ROOT_PATH}" "${OT_CLIBR_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_MODELENTITIES_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_DATASTORAGE_ROOT_PATH AND OT_CLIBD_PATH)
        join_paths(OT_DATASTORAGE_DEBUG_LIB_DIR "${OT_DATASTORAGE_ROOT_PATH}" "${OT_CLIBD_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_DATASTORAGE_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_DATASTORAGE_ROOT_PATH AND OT_CLIBR_PATH)
        join_paths(OT_DATASTORAGE_RELEASE_LIB_DIR "${OT_DATASTORAGE_ROOT_PATH}" "${OT_CLIBR_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_CADMODELENTITIES_ROOT_PATH AND OT_CLIBD_PATH)
        join_paths(OT_CADMODELENTITIES_DEBUG_LIB_DIR "${OT_CADMODELENTITIES_ROOT_PATH}" "${OT_CLIBD_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_CADMODELENTITIES_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_CADMODELENTITIES_ROOT_PATH AND OT_CLIBR_PATH)
        join_paths(OT_CADMODELENTITIES_RELEASE_LIB_DIR "${OT_CADMODELENTITIES_ROOT_PATH}" "${OT_CLIBR_PATH}")
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OT_CADMODELENTITIES_RELEASE_LIB_DIR}>
        )
    endif()

    # cURL
    if(CURL_LIBPATHD_PATH)
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<CONFIG:Debug>:${CURL_LIBPATHD_PATH}>
        )
    endif()
    if(CURL_LIBPATHR_PATH)
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${CURL_LIBPATHR_PATH}>
        )
    endif()

    # Mongo C / C++
    if(MONGO_C_LIBPATHD_PATH)
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<CONFIG:Debug>:${MONGO_C_LIBPATHD_PATH}>
        )
    endif()
    if(MONGO_C_LIBPATHR_PATH)
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${MONGO_C_LIBPATHR_PATH}>
        )
    endif()

    if(MONGO_CXX_LIBPATHD_PATH)
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<CONFIG:Debug>:${MONGO_CXX_LIBPATHD_PATH}>
        )
    endif()
    if(MONGO_CXX_LIBPATHR_PATH)
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${MONGO_CXX_LIBPATHR_PATH}>
        )
    endif()

    # Zlib
    if(ZLIB_LIBPATHD_PATH)
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<CONFIG:Debug>:${ZLIB_LIBPATHD_PATH}>
        )
    endif()
    if(ZLIB_LIBPATHR_PATH)
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${ZLIB_LIBPATHR_PATH}>
        )
    endif()

    # OpenCascade
    if(OC_LIBPATHD_PATH)
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<CONFIG:Debug>:${OC_LIBPATHD_PATH}>
        )
    endif()
    if(OC_LIBPATHR_PATH)
        target_link_directories(OTServiceFoundation_tests PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:${OC_LIBPATHR_PATH}>
        )
    endif()

    # System & third-party libs
    target_link_libraries(OTServiceFoundation_tests PRIVATE
        OTSystem
        OTCore
        OTGui
        OTGuiAPI
        OTCommunication
        OTModelAPI
        ModelEntities
        DataStorage
        CADModelEntities

        ${MONGO_CXX_LIBS_NAME}
        ${MONGO_C_LIBS_NAME}
        ${OC_LIBS_NAME}

        $<$<CONFIG:Debug>:${CURL_LIBD_NAME}>
        $<$<NOT:$<CONFIG:Debug>>:${CURL_LIBR_NAME}>

        $<$<CONFIG:Debug>:zlibd>
        $<$<NOT:$<CONFIG:Debug>>:zlib>

        Userenv
        Bcrypt
        ws2_32
        Advapi32
    )
endif()

# ------------------------------------------------------------
# GoogleTest integration
# ------------------------------------------------------------
# Assumes that GTest::gtest_main is provided by parent project
# (e.g. via find_package(GTest CONFIG REQUIRED) in a top-level)
target_link_libraries(OTServiceFoundation_tests
    PRIVATE
        GTest::gtest_main
)

include(GoogleTest)
add_test(NAME OTServiceFoundation_tests COMMAND OTServiceFoundation_tests)

set_tests_properties(OTServiceFoundation_tests PROPERTIES
    WORKING_DIRECTORY $<TARGET_FILE_DIR:OTServiceFoundation_tests>
)
