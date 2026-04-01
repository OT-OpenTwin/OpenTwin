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

function(ot_assert_path_under_root PATH_VALUE ROOT_VALUE WHAT)
    if("${PATH_VALUE}" STREQUAL "" OR "${ROOT_VALUE}" STREQUAL "")
        message(FATAL_ERROR "ot_assert_path_under_root: empty path(s) for ${WHAT}")
    endif()

    set(_p "${PATH_VALUE}")
    set(_r "${ROOT_VALUE}")

    string(REPLACE "\"" "" _p "${_p}")
    string(REPLACE "\"" "" _r "${_r}")
    file(TO_CMAKE_PATH "${_p}" _p)
    file(TO_CMAKE_PATH "${_r}" _r)

    string(TOLOWER "${_p}" _p_l)
    string(TOLOWER "${_r}" _r_l)

    if(NOT _r_l MATCHES "/$")
        set(_r_l "${_r_l}/")
    endif()

    string(FIND "${_p_l}" "${_r_l}" _idx)
    if(NOT _idx EQUAL 0)
        message(FATAL_ERROR
            "${WHAT} is not under the expected ThirdParty root.\n"
            "  ${WHAT}: ${_p}\n"
            "  ThirdParty: ${_r}\n"
            "This likely means you're picking up a system Qt, which is not allowed."
        )
    endif()
endfunction()

function(ot_validate_qt_env)
    if(NOT DEFINED ENV{QT_INC} OR "$ENV{QT_INC}" STREQUAL "")
        message(FATAL_ERROR "QT_INC env var not set (SetupEnvironment.bat not applied?)")
    endif()

    if(NOT THIRDPARTY_ROOT_PATH)
        message(FATAL_ERROR "THIRDPARTY_ROOT_PATH not set (expected OPENTWIN_THIRDPARTY_ROOT)")
    endif()

    set(_qt_inc "$ENV{QT_INC}")
    file(TO_CMAKE_PATH "${_qt_inc}" _qt_inc)

    ot_assert_path_under_root("${_qt_inc}" "${THIRDPARTY_ROOT_PATH}" "QT_INC")

    if(NOT EXISTS "${_qt_inc}/QtCore/qglobal.h")
        message(FATAL_ERROR
            "QT_INC does not look like a Qt include directory.\n"
            "Expected to find: ${_qt_inc}/QtCore/qglobal.h"
        )
    endif()
endfunction()

