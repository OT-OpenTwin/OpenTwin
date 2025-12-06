# ============================================================
# BlockEntities test CMake
# Reuses BlockEntities_core object files and links against
# OT libraries, Mongo and GTest.
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
# Environment: OT / Mongo / RapidJSON / ResultCollectionAccess
# ------------------------------------------------------------

# OT roots
get_env_path(OT_SYSTEM_ROOT_PATH            OT_SYSTEM_ROOT)
get_env_path(OT_CORE_ROOT_PATH              OT_CORE_ROOT)
get_env_path(OT_GUI_ROOT_PATH               OT_GUI_ROOT)
get_env_path(OT_GUIAPI_ROOT_PATH            OT_GUIAPI_ROOT)
get_env_path(OT_COMMUNICATION_ROOT_PATH     OT_COMMUNICATION_ROOT)
get_env_path(OT_MODELENTITIES_ROOT_PATH     OT_MODELENTITIES_ROOT)
get_env_path(OT_DATASTORAGE_ROOT_PATH       OT_DATASTORAGE_ROOT)
get_env_path(OT_RESULT_COLLECTION_ROOT_PATH OT_RESULT_COLLECTION_ACCESS_ROOT)
get_env_path(OT_INC_PATH                    OT_INC)
get_env_path(OT_CLIBD_PATH                  OT_CLIBD)
get_env_path(OT_CLIBR_PATH                  OT_CLIBR)

# RapidJSON
get_env_path(R_JSON_INCD_PATH               R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH               R_JSON_INCR)

# Mongo C++ / C / Boost
get_env_path(MONGO_CXX_INC_PATH             MONGO_CXX_INC)
get_env_path(MONGO_C_INC_PATH               MONGO_C_INC)
get_env_path(MONGO_CXX_INCD_PATH            MONGO_CXX_INCD)
get_env_path(MONGO_C_INCD_PATH              MONGO_C_INCD)
get_env_path(MONGO_BOOST_ROOT_PATH          MONGO_BOOST_ROOT)

get_env_path(MONGO_CXX_LIBPATHD_PATH        MONGO_CXX_LIBPATHD)
get_env_path(MONGO_CXX_LIBPATHR_PATH        MONGO_CXX_LIBPATHR)
get_env_path(MONGO_C_LIBPATHD_PATH          MONGO_C_LIBPATHD)
get_env_path(MONGO_C_LIBPATHR_PATH          MONGO_C_LIBPATHR)

get_env_libname(MONGO_CXX_LIBS_NAME         MONGO_CXX_LIB)
get_env_libname(MONGO_C_LIBS_NAME           MONGO_C_LIB)

# OT library directories
join_paths(OT_SYSTEM_DEBUG_LIB_DIR          "${OT_SYSTEM_ROOT_PATH}"        "${OT_CLIBD_PATH}")
join_paths(OT_SYSTEM_RELEASE_LIB_DIR        "${OT_SYSTEM_ROOT_PATH}"        "${OT_CLIBR_PATH}")
join_paths(OT_CORE_DEBUG_LIB_DIR            "${OT_CORE_ROOT_PATH}"          "${OT_CLIBD_PATH}")
join_paths(OT_CORE_RELEASE_LIB_DIR          "${OT_CORE_ROOT_PATH}"          "${OT_CLIBR_PATH}")
join_paths(OT_GUI_DEBUG_LIB_DIR             "${OT_GUI_ROOT_PATH}"           "${OT_CLIBD_PATH}")
join_paths(OT_GUI_RELEASE_LIB_DIR           "${OT_GUI_ROOT_PATH}"           "${OT_CLIBR_PATH}")
join_paths(OT_GUIAPI_DEBUG_LIB_DIR          "${OT_GUIAPI_ROOT_PATH}"        "${OT_CLIBD_PATH}")
join_paths(OT_GUIAPI_RELEASE_LIB_DIR        "${OT_GUIAPI_ROOT_PATH}"        "${OT_CLIBR_PATH}")
join_paths(OT_COMM_DEBUG_LIB_DIR            "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBD_PATH}")
join_paths(OT_COMM_RELEASE_LIB_DIR          "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBR_PATH}")
join_paths(OT_MODELENTITIES_DEBUG_LIB_DIR   "${OT_MODELENTITIES_ROOT_PATH}" "${OT_CLIBD_PATH}")
join_paths(OT_MODELENTITIES_RELEASE_LIB_DIR "${OT_MODELENTITIES_ROOT_PATH}" "${OT_CLIBR_PATH}")
join_paths(OT_DATASTORAGE_DEBUG_LIB_DIR     "${OT_DATASTORAGE_ROOT_PATH}"   "${OT_CLIBD_PATH}")
join_paths(OT_DATASTORAGE_RELEASE_LIB_DIR   "${OT_DATASTORAGE_ROOT_PATH}"   "${OT_CLIBR_PATH}")

