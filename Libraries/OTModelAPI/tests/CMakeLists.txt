cmake_minimum_required(VERSION 3.20)

# ============================================================
# Helper functions (same as in top-level)
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

function(join_paths OUT_VAR BASE SUBDIR)
    if(NOT "${BASE}" STREQUAL "" AND NOT "${SUBDIR}" STREQUAL "")
        set(_p "${BASE}/${SUBDIR}")
        file(TO_CMAKE_PATH "${_p}" _p)
        set(${OUT_VAR} "${_p}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

# ============================================================
# Environment (same style as main CMakeLists)
# ============================================================

get_env_path(OT_SYSTEM_ROOT_PATH        OT_SYSTEM_ROOT)
get_env_path(OT_CORE_ROOT_PATH          OT_CORE_ROOT)
get_env_path(OT_GUI_ROOT_PATH           OT_GUI_ROOT)
get_env_path(OT_COMMUNICATION_ROOT_PATH OT_COMMUNICATION_ROOT)
get_env_path(OT_MODELENTITIES_ROOT_PATH OT_MODELENTITIES_ROOT)
get_env_path(OT_DATASTORAGE_ROOT_PATH   OT_DATASTORAGE_ROOT)

get_env_path(OT_INC_PATH                OT_INC)
get_env_path(OT_CLIBD_PATH              OT_CLIBD)
get_env_path(OT_CLIBR_PATH              OT_CLIBR)

get_env_path(R_JSON_INCD_PATH           R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH           R_JSON_INCR)

get_env_path(MONGO_CXX_INC_PATH         MONGO_CXX_INC)
get_env_path(MONGO_BOOST_ROOT_PATH      MONGO_BOOST_ROOT)

get_env_path(MONGO_C_LIBPATHD_PATH      MONGO_C_LIBPATHD)
get_env_path(MONGO_C_LIBPATHR_PATH      MONGO_C_LIBPATHR)
get_env_path(MONGO_CXX_LIBPATHD_PATH    MONGO_CXX_LIBPATHD)
get_env_path(MONGO_CXX_LIBPATHR_PATH    MONGO_CXX_LIBPATHR)

get_env_libname(MONGO_CXX_LIBS_NAME     MONGO_CXX_LIB)
get_env_libname(MONGO_C_LIBS_NAME       MONGO_C_LIB)

join_paths(OT_SYSTEM_DEBUG_LIB_DIR      "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBD_PATH}")
join_paths(OT_SYSTEM_RELEASE_LIB_DIR    "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBR_PATH}")
join_paths(OT_CORE_DEBUG_LIB_DIR        "${OT_CORE_ROOT_PATH}"   "${OT_CLIBD_PATH}")
join_paths(OT_CORE_RELEASE_LIB_DIR      "${OT_CORE_ROOT_PATH}"   "${OT_CLIBR_PATH}")
join_paths(OT_GUI_DEBUG_LIB_DIR         "${OT_GUI_ROOT_PATH}"    "${OT_CLIBD_PATH}")
join_paths(OT_GUI_RELEASE_LIB_DIR       "${OT_GUI_ROOT_PATH}"    "${OT_CLIBR_PATH}")
join_paths(OT_COMM_DEBUG_LIB_DIR        "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBD_PATH}")
join_paths(OT_COMM_RELEASE_LIB_DIR      "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBR_PATH}")
join_paths(OT_MODELENT_DEBUG_LIB_DIR    "${OT_MODELENTITIES_ROOT_PATH}" "${OT_CLIBD_PATH}")
join_paths(OT_MODELENT_RELEASE_LIB_DIR  "${OT_MODELENTITIES_ROOT_PATH}" "${OT_CLIBR_PATH}")
join_paths(OT_DATASTORAGE_DEBUG_LIB_DIR "${OT_DATASTORAGE_ROOT_PATH}"   "${OT_CLIBD_PATH}")
join_paths(OT_DATASTORAGE_RELEASE_LIB_DIR "${OT_DATASTORAGE_ROOT_PATH}" "${OT_CLIBR_PATH}")

# ============================================================
# Collect test sources
# ============================================================

file(GLOB_RECURSE OTModelAPI_TEST_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(OTModelAPI_tests
    ${OTModelAPI_TEST_SOURCES}
    $<TARGET_OBJECTS:OTModelAPI_core>
)

target_compile_definitions(OTModelAPI_tests
    PRIVATE
        OTMODELAPI_EXPORTS
)

target_include_directories(OTModelAPI_tests
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"         # optional local test headers
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"      # OTModelAPI public headers
)

# OT include trees
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelAPI_tests PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelAPI_tests PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_GUI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelAPI_tests PRIVATE
        "${OT_GUI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_COMMUNICATION_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelAPI_tests PRIVATE
        "${OT_COMMUNICATION_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_MODELENTITIES_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelAPI_tests PRIVATE
        "${OT_MODELENTITIES_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_DATASTORAGE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelAPI_tests PRIVATE
        "${OT_DATASTORAGE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# RapidJSON
if(R_JSON_INCD_PATH)
    target_include_directories(OTModelAPI_tests PRIVATE "${R_JSON_INCD_PATH}")
endif()
if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(OTModelAPI_tests PRIVATE "${R_JSON_INCR_PATH}")
endif()

# Mongo / Boost
if(MONGO_CXX_INC_PATH)
    target_include_directories(OTModelAPI_tests PRIVATE "${MONGO_CXX_INC_PATH}")
endif()

if(MONGO_BOOST_ROOT_PATH)
    target_include_directories(OTModelAPI_tests PRIVATE "${MONGO_BOOST_ROOT_PATH}")
endif()

target_compile_features(OTModelAPI_tests PRIVATE cxx_std_20)

# ============================================================
# Linkage (Windows)
# ============================================================

if(WIN32)
    # OT libs
    if(OT_SYSTEM_DEBUG_LIB_DIR)
        target_link_directories(OTModelAPI_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(OTModelAPI_tests PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_CORE_DEBUG_LIB_DIR)
        target_link_directories(OTModelAPI_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_CORE_RELEASE_LIB_DIR)
        target_link_directories(OTModelAPI_tests PRIVATE
            $<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_GUI_DEBUG_LIB_DIR)
        target_link_directories(OTModelAPI_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_GUI_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_GUI_RELEASE_LIB_DIR)
        target_link_directories(OTModelAPI_tests PRIVATE
            $<$<CONFIG:Release>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_GUI_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_COMM_DEBUG_LIB_DIR)
        target_link_directories(OTModelAPI_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_COMM_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_COMM_RELEASE_LIB_DIR)
        target_link_directories(OTModelAPI_tests PRIVATE
            $<$<CONFIG:Release>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_COMM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_MODELENT_DEBUG_LIB_DIR)
        target_link_directories(OTModelAPI_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_MODELENT_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_MODELENT_RELEASE_LIB_DIR)
        target_link_directories(OTModelAPI_tests PRIVATE
            $<$<CONFIG:Release>:${OT_MODELENT_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_MODELENT_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_MODELENT_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_DATASTORAGE_DEBUG_LIB_DIR)
        target_link_directories(OTModelAPI_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_DATASTORAGE_DEBUG_LIB_DIR}>
        )
    endif()
    if(OT_DATASTORAGE_RELEASE_LIB_DIR)
        target_link_directories(OTModelAPI_tests PRIVATE
            $<$<CONFIG:Release>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
        )
    endif()

    # Mongo C/C++
    if(MONGO_C_LIBPATHD_PATH)
        target_link_directories(OTModelAPI_tests PRIVATE
            $<$<CONFIG:Debug>:${MONGO_C_LIBPATHD_PATH}>
        )
    endif()
    if(MONGO_C_LIBPATHR_PATH)
        target_link_directories(OTModelAPI_tests PRIVATE
            $<$<CONFIG:Release>:${MONGO_C_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${MONGO_C_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${MONGO_C_LIBPATHR_PATH}>
        )
    endif()

    if(MONGO_CXX_LIBPATHD_PATH)
        target_link_directories(OTModelAPI_tests PRIVATE
            $<$<CONFIG:Debug>:${MONGO_CXX_LIBPATHD_PATH}>
        )
    endif()
    if(MONGO_CXX_LIBPATHR_PATH)
        target_link_directories(OTModelAPI_tests PRIVATE
            $<$<CONFIG:Release>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${MONGO_CXX_LIBPATHR_PATH}>
        )
    endif()

    target_link_libraries(OTModelAPI_tests
        PRIVATE
            OTModelAPI           # main DLL
            OTSystem
            OTCore
            OTGui
            OTCommunication
            ModelEntities
            DataStorage
            Userenv
            wsock32
            Advapi32
            ${MONGO_CXX_LIBS_NAME}
            ${MONGO_C_LIBS_NAME}
    )
endif()

# ============================================================
# Optional GoogleTest integration
# ============================================================

if(TARGET GTest::gtest_main)
    target_link_libraries(OTModelAPI_tests
        PRIVATE
            GTest::gtest_main
    )

    include(GoogleTest)
    add_test(NAME OTModelAPI_tests COMMAND OTModelAPI_tests)

    set_tests_properties(OTModelAPI_tests PROPERTIES
        WORKING_DIRECTORY $<TARGET_FILE_DIR:OTModelAPI_tests>
    )
else()
    message(WARNING
        "GTest::gtest_main not found. "
        "OTModelAPI_tests will be built, but not registered as GoogleTest/CTest tests."
    )
endif()