# ------------------------------------------------------------
# Tell CMake's AUTOGEN where Qt tools are (moc/uic/rcc)
# AND provide Qt6::moc imported target (required by some CMake versions)
# ------------------------------------------------------------
function(ot_qt_setup_autogen_tools)
    if(NOT DEFINED ENV{QT_DLLR} OR "$ENV{QT_DLLR}" STREQUAL "")
        message(FATAL_ERROR "QT_DLLR not set; cannot locate Qt tools (moc/uic/rcc).")
    endif()

    file(TO_CMAKE_PATH "$ENV{QT_DLLR}" _qt_bin)

    set(_moc "${_qt_bin}/moc.exe")
    set(_uic "${_qt_bin}/uic.exe")
    set(_rcc "${_qt_bin}/rcc.exe")

    if(NOT EXISTS "${_moc}")
        message(FATAL_ERROR "Qt moc.exe not found at: ${_moc}")
    endif()

    # --- Imported tool targets expected by AUTOGEN in some CMake versions ---
    if(NOT TARGET Qt6::moc)
        add_executable(Qt6::moc IMPORTED GLOBAL)
        set_target_properties(Qt6::moc PROPERTIES
            IMPORTED_LOCATION "${_moc}"
        )
    endif()

    if(EXISTS "${_uic}" AND NOT TARGET Qt6::uic)
        add_executable(Qt6::uic IMPORTED GLOBAL)
        set_target_properties(Qt6::uic PROPERTIES
            IMPORTED_LOCATION "${_uic}"
        )
    endif()

    if(EXISTS "${_rcc}" AND NOT TARGET Qt6::rcc)
        add_executable(Qt6::rcc IMPORTED GLOBAL)
        set_target_properties(Qt6::rcc PROPERTIES
            IMPORTED_LOCATION "${_rcc}"
        )
    endif()

    # --- Variables used by different CMake AUTOGEN implementations ---
    set(Qt6Core_MOC_EXECUTABLE "${_moc}" CACHE FILEPATH "Qt moc executable" FORCE)
    set(QT_MOC_EXECUTABLE      "${_moc}" CACHE FILEPATH "Qt moc executable (legacy)" FORCE)

    set(CMAKE_AUTOMOC_EXECUTABLE "${_moc}" CACHE FILEPATH "CMake AUTOMOC executable" FORCE)

    if(EXISTS "${_uic}")
        set(Qt6Core_UIC_EXECUTABLE "${_uic}" CACHE FILEPATH "Qt uic executable" FORCE)
        set(QT_UIC_EXECUTABLE      "${_uic}" CACHE FILEPATH "Qt uic executable (legacy)" FORCE)
        set(CMAKE_AUTOUIC_EXECUTABLE "${_uic}" CACHE FILEPATH "CMake AUTOUIC executable" FORCE)
    endif()

    if(EXISTS "${_rcc}")
        set(Qt6Core_RCC_EXECUTABLE "${_rcc}" CACHE FILEPATH "Qt rcc executable" FORCE)
        set(QT_RCC_EXECUTABLE      "${_rcc}" CACHE FILEPATH "Qt rcc executable (legacy)" FORCE)
        set(CMAKE_AUTORCC_EXECUTABLE "${_rcc}" CACHE FILEPATH "CMake AUTORCC executable" FORCE)
    endif()
endfunction()

# ------------------------------------------------------------
# Force AUTOGEN recognition (avoid "No valid Qt version found")
# ------------------------------------------------------------
function(ot_qt_force_autogen_recognition)
    if(NOT DEFINED ENV{QTDIR} OR "$ENV{QTDIR}" STREQUAL "")
        message(STATUS "QTDIR not set; AUTOGEN Qt recognition may be incomplete.")
        return()
    endif()

    file(TO_CMAKE_PATH "$ENV{QTDIR}" _qt_root)

    set(QT_VERSION_MAJOR 6 CACHE STRING "Qt major version" FORCE)
    set(Qt6Core_VERSION_MAJOR 6 CACHE STRING "Qt6Core major version" FORCE)

    set(Qt6_FOUND TRUE CACHE BOOL "Qt6 found" FORCE)
    set(Qt6Core_FOUND TRUE CACHE BOOL "Qt6Core found" FORCE)

    set(Qt6_DIR "${_qt_root}" CACHE PATH "Qt6 root dir" FORCE)
    set(Qt6Core_DIR "${_qt_root}" CACHE PATH "Qt6Core root dir" FORCE)
endfunction()

# ------------------------------------------------------------
# Qt imported targets (NO find_package)
# ------------------------------------------------------------
function(_ot_qt_get_libdir_release OUT_VAR)
    if(DEFINED ENV{QT_LIBPATHR} AND NOT "$ENV{QT_LIBPATHR}" STREQUAL "")
        file(TO_CMAKE_PATH "$ENV{QT_LIBPATHR}" _d)
        set(${OUT_VAR} "${_d}" PARENT_SCOPE)
        return()
    endif()

    if(DEFINED ENV{QT_LIBPATH} AND NOT "$ENV{QT_LIBPATH}" STREQUAL "")
        file(TO_CMAKE_PATH "$ENV{QT_LIBPATH}" _d)
        set(${OUT_VAR} "${_d}" PARENT_SCOPE)
        return()
    endif()

    message(FATAL_ERROR
        "No Qt lib path env var found. Expected QT_LIBPATH (or QT_LIBPATHR). "
        "Your third-party environment should set QT_LIBPATH=%QDIR%\\lib."
    )
