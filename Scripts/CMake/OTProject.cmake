# License:
# Copyright 2026 by OpenTwin
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include_guard(GLOBAL)

# OpenTwin CMake project helper functions
# This file provides helper functions to define CMake targets for OpenTwin libraries and applications,
# following the conventions of the OpenTwin project structure and environment variables.
#
# =====================================================================
# Usage for library:
#   cmake_minimum_required(VERSION 3.20)
#   project(<LIB_NAME> LANGUAGES CXX)
#
#   include("$ENV{OT_CMAKE_DIR}/OTEnvironment.cmake")
#   include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")
#
#   ot_initialize_lib(<LIB_NAME> <LIB>_ROOT_PATH)
#   ot_add_export(<LIB_NAME>)
#
#   ot_add_dependency(<LIB_NAME> <DEP_TOKEN_1> <DEP_TOKEN_2> ...)
#
#   ot_finalize_lib(<LIB_NAME>)
#   ot_add_test(<LIB_NAME>)
#
# =====================================================================
# Usage for binary:
#   cmake_minimum_required(VERSION 3.20)
#   project(<APP_NAME> LANGUAGES CXX)
#
#   include("$ENV{OT_CMAKE_DIR}/OTEnvironment.cmake")
#   include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")
#
#   ot_initialize_bin(<APP_NAME> <APP>_ROOT_PATH)
#
#   ot_add_dependency(<APP_NAME> <DEP_TOKEN_1> <DEP_TOKEN_2> ...)
#
#   ot_finalize_bin(<APP_NAME>)
#   ot_add_test(<APP_NAME>)

# Requires OTEnvironment.cmake first (for THIRDPARTY_ROOT_PATH, OT_* paths, etc.)
include("$ENV{OT_CMAKE_DIR}/OTQt.cmake")

if(MSVC)
    add_compile_options(/Zc:__cplusplus /permissive- /Zc:preprocessor /MP)
endif()

# ------------------------------------------------------------
# internal helpers
# ------------------------------------------------------------
# Converts a target name like "OTFoo" to "OTFoo_core" for the internal object library
function(_ot_target_core_name OUT_VAR TARGET_NAME)
    set(${OUT_VAR} "${TARGET_NAME}_core" PARENT_SCOPE)
endfunction()

# ------------------------------------------------------------
# OT token -> env var mapping
# ------------------------------------------------------------
# Default rule: OTFooBar -> OT_FOO_BAR_ROOT
function(_ot_guess_ot_root_envvar OUT_VAR DEP_TOKEN)
    string(REGEX REPLACE "^OT" "" _name "${DEP_TOKEN}")
    if("${_name}" STREQUAL "")
        set(${OUT_VAR} "" PARENT_SCOPE)
        return()
    endif()

    string(REGEX REPLACE "([a-z0-9])([A-Z])" "\\1_\\2" _snake "${_name}")
    string(TOUPPER "${_snake}" _snake_u)
    set(${OUT_VAR} "OT_${_snake_u}_ROOT" PARENT_SCOPE)
endfunction()

# Overrides for exceptions in your SetupEnvironment.bat
function(_ot_get_ot_root_envvar OUT_VAR DEP_TOKEN)
    if(DEP_TOKEN STREQUAL "OTServiceFoundation")
        set(${OUT_VAR} "OT_FOUNDATION_ROOT" PARENT_SCOPE)
        return()
    endif()

    if(DEP_TOKEN STREQUAL "OTRubberband")
        set(${OUT_VAR} "OT_RUBBERBANDAPI_ROOT" PARENT_SCOPE)
        return()
    endif()

    # no extra underscore in env var names
    if(DEP_TOKEN STREQUAL "OTDataStorage")
        set(${OUT_VAR} "OT_DATASTORAGE_ROOT" PARENT_SCOPE)
        return()
    endif()

    if(DEP_TOKEN STREQUAL "OTModelEntities")
        set(${OUT_VAR} "OT_MODELENTITIES_ROOT" PARENT_SCOPE)
        return()
    endif()

    _ot_guess_ot_root_envvar(_guess "${DEP_TOKEN}")
    set(${OUT_VAR} "${_guess}" PARENT_SCOPE)
