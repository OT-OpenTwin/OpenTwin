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

# Platform and toolchain specifics, used by OTProject.cmake and OTQt.cmake
# instead of testing WIN32/MSVC inline. Third-party library names come from
# the ThirdParty SetupEnvironment, not from here.

set(OT_PLATFORM_WINDOWS FALSE)
set(OT_PLATFORM_LINUX   FALSE)
set(OT_PLATFORM_MACOS   FALSE)

if(WIN32)
    set(OT_PLATFORM_WINDOWS TRUE)
    set(OT_PLATFORM "windows")
elseif(APPLE)
    set(OT_PLATFORM_MACOS TRUE)
    set(OT_PLATFORM "macos")
elseif(UNIX)
    set(OT_PLATFORM_LINUX TRUE)
    set(OT_PLATFORM "linux")
else()
    set(OT_PLATFORM "unknown")
endif()

set(OT_TOOLCHAIN_MSVC  FALSE)
set(OT_TOOLCHAIN_GCC   FALSE)
set(OT_TOOLCHAIN_CLANG FALSE)

if(MSVC)
    set(OT_TOOLCHAIN_MSVC TRUE)
    set(OT_TOOLCHAIN "msvc")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(OT_TOOLCHAIN_GCC TRUE)
    set(OT_TOOLCHAIN "gcc")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(OT_TOOLCHAIN_CLANG TRUE)
    set(OT_TOOLCHAIN "clang")
else()
    set(OT_TOOLCHAIN "unknown")
endif()

message(STATUS "[OT] ${OT_PLATFORM} / ${OT_TOOLCHAIN} ${CMAKE_CXX_COMPILER_VERSION}")

if(OT_PLATFORM_WINDOWS)
    set(OT_EXE_SUFFIX ".exe")
else()
    set(OT_EXE_SUFFIX "")
endif()

# Resolved by the OSLibs dependency token.
if(OT_PLATFORM_WINDOWS)
    set(OT_SYSTEM_LIBS
        "userenv"
        "ws2_32"
        "advapi32"
        "shell32"
        "bcrypt"
        "secur32"
        "pdh"
        "odbc32"
    )
else()
    # TODO(linux): usually pthread/dl/m/rt. Empty keeps OSLibs a no-op.
    set(OT_SYSTEM_LIBS "")
endif()

# MSVC warning ID -> GCC flag that turns off the same warning
set(_OT_GCC_WARNING_FLAGS
    "4996:-Wno-deprecated-declarations"
)

function(_ot_platform_gcc_warning_flag OUT_VAR WARNING_ID)
    set(_flag "")
    foreach(_entry IN LISTS _OT_GCC_WARNING_FLAGS)
        if(_entry MATCHES "^${WARNING_ID}:(.+)$")
            set(_flag "${CMAKE_MATCH_1}")
        endif()
    endforeach()
    set(${OUT_VAR} "${_flag}" PARENT_SCOPE)
endfunction()

# Numeric MSVC warning ID -> disable flag, empty where there is no equivalent.
function(ot_platform_warning_option OUT_VAR WARNING_ID)
    if(OT_TOOLCHAIN_MSVC)
        set(${OUT_VAR} "/wd${WARNING_ID}" PARENT_SCOPE)
    else()
        _ot_platform_gcc_warning_flag(_flag "${WARNING_ID}")
        set(${OUT_VAR} "${_flag}" PARENT_SCOPE)
    endif()
endfunction()

