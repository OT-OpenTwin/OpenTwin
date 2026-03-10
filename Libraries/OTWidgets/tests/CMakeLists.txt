# ============================================================
# OTWidgets test CMake
# Reuses OTWidgets_core object files and links against
# OTSystem, OTCore, OTGui, Qt, and Windows system libraries.
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
# Environment: OTSystem / OTCore / OTGui / RapidJSON
# ------------------------------------------------------------

# OpenTwin subdirectories
get_env_path(OT_INC_PATH          OT_INC)
get_env_path(OT_CLIBD_PATH        OT_CLIBD)
get_env_path(OT_CLIBR_PATH        OT_CLIBR)

# OpenTwin root paths
get_env_path(OT_SYSTEM_ROOT_PATH  OT_SYSTEM_ROOT)
join_paths(OT_SYSTEM_DEBUG_LIB_DIR   "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBD_PATH}")
join_paths(OT_SYSTEM_RELEASE_LIB_DIR "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBR_PATH}")

get_env_path(OT_CORE_ROOT_PATH    OT_CORE_ROOT)
join_paths(OT_CORE_DEBUG_LIB_DIR     "${OT_CORE_ROOT_PATH}" "${OT_CLIBD_PATH}")
join_paths(OT_CORE_RELEASE_LIB_DIR   "${OT_CORE_ROOT_PATH}" "${OT_CLIBR_PATH}")

get_env_path(OT_GUI_ROOT_PATH     OT_GUI_ROOT)
join_paths(OT_GUI_DEBUG_LIB_DIR      "${OT_GUI_ROOT_PATH}" "${OT_CLIBD_PATH}")
join_paths(OT_GUI_RELEASE_LIB_DIR    "${OT_GUI_ROOT_PATH}" "${OT_CLIBR_PATH}")

get_env_path(OT_WIDGETS_ROOT_PATH OT_WIDGETS_ROOT)

# RapidJSON include
get_env_path(R_JSON_INCD_PATH      R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH      R_JSON_INCR)

# QWT library paths
get_env_path(QWT_INC              QWT_LIB_INC)
get_env_path(QWT_LIB_DEBUG        QWT_LIB_LIBD)
get_env_path(QWT_LIB_PATH_DEBUG   QWT_LIB_LIBPATHD)
get_env_path(QWT_LIB_RELEASE      QWT_LIB_LIBR)
get_env_path(QWT_LIB_PATH_RELEASE QWT_LIB_LIBPATHR)

# Qt TabToolbar paths
get_env_path(QTTTB_INC_DEBUG      QT_TT_INCD)
get_env_path(QTTTB_LIB_DEBUG      QT_TT_LIBD)
get_env_path(QTTTB_LIB_PATH_D     QT_TT_LIBPATHD)
get_env_path(QTTTB_INC_RELEASE    QT_TT_INCR)
get_env_path(QTTTB_LIB_RELEASE    QT_TT_LIBR)
get_env_path(QTTTB_LIB_PATH_R     QT_TT_LIBPATHR)

# ADS library paths
get_env_path(QTADS_INC            QT_ADS_INC)
get_env_path(QTADS_LIB_DEBUG      QT_ADS_LIBD)
get_env_path(QTADS_LIB_RELEASE    QT_ADS_LIBR)
get_env_path(QTADS_LIB_PATH       QT_ADS_LIBPATH)

# ------------------------------------------------------------
# Collect all test sources from tests/src/
# ------------------------------------------------------------
file(GLOB_RECURSE OTWidgets_TEST_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

file(GLOB_RECURSE OTWidgets_TEST_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ============================================================
# C++ standard
# ============================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# Qt automatic processing (replaces vcxproj QtMoc rules)
# ============================================================
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ============================================================
# Qt
# ============================================================

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Svg
    Xml
    SvgWidgets
)

# ------------------------------------------------------------
# Test executable
# Reuse compiled object files from OTWidgets_core
# ------------------------------------------------------------
add_executable(OTWidgets_tests
    ${OTWidgets_TEST_HEADERS}
    ${OTWidgets_TEST_SOURCES}
    $<TARGET_OBJECTS:OTWidgets_core>
)

# Use same export macro as the DLL
target_compile_definitions(OTWidgets_tests PRIVATE
    OT_WIDGETS_EXPORTS
)

# ============================================================
# Source directories
# ============================================================

target_include_directories(OTWidgets_tests
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_include_directories(OTWidgets_tests PRIVATE
    "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    "${OT_GUI_ROOT_PATH}/${OT_INC_PATH}"
    "${OT_WIDGETS_ROOT_PATH}/${OT_INC_PATH}"
    
    $<$<CONFIG:Debug>:${R_JSON_INCD_PATH}>
    $<$<NOT:$<CONFIG:Debug>>:${R_JSON_INCR_PATH}>
    
    $<$<CONFIG:Debug>:${QTTTB_INC_DEBUG}>
    $<$<NOT:$<CONFIG:Debug>>:${QTTTB_INC_RELEASE}>
    
    "${QWT_INC}"
    "${QTADS_INC}"
)

# ============================================================
# Library directories
# ============================================================

target_link_directories(OTWidgets_tests PRIVATE
    $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
    $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>

    $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
    $<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>

    $<$<CONFIG:Debug>:${OT_GUI_DEBUG_LIB_DIR}>
    $<$<CONFIG:Release>:${OT_GUI_RELEASE_LIB_DIR}>

    $<$<CONFIG:Debug>:${QWT_LIB_PATH_DEBUG}>
    $<$<CONFIG:Release>:${QWT_LIB_PATH_RELEASE}>

    $<$<CONFIG:Debug>:${QTTTB_LIB_PATH_D}>
    $<$<CONFIG:Release>:${QTTTB_LIB_PATH_R}>

    "${QTADS_LIB_PATH}"
)

# ============================================================
# Link libraries
# ============================================================

target_link_libraries(OTWidgets_tests
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
    Qt6::Xml
    Qt6::SvgWidgets

    $<$<CONFIG:Debug>:${QWT_LIB_DEBUG}>
    $<$<CONFIG:Release>:${QWT_LIB_RELEASE}>

    $<$<CONFIG:Debug>:${QTTTB_LIB_DEBUG}>
    $<$<CONFIG:Release>:${QTTTB_LIB_RELEASE}>

    $<$<CONFIG:Debug>:${QTADS_LIB_DEBUG}>
    $<$<CONFIG:Release>:${QTADS_LIB_RELEASE}>

    OTSystem
    OTCore
    OTGui

    Userenv
    wsock32
    Advapi32
)

# ------------------------------------------------------------
# Windows-specific
# ------------------------------------------------------------
if(MSVC)

    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    target_compile_options(OTWidgets_tests PRIVATE
        /W3
        /sdl
        /MP
        /permissive-
        /Zc:__cplusplus
    )

endif()

# ------------------------------------------------------------
# GoogleTest integration
# GTest::gtest_main is provided by the parent CMake (OTWidgets).
# ------------------------------------------------------------
target_link_libraries(OTWidgets_tests
    PRIVATE
        GTest::gtest_main
)

include(GoogleTest)

add_test(NAME OTWidgets_tests COMMAND OTWidgets_tests)

# Use the executable directory as working directory (DLL lookup etc.)
set_tests_properties(OTWidgets_tests PROPERTIES
    WORKING_DIRECTORY $<TARGET_FILE_DIR:OTWidgets_tests>
)