endfunction()

function(_ot_get_ot_inc_suffix OUT_VAR)
    if(DEFINED ENV{OT_INC} AND NOT "$ENV{OT_INC}" STREQUAL "")
        set(${OUT_VAR} "$ENV{OT_INC}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "include" PARENT_SCOPE)
    endif()
endfunction()


function(_ot_env_get_path OUT_VAR ENV_NAME)
    if(DEFINED ENV{${ENV_NAME}} AND NOT "$ENV{${ENV_NAME}}" STREQUAL "")
        set(_v "$ENV{${ENV_NAME}}")
        string(REPLACE "\"" "" _v "${_v}")
        file(TO_CMAKE_PATH "${_v}" _v)
        set(${OUT_VAR} "${_v}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

function(_ot_env_get_list OUT_VAR ENV_NAME)
    if(DEFINED ENV{${ENV_NAME}} AND NOT "$ENV{${ENV_NAME}}" STREQUAL "")
        set(_v "$ENV{${ENV_NAME}}")
        string(REPLACE "\"" "" _v "${_v}")
        set(${OUT_VAR} "${_v}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

function(_ot_apply_tp_to_core CORE_TARGET TP_NAME)
    _ot_env_get_path(_inc  "${TP_NAME}_INC")
    _ot_env_get_path(_incd "${TP_NAME}_INCD")
    _ot_env_get_path(_incr "${TP_NAME}_INCR")

    # Prefer the single include dir if provided
    if(NOT "${_inc}" STREQUAL "")
        target_include_directories("${CORE_TARGET}" PRIVATE "${_inc}")
        return()
    endif()

    if("${_incd}" STREQUAL "" AND "${_incr}" STREQUAL "")
        message(FATAL_ERROR
            "TP:${TP_NAME}: no include env vars found. Expected ${TP_NAME}_INC or ${TP_NAME}_INCD/${TP_NAME}_INCR."
        )
    endif()

    if(NOT "${_incd}" STREQUAL "")
        target_include_directories("${CORE_TARGET}" PRIVATE
            $<$<CONFIG:Debug>:${_incd}>
        )
    endif()

    if(NOT "${_incr}" STREQUAL "")
        target_include_directories("${CORE_TARGET}" PRIVATE
            $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:${_incr}>
        )
    endif()
endfunction()

function(_ot_apply_tp_to_final FINAL_TARGET TP_NAME)
    _ot_env_get_path(_libpathd "${TP_NAME}_LIBPATHD")
    _ot_env_get_path(_libpathr "${TP_NAME}_LIBPATHR")

    if(NOT "${_libpathd}" STREQUAL "")
        target_link_directories("${FINAL_TARGET}" PRIVATE
            $<$<CONFIG:Debug>:${_libpathd}>
        )
    endif()

    if(NOT "${_libpathr}" STREQUAL "")
        target_link_directories("${FINAL_TARGET}" PRIVATE
            $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:${_libpathr}>
        )
    endif()

    # Preferred: per-config lib lists
    _ot_env_get_list(_libd "${TP_NAME}_LIBD")
    _ot_env_get_list(_libr "${TP_NAME}_LIBR")

    if(NOT "${_libd}" STREQUAL "")
        target_link_libraries("${FINAL_TARGET}" PRIVATE
            $<$<CONFIG:Debug>:${_libd}>
        )
    endif()

    if(NOT "${_libr}" STREQUAL "")
        target_link_libraries("${FINAL_TARGET}" PRIVATE
            $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:${_libr}>
        )
    endif()

    # Compatibility fallback: single list used for all configs
    if("${_libd}" STREQUAL "" AND "${_libr}" STREQUAL "")
        _ot_env_get_list(_lib_any "${TP_NAME}_LIB")
        if("${_lib_any}" STREQUAL "")
            _ot_env_get_list(_lib_any "${TP_NAME}_LIBS")
        endif()

        if(NOT "${_lib_any}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE "${_lib_any}")
        endif()
    endif()
endfunction()

# ------------------------------------------------------------
# initialize_lib / initialize_app
# ------------------------------------------------------------
function(ot_initialize_lib TARGET_NAME ROOT_PATH_VAR)
    if(NOT DEFINED ${ROOT_PATH_VAR} OR "${${ROOT_PATH_VAR}}" STREQUAL "")
        message(FATAL_ERROR
            "ot_initialize_lib(${TARGET_NAME} ...): root path var '${ROOT_PATH_VAR}' is not set. "
            "Did you run SetupEnvironment.bat and include OTEnvironment.cmake?"
        )
    endif()

    _ot_target_core_name(_core ${TARGET_NAME})
    set(_rootPath "${${ROOT_PATH_VAR}}")

    file(GLOB_RECURSE _src CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    )
    file(GLOB_RECURSE _hdr CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    )

    add_library(${_core} OBJECT ${_src} ${_hdr})

    set_target_properties(${_core} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS NO
    )

    # export function
    if(WIN32)
        target_compile_definitions(${_core} PRIVATE
            UNICODE
            _UNICODE
            _WINDOWS
            _USRDLL
        )
    endif()
    target_compile_definitions(${_core} PRIVATE
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

    target_include_directories(${_core} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

    # Save metadata on the core target
    set_property(TARGET ${_core} PROPERTY OT_ROOT_VAR "${ROOT_PATH_VAR}")
    set_property(TARGET ${_core} PROPERTY OT_ROOT_PATH "${_rootPath}")
    set_property(TARGET ${_core} PROPERTY OT_DEPS "")

    # Add root include path using OT_INC suffix (or include)
    set(_incSuffix "${OT_INC_PATH}")
    if("${_incSuffix}" STREQUAL "")
        set(_incSuffix "include")
    endif()

    target_include_directories(${_core} PRIVATE
        "${_rootPath}/${_incSuffix}"
    )
endfunction()

function(ot_initialize_bin TARGET_NAME ROOT_PATH_VAR)
    ot_initialize_lib(${TARGET_NAME} ${ROOT_PATH_VAR})
    _ot_target_core_name(_core ${TARGET_NAME})
    set_property(TARGET ${_core} PROPERTY OT_IS_APP TRUE)
endfunction()

function(ot_add_export TARGET_NAME)
    string(REGEX REPLACE "^OT" "" _lib "${TARGET_NAME}")
    string(TOUPPER "${_lib}" _lib_upper)
    set(_export_macro "OPENTWIN${_lib_upper}_EXPORTS")
    target_compile_definitions(${TARGET_NAME}_core PRIVATE ${_export_macro})
endfunction()

# ------------------------------------------------------------
# add_dependency (collect tokens)
# ------------------------------------------------------------
function(ot_add_dependency TARGET_NAME)
    _ot_target_core_name(_core ${TARGET_NAME})
    if(NOT TARGET ${_core})
        message(FATAL_ERROR "ot_add_dependency: core target '${_core}' does not exist. Call ot_initialize_lib/app first.")
    endif()

    get_property(_deps TARGET ${_core} PROPERTY OT_DEPS)
    if(NOT _deps)
        set(_deps "")
    endif()

    foreach(dep IN LISTS ARGN)
        list(APPEND _deps "${dep}")
    endforeach()

    set_property(TARGET ${_core} PROPERTY OT_DEPS "${_deps}")
endfunction()

# ------------------------------------------------------------
# dependency resolution helpers
# ------------------------------------------------------------
function(_ot_apply_dep_to_core CORE_TARGET DEP)
    # OT* deps: add include dirs to compile *_core
    if(DEP MATCHES "^OT")
        _ot_get_ot_root_envvar(_rootVar "${DEP}")
        if("${_rootVar}" STREQUAL "")
            return()
        endif()

        _ot_get_ot_inc_suffix(_incSuffix)

        if(DEFINED ENV{${_rootVar}} AND NOT "$ENV{${_rootVar}}" STREQUAL "")
            file(TO_CMAKE_PATH "$ENV{${_rootVar}}" _root)
            target_include_directories("${CORE_TARGET}" PRIVATE "${_root}/${_incSuffix}")
        else()
            message(FATAL_ERROR
                "Could not add OT include directory for token '${DEP}'. "
                "Expected env var '${_rootVar}' to be set."
            )
        endif()

        if(DEP STREQUAL "OTRubberbandOSG")
            _ot_apply_dep_to_core("${CORE_TARGET}" "OSG")
        endif()

        return()
    endif()

    # Qt tokens: create imported Qt targets AND link them to the core object library
    if(DEP STREQUAL "QtCore")
        ot_define_qt6_targets("Core")
        target_link_libraries("${CORE_TARGET}" PRIVATE Qt6::Core)
        return()
    endif()

    if(DEP STREQUAL "QtWidgets")
        ot_define_qt6_targets("Core;Gui;Widgets")
        target_link_libraries("${CORE_TARGET}" PRIVATE Qt6::Widgets)
        return()
    endif()

    if(DEP STREQUAL "QtFull")
        ot_define_qt6_targets("Core;Gui;Widgets;Network;Svg;Xml;OpenGLWidgets;SvgWidgets")
        target_link_libraries("${CORE_TARGET}" PRIVATE
            Qt6::Widgets
            Qt6::Network
            Qt6::Svg
            Qt6::Xml
            Qt6::OpenGLWidgets
            Qt6::SvgWidgets
        )
        return()
    endif()

    # RapidJSON headers
    if(DEP STREQUAL "RJSON")
        if(R_JSON_INCD_PATH)
            target_include_directories("${CORE_TARGET}" PRIVATE "${R_JSON_INCD_PATH}")
        endif()
        if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
            target_include_directories("${CORE_TARGET}" PRIVATE "${R_JSON_INCR_PATH}")
        endif()
        return()
    endif()

    # OSG headers
    if(DEP STREQUAL "OSG")
        if(OSG_INCD_PATH)
            target_include_directories("${CORE_TARGET}" PRIVATE "${OSG_INCD_PATH}")
        endif()
        if(OSG_INCR_PATH AND NOT OSG_INCR_PATH STREQUAL OSG_INCD_PATH)
            target_include_directories("${CORE_TARGET}" PRIVATE "${OSG_INCR_PATH}")
        endif()
        return()
    endif()

    # ADS headers
    if(DEP STREQUAL "ADS")
        if(QT_ADS_ROOT_PATH)
            target_include_directories("${CORE_TARGET}" PRIVATE "${QT_ADS_ROOT_PATH}/include")
        else()
            message(FATAL_ERROR "ADS token used but QT_ADS_ROOT_PATH is empty (env QT_ADS_ROOT not set?)")
        endif()
        return()
    endif()

    # QWT / QWTPOLAR headers
    if(DEP STREQUAL "QWT")
        if(QWT_LIB_INCD_PATH)
            target_include_directories("${CORE_TARGET}" PRIVATE "${QWT_LIB_INCD_PATH}")
        endif()
        if(QWT_LIB_INCR_PATH AND NOT QWT_LIB_INCR_PATH STREQUAL QWT_LIB_INCD_PATH)
            target_include_directories("${CORE_TARGET}" PRIVATE "${QWT_LIB_INCR_PATH}")
        endif()
        return()
    endif()

    if(DEP STREQUAL "EARCUT")
        _ot_apply_tp_to_core("${CORE_TARGET}" "EARCUT")
        return()
    endif()

    _ot_env_get_path(_inc  "${DEP}_INC")
    _ot_env_get_path(_incd "${DEP}_INCD")
    _ot_env_get_path(_incr "${DEP}_INCR")
    if(NOT ("${_inc}" STREQUAL "" AND "${_incd}" STREQUAL "" AND "${_incr}" STREQUAL ""))
        _ot_apply_tp_to_core("${CORE_TARGET}" "${DEP}")
        return()
    endif()

endfunction()

# Adds BOTH new (CMake build output) and old (x64/<Config>) link dir layouts.
function(_ot_add_ot_dep_link_dirs FINAL_TARGET ROOT_DIR)
    if("${ROOT_DIR}" STREQUAL "")
        return()
    endif()

    if(OT_CLIBD_PATH AND OT_CLIBR_PATH)
        ot_add_debug_release_link_dirs("${FINAL_TARGET}"
            "${ROOT_DIR}/${OT_CLIBD_PATH}"
            "${ROOT_DIR}/${OT_CLIBR_PATH}"
        )
    endif()

    if(OT_LIBD_PATH AND OT_LIBR_PATH)
        ot_add_debug_release_link_dirs("${FINAL_TARGET}"
            "${ROOT_DIR}/${OT_LIBD_PATH}"
            "${ROOT_DIR}/${OT_LIBR_PATH}"
        )
    endif()

    if(OT_CDLLD_PATH AND OT_CDLLR_PATH)
        ot_add_debug_release_link_dirs("${FINAL_TARGET}"
            "${ROOT_DIR}/${OT_CDLLD_PATH}"
            "${ROOT_DIR}/${OT_CDLLR_PATH}"
        )
    endif()
endfunction()

function(_ot_apply_dep_to_final FINAL_TARGET DEP)
    # OT libs: link + compatibility link dirs
    if(DEP MATCHES "^OT")
        target_link_libraries("${FINAL_TARGET}" PRIVATE ${DEP})

        _ot_get_ot_root_envvar(_rootVar "${DEP}")
        if("${_rootVar}" STREQUAL "")
            message(FATAL_ERROR "OT dependency '${DEP}' requested but no root env var mapping was found.")
        endif()

        if(NOT DEFINED ENV{${_rootVar}} OR "$ENV{${_rootVar}}" STREQUAL "")
            message(FATAL_ERROR
                "OT dependency '${DEP}' requested but its root env var '${_rootVar}' is not set."
            )
        endif()

        file(TO_CMAKE_PATH "$ENV{${_rootVar}}" _root)
        _ot_add_ot_dep_link_dirs("${FINAL_TARGET}" "${_root}")

        return()
    endif()

    # Qt tokens
    if(DEP STREQUAL "QtCore")
        ot_define_qt6_targets("Core")
        target_link_libraries("${FINAL_TARGET}" PRIVATE Qt6::Core)
        return()
    endif()

    if(DEP STREQUAL "QtWidgets")
        ot_define_qt6_targets("Core;Gui;Widgets")
        target_link_libraries("${FINAL_TARGET}" PRIVATE Qt6::Widgets)
        return()
    endif()

    if(DEP STREQUAL "QtFull")
        ot_define_qt6_targets("Core;Gui;Widgets;Network;Svg;Xml;OpenGLWidgets;SvgWidgets")
        target_link_libraries("${FINAL_TARGET}" PRIVATE
            Qt6::Widgets
            Qt6::Network
            Qt6::Svg
            Qt6::Xml
            Qt6::OpenGLWidgets
            Qt6::SvgWidgets
        )
        return()
    endif()

    # ADS libs
    if(DEP STREQUAL "ADS")
        if(QT_ADS_ROOT_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE "${QT_ADS_ROOT_PATH}/lib")
        endif()

        if(DEFINED ENV{QT_ADS_LIBD} AND NOT "$ENV{QT_ADS_LIBD}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<$<CONFIG:Debug>:$ENV{QT_ADS_LIBD}>)
        endif()
        if(DEFINED ENV{QT_ADS_LIBR} AND NOT "$ENV{QT_ADS_LIBR}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<$<NOT:$<CONFIG:Debug>>:$ENV{QT_ADS_LIBR}>)
        endif()
        return()
    endif()

    # QWT libs
    if(DEP STREQUAL "QWT")
        if(QWT_LIB_LIBPATHD_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE $<$<CONFIG:Debug>:${QWT_LIB_LIBPATHD_PATH}>)
        endif()
        if(QWT_LIB_LIBPATHR_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE
                $<$<CONFIG:Release>:${QWT_LIB_LIBPATHR_PATH}>
                $<$<CONFIG:RelWithDebInfo>:${QWT_LIB_LIBPATHR_PATH}>
                $<$<CONFIG:MinSizeRel>:${QWT_LIB_LIBPATHR_PATH}>
            )
        endif()

        if(QWT_LIB_LIBD_NAME AND NOT "${QWT_LIB_LIBD_NAME}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<$<CONFIG:Debug>:${QWT_LIB_LIBD_NAME}>)
        elseif(DEFINED ENV{QWT_LIB_LIBD} AND NOT "$ENV{QWT_LIB_LIBD}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<$<CONFIG:Debug>:$ENV{QWT_LIB_LIBD}>)
        endif()

        if(QWT_LIB_LIBR_NAME AND NOT "${QWT_LIB_LIBR_NAME}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<$<NOT:$<CONFIG:Debug>>:${QWT_LIB_LIBR_NAME}>)
        elseif(DEFINED ENV{QWT_LIB_LIBR} AND NOT "$ENV{QWT_LIB_LIBR}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<$<NOT:$<CONFIG:Debug>>:$ENV{QWT_LIB_LIBR}>)
        endif()

        return()
    endif()

    # OSG libs
    if(DEP STREQUAL "OSG")
        if(OSG_LIBPATHD_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE $<$<CONFIG:Debug>:${OSG_LIBPATHD_PATH}>)
        endif()
        if(OSG_LIBPATHR_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE
                $<$<CONFIG:Release>:${OSG_LIBPATHR_PATH}>
                $<$<CONFIG:RelWithDebInfo>:${OSG_LIBPATHR_PATH}>
                $<$<CONFIG:MinSizeRel>:${OSG_LIBPATHR_PATH}>
            )
        endif()

        if(OSG_LIBD_NAME AND NOT "${OSG_LIBD_NAME}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<$<CONFIG:Debug>:${OSG_LIBD_NAME}>)
        endif()
        if(OSG_LIBR_NAME AND NOT "${OSG_LIBR_NAME}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<$<NOT:$<CONFIG:Debug>>:${OSG_LIBR_NAME}>)
        endif()

        return()
    endif()

    # Windows system libs
    if(WIN32 AND DEP MATCHES "^WINLIB:.+")
        string(REPLACE "WINLIB:" "" _winlib "${DEP}")

        if("${_winlib}" STREQUAL "")
            message(FATAL_ERROR "Empty WINLIB token: '${DEP}'")
        endif()

        if(NOT _winlib MATCHES "^[A-Za-z0-9_.-]+$")
            message(FATAL_ERROR "Invalid WINLIB name '${_winlib}' from token '${DEP}'")
        endif()

        target_link_libraries("${FINAL_TARGET}" PRIVATE "${_winlib}")
        return()
    endif()

    # Include-only tokens
    if(DEP STREQUAL "RJSON")
        return()
    endif()

    # Friendly alias: EARCUT (header-only)
    if(DEP STREQUAL "EARCUT")
        _ot_apply_tp_to_final("${FINAL_TARGET}" "EARCUT")
        return()
    endif()

    _ot_env_get_path(_libpathd "${DEP}_LIBPATHD")
    _ot_env_get_path(_libpathr "${DEP}_LIBPATHR")
    if(NOT ("${_libpathd}" STREQUAL "" AND "${_libpathr}" STREQUAL ""))
        _ot_apply_tp_to_final("${FINAL_TARGET}" "${DEP}")
        return()
    endif()

    message(WARNING "ot_finalize_lib(${FINAL_TARGET}): Unknown dependency token '${DEP}'")

endfunction()

# ------------------------------------------------------------
# finalize helpers
# ------------------------------------------------------------
function(_ot_set_automoc_if_qt TARGET_NAME DEPS_LIST)
    foreach(dep IN LISTS DEPS_LIST)
        if(dep MATCHES "^Qt" OR dep STREQUAL "QtFull")
            set_target_properties(${TARGET_NAME} PROPERTIES
                AUTOMOC ON
                # AUTOUIC ON
                # AUTORCC ON
            )
            return()
        endif()
    endforeach()
endfunction()

function(_ot_apply_all_deps TARGET_NAME CORE_NAME DEPS)
    foreach(dep IN LISTS DEPS)
        _ot_apply_dep_to_core(${CORE_NAME} "${dep}")
        _ot_apply_dep_to_final(${TARGET_NAME} "${dep}")
    endforeach()
endfunction()

# ------------------------------------------------------------
# finalize_lib / finalize_bin
# ------------------------------------------------------------
function(ot_finalize_lib TARGET_NAME)
    _ot_target_core_name(_core ${TARGET_NAME})
    if(NOT TARGET ${_core})
        message(FATAL_ERROR "ot_finalize_lib: core target '${_core}' does not exist. Call ot_initialize_lib first.")
    endif()

    get_property(_deps TARGET ${_core} PROPERTY OT_DEPS)
    if(NOT _deps)
        set(_deps "")
    endif()

    add_library(${TARGET_NAME} SHARED $<TARGET_OBJECTS:${_core}>)
    target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

    _ot_apply_all_deps(${TARGET_NAME} ${_core} "${_deps}")

    _ot_set_automoc_if_qt(${_core} "${_deps}")
endfunction()

function(ot_finalize_bin TARGET_NAME)
    _ot_target_core_name(_core ${TARGET_NAME})
    if(NOT TARGET ${_core})
        message(FATAL_ERROR "ot_finalize_bin: core target '${_core}' does not exist. Call ot_initialize_app first.")
    endif()

    get_property(_deps TARGET ${_core} PROPERTY OT_DEPS)
    if(NOT _deps)
        set(_deps "")
    endif()

    add_executable(${TARGET_NAME} $<TARGET_OBJECTS:${_core}>)

    _ot_apply_all_deps(${TARGET_NAME} ${_core} "${_deps}")

    _ot_set_automoc_if_qt(${_core} "${_deps}")
endfunction()

# ------------------------------------------------------------
#  tests
# ------------------------------------------------------------
function(ot_add_test TARGET_NAME)
    include(CTest)
    if(BUILD_TESTING)
        enable_testing()
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
            add_subdirectory(tests)
        else()
            message(STATUS "ot_add_test(${TARGET_NAME}): no tests/ directory found")
        endif()
    endif()
endfunction()

function(ot_initialize_test TEST_TARGET_NAME MAIN_TARGET_NAME)
    file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

    add_executable(${TEST_TARGET_NAME} ${TEST_SOURCES})

    set_target_properties(${TEST_TARGET_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS NO
    )

    target_include_directories(${TEST_TARGET_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )

    target_compile_definitions(${TEST_TARGET_NAME} PRIVATE
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
    )

    target_link_libraries(${TEST_TARGET_NAME} PRIVATE ${MAIN_TARGET_NAME})

    if(DEFINED ENV{GOOGLE_TEST_INC} AND NOT "$ENV{GOOGLE_TEST_INC}" STREQUAL "")
        target_include_directories(${TEST_TARGET_NAME} PRIVATE "$ENV{GOOGLE_TEST_INC}")
    endif()
    if(DEFINED ENV{GOOGLE_TEST_LIB} AND NOT "$ENV{GOOGLE_TEST_LIB}" STREQUAL "")
        target_link_libraries(${TEST_TARGET_NAME} PRIVATE $ENV{GOOGLE_TEST_LIB})
    endif()
    if(DEFINED ENV{GOOGLE_TEST_LIBPATHD} AND NOT "$ENV{GOOGLE_TEST_LIBPATHD}" STREQUAL "")
        target_link_directories(${TEST_TARGET_NAME} PRIVATE $<$<CONFIG:Debug>:$ENV{GOOGLE_TEST_LIBPATHD}>)
    endif()
    if(DEFINED ENV{GOOGLE_TEST_LIBPATHR} AND NOT "$ENV{GOOGLE_TEST_LIBPATHR}" STREQUAL "")
        target_link_directories(${TEST_TARGET_NAME} PRIVATE $<$<NOT:$<CONFIG:Debug>>:$ENV{GOOGLE_TEST_LIBPATHR}>)
    endif()

    add_test(NAME ${TEST_TARGET_NAME} COMMAND ${TEST_TARGET_NAME})
endfunction()