# ------------------------------------------------------------
# Collect test sources
# ------------------------------------------------------------
file(GLOB_RECURSE BlockEntities_TEST_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ------------------------------------------------------------
# Test executable - reuse BlockEntities_core
# ------------------------------------------------------------
add_executable(BlockEntities_tests
    ${BlockEntities_TEST_SOURCES}
    $<TARGET_OBJECTS:BlockEntities_core>
)

target_compile_definitions(BlockEntities_tests
    PRIVATE
        _CONSOLE
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

# ------------------------------------------------------------
# Include directories for tests
# ------------------------------------------------------------
target_include_directories(BlockEntities_tests
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

# OT includes
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(BlockEntities_tests PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(BlockEntities_tests PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_GUI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(BlockEntities_tests PRIVATE
        "${OT_GUI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_GUIAPI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(BlockEntities_tests PRIVATE
        "${OT_GUIAPI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_COMMUNICATION_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(BlockEntities_tests PRIVATE
        "${OT_COMMUNICATION_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_MODELENTITIES_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(BlockEntities_tests PRIVATE
        "${OT_MODELENTITIES_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_DATASTORAGE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(BlockEntities_tests PRIVATE
        "${OT_DATASTORAGE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# Local OT-style include for BlockEntities itself
if(OT_INC_PATH)
    target_include_directories(BlockEntities_tests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../${OT_INC_PATH}"
    )
endif()

# ResultCollectionAccess includes (used in DebugTest/ReleaseTest)
if(OT_RESULT_COLLECTION_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(BlockEntities_tests PRIVATE
        "${OT_RESULT_COLLECTION_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# RapidJSON
if(R_JSON_INCD_PATH)
    target_include_directories(BlockEntities_tests PRIVATE "${R_JSON_INCD_PATH}")
endif()

if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(BlockEntities_tests PRIVATE "${R_JSON_INCR_PATH}")
endif()

# Mongo includes
if(MONGO_CXX_INC_PATH)
    target_include_directories(BlockEntities_tests PRIVATE "${MONGO_CXX_INC_PATH}")
endif()

if(MONGO_C_INC_PATH)
    target_include_directories(BlockEntities_tests PRIVATE "${MONGO_C_INC_PATH}")
endif()

if(MONGO_CXX_INCD_PATH)
    target_include_directories(BlockEntities_tests PRIVATE "${MONGO_CXX_INCD_PATH}")
endif()

if(MONGO_C_INCD_PATH)
    target_include_directories(BlockEntities_tests PRIVATE "${MONGO_C_INCD_PATH}")
endif()

if(MONGO_BOOST_ROOT_PATH)
    target_include_directories(BlockEntities_tests PRIVATE "${MONGO_BOOST_ROOT_PATH}")
endif()

# ------------------------------------------------------------
# Linker settings
# ------------------------------------------------------------
if(WIN32)
    # OT lib directories
    if(OT_SYSTEM_DEBUG_LIB_DIR)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_CORE_DEBUG_LIB_DIR)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_CORE_RELEASE_LIB_DIR)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_GUI_DEBUG_LIB_DIR)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_GUI_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_GUI_RELEASE_LIB_DIR)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Release>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_GUI_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_GUIAPI_DEBUG_LIB_DIR)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_GUIAPI_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_GUIAPI_RELEASE_LIB_DIR)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Release>:${OT_GUIAPI_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_GUIAPI_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_GUIAPI_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_COMM_DEBUG_LIB_DIR)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_COMM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_COMM_RELEASE_LIB_DIR)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Release>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_COMM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_MODELENTITIES_DEBUG_LIB_DIR)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_MODELENTITIES_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_MODELENTITIES_RELEASE_LIB_DIR)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Release>:${OT_MODELENTITIES_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_MODELENTITIES_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_MODELENTITIES_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_DATASTORAGE_DEBUG_LIB_DIR)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_DATASTORAGE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_DATASTORAGE_RELEASE_LIB_DIR)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Release>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
        )
    endif()

    # Mongo lib dirs
    if(MONGO_CXX_LIBPATHD_PATH)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Debug>:${MONGO_CXX_LIBPATHD_PATH}>
        )
    endif()

    if(MONGO_CXX_LIBPATHR_PATH)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Release>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${MONGO_CXX_LIBPATHR_PATH}>
        )
    endif()

    if(MONGO_C_LIBPATHD_PATH)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Debug>:${MONGO_C_LIBPATHD_PATH}>
        )
    endif()

    if(MONGO_C_LIBPATHR_PATH)
        target_link_directories(BlockEntities_tests PRIVATE
            $<$<CONFIG:Release>:${MONGO_C_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${MONGO_C_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${MONGO_C_LIBPATHR_PATH}>
        )
    endif()

    target_link_libraries(BlockEntities_tests
        PRIVATE
            OTSystem
            OTCore
            OTGui
            OTGuiAPI
            OTCommunication
            ModelEntities
            DataStorage
            ${MONGO_CXX_LIBS_NAME}
            ${MONGO_C_LIBS_NAME}
    )

    if(MSVC)
        target_link_options(BlockEntities_tests PRIVATE
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

# ------------------------------------------------------------
# GoogleTest integration
# ------------------------------------------------------------
target_link_libraries(BlockEntities_tests
    PRIVATE
        GTest::gtest_main
)

include(GoogleTest)

add_test(NAME BlockEntities_tests COMMAND BlockEntities_tests)

set_tests_properties(BlockEntities_tests PROPERTIES
    WORKING_DIRECTORY $<TARGET_FILE_DIR:BlockEntities_tests>
)