# MSVC compile flag -> equivalent for the active toolchain, empty where there is none.
function(ot_platform_compile_option OUT_VAR OPTION)
    _ot_platform_gcc_compile_option(_gcc "${OPTION}")
    if(OT_TOOLCHAIN_MSVC)
        set(${OUT_VAR} "${OPTION}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "${_gcc}" PARENT_SCOPE)
    endif()
endfunction()

function(_ot_platform_gcc_compile_option OUT_VAR OPTION)
    # $<condition:flag>: the flag is translated, the condition is kept
    if(OPTION MATCHES "^\\$<(\\$<[^<>]+>):(.+)>$")
        set(_condition "${CMAKE_MATCH_1}")
        _ot_platform_gcc_compile_option(_inner "${CMAKE_MATCH_2}")
        set(_wrapped "")
        foreach(_flag IN LISTS _inner)
            list(APPEND _wrapped "$<${_condition}:${_flag}>")
        endforeach()
        set(${OUT_VAR} "${_wrapped}" PARENT_SCOPE)
        return()
    endif()

    if(OPTION MATCHES "^\\$<" AND OPTION MATCHES "[:,]/")
        message(FATAL_ERROR
            "ot_platform_compile_option: MSVC flag inside an unsupported generator expression '${OPTION}'. "
            "Use the form $<$<CONFIG:...>:/flag> or add the case in OTPlatform.cmake."
        )
    endif()

    if(NOT OPTION MATCHES "^/")
        set(${OUT_VAR} "${OPTION}" PARENT_SCOPE)
        return()
    endif()

    if(OPTION STREQUAL "/utf-8")
        set(${OUT_VAR} "-finput-charset=UTF-8;-fexec-charset=UTF-8" PARENT_SCOPE)
    elseif(OPTION MATCHES "^/source-charset:(.+)$")
        set(${OUT_VAR} "-finput-charset=${CMAKE_MATCH_1}" PARENT_SCOPE)
    elseif(OPTION MATCHES "^/execution-charset:(.+)$")
        set(${OUT_VAR} "-fexec-charset=${CMAKE_MATCH_1}" PARENT_SCOPE)
    elseif(OPTION MATCHES "^/wd([0-9]+)$")
        _ot_platform_gcc_warning_flag(_warning "${CMAKE_MATCH_1}")
        set(${OUT_VAR} "${_warning}" PARENT_SCOPE)
    else()
        message(FATAL_ERROR
            "ot_platform_compile_option: no GCC equivalent for the MSVC flag '${OPTION}'. "
            "Add it in OTPlatform.cmake."
        )
    endif()
endfunction()

# "WINLIB:<name>" -> linkable name, empty off Windows. Token name kept so
# existing CMakeLists need no change; a POSIXLIB: form can be added here.
function(ot_platform_system_lib OUT_VAR TOKEN)
    set(${OUT_VAR} "" PARENT_SCOPE)

    if(TOKEN MATCHES "^WINLIB:(.+)$")
        if(NOT OT_PLATFORM_WINDOWS)
            return()
        endif()

        set(_name "${CMAKE_MATCH_1}")
        if(NOT _name MATCHES "^[A-Za-z0-9_.-]+$")
            message(FATAL_ERROR "Invalid WINLIB name '${_name}' from token '${TOKEN}'")
        endif()

        set(${OUT_VAR} "${_name}" PARENT_SCOPE)
    endif()
endfunction()

# TRUE for a system-lib token whether or not it resolves on this platform.
function(ot_platform_is_system_lib_token OUT_VAR TOKEN)
    if(TOKEN MATCHES "^WINLIB:")
        set(${OUT_VAR} TRUE PARENT_SCOPE)
    else()
        set(${OUT_VAR} FALSE PARENT_SCOPE)
    endif()
endfunction()

# Qt module library file name, CONFIG is DEBUG or RELEASE.
function(ot_platform_qt_lib OUT_VAR BASENAME CONFIG)
    if(OT_PLATFORM_WINDOWS)
        if(CONFIG STREQUAL "DEBUG")
            set(${OUT_VAR} "${BASENAME}d.lib" PARENT_SCOPE)
        else()
            set(${OUT_VAR} "${BASENAME}.lib" PARENT_SCOPE)
        endif()
        return()
    endif()

    # TODO(linux): not guessed, naming depends on how Qt is provided.
    message(FATAL_ERROR
        "ot_platform_qt_lib: Qt library naming is not defined for platform "
        "'${OT_PLATFORM}'. Add the rule in OTPlatform.cmake."
    )
endfunction()

function(ot_platform_tool OUT_VAR DIRECTORY STEM)
    set(${OUT_VAR} "${DIRECTORY}/${STEM}${OT_EXE_SUFFIX}" PARENT_SCOPE)
endfunction()
