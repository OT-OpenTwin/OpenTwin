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

# OpenTwin CMake project configuration and helper functions.
# Designed for a centralized, environment-variable-driven approach to dependency management.
# Check documentation for usage details.

include("$ENV{OT_CMAKE_DIR}/OTEnvironment.cmake")
include("$ENV{OT_CMAKE_DIR}/OTQt.cmake")

set(_OT_CFG_DEBUG   "$<CONFIG:Debug>")
set(_OT_CFG_RELEASE "$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>")

if(MSVC)
    add_compile_options(
        /Zc:__cplusplus
        /Zc:wchar_t
        /Zc:inline
        /Zc:forScope
        /permissive-
        /Zc:preprocessor
        /MP
        /Gm-
        /external:anglebrackets
        /external:W0
        /external:templates-
        /GS
        /fp:precise
        /EHsc
        /Gd
        /WX-
        /errorReport:queue
        /FC
        /diagnostics:column
    )

    if(NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    endif()

    add_compile_definitions(
        WIN32 _WIN32 _WINDOWS UNICODE _UNICODE
        $<$<CONFIG:Debug>:QT_NO_CAST_TO_ASCII>
        $<$<CONFIG:Debug>:_CRT_SECURE_NO_WARNINGS>
        $<$<CONFIG:Debug>:_CRT_NONSTDC_NO_WARNINGS>
    )

    add_link_options(
        $<${_OT_CFG_RELEASE}:/OPT:REF>
        $<${_OT_CFG_RELEASE}:/OPT:ICF>
    )

endif()

# ------------------------------------------------------------
# internal helpers
# ------------------------------------------------------------
# Converts a target name like "OTFoo" to "OTFoo_core" for the internal object library
function(_ot_target_core_name OUT_VAR TARGET_NAME)
    set(${OUT_VAR} "${TARGET_NAME}_core" PARENT_SCOPE)
endfunction()

# _ot_add_debug_release_link_dirs(<target> <debug-dir> <release-dir>)
#
# Adds configuration-specific link directories to <target>.
#   - <debug-dir> is used only for Debug
#   - <release-dir> is used for Release, RelWithDebInfo, and MinSizeRel
#
# Both inputs are expected to be already resolved directory paths, not
# environment variable names.
#
# Example:
#   _ot_add_debug_release_link_dirs(OTViewer
#     "${OT_SYSTEM_DEBUG_LIB_DIR}"
#     "${OT_SYSTEM_RELEASE_LIB_DIR}"
#   )
function(_ot_add_debug_release_link_dirs TARGET_NAME DEBUG_DIR RELEASE_DIR)
    if(NOT "${DEBUG_DIR}" STREQUAL "")
        target_link_directories(${TARGET_NAME} PRIVATE
            $<${_OT_CFG_DEBUG}:${DEBUG_DIR}>
        )
    endif()

    if(NOT "${RELEASE_DIR}" STREQUAL "")
        target_link_directories(${TARGET_NAME} PRIVATE
            $<${_OT_CFG_RELEASE}:${RELEASE_DIR}>
        )
    endif()
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

    if(DEP_TOKEN STREQUAL "OTDataStorage")
        set(${OUT_VAR} "OT_DATASTORAGE_ROOT" PARENT_SCOPE)
        return()
    endif()

    if(DEP_TOKEN STREQUAL "OTModelEntities")
        set(${OUT_VAR} "OT_MODELENTITIES_ROOT" PARENT_SCOPE)
        return()
    endif()

    if(DEP_TOKEN STREQUAL "OTCADEntities")
        set(${OUT_VAR} "OT_CADMODELENTITIES_ROOT" PARENT_SCOPE)
        return()
    endif()

    if(DEP_TOKEN STREQUAL "UICore")
        set(${OUT_VAR} "OT_UICORE_ROOT" PARENT_SCOPE)
        return()
    endif()

    # The generic guess would strip the leading "OT" and produce OT_OOLKIT_ROOT.
    if(DEP_TOKEN STREQUAL "OToolkitAPI")
        set(${OUT_VAR} "OT_OTOOLKITAPI_ROOT" PARENT_SCOPE)
        return()
    endif()

    if(DEP_TOKEN STREQUAL "OToolkit")
        set(${OUT_VAR} "OT_OTOOLKIT_ROOT" PARENT_SCOPE)
        return()
    endif()

    if(DEP_TOKEN STREQUAL "OTFMC")
        set(${OUT_VAR} "OT_FILE_MANAGER_CONNECTOR_ROOT" PARENT_SCOPE)
        return()
    endif()

    _ot_guess_ot_root_envvar(_guess "${DEP_TOKEN}")

    # Fallback for tokens whose env var does not separate the words, e.g.
    # OTGuiAPI -> OT_GUIAPI_ROOT, where the guess would produce OT_GUI_API_ROOT.
    string(REGEX REPLACE "^OT" "" _name "${DEP_TOKEN}")
    string(TOUPPER "${_name}" _name_u)
    set(_candidate_split "OT_${_name_u}_ROOT")

    set(_candidates "")
    list(APPEND _candidates "${_guess}")
    list(APPEND _candidates "${_candidate_split}")

    foreach(_envName IN LISTS _candidates)
        if(DEFINED ENV{${_envName}} AND NOT "$ENV{${_envName}}" STREQUAL "")
            set(${OUT_VAR} "${_envName}" PARENT_SCOPE)
            return()
        endif()
    endforeach()

    set(${OUT_VAR} "${_guess}" PARENT_SCOPE)
endfunction()

# Subdirectory holding the public headers of an OT project, e.g. "include".
# OT_INC_PATH comes from OTEnvironment.cmake.
function(_ot_get_ot_inc_suffix OUT_VAR)
    if(NOT "${OT_INC_PATH}" STREQUAL "")
        set(${OUT_VAR} "${OT_INC_PATH}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "include" PARENT_SCOPE)
    endif()
endfunction()

function(_ot_apply_tp_to_core CORE_TARGET TP_NAME)
    ot_get_env_path(_inc  "${TP_NAME}_INC")
    ot_get_env_path(_incd "${TP_NAME}_INCD")
    ot_get_env_path(_incr "${TP_NAME}_INCR")

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
            $<${_OT_CFG_DEBUG}:${_incd}>
        )
    endif()

    if(NOT "${_incr}" STREQUAL "")
        target_include_directories("${CORE_TARGET}" PRIVATE
            $<${_OT_CFG_RELEASE}:${_incr}>
        )
    endif()
endfunction()

function(_ot_apply_tp_to_final FINAL_TARGET TP_NAME)
    ot_get_env_path(_libpathd "${TP_NAME}_LIBPATHD")
    ot_get_env_path(_libpathr "${TP_NAME}_LIBPATHR")

    if(NOT "${_libpathd}" STREQUAL "")
        target_link_directories("${FINAL_TARGET}" PRIVATE
            $<${_OT_CFG_DEBUG}:${_libpathd}>
        )
    endif()

    if(NOT "${_libpathr}" STREQUAL "")
        target_link_directories("${FINAL_TARGET}" PRIVATE
            $<${_OT_CFG_RELEASE}:${_libpathr}>
        )
    endif()

    # Preferred: per-config lib lists
    ot_get_env_list(_libd "${TP_NAME}_LIBD")
    ot_get_env_list(_libr "${TP_NAME}_LIBR")

    if(NOT "${_libd}" STREQUAL "")
        target_link_libraries("${FINAL_TARGET}" PRIVATE
            $<${_OT_CFG_DEBUG}:${_libd}>
        )
    endif()

    if(NOT "${_libr}" STREQUAL "")
        target_link_libraries("${FINAL_TARGET}" PRIVATE
            $<${_OT_CFG_RELEASE}:${_libr}>
        )
    endif()

    # Compatibility fallback: single list used for all configs
    if("${_libd}" STREQUAL "" AND "${_libr}" STREQUAL "")
        ot_get_env_list(_lib_any "${TP_NAME}_LIB")
        if("${_lib_any}" STREQUAL "")
            ot_get_env_list(_lib_any "${TP_NAME}_LIBS")
        endif()

        if(NOT "${_lib_any}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE "${_lib_any}")
        endif()
    endif()
endfunction()

# ------------------------------------------------------------
# initialize_lib / initialize_app
# ------------------------------------------------------------
function(_ot_initialize_target TARGET_NAME ROOT_PATH_VAR)
    if(NOT DEFINED ${ROOT_PATH_VAR} OR "${${ROOT_PATH_VAR}}" STREQUAL "")
        message(FATAL_ERROR
            "ot_initialize_lib/bin(${TARGET_NAME} ...): root path var '${ROOT_PATH_VAR}' is not set. "
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

    if("${ARGV2}" STREQUAL "PYTHON")
        target_compile_definitions(${_core} PRIVATE
            $<${_OT_CFG_DEBUG}:_RELEASEDEBUG>
            $<${_OT_CFG_RELEASE}:NDEBUG>
        )

        if(MSVC)
            target_compile_options(${_core} PRIVATE $<${_OT_CFG_DEBUG}:/Od>)
        endif()

        set_property(TARGET ${_core} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    else()
        target_compile_definitions(${_core} PRIVATE
            $<${_OT_CFG_DEBUG}:_DEBUG>
            $<${_OT_CFG_RELEASE}:NDEBUG>
        )
    endif()

    target_include_directories(${_core} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

    # Save metadata on the core target
    set_property(TARGET ${_core} PROPERTY OT_ROOT_VAR "${ROOT_PATH_VAR}")
    set_property(TARGET ${_core} PROPERTY OT_ROOT_PATH "${_rootPath}")
    set_property(TARGET ${_core} PROPERTY OT_DEPS "")

    # Set SubSystem for core target
    if(WIN32)
        set_property(TARGET ${_core} PROPERTY OT_SUBSYSTEM "WINDOWS")
    else()
        set_property(TARGET ${_core} PROPERTY OT_SUBSYSTEM "")
    endif()
    set_property(TARGET ${_core} PROPERTY OT_SUBSYSTEM_OVERRIDES "")

    # Add root include path using OT_INC suffix (or include)
    _ot_get_ot_inc_suffix(_incSuffix)

    target_include_directories(${_core} PRIVATE
        "${_rootPath}/${_incSuffix}"
    )
endfunction()

function(_ot_apply_export_macro TARGET_NAME EXPORT_MACRO)
    _ot_target_core_name(_core ${TARGET_NAME})
    if(NOT TARGET ${_core})
        message(FATAL_ERROR "_ot_apply_export_macro: core target '${_core}' not found.")
    endif()

    if(NOT "${EXPORT_MACRO}" STREQUAL "")
        set(_selected_macro "${EXPORT_MACRO}")
    else()
        string(REGEX REPLACE "^OT" "" _lib_raw "${TARGET_NAME}")
        string(TOUPPER "${_lib_raw}" _lib_upper)
        set(_selected_macro "OPENTWIN${_lib_upper}_EXPORTS")
    endif()

    target_compile_definitions(${_core} PRIVATE "${_selected_macro}")
    set_property(TARGET ${_core} PROPERTY OT_EXPORT_MACRO "${_selected_macro}")
    message(STATUS "[${TARGET_NAME}] Export Macro: ${_selected_macro}")
endfunction()

function(ot_initialize_lib TARGET_NAME ROOT_PATH_VAR)
    _ot_initialize_target(${TARGET_NAME} ${ROOT_PATH_VAR})
    _ot_target_core_name(_core ${TARGET_NAME})

    if(WIN32)
        target_compile_definitions(${_core} PRIVATE 
            _USRDLL
        )
    endif()

    _ot_apply_export_macro(${TARGET_NAME} "${ARGV2}")
endfunction()

function(ot_initialize_bin TARGET_NAME ROOT_PATH_VAR)
    _ot_initialize_target(${TARGET_NAME} ${ROOT_PATH_VAR})
    _ot_target_core_name(_core ${TARGET_NAME})
    set_property(TARGET ${_core} PROPERTY OT_IS_APP TRUE)

    if(NOT "${ARGV2}" STREQUAL "")
        _ot_apply_export_macro(${TARGET_NAME} "${ARGV2}")
    endif()
endfunction()

function(ot_initialize_bin_python TARGET_NAME ROOT_PATH_VAR)
    _ot_initialize_target(${TARGET_NAME} ${ROOT_PATH_VAR} PYTHON)
    _ot_target_core_name(_core ${TARGET_NAME})
    set_property(TARGET ${_core} PROPERTY OT_IS_APP TRUE)

    if(NOT "${ARGV2}" STREQUAL "")
        _ot_apply_export_macro(${TARGET_NAME} "${ARGV2}")
    endif()
endfunction()

function(ot_add_resources TARGET_NAME)
    _ot_target_core_name(_core ${TARGET_NAME})

    if(NOT TARGET ${_core})
        message(FATAL_ERROR "ot_add_resources: core target '${_core}' not found.")
    endif()

    file(GLOB_RECURSE _rc CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.rc"
    )

    file(GLOB_RECURSE _ico CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.ico"
    )

    set(_all_res "")
    list(APPEND _all_res ${_rc} ${_ico})

    if(_all_res)
        target_sources(${_core} PRIVATE ${_all_res})
    endif()
endfunction()

function(ot_deploy_app_configuration TARGET_NAME)
    set(_qtconf "${CMAKE_CURRENT_SOURCE_DIR}/qt.conf")
    if(EXISTS "${_qtconf}")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_qtconf}"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/qt.conf"
            COMMENT "[${TARGET_NAME}] Deploying qt.conf to binary directory"
        )
    endif()
endfunction()

# ------------------------------------------------------------
# Compiler definitons and options wrappers
# ------------------------------------------------------------
function(_ot_apply_compile_option_internal CORE_TARGET OPTION)
    if(MSVC)
        if(OPTION MATCHES "^[0-9]+$")
            target_compile_options(${CORE_TARGET} PRIVATE "/wd${OPTION}")
        else()
            target_compile_options(${CORE_TARGET} PRIVATE "${OPTION}")
        endif()
    else()
        target_compile_options(${CORE_TARGET} PRIVATE "${OPTION}")
    endif()
endfunction()

function(ot_add_compile_options TARGET_NAME)
    _ot_target_core_name(_core ${TARGET_NAME})
    foreach(opt IN LISTS ARGN)
        _ot_apply_compile_option_internal(${_core} "${opt}")
    endforeach()
endfunction()

function(ot_disable_warnings TARGET_NAME)
    _ot_target_core_name(_core ${TARGET_NAME})
    foreach(warn_id IN LISTS ARGN)
        if(NOT warn_id MATCHES "^[0-9]+$")
            message(AUTHOR_WARNING "ot_disable_warnings(${TARGET_NAME}): '${warn_id}' is not a numeric ID. Use ot_add_compile_options for flags.")
        endif()
        _ot_apply_compile_option_internal(${_core} "${warn_id}")
    endforeach()
endfunction()

function(ot_add_compile_definitions TARGET_NAME)
    _ot_target_core_name(_core ${TARGET_NAME})
    if(NOT TARGET ${_core})
        message(FATAL_ERROR "ot_add_compile_definitions: target '${_core}' not found.")
    endif()

    foreach(def IN LISTS ARGN)
        target_compile_definitions(${_core} PRIVATE "${def}")
    endforeach()
endfunction()

# Use the static CRT (/MT) in release and the default /MDd in debug. Matches the
# standalone tools that ship as self-contained executables.
function(ot_set_runtime_static_release TARGET_NAME)
    _ot_target_core_name(_core ${TARGET_NAME})
    if(NOT TARGET ${_core})
        message(FATAL_ERROR "ot_set_runtime_static_release: target '${_core}' not found.")
    endif()
    set_property(TARGET ${_core} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:DebugDLL>")
endfunction()

function(ot_add_qt_resources TARGET_NAME)
    _ot_target_core_name(_core ${TARGET_NAME})
    if(NOT TARGET ${_core})
        message(FATAL_ERROR "ot_add_qt_resources: target '${_core}' not found.")
    endif()
    file(GLOB_RECURSE _qrc CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc"
    )
    if(_qrc)
        set_property(TARGET ${_core} PROPERTY AUTORCC ON)
        target_sources(${_core} PRIVATE ${_qrc})
    endif()
endfunction()

function(ot_set_subsystem TARGET_NAME SUBSYSTEM)
    _ot_target_core_name(_core ${TARGET_NAME})
    if(NOT TARGET ${_core})
        message(FATAL_ERROR "ot_set_subsystem: target '${_core}' not found.")
    endif()

    string(TOUPPER "${SUBSYSTEM}" _subsystem)
    if(NOT (_subsystem STREQUAL "WINDOWS" OR _subsystem STREQUAL "CONSOLE"))
        message(FATAL_ERROR "ot_set_subsystem(${TARGET_NAME} ...): unsupported subsystem '${SUBSYSTEM}'. Use WINDOWS or CONSOLE.")
    endif()

    set_property(TARGET ${_core} PROPERTY OT_SUBSYSTEM "${_subsystem}")
endfunction()

function(ot_set_subsystem_for_config TARGET_NAME CONFIG_NAME SUBSYSTEM)
    _ot_target_core_name(_core ${TARGET_NAME})
    if(NOT TARGET ${_core})
        message(FATAL_ERROR "ot_set_subsystem_for_config: target '${_core}' not found.")
    endif()

    string(TOUPPER "${SUBSYSTEM}" _subsystem)
    if(NOT (_subsystem STREQUAL "WINDOWS" OR _subsystem STREQUAL "CONSOLE"))
        message(FATAL_ERROR "ot_set_subsystem_for_config(${TARGET_NAME} ...): unsupported subsystem '${SUBSYSTEM}'. Use WINDOWS or CONSOLE.")
    endif()

    string(TOUPPER "${CONFIG_NAME}" _config_upper)
    if(_config_upper STREQUAL "DEBUG")
        set(_config "Debug")
    elseif(_config_upper STREQUAL "RELEASE")
        set(_config "Release")
    elseif(_config_upper STREQUAL "RELWITHDEBINFO")
        set(_config "RelWithDebInfo")
    elseif(_config_upper STREQUAL "MINSIZEREL")
        set(_config "MinSizeRel")
    else()
        message(FATAL_ERROR
            "ot_set_subsystem_for_config(${TARGET_NAME} ...): unsupported config '${CONFIG_NAME}'. "
            "Use Debug, Release, RelWithDebInfo, or MinSizeRel."
        )
    endif()

    get_property(_overrides TARGET ${_core} PROPERTY OT_SUBSYSTEM_OVERRIDES)
    if(NOT _overrides)
        set(_overrides "")
    endif()

    set(_filtered_overrides "")
    foreach(_entry IN LISTS _overrides)
        string(FIND "${_entry}" ":" _sep)
        if(_sep GREATER -1)
            string(SUBSTRING "${_entry}" 0 ${_sep} _entry_config)
            if(NOT _entry_config STREQUAL "${_config}")
                list(APPEND _filtered_overrides "${_entry}")
            endif()
        endif()
    endforeach()

    list(APPEND _filtered_overrides "${_config}:${_subsystem}")
    set_property(TARGET ${_core} PROPERTY OT_SUBSYSTEM_OVERRIDES "${_filtered_overrides}")
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
    if(DEP STREQUAL "OTEncryptionKey")
        if(DEFINED ENV{OT_ENCRYPTIONKEY_ROOT} AND NOT "$ENV{OT_ENCRYPTIONKEY_ROOT}" STREQUAL "")
            target_include_directories("${CORE_TARGET}" PRIVATE "$ENV{OT_ENCRYPTIONKEY_ROOT}")
        endif()
        return()
    endif()

    # Skip PYTHON tokens here. Their guessed root OT_PYTHON_ROOT is a real env var and
    # would be mistaken for an OT library. They are resolved by the branches below.
    if(NOT (DEP STREQUAL "PYTHON" OR DEP STREQUAL "PYTHON_LEGACY"))
        _ot_get_ot_root_envvar(_rootVar "${DEP}")
        if(DEFINED ENV{${_rootVar}} AND NOT "$ENV{${_rootVar}}" STREQUAL "")
            _ot_get_ot_inc_suffix(_incSuffix)
            file(TO_CMAKE_PATH "$ENV{${_rootVar}}" _root)
            target_include_directories("${CORE_TARGET}" PRIVATE "${_root}/${_incSuffix}")

            if(DEP STREQUAL "OTRubberbandOSG")
                _ot_apply_dep_to_core("${CORE_TARGET}" "OSG")
            endif()
            return()
        endif()
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
        ot_define_qt6_targets("Core;Gui;Widgets;Network;Svg;Xml;OpenGLWidgets;SvgWidgets;Qml;WebSockets")
        target_link_libraries("${CORE_TARGET}" PRIVATE
            Qt6::Widgets
            Qt6::Network
            Qt6::Svg
            Qt6::Xml
            Qt6::OpenGLWidgets
            Qt6::SvgWidgets
            Qt6::Qml
            Qt6::WebSockets
        )
        return()
    endif()

    if(DEP STREQUAL "QtNetwork")
        ot_define_qt6_targets("Core;Network")
        target_link_libraries("${CORE_TARGET}" PRIVATE Qt6::Network)
        return()
    endif()

    if(DEP STREQUAL "QtGui")
        ot_define_qt6_targets("Core;Gui")
        target_link_libraries("${CORE_TARGET}" PRIVATE Qt6::Gui)
        return()
    endif()

    if(DEP STREQUAL "QtWebSockets")
        ot_define_qt6_targets("Core;Network;WebSockets")
        target_link_libraries("${CORE_TARGET}" PRIVATE Qt6::WebSockets)
        return()
    endif()

    if(DEP STREQUAL "QtEntryPoint")
        ot_define_qt6_targets("EntryPoint")
        target_link_libraries("${CORE_TARGET}" PRIVATE Qt6::EntryPoint)
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

    # QWT headers
    if(DEP STREQUAL "QWT")
        if(QWT_LIB_INCD_PATH)
            target_include_directories("${CORE_TARGET}" PRIVATE "${QWT_LIB_INCD_PATH}")
        endif()
        if(QWT_LIB_INCR_PATH AND NOT QWT_LIB_INCR_PATH STREQUAL QWT_LIB_INCD_PATH)
            target_include_directories("${CORE_TARGET}" PRIVATE "${QWT_LIB_INCR_PATH}")
        endif()
        return()
    endif()

    if(DEP STREQUAL "OCCT")
        if(OC_INCD_PATH)
            target_include_directories("${CORE_TARGET}" PRIVATE $<${_OT_CFG_DEBUG}:${OC_INCD_PATH}>)
        endif()
        if(OC_INCR_PATH)
            target_include_directories("${CORE_TARGET}" PRIVATE $<${_OT_CFG_RELEASE}:${OC_INCR_PATH}>)
        endif()
        return()
    endif()

    if(DEP STREQUAL "EARCUT")
        _ot_apply_tp_to_core("${CORE_TARGET}" "EARCUT")
        return()
    endif()

    if(DEP STREQUAL "BASE64")
        set(_b64_src "$ENV{BASE64_ROOT}/base64.c")
        set_source_files_properties("${_b64_src}" PROPERTIES LANGUAGE CXX)

        target_sources("${CORE_TARGET}" PRIVATE "${_b64_src}")
        target_include_directories("${CORE_TARGET}" PUBLIC "$ENV{BASE64_ROOT}")
        return()
    endif()

    if(DEP STREQUAL "EXPREVAL")
        if(NOT DEFINED ENV{EXPREVAL_ROOT} OR "$ENV{EXPREVAL_ROOT}" STREQUAL "")
            message(FATAL_ERROR "EXPREVAL token used but EXPREVAL_ROOT is not set.")
        endif()

        get_property(_enabled_langs GLOBAL PROPERTY ENABLED_LANGUAGES)
        list(FIND _enabled_langs "C" _c_lang_idx)
        if(_c_lang_idx EQUAL -1)
            enable_language(C)
        endif()

        set(_expr_src "$ENV{EXPREVAL_ROOT}/tinyexpr.c")
        set_source_files_properties("${_expr_src}" PROPERTIES LANGUAGE C)
        target_sources("${CORE_TARGET}" PRIVATE "${_expr_src}")

        if(DEFINED ENV{EXPREVAL_INC} AND NOT "$ENV{EXPREVAL_INC}" STREQUAL "")
            target_include_directories("${CORE_TARGET}" PUBLIC "$ENV{EXPREVAL_INC}")
        else()
            target_include_directories("${CORE_TARGET}" PUBLIC "$ENV{EXPREVAL_ROOT}")
        endif()
        return()
    endif()

    if(DEP STREQUAL "TINYXML2")
        set(_tinyxml2_src "$ENV{TINYXML2_ROOT}/tinyxml2.cpp")
        target_sources("${CORE_TARGET}" PRIVATE "${_tinyxml2_src}")
        target_include_directories("${CORE_TARGET}" PUBLIC "$ENV{TINYXML2_ROOT}")
        return()
    endif()

    if(DEP STREQUAL "MONGO_BOOST")
        ot_get_env_path(_mongo_boost_root "MONGO_BOOST_ROOT")
        if(NOT "${_mongo_boost_root}" STREQUAL "")
            target_include_directories("${CORE_TARGET}" PRIVATE "${_mongo_boost_root}")
        endif()
        return()
    endif()

    if(DEP STREQUAL "GMSH")
        if(GMSH_INC_PATH)
            target_include_directories("${CORE_TARGET}" PRIVATE "${GMSH_INC_PATH}")
        endif()
        return()
    endif()

    if(DEP STREQUAL "NGSPICE")
        if(NGSPICE_ROOT_PATH)
            target_include_directories("${CORE_TARGET}" PRIVATE "${NGSPICE_ROOT_PATH}/src/include")
        endif()
        return()
    endif()

    if(DEP STREQUAL "VTK")
        if(VTK_INC_LIST)
            target_include_directories("${CORE_TARGET}" PRIVATE ${VTK_INC_LIST})
        endif()
        return()
    endif()

    if(DEP STREQUAL "PYTHON")
        if(OT_PYTHON_INC_PATH)
            target_include_directories("${CORE_TARGET}" PRIVATE "${OT_PYTHON_INC_PATH}")
        endif()
        if(OT_PYTHON_NUMPY_INC_PATH)
            target_include_directories("${CORE_TARGET}" PRIVATE "${OT_PYTHON_NUMPY_INC_PATH}")
        endif()
        return()
    endif()

    if(DEP STREQUAL "PYTHON_LEGACY")
        if(OT_PYTHON_INC_LEGACY_PATH)
            target_include_directories("${CORE_TARGET}" PRIVATE "${OT_PYTHON_INC_LEGACY_PATH}")
        endif()
        if(OT_PYTHON_ROOT_LEGACY_PATH)
            target_include_directories("${CORE_TARGET}" PRIVATE "${OT_PYTHON_ROOT_LEGACY_PATH}/Lib/site-packages/numpy/core/include")
        endif()
        return()
    endif()

    if(DEP STREQUAL "BOOST_FILESYSTEM")
        _ot_apply_tp_to_core("${CORE_TARGET}" "BOOST")
        return()
    endif()

    ot_get_env_path(_inc  "${DEP}_INC")
    ot_get_env_path(_incd "${DEP}_INCD")
    ot_get_env_path(_incr "${DEP}_INCR")
    if(NOT ("${_inc}" STREQUAL "" AND "${_incd}" STREQUAL "" AND "${_incr}" STREQUAL ""))
        _ot_apply_tp_to_core("${CORE_TARGET}" "${DEP}")
        return()
    endif()

endfunction()

function(_ot_add_ot_dep_link_dirs FINAL_TARGET ROOT_DIR)
    if("${ROOT_DIR}" STREQUAL "")
        return()
    endif()

    if(OT_CLIBD_PATH AND OT_CLIBR_PATH)
        _ot_add_debug_release_link_dirs("${FINAL_TARGET}"
            "${ROOT_DIR}/${OT_CLIBD_PATH}"
            "${ROOT_DIR}/${OT_CLIBR_PATH}"
        )
    endif()

    if(OT_LIBD_PATH AND OT_LIBR_PATH)
        _ot_add_debug_release_link_dirs("${FINAL_TARGET}"
            "${ROOT_DIR}/${OT_LIBD_PATH}"
            "${ROOT_DIR}/${OT_LIBR_PATH}"
        )
    endif()

    if(OT_CDLLD_PATH AND OT_CDLLR_PATH)
        _ot_add_debug_release_link_dirs("${FINAL_TARGET}"
            "${ROOT_DIR}/${OT_CDLLD_PATH}"
            "${ROOT_DIR}/${OT_CDLLR_PATH}"
        )
    endif()
endfunction()

function(_ot_apply_dep_to_final FINAL_TARGET DEP)
    if(DEP STREQUAL "OTEncryptionKey")
        return()
    endif()

    # Skip PYTHON tokens here. Their guessed root OT_PYTHON_ROOT is a real env var and
    # would be mistaken for an OT library. They are resolved by the branches below.
    if(NOT (DEP STREQUAL "PYTHON" OR DEP STREQUAL "PYTHON_LEGACY"))
        _ot_get_ot_root_envvar(_rootVar "${DEP}")
        if(DEFINED ENV{${_rootVar}} AND NOT "$ENV{${_rootVar}}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE ${DEP})

            file(TO_CMAKE_PATH "$ENV{${_rootVar}}" _root)
            _ot_add_ot_dep_link_dirs("${FINAL_TARGET}" "${_root}")
            return()
        endif()
    endif()

    # Qt tokens
    if(DEP STREQUAL "QtCore")
        ot_define_qt6_targets("Core")
        target_link_libraries("${FINAL_TARGET}" PRIVATE Qt6::Core)
        return()
    endif()

    if(DEP STREQUAL "QtNetwork")
        ot_define_qt6_targets("Network")
        target_link_libraries("${FINAL_TARGET}" PRIVATE Qt6::Network)
        return()
    endif()

    if(DEP STREQUAL "QtGui")
        ot_define_qt6_targets("Gui")
        target_link_libraries("${FINAL_TARGET}" PRIVATE Qt6::Gui)
        return()
    endif()

    if(DEP STREQUAL "QtOpenGLWidgets")
        ot_define_qt6_targets("OpenGLWidgets")
        target_link_libraries("${FINAL_TARGET}" PRIVATE Qt6::OpenGLWidgets)
        return()
    endif()

    if(DEP STREQUAL "QtWidgets")
        ot_define_qt6_targets("Core;Gui;Widgets")
        target_link_libraries("${FINAL_TARGET}" PRIVATE Qt6::Widgets)
        return()
    endif()

    if(DEP STREQUAL "QtWebSockets")
        ot_define_qt6_targets("Core;Network;WebSockets")
        target_link_libraries("${FINAL_TARGET}" PRIVATE Qt6::WebSockets)
        return()
    endif()

    if(DEP STREQUAL "QtEntryPoint")
        if(MSVC)
            ot_define_qt6_targets("EntryPoint")
            target_link_libraries("${FINAL_TARGET}" PRIVATE Qt6::EntryPoint)
            return()
        endif()
    endif()

    if(DEP STREQUAL "QtFull")
        ot_define_qt6_targets("Core;Gui;Widgets;Network;Svg;Xml;OpenGLWidgets;SvgWidgets;Qml;WebSockets")
        target_link_libraries("${FINAL_TARGET}" PRIVATE
            Qt6::Widgets
            Qt6::Network
            Qt6::Svg
            Qt6::Xml
            Qt6::OpenGLWidgets
            Qt6::SvgWidgets
            Qt6::Qml
            Qt6::WebSockets
        )
        return()
    endif()

    # ADS libs
    if(DEP STREQUAL "ADS")
        if(QT_ADS_ROOT_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE "${QT_ADS_ROOT_PATH}/lib")
        endif()

        if(DEFINED ENV{QT_ADS_LIBD} AND NOT "$ENV{QT_ADS_LIBD}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_DEBUG}:$ENV{QT_ADS_LIBD}>)
        endif()
        if(DEFINED ENV{QT_ADS_LIBR} AND NOT "$ENV{QT_ADS_LIBR}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_RELEASE}:$ENV{QT_ADS_LIBR}>)
        endif()
        return()
    endif()

    # QWT libs
    if(DEP STREQUAL "QWT")
        if(QWT_LIB_LIBPATHD_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_DEBUG}:${QWT_LIB_LIBPATHD_PATH}>)
        endif()
        if(QWT_LIB_LIBPATHR_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_RELEASE}:${QWT_LIB_LIBPATHR_PATH}>)
        endif()

        if(QWT_LIB_LIBD_NAME AND NOT "${QWT_LIB_LIBD_NAME}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_DEBUG}:${QWT_LIB_LIBD_NAME}>)
        elseif(DEFINED ENV{QWT_LIB_LIBD} AND NOT "$ENV{QWT_LIB_LIBD}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_DEBUG}:$ENV{QWT_LIB_LIBD}>)
        endif()

        if(QWT_LIB_LIBR_NAME AND NOT "${QWT_LIB_LIBR_NAME}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_RELEASE}:${QWT_LIB_LIBR_NAME}>)
        elseif(DEFINED ENV{QWT_LIB_LIBR} AND NOT "$ENV{QWT_LIB_LIBR}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_RELEASE}:$ENV{QWT_LIB_LIBR}>)
        endif()

        return()
    endif()

    # OSG libs
    if(DEP STREQUAL "OSG")
        if(OSG_LIBPATHD_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_DEBUG}:${OSG_LIBPATHD_PATH}>)
        endif()
        if(OSG_LIBPATHR_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_RELEASE}:${OSG_LIBPATHR_PATH}>)
        endif()

        if(OSG_LIBD_NAME AND NOT "${OSG_LIBD_NAME}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_DEBUG}:${OSG_LIBD_NAME}>)
        endif()
        if(OSG_LIBR_NAME AND NOT "${OSG_LIBR_NAME}" STREQUAL "")
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_RELEASE}:${OSG_LIBR_NAME}>)
        endif()

        return()
    endif()

    if(DEP STREQUAL "OCCT")
        if(OC_LIBPATHD_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_DEBUG}:${OC_LIBPATHD_PATH}>)
        endif()
        if(OC_LIBPATHR_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_RELEASE}:${OC_LIBPATHR_PATH}>)
        endif()
        if(OC_LIBS_LIST)
            target_link_libraries("${FINAL_TARGET}" PRIVATE ${OC_LIBS_LIST})
        endif()
        return()
    endif()

    if(DEP STREQUAL "OSLibs")
        if(WIN32)
            set(_system_libs
                "userenv"
                "ws2_32"
                "advapi32"
                "shell32"
                "bcrypt"
                "secur32"
                "pdh"
                "odbc32"
            )
            target_link_libraries("${FINAL_TARGET}" PRIVATE ${_system_libs})
        endif()
        # No UNIX/Linux system libraries are required yet
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
    if(DEP STREQUAL "RJSON" OR DEP STREQUAL "BASE64" OR DEP STREQUAL "TINYXML2" OR DEP STREQUAL "EXPREVAL" OR DEP STREQUAL "MONGO_BOOST")
        return()
    endif()

    if(DEP STREQUAL "EARCUT")
        _ot_apply_tp_to_final("${FINAL_TARGET}" "EARCUT")
        return()
    endif()

    if(DEP STREQUAL "EMBREE")
        if(EMBREE_LIBPATHD_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_DEBUG}:${EMBREE_LIBPATHD_PATH}>)
        endif()
        if(EMBREE_LIBPATHR_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_RELEASE}:${EMBREE_LIBPATHR_PATH}>)
        endif()
        target_link_libraries("${FINAL_TARGET}" PRIVATE embree3.lib)
        return()
    endif()

    if(DEP STREQUAL "GMP")
        if(GMP_LIBPATHD_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_DEBUG}:${GMP_LIBPATHD_PATH}>)
        endif()
        if(GMP_LIBPATHR_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_RELEASE}:${GMP_LIBPATHR_PATH}>)
        endif()
        target_link_libraries("${FINAL_TARGET}" PRIVATE libgmp-10.lib)
        return()
    endif()

    if(DEP STREQUAL "CGAL")
        return()
    endif()

    if(DEP STREQUAL "GMSH")
        if(GMSH_LIBPATH_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE "${GMSH_LIBPATH_PATH}")
        endif()
        target_link_libraries("${FINAL_TARGET}" PRIVATE gmsh.lib)
        return()
    endif()

    if(DEP STREQUAL "NGSPICE")
        if(NGSPICE_LIBPATHD_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_DEBUG}:${NGSPICE_LIBPATHD_PATH}>)
        endif()
        if(NGSPICE_LIBPATHR_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_RELEASE}:${NGSPICE_LIBPATHR_PATH}>)
        endif()
        target_link_libraries("${FINAL_TARGET}" PRIVATE ngspice.lib)
        return()
    endif()

    if(DEP STREQUAL "VTK")
        if(VTK_LIB_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE "${VTK_LIB_PATH}")
        endif()
        if(VTK_LIBLIST_DEBUG_LIST)
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_DEBUG}:${VTK_LIBLIST_DEBUG_LIST}>)
        endif()
        if(VTK_LIBLIST_RELEASE_LIST)
            target_link_libraries("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_RELEASE}:${VTK_LIBLIST_RELEASE_LIST}>)
        endif()
        return()
    endif()

    if(DEP STREQUAL "PYTHON")
        if(OT_PYTHON_LIBPATH_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE "${OT_PYTHON_LIBPATH_PATH}/Release")
        endif()
        target_link_libraries("${FINAL_TARGET}" PRIVATE python311.lib)
        return()
    endif()

    if(DEP STREQUAL "PYTHON_LEGACY")
        return()
    endif()

    if(DEP STREQUAL "BOOST_FILESYSTEM")
        if(BOOST_LIBPATHD_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_DEBUG}:${BOOST_LIBPATHD_PATH}>)
        endif()
        if(BOOST_LIBPATHR_PATH)
            target_link_directories("${FINAL_TARGET}" PRIVATE $<${_OT_CFG_RELEASE}:${BOOST_LIBPATHR_PATH}>)
        endif()
        target_link_libraries("${FINAL_TARGET}" PRIVATE
            $<${_OT_CFG_DEBUG}:boost_filesystem-vc143-mt-gd-x64-1_86.lib>
            $<${_OT_CFG_RELEASE}:boost_filesystem-vc143-mt-x64-1_86.lib>
        )
        return()
    endif()

    ot_get_env_path(_libpathd "${DEP}_LIBPATHD")
    ot_get_env_path(_libpathr "${DEP}_LIBPATHR")
    if(NOT ("${_libpathd}" STREQUAL "" AND "${_libpathr}" STREQUAL ""))
        _ot_apply_tp_to_final("${FINAL_TARGET}" "${DEP}")
        return()
    endif()

    message(WARNING "ot_add_dependency(${FINAL_TARGET} ...): Unknown dependency token '${DEP}'")

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