endfunction()

function(_ot_qt_get_libdir_debug OUT_VAR)
    if(DEFINED ENV{QT_LIBPATHD} AND NOT "$ENV{QT_LIBPATHD}" STREQUAL "")
        file(TO_CMAKE_PATH "$ENV{QT_LIBPATHD}" _d)
        set(${OUT_VAR} "${_d}" PARENT_SCOPE)
        return()
    endif()

    if(DEFINED ENV{QT_LIBPATH} AND NOT "$ENV{QT_LIBPATH}" STREQUAL "")
        file(TO_CMAKE_PATH "$ENV{QT_LIBPATH}" _d)
        set(${OUT_VAR} "${_d}" PARENT_SCOPE)
        return()
    endif()

    set(${OUT_VAR} "" PARENT_SCOPE)
endfunction()

function(_ot_qt_module_include_subdir OUT_VAR MODULE_NAME)
    if(MODULE_NAME STREQUAL "Core")
        set(${OUT_VAR} "QtCore" PARENT_SCOPE)
    elseif(MODULE_NAME STREQUAL "Gui")
        set(${OUT_VAR} "QtGui" PARENT_SCOPE)
    elseif(MODULE_NAME STREQUAL "Widgets")
        set(${OUT_VAR} "QtWidgets" PARENT_SCOPE)
    elseif(MODULE_NAME STREQUAL "Network")
        set(${OUT_VAR} "QtNetwork" PARENT_SCOPE)
    elseif(MODULE_NAME STREQUAL "Svg")
        set(${OUT_VAR} "QtSvg" PARENT_SCOPE)
    elseif(MODULE_NAME STREQUAL "Xml")
        set(${OUT_VAR} "QtXml" PARENT_SCOPE)
    elseif(MODULE_NAME STREQUAL "OpenGLWidgets")
        set(${OUT_VAR} "QtOpenGLWidgets" PARENT_SCOPE)
    elseif(MODULE_NAME STREQUAL "SvgWidgets")
        set(${OUT_VAR} "QtSvgWidgets" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

function(ot_import_qt6_module MODULE_NAME LIB_BASENAME)
    ot_validate_qt_env()

    file(TO_CMAKE_PATH "$ENV{QT_INC}" _qt_inc)

    _ot_qt_get_libdir_release(_qt_libr)
    ot_assert_path_under_root("${_qt_libr}" "${THIRDPARTY_ROOT_PATH}" "QT_LIBPATH")

    set(_release_lib "${_qt_libr}/${LIB_BASENAME}.lib")
    if(NOT EXISTS "${_release_lib}")
        message(FATAL_ERROR
            "Qt library not found: ${_release_lib}\n"
            "Check that QT_LIBPATH points to the folder containing Qt6*.lib."
        )
    endif()

    _ot_qt_module_include_subdir(_qt_mod_inc "${MODULE_NAME}")

    if(NOT TARGET Qt6::${MODULE_NAME})
        add_library(Qt6::${MODULE_NAME} UNKNOWN IMPORTED)
        set_target_properties(Qt6::${MODULE_NAME} PROPERTIES
            IMPORTED_LOCATION_RELEASE "${_release_lib}"
            IMPORTED_LOCATION_RELWITHDEBINFO "${_release_lib}"
            IMPORTED_LOCATION_MINSIZEREL "${_release_lib}"
        )

        if(NOT "${_qt_mod_inc}" STREQUAL "")
            target_include_directories(Qt6::${MODULE_NAME} INTERFACE
                "${_qt_inc}"
                "${_qt_inc}/${_qt_mod_inc}"
            )
        else()
            target_include_directories(Qt6::${MODULE_NAME} INTERFACE
                "${_qt_inc}"
            )
        endif()
    endif()

    _ot_qt_get_libdir_debug(_qt_libd)
    if(NOT "${_qt_libd}" STREQUAL "")
        ot_assert_path_under_root("${_qt_libd}" "${THIRDPARTY_ROOT_PATH}" "QT_LIBPATH(D)")
        set(_debug_lib "${_qt_libd}/${LIB_BASENAME}d.lib")
        if(EXISTS "${_debug_lib}")
            set_target_properties(Qt6::${MODULE_NAME} PROPERTIES
                IMPORTED_LOCATION_DEBUG "${_debug_lib}"
            )
        else()
            set_target_properties(Qt6::${MODULE_NAME} PROPERTIES
                IMPORTED_LOCATION_DEBUG "${_release_lib}"
            )
        endif()
    else()
        set_target_properties(Qt6::${MODULE_NAME} PROPERTIES
            IMPORTED_LOCATION_DEBUG "${_release_lib}"
        )
    endif()
endfunction()

function(ot_define_qt6_targets COMPONENTS_LIST)
    ot_qt_setup_autogen_tools()
    ot_qt_force_autogen_recognition()

    foreach(c IN LISTS COMPONENTS_LIST)
        if(TARGET Qt6::${c})
            get_target_property(_already_defined Qt6::${c} OT_Qt_INITIALIZED)
            if(_already_defined)
                continue()
            endif()
        endif()

        if(c STREQUAL "Core")
            ot_import_qt6_module(Core Qt6Core)

        elseif(c STREQUAL "Gui")
            ot_import_qt6_module(Gui Qt6Gui)
            if(TARGET Qt6::Core)
                target_link_libraries(Qt6::Gui INTERFACE Qt6::Core)
            endif()

        elseif(c STREQUAL "Widgets")
            ot_import_qt6_module(Widgets Qt6Widgets)
            if(TARGET Qt6::Core AND TARGET Qt6::Gui)
                target_link_libraries(Qt6::Widgets INTERFACE Qt6::Core Qt6::Gui)
            endif()

        elseif(c STREQUAL "Network")
            ot_import_qt6_module(Network Qt6Network)
            if(TARGET Qt6::Core)
                target_link_libraries(Qt6::Network INTERFACE Qt6::Core)
            endif()

        elseif(c STREQUAL "Svg")
            ot_import_qt6_module(Svg Qt6Svg)
            if(TARGET Qt6::Core AND TARGET Qt6::Gui)
                target_link_libraries(Qt6::Svg INTERFACE Qt6::Core Qt6::Gui)
            endif()

        elseif(c STREQUAL "Xml")
            ot_import_qt6_module(Xml Qt6Xml)
            if(TARGET Qt6::Core)
                target_link_libraries(Qt6::Xml INTERFACE Qt6::Core)
            endif()

        elseif(c STREQUAL "OpenGLWidgets")
            ot_import_qt6_module(OpenGLWidgets Qt6OpenGLWidgets)
            if(TARGET Qt6::Core AND TARGET Qt6::Gui AND TARGET Qt6::Widgets)
                target_link_libraries(Qt6::OpenGLWidgets INTERFACE Qt6::Core Qt6::Gui Qt6::Widgets)
            endif()

        elseif(c STREQUAL "SvgWidgets")
            ot_import_qt6_module(SvgWidgets Qt6SvgWidgets)
            if(TARGET Qt6::Core AND TARGET Qt6::Gui AND TARGET Qt6::Widgets AND TARGET Qt6::Svg)
                target_link_libraries(Qt6::SvgWidgets INTERFACE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg)
            endif()

        else()
            message(FATAL_ERROR "ot_define_qt6_targets: unsupported Qt component '${c}'")
        endif()

        if(TARGET Qt6::${c})
            set_target_properties(Qt6::${c} PROPERTIES OT_Qt_INITIALIZED TRUE)
        endif()
    endforeach()
endfunction()
