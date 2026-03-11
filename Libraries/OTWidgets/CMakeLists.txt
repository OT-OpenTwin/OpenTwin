cmake_minimum_required(VERSION 3.20)

project(OTWidgets LANGUAGES CXX)

# ============================================================
# Helper functions (same style as OTGui / OTCore)
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
# Environment configuration
# ============================================================

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

# ============================================================
# Source collection
# (much safer than listing 150 files manually)
# ============================================================

file(GLOB_RECURSE OTWIDGETS_SOURCE_FILES
    CONFIGURE_DEPENDS
    "${OT_WIDGETS_ROOT_PATH}/src/*.cpp"
)

file(GLOB_RECURSE OTWIDGETS_HEADER_FILES
    CONFIGURE_DEPENDS
    "${OT_WIDGETS_ROOT_PATH}/include/*.h"
    "${OT_WIDGETS_ROOT_PATH}/include/*.hpp"
)

# ============================================================
# OBJECT library
# ============================================================

add_library(OTWidgets_core OBJECT
    ${OTWIDGETS_SOURCE_FILES}
    ${OTWIDGETS_HEADER_FILES}
)

target_compile_definitions(OTWidgets_core
    PRIVATE
        UNICODE
        _UNICODE
        _WINDOWS
        _USRDLL
        OT_WIDGETS_EXPORTS
        QWT_DLL
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

# ============================================================
# Include directories
# ============================================================

target_include_directories(OTWidgets_core
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# OT include roots

target_include_directories(OTWidgets_core PRIVATE
    "${OT_WIDGETS_ROOT_PATH}/${OT_INC_PATH}"
    "${OT_GUI_ROOT_PATH}/${OT_INC_PATH}"
    "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"

    $<$<CONFIG:Debug>:${R_JSON_INCD_PATH}>
    $<$<NOT:$<CONFIG:Debug>>:${R_JSON_INCR_PATH}>

    $<$<CONFIG:Debug>:${QTTTB_INC_DEBUG}>
    $<$<NOT:$<CONFIG:Debug>>:${QTTTB_INC_RELEASE}>

    "${QWT_INC}"
    "${QTADS_INC}"
)

# Qt

target_link_libraries(OTWidgets_core
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Svg
        Qt6::Xml
        Qt6::SvgWidgets
)

# ============================================================
# MSVC flags
# ============================================================

if(MSVC)

    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    target_compile_options(OTWidgets_core PRIVATE
        /W3
        /sdl
        /MP
        /permissive-
        /Zc:__cplusplus
    )

endif()

# ============================================================
# Shared library
# ============================================================

add_library(OTWidgets SHARED
    $<TARGET_OBJECTS:OTWidgets_core>
)

target_include_directories(OTWidgets
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# ============================================================
# Link libraries
# ============================================================

target_link_directories(OTWidgets PRIVATE
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

target_link_libraries(OTWidgets PRIVATE
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

# ============================================================
# MSVC linker options
# ============================================================

if(MSVC)

    target_link_options(OTWidgets PRIVATE
        /SUBSYSTEM:WINDOWS
        /MANIFESTUAC:NO
        $<$<CONFIG:Release>:/OPT:REF /OPT:ICF>
    )

endif()

# ============================================================
# GoogleTest + test integration (OTWidgets_tests in tests/)
# ============================================================
include(CTest)

if(BUILD_TESTING)
    include(FetchContent)

    if(NOT TARGET GTest::gtest_main)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.15.2
        )

        set(BUILD_GTEST ON  CACHE BOOL "" FORCE)
        set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

        FetchContent_MakeAvailable(googletest)
    endif()

    enable_testing()
    add_subdirectory(tests)
endif()