function(_ot_apply_subsystem_if_requested TARGET_NAME CORE_NAME)
    if(NOT WIN32)
        return()
    endif()

    get_property(_subsystem TARGET ${CORE_NAME} PROPERTY OT_SUBSYSTEM)
    get_property(_overrides TARGET ${CORE_NAME} PROPERTY OT_SUBSYSTEM_OVERRIDES)

    if(NOT _overrides)
        set(_overrides "")
    endif()

    set(_overridden_configs "")
    foreach(_entry IN LISTS _overrides)
        string(FIND "${_entry}" ":" _sep)
        if(_sep GREATER -1)
            math(EXPR _value_start "${_sep} + 1")
            string(SUBSTRING "${_entry}" 0 ${_sep} _config)
            string(SUBSTRING "${_entry}" ${_value_start} -1 _entry_subsystem)

            if(NOT "${_config}" STREQUAL "" AND NOT "${_entry_subsystem}" STREQUAL "")
                list(APPEND _overridden_configs "${_config}")
                target_link_options(${TARGET_NAME} PRIVATE
                    "$<$<CONFIG:${_config}>:/SUBSYSTEM:${_entry_subsystem}>"
                )
            endif()
        endif()
    endforeach()

    if(NOT "${_subsystem}" STREQUAL "")
        foreach(_cfg IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
            if(NOT _cfg IN_LIST _overridden_configs)
                target_link_options(${TARGET_NAME} PRIVATE
                    "$<$<CONFIG:${_cfg}>:/SUBSYSTEM:${_subsystem}>"
                )
            endif()
        endforeach()
    endif()
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
    _ot_apply_subsystem_if_requested(${TARGET_NAME} ${_core})

    _ot_apply_all_deps(${TARGET_NAME} ${_core} "${_deps}")

    _ot_set_automoc_if_qt(${_core} "${_deps}")

    if(CMAKE_CURRENT_SOURCE_DIR MATCHES "/Services/")
        _ot_apply_service_debugger(${TARGET_NAME})
    endif()
endfunction()

function(ot_finalize_bin TARGET_NAME)
    _ot_target_core_name(_core ${TARGET_NAME})
    if(NOT TARGET ${_core})
        message(FATAL_ERROR "ot_finalize_bin: core target '${_core}' does not exist. Call ot_initialize_app first.")
    endif()

    set(_output_name "${ARGV1}")

    get_property(_deps TARGET ${_core} PROPERTY OT_DEPS)
    if(NOT _deps)
        set(_deps "")
    endif()

    add_executable(${TARGET_NAME} $<TARGET_OBJECTS:${_core}>)

    if(WIN32)
        get_property(_subsystem TARGET ${_core} PROPERTY OT_SUBSYSTEM)
        if("${_subsystem}" STREQUAL "CONSOLE")
            set_property(TARGET ${TARGET_NAME} PROPERTY WIN32_EXECUTABLE OFF)
        else()
            set_property(TARGET ${TARGET_NAME} PROPERTY WIN32_EXECUTABLE "${_OT_CFG_RELEASE}")
        endif()
    endif()

    if(NOT "${_output_name}" STREQUAL "")
        set_target_properties(${TARGET_NAME} PROPERTIES
            OUTPUT_NAME "${_output_name}"
        )
    endif()

    _ot_apply_subsystem_if_requested(${TARGET_NAME} ${_core})

    _ot_apply_all_deps(${TARGET_NAME} ${_core} "${_deps}")

    _ot_set_automoc_if_qt(${_core} "${_deps}")
endfunction()

function(ot_bin_use_release_qt TARGET_NAME)
    if(NOT MSVC)
        return()
    endif()
    foreach(_qtmod IN ITEMS Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network
                            Qt6::Svg Qt6::Xml Qt6::OpenGLWidgets Qt6::SvgWidgets Qt6::Qml Qt6::WebSockets)
        if(TARGET ${_qtmod})
            get_target_property(_qt_rel ${_qtmod} IMPORTED_LOCATION_RELEASE)
            if(_qt_rel)
                set_target_properties(${_qtmod} PROPERTIES IMPORTED_LOCATION_DEBUG "${_qt_rel}")
            endif()
        endif()
    endforeach()
endfunction()

function(ot_service_debug_launch TARGET_NAME)
    cmake_parse_arguments(PARSE_ARGV 1 _OTL "" "" "ARGS")
    set_property(GLOBAL PROPERTY OT_DEBUG_LAUNCH_ARGS_${TARGET_NAME} "${_OTL_ARGS}")
endfunction()

function(_ot_launch_arg_json OUT_VAR RAW)
    string(REGEX MATCHALL "@[A-Za-z_][A-Za-z0-9_]*@" _refs "${RAW}")
    foreach(_ref IN LISTS _refs)
        string(REGEX REPLACE "^@(.*)@$" "\\1" _name "${_ref}")
        if("$ENV{${_name}}" STREQUAL "")
            set(${OUT_VAR} "\\\"\\\"" PARENT_SCOPE)
            return()
        endif()
    endforeach()

    string(REPLACE "\\" "\\\\" _v "${RAW}")
    string(REPLACE "\"" "\\\"" _v "${_v}")
    string(REGEX REPLACE "@([A-Za-z_][A-Za-z0-9_]*)@" "\${env.\\1}" _v "${_v}")
    set(${OUT_VAR} "${_v}" PARENT_SCOPE)
endfunction()

function(_ot_service_launch_args_json OUT_VAR TARGET_NAME)
    get_property(_args GLOBAL PROPERTY OT_DEBUG_LAUNCH_ARGS_${TARGET_NAME})
    set(_s "")
    foreach(_arg IN LISTS _args)
        _ot_launch_arg_json(_arg "${_arg}")
        string(APPEND _s ", \"${_arg}\"")
    endforeach()
    set(${OUT_VAR} "${_s}" PARENT_SCOPE)
endfunction()

function(_ot_apply_service_debugger TARGET_NAME)
    if(NOT MSVC)
        return()
    endif()

    _ot_service_launch_args_json(OT_ARGS ${TARGET_NAME})

    # Build one configuration entry per build type from the entry template.
    file(READ "$ENV{OT_CMAKE_DIR}/templates/launch_entry_dll.vs.json.in" _entry_tpl)
    string(REPLACE "\r" "" _entry_tpl "${_entry_tpl}")
    string(REGEX REPLACE "\n+$" "" _entry_tpl "${_entry_tpl}")

    set(_entries "")
    foreach(_cfg IN ITEMS Debug Release)
        set(OT_TARGET "${TARGET_NAME}")
        set(OT_CFG "${_cfg}")

        # PATH matches the original .vcxproj.user
        if(_cfg STREQUAL "Debug")
            set(OT_SUB  "debug")
            set(OT_PATH "\${env.OT_ALL_DLLD};\${env.PATH}")
        else()
            set(OT_SUB  "release")
            set(OT_PATH "\${env.OT_ALL_DLLR};\${env.PATH}")
        endif()

        # Absolute path to the built service .dll, passed to the loader as argv[1]
        file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/build/windows-${OT_SUB}/${_cfg}/${TARGET_NAME}.dll" _dll)
        string(REPLACE "\\" "\\\\" OT_DLL "${_dll}")
        string(CONFIGURE "${_entry_tpl}" _entry @ONLY)
        if(_entries STREQUAL "")
            set(_entries "${_entry}")
        else()
            string(APPEND _entries ",\n${_entry}")
        endif()
    endforeach()

    set(OT_LAUNCH_ENTRIES "${_entries}")
    file(READ "$ENV{OT_CMAKE_DIR}/templates/launch.vs.json.in" _envelope)
    string(REPLACE "\r" "" _envelope "${_envelope}")
    string(CONFIGURE "${_envelope}" _content @ONLY)
    file(WRITE "${CMAKE_SOURCE_DIR}/.vs/launch.vs.json" "${_content}")
endfunction()

function(ot_bin_debug_launch TARGET_NAME)
    if(NOT MSVC)
        return()
    endif()

    cmake_parse_arguments(PARSE_ARGV 1 _OTB "" "PATH" "ARGS")

    if(DEFINED _OTB_PATH)
        set(_path "${_OTB_PATH}")
    else()
        set(_path "\${env.PATH}")
    endif()
    string(REPLACE "\\" "\\\\" _path "${_path}")

    set(_args "")
    foreach(_a IN LISTS _OTB_ARGS)
        _ot_launch_arg_json(_a "${_a}")
        if(_args STREQUAL "")
            set(_args "\"${_a}\"")
        else()
            string(APPEND _args ", \"${_a}\"")
        endif()
    endforeach()

    file(READ "$ENV{OT_CMAKE_DIR}/templates/launch_entry_bin.vs.json.in" _entry_tpl)
    string(REPLACE "\r" "" _entry_tpl "${_entry_tpl}")
    string(REGEX REPLACE "\n+$" "" _entry_tpl "${_entry_tpl}")

    set(_entries "")
    foreach(_cfg IN ITEMS Debug Release)
        set(OT_TARGET "${TARGET_NAME}")
        set(OT_CFG "${_cfg}")
        set(OT_PATH "${_path}")
        set(OT_ARGS "${_args}")
        string(CONFIGURE "${_entry_tpl}" _entry @ONLY)

        if(_entries STREQUAL "")
            set(_entries "${_entry}")
        else()
            string(APPEND _entries ",\n${_entry}")
        endif()

    endforeach()

    set(OT_LAUNCH_ENTRIES "${_entries}")
    file(READ "$ENV{OT_CMAKE_DIR}/templates/launch.vs.json.in" _envelope)
    string(REPLACE "\r" "" _envelope "${_envelope}")
    string(CONFIGURE "${_envelope}" _content @ONLY)
    file(WRITE "${CMAKE_SOURCE_DIR}/.vs/launch.vs.json" "${_content}")
endfunction()

function(_ot_tool_json_escape OUT_VAR RAW)
    string(REPLACE "\\" "\\\\" _v "${RAW}")
    string(REPLACE "\"" "\\\"" _v "${_v}")
    string(REGEX REPLACE "@([A-Za-z_][A-Za-z0-9_]*)@" "\${env.\\1}" _v "${_v}")
    set(${OUT_VAR} "${_v}" PARENT_SCOPE)
endfunction()

function(_ot_tool_args_json OUT_VAR LEADING)
    set(_s "")
    foreach(_arg IN LISTS ARGN)
        _ot_launch_arg_json(_arg "${_arg}")
        if(_s STREQUAL "" AND LEADING STREQUAL "")
            set(_s "\"${_arg}\"")
        else()
            string(APPEND _s ", \"${_arg}\"")
        endif()
    endforeach()
    set(${OUT_VAR} "${_s}" PARENT_SCOPE)
endfunction()

function(_ot_tool_launch_path OUT_VAR RAW FALLBACK)
    if("${RAW}" STREQUAL "")
        set(${OUT_VAR} "${FALLBACK}" PARENT_SCOPE)
        return()
    endif()
    _ot_tool_json_escape(_v "${RAW}")
    set(${OUT_VAR} "${_v}" PARENT_SCOPE)
endfunction()

function(_ot_tool_write_launch CONTENT)
    set(OT_LAUNCH_ENTRIES "${CONTENT}")
    file(READ "$ENV{OT_CMAKE_DIR}/templates/launch.vs.json.in" _envelope)
    string(REPLACE "\r" "" _envelope "${_envelope}")
    string(CONFIGURE "${_envelope}" _content @ONLY)
    file(WRITE "${CMAKE_SOURCE_DIR}/.vs/launch.vs.json" "${_content}")
endfunction()

function(ot_tool_debug_launch TARGET_NAME)
    if(NOT MSVC)
        return()
    endif()

    cmake_parse_arguments(PARSE_ARGV 1 _OTT "" "PATHD;PATHR" "ARGSD;ARGSR")

    file(READ "$ENV{OT_CMAKE_DIR}/templates/launch_entry_dll.vs.json.in" _entry_tpl)
    string(REPLACE "\r" "" _entry_tpl "${_entry_tpl}")
    string(REGEX REPLACE "\n+$" "" _entry_tpl "${_entry_tpl}")

    set(_entries "")
    foreach(_cfg IN ITEMS Debug Release)
        set(OT_TARGET "${TARGET_NAME}")
        set(OT_CFG "${_cfg}")

        if(_cfg STREQUAL "Debug")
            set(OT_SUB "debug")
            _ot_tool_args_json(OT_ARGS "," ${_OTT_ARGSD})
            _ot_tool_launch_path(OT_PATH "${_OTT_PATHD}" "\${env.OT_ALL_DLLD};\${env.PATH}")
        else()
            set(OT_SUB "release")
            _ot_tool_args_json(OT_ARGS "," ${_OTT_ARGSR})
            _ot_tool_launch_path(OT_PATH "${_OTT_PATHR}" "\${env.OT_ALL_DLLR};\${env.PATH}")
        endif()

        file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/build/windows-${OT_SUB}/${_cfg}/${TARGET_NAME}.dll" _dll)
        string(REPLACE "\\" "\\\\" OT_DLL "${_dll}")

        string(CONFIGURE "${_entry_tpl}" _entry @ONLY)
        if(_entries STREQUAL "")
            set(_entries "${_entry}")
        else()
            string(APPEND _entries ",\n${_entry}")
        endif()
    endforeach()

    _ot_tool_write_launch("${_entries}")
endfunction()

function(ot_tool_bin_debug_launch TARGET_NAME)
    if(NOT MSVC)
        return()
    endif()

    cmake_parse_arguments(PARSE_ARGV 1 _OTT "" "PATHD;PATHR" "ARGSD;ARGSR")

    file(READ "$ENV{OT_CMAKE_DIR}/templates/launch_entry_bin.vs.json.in" _entry_tpl)
    string(REPLACE "\r" "" _entry_tpl "${_entry_tpl}")
    string(REGEX REPLACE "\n+$" "" _entry_tpl "${_entry_tpl}")

    set(_entries "")
    foreach(_cfg IN ITEMS Debug Release)
        set(OT_TARGET "${TARGET_NAME}")
        set(OT_CFG "${_cfg}")

        if(_cfg STREQUAL "Debug")
            _ot_tool_args_json(OT_ARGS "" ${_OTT_ARGSD})
            _ot_tool_launch_path(OT_PATH "${_OTT_PATHD}" "\${env.PATH}")
        else()
            _ot_tool_args_json(OT_ARGS "" ${_OTT_ARGSR})
            _ot_tool_launch_path(OT_PATH "${_OTT_PATHR}" "\${env.PATH}")
        endif()

        string(CONFIGURE "${_entry_tpl}" _entry @ONLY)
        if(_entries STREQUAL "")
            set(_entries "${_entry}")
        else()
            string(APPEND _entries ",\n${_entry}")
        endif()
    endforeach()

    _ot_tool_write_launch("${_entries}")
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

    _ot_target_core_name(_coreName ${MAIN_TARGET_NAME})
    if(TARGET ${_coreName})
        target_sources(${TEST_TARGET_NAME} PRIVATE $<TARGET_OBJECTS:${_coreName}>)

        get_target_property(_export_macro ${_coreName} OT_EXPORT_MACRO)
        if(_export_macro)
            target_compile_definitions(${TEST_TARGET_NAME} PRIVATE "${_export_macro}")
        endif()

        get_target_property(_main_deps ${_coreName} OT_DEPS)
        if(_main_deps)
            _ot_apply_all_deps(${TEST_TARGET_NAME} ${_coreName} "${_main_deps}")
        endif()

        # Match the core runtime so test and core objects agree on /MD vs /MDd.
        get_target_property(_core_runtime ${_coreName} MSVC_RUNTIME_LIBRARY)
        if(_core_runtime)
            set_property(TARGET ${TEST_TARGET_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "${_core_runtime}")
        endif()

    elseif(TARGET ${MAIN_TARGET_NAME})
        target_link_libraries(${TEST_TARGET_NAME} PRIVATE ${MAIN_TARGET_NAME})
    else()
        message(FATAL_ERROR "ot_initialize_test: main target '${MAIN_TARGET_NAME}' not found.")
    endif()

    target_include_directories(${TEST_TARGET_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    target_include_directories(${TEST_TARGET_NAME} PRIVATE 
        $<TARGET_PROPERTY:${MAIN_TARGET_NAME},INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:${_coreName},INCLUDE_DIRECTORIES>
    )

    if(MSVC)
        target_link_options(${TEST_TARGET_NAME} PRIVATE "/ignore:4099")
    endif()

    if(DEFINED ENV{GOOGLE_TEST_INC} AND NOT "$ENV{GOOGLE_TEST_INC}" STREQUAL "")
        target_include_directories(${TEST_TARGET_NAME} PRIVATE "$ENV{GOOGLE_TEST_INC}")
    endif()
    if(DEFINED ENV{GOOGLE_TEST_LIB} AND NOT "$ENV{GOOGLE_TEST_LIB}" STREQUAL "")
        target_link_libraries(${TEST_TARGET_NAME} PRIVATE $ENV{GOOGLE_TEST_LIB})
    endif()
    # A Python core runs the release runtime in every config, so its test links the
    # release gtest even in debug to avoid a /MD vs /MDd clash.
    set(_test_release_gtest FALSE)
    if(DEFINED _core_runtime AND "${_core_runtime}" STREQUAL "MultiThreadedDLL")
        set(_test_release_gtest TRUE)
    endif()

    if(DEFINED ENV{GOOGLE_TEST_LIBPATHD} AND NOT "$ENV{GOOGLE_TEST_LIBPATHD}" STREQUAL "" AND NOT _test_release_gtest)
        target_link_directories(${TEST_TARGET_NAME} PRIVATE $<${_OT_CFG_DEBUG}:$ENV{GOOGLE_TEST_LIBPATHD}>)
    endif()
    if(DEFINED ENV{GOOGLE_TEST_LIBPATHR} AND NOT "$ENV{GOOGLE_TEST_LIBPATHR}" STREQUAL "")
        if(_test_release_gtest)
            target_link_directories(${TEST_TARGET_NAME} PRIVATE "$ENV{GOOGLE_TEST_LIBPATHR}")
        else()
            target_link_directories(${TEST_TARGET_NAME} PRIVATE $<${_OT_CFG_RELEASE}:$ENV{GOOGLE_TEST_LIBPATHR}>)
        endif()
    endif()

    add_test(NAME ${TEST_TARGET_NAME} COMMAND ${TEST_TARGET_NAME})
endfunction()

