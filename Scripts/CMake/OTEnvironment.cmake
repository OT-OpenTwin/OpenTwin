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

# Centralized environment parsing for OpenTwin subprojects
# Purpose:
#   - Used by OTProject.cmake to help parse include and link setup
#   - Read environment variables set by SetupEnvironment.bat
#   - Expose parsed values as regular CMake variables
#
#
# Notes:
#   - Helpers that use ENV{...} expect environment variable names, not CMake variable names.
#   - Variables ending in _PATH are normal CMake variables already parsed from the environment.
#   - Project-specific target wiring should remain in the consuming CMakeLists.txt.

include_guard(GLOBAL)

# ============================================================
# helpers
# ============================================================

# ot_get_env_path(<out-var> <env-name>)
#
# Reads environment variable <env-name>, removes surrounding quote characters,
# converts the value to CMake path format, and stores the result in <out-var>
# in the caller scope.
#
# Use for environment variables that represent a single filesystem path.
#
# Example:
#   ot_get_env_path(OT_SYSTEM_ROOT_PATH OT_SYSTEM_ROOT)
function(ot_get_env_path OUT_VAR ENV_NAME)
    if(DEFINED ENV{${ENV_NAME}} AND NOT "$ENV{${ENV_NAME}}" STREQUAL "")
        set(_val "$ENV{${ENV_NAME}}")
        string(REPLACE "\"" "" _val "${_val}")
        file(TO_CMAKE_PATH "${_val}" _val)
        set(${OUT_VAR} "${_val}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

# ot_get_env_string(<out-var> <env-name>)
#
# Reads environment variable <env-name> as a plain string, removes quote
# characters, and stores the result in <out-var> in the caller scope.
#
# Use for environment variables that are not filesystem paths, e.g. library
# names such as OSG_LIBD or MONGO_C_LIB.
#
# Example:
#   ot_get_env_string(OSG_LIBD_NAME OSG_LIBD)
function(ot_get_env_string OUT_VAR ENV_NAME)
    if(DEFINED ENV{${ENV_NAME}} AND NOT "$ENV{${ENV_NAME}}" STREQUAL "")
        set(_val "$ENV{${ENV_NAME}}")
        string(REPLACE "\"" "" _val "${_val}")
        set(${OUT_VAR} "${_val}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

# ot_get_env_list(<out-var> <env-name>)
#
# Reads environment variable <env-name> as a semicolon-separated CMake list,
# removes quote characters, and stores the resulting list string in <out-var>
# in the caller scope.
#
# This helper does not normalize path separators.
#
# Example:
#   ot_get_env_list(OT_DEFAULT_SERVICE_LIBD_LIST OT_DEFAULT_SERVICE_LIBD)
function(ot_get_env_list OUT_VAR ENV_NAME)
    if(DEFINED ENV{${ENV_NAME}} AND NOT "$ENV{${ENV_NAME}}" STREQUAL "")
        set(_raw "$ENV{${ENV_NAME}}")
        string(REPLACE "\"" "" _raw "${_raw}")
        set(${OUT_VAR} "${_raw}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

# ot_get_env_path_list(<out-var> <env-name>)
#
# Reads environment variable <env-name> as a semicolon-separated list of paths,
# removes quote characters, normalizes each entry to CMake path format, and
# stores the resulting list in <out-var> in the caller scope.
#
# Use for environment variables that contain multiple directories.
#
# Example:
#   ot_get_env_path_list(OT_ALL_DLLD_LIST OT_ALL_DLLD)
function(ot_get_env_path_list OUT_VAR ENV_NAME)
    if(DEFINED ENV{${ENV_NAME}} AND NOT "$ENV{${ENV_NAME}}" STREQUAL "")
        set(_raw "$ENV{${ENV_NAME}}")
        string(REPLACE "\"" "" _raw "${_raw}")

        set(_norm "")
        foreach(_p IN LISTS _raw)
            if(NOT _p STREQUAL "")
                file(TO_CMAKE_PATH "${_p}" _p)
                list(APPEND _norm "${_p}")
            endif()
        endforeach()

        set(${OUT_VAR} "${_norm}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

# ot_join_paths(<out-var> <base> <subdir>)
#
# Creates "<base>/<subdir>", normalizes it to CMake path format, and stores the
# result in <out-var> in the caller scope.
#
# If either <base> or <subdir> is empty, <out-var> is set to an empty string.
#
# Typical use:
#   Build derived debug/release include/lib directories from a root path and one
#   of the standard OT_* subdirectory variables such as OT_CLIBD_PATH.
#
# Example:
#   ot_join_paths(OT_SYSTEM_DEBUG_LIB_DIR "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBD_PATH}")
function(ot_join_paths OUT_VAR BASE SUBDIR)
    if(NOT "${BASE}" STREQUAL "" AND NOT "${SUBDIR}" STREQUAL "")
        set(_p "${BASE}/${SUBDIR}")
        file(TO_CMAKE_PATH "${_p}" _p)
        set(${OUT_VAR} "${_p}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

# ot_add_debug_release_link_dirs(<target> <debug-dir> <release-dir>)
#
# Adds configuration-specific link directories to <target>.
#   - <debug-dir> is used only for Debug
#   - <release-dir> is used for Release, RelWithDebInfo, and MinSizeRel
#
# Both inputs are expected to be already resolved directory paths, not
# environment variable names.
#
# Example:
#   ot_add_debug_release_link_dirs(OTViewer
#     "${OT_SYSTEM_DEBUG_LIB_DIR}"
#     "${OT_SYSTEM_RELEASE_LIB_DIR}"
#   )
function(ot_add_debug_release_link_dirs TARGET_NAME DEBUG_DIR RELEASE_DIR)
    if(NOT "${DEBUG_DIR}" STREQUAL "")
        target_link_directories(${TARGET_NAME} PRIVATE
            $<$<CONFIG:Debug>:${DEBUG_DIR}>
        )
    endif()

    if(NOT "${RELEASE_DIR}" STREQUAL "")
        target_link_directories(${TARGET_NAME} PRIVATE
            $<$<CONFIG:Release>:${RELEASE_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${RELEASE_DIR}>
            $<$<CONFIG:MinSizeRel>:${RELEASE_DIR}>
        )
    endif()
endfunction()

# ============================================================
# setup checks
# ============================================================
if(NOT DEFINED ENV{OPENTWIN_DEV_ENV_DEFINED} OR NOT "$ENV{OPENTWIN_DEV_ENV_DEFINED}" STREQUAL "1")
    message(WARNING
        "OPENTWIN_DEV_ENV_DEFINED is not 1. Did you run SetupEnvironment.bat before configuring CMake?"
    )
endif()

if(NOT DEFINED ENV{OT_CMAKE_DIR} OR "$ENV{OT_CMAKE_DIR}" STREQUAL "")
    message(STATUS "OT_CMAKE_DIR is not set")
endif()

# ============================================================
# common OpenTwin subdir vars from SetupEnvironment.bat
# ============================================================
ot_get_env_path(OT_INC_PATH                       OT_INC)
ot_get_env_path(OT_SRC_PATH                       OT_SRC)
ot_get_env_path(OT_DLLD_PATH                      OT_DLLD)
ot_get_env_path(OT_DLLR_PATH                      OT_DLLR)
ot_get_env_path(OT_LIBD_PATH                      OT_LIBD)
ot_get_env_path(OT_LIBR_PATH                      OT_LIBR)
ot_get_env_path(OT_CDLLD_PATH                     OT_CDLLD)
ot_get_env_path(OT_CDLLR_PATH                     OT_CDLLR)
ot_get_env_path(OT_CLIBD_PATH                     OT_CLIBD)
ot_get_env_path(OT_CLIBR_PATH                     OT_CLIBR)
ot_get_env_path(OT_TEST_PATH                      OT_TEST)
ot_get_env_path(OT_LIBTESTD_PATH                  OT_LIBTESTD)
ot_get_env_path(OT_LIBTESTR_PATH                  OT_LIBTESTR)

# ============================================================
# OpenTwin service roots from SetupEnvironment.bat
# ============================================================
ot_get_env_path(OT_AUTHORISATION_SERVICE_ROOT_PATH         OT_AUTHORISATION_SERVICE_ROOT)
ot_get_env_path(OT_MODEL_SERVICE_ROOT_PATH                 OT_MODEL_SERVICE_ROOT)
ot_get_env_path(OT_GLOBAL_SESSION_SERVICE_ROOT_PATH        OT_GLOBAL_SESSION_SERVICE_ROOT)
ot_get_env_path(OT_LOCAL_SESSION_SERVICE_ROOT_PATH         OT_LOCAL_SESSION_SERVICE_ROOT)
ot_get_env_path(OT_GLOBAL_DIRECTORY_SERVICE_ROOT_PATH      OT_GLOBAL_DIRECTORY_SERVICE_ROOT)
ot_get_env_path(OT_LOCAL_DIRECTORY_SERVICE_ROOT_PATH       OT_LOCAL_DIRECTORY_SERVICE_ROOT)
ot_get_env_path(OT_RELAY_SERVICE_ROOT_PATH                 OT_RELAY_SERVICE_ROOT)
ot_get_env_path(OT_LOGGER_SERVICE_ROOT_PATH                OT_LOGGER_SERVICE_ROOT)
ot_get_env_path(OT_PHREEC_SERVICE_ROOT_PATH                OT_PHREEC_SERVICE_ROOT)
ot_get_env_path(OT_KRIGING_SERVICE_ROOT_PATH               OT_KRIGING_SERVICE_ROOT)
ot_get_env_path(OT_MODELING_SERVICE_ROOT_PATH              OT_MODELING_SERVICE_ROOT)
ot_get_env_path(OT_VISUALIZATION_SERVICE_ROOT_PATH         OT_VISUALIZATION_SERVICE_ROOT)
ot_get_env_path(OT_FITTD_SERVICE_ROOT_PATH                 OT_FITTD_SERVICE_ROOT)
ot_get_env_path(OT_CARTESIAN_MESH_SERVICE_ROOT_PATH        OT_CARTESIAN_MESH_SERVICE_ROOT)
ot_get_env_path(OT_TET_MESH_SERVICE_ROOT_PATH              OT_TET_MESH_SERVICE_ROOT)
ot_get_env_path(OT_IMPORT_PARAMETERIZED_DATA_SERVICE_ROOT_PATH OT_IMPORT_PARAMETERIZED_DATA_SERVICE_ROOT)
ot_get_env_path(OT_GETDP_SERVICE_ROOT_PATH                 OT_GETDP_SERVICE_ROOT)
ot_get_env_path(OT_FDTD_SERVICE_ROOT_PATH                  OT_FDTD_SERVICE_ROOT)
ot_get_env_path(OT_ELMERFEM_SERVICE_ROOT_PATH              OT_ELMERFEM_SERVICE_ROOT)
ot_get_env_path(OT_STUDIOSUITE_SERVICE_ROOT_PATH           OT_STUDIOSUITE_SERVICE_ROOT)
ot_get_env_path(OT_LTSPICE_SERVICE_ROOT_PATH               OT_LTSPICE_SERVICE_ROOT)
ot_get_env_path(OT_UI_SERVICE_ROOT_PATH                    OT_UI_SERVICE_ROOT)
ot_get_env_path(OT_PYTHON_EXECUTION_SERVICE_ROOT_PATH      OT_PYTHON_EXECUTION_SERVICE_ROOT)
ot_get_env_path(OT_PYTHON_EXECUTION_ROOT_PATH              OT_PYTHON_EXECUTION_ROOT)
ot_get_env_path(OT_DATA_PROCESSING_SERVICE_ROOT_PATH       OT_DATA_PROCESSING_SERVICE_ROOT)
ot_get_env_path(OT_CIRCUIT_SIMULATOR_SERVICE_ROOT_PATH     OT_CIRCUIT_SIMULATOR_SERVICE_ROOT)
ot_get_env_path(OT_CIRCUIT_EXECUTION_ROOT_PATH             OT_CIRCUIT_EXECUTION_ROOT)
ot_get_env_path(OT_PYRIT_SERVICE_ROOT_PATH                 OT_PYRIT_SERVICE_ROOT)
ot_get_env_path(OT_DEBUGSERVICE_ROOT_PATH                  OT_DEBUGSERVICE_ROOT)
ot_get_env_path(OT_LIBRARY_MANAGEMENT_SERVICE_ROOT_PATH    OT_LIBRARY_MANAGEMENT_SERVICE_ROOT)
ot_get_env_path(OT_HIERARCHICAL_PROJECT_SERVICE_ROOT_PATH  OT_HIERARCHICAL_PROJECT_SERVICE_ROOT)
ot_get_env_path(OT_FILEMANAGEMENT_PROJECT_SERVICE_ROOT_PATH OT_FILEMANAGEMENT_PROJECT_SERVICE_ROOT)

# ============================================================
# OpenTwin library roots from SetupEnvironment.bat
# ============================================================
ot_get_env_path(OT_CADMODELENTITIES_ROOT_PATH         OT_CADMODELENTITIES_ROOT)
ot_get_env_path(OT_DATASTORAGE_ROOT_PATH              OT_DATASTORAGE_ROOT)
ot_get_env_path(OT_MODELENTITIES_ROOT_PATH            OT_MODELENTITIES_ROOT)
ot_get_env_path(OT_BLOCKENTITIES_ROOT_PATH            OT_BLOCKENTITIES_ROOT)

ot_get_env_path(OT_COMMUNICATION_ROOT_PATH            OT_COMMUNICATION_ROOT)
ot_get_env_path(OT_CORE_ROOT_PATH                     OT_CORE_ROOT)
ot_get_env_path(OT_RANDOM_ROOT_PATH                   OT_RANDOM_ROOT)
ot_get_env_path(OT_FOUNDATION_ROOT_PATH               OT_FOUNDATION_ROOT)
ot_get_env_path(OT_SYSTEM_ROOT_PATH                   OT_SYSTEM_ROOT)
ot_get_env_path(OT_MODELAPI_ROOT_PATH                 OT_MODELAPI_ROOT)
ot_get_env_path(OT_GUI_ROOT_PATH                      OT_GUI_ROOT)
ot_get_env_path(OT_GUIAPI_ROOT_PATH                   OT_GUIAPI_ROOT)
ot_get_env_path(OT_WIDGETS_ROOT_PATH                  OT_WIDGETS_ROOT)
ot_get_env_path(OT_FRONTEND_CONNECTOR_API_ROOT_PATH   OT_FRONTEND_CONNECTOR_API_ROOT)
ot_get_env_path(OT_FILE_MANAGER_CONNECTOR_ROOT_PATH   OT_FILE_MANAGER_CONNECTOR_ROOT)
ot_get_env_path(OT_STUDIO_SUITE_CONNECTOR_ROOT_PATH   OT_STUDIO_SUITE_CONNECTOR_ROOT)
ot_get_env_path(OT_LTSPICE_CONNECTOR_ROOT_PATH        OT_LTSPICE_CONNECTOR_ROOT)

ot_get_env_path(OT_RUBBERBANDAPI_ROOT_PATH            OT_RUBBERBANDAPI_ROOT)
ot_get_env_path(OT_RUBBERBAND_OSG_ROOT_PATH           OT_RUBBERBAND_OSG_ROOT)
ot_get_env_path(OT_UICORE_ROOT_PATH                   OT_UICORE_ROOT)
ot_get_env_path(OT_VIEWER_ROOT_PATH                   OT_VIEWER_ROOT)

ot_get_env_path(OT_RESULT_DATA_ACCESS_ROOT_PATH       OT_RESULT_DATA_ACCESS_ROOT)

# aliases for mixed naming in project CMakeLists
set(OT_CADENTITIES_ROOT_PATH              "${OT_CADMODELENTITIES_ROOT_PATH}")
set(OT_FRONTENDCONNECTORAPI_ROOT_PATH     "${OT_FRONTEND_CONNECTOR_API_ROOT_PATH}")
set(OT_FMC_ROOT_PATH                      "${OT_FILE_MANAGER_CONNECTOR_ROOT_PATH}")
set(OT_STUDIOSUITECONNECTOR_ROOT_PATH     "${OT_STUDIO_SUITE_CONNECTOR_ROOT_PATH}")
set(OT_LTSPICECONNECTOR_ROOT_PATH         "${OT_LTSPICE_CONNECTOR_ROOT_PATH}")
set(OT_RESULTDATAACCESS_ROOT_PATH         "${OT_RESULT_DATA_ACCESS_ROOT_PATH}")

# ============================================================
# documentation / tools / misc roots
# ============================================================
ot_get_env_path(OT_DOCUMENTATION_ROOT_PATH  OT_DOCUMENTATION_ROOT)
ot_get_env_path(OT_OTOOLKITAPI_ROOT_PATH    OT_OTOOLKITAPI_ROOT)
ot_get_env_path(OT_OTOOLKIT_ROOT_PATH       OT_OTOOLKIT_ROOT)
ot_get_env_path(OT_FILEHEADERUPDATER_ROOT_PATH OT_FILEHEADERUPDATER_ROOT)
ot_get_env_path(OT_ENCRYPTIONKEY_ROOT_PATH  OT_ENCRYPTIONKEY_ROOT)
ot_get_env_path(THIRDPARTY_ROOT_PATH        OPENTWIN_THIRDPARTY_ROOT)
ot_get_env_path(BASE64_ROOT_PATH            BASE64_ROOT)

# ============================================================
# certificates
# ============================================================
ot_get_env_path(OPEN_TWIN_CA_CERT_PATH          OPEN_TWIN_CA_CERT)
ot_get_env_path(OPEN_TWIN_CERT_KEY_PATH         OPEN_TWIN_CERT_KEY)
ot_get_env_path(OPEN_TWIN_SERVER_CERT_PATH      OPEN_TWIN_SERVER_CERT)
ot_get_env_path(OPEN_TWIN_SERVER_CERT_KEY_PATH  OPEN_TWIN_SERVER_CERT_KEY)

# ============================================================
# common third-party env vars used by converted projects
# ============================================================
ot_get_env_path(R_JSON_INCD_PATH                  R_JSON_INCD)
ot_get_env_path(R_JSON_INCR_PATH                  R_JSON_INCR)

ot_get_env_path(ZLIB_INCD_PATH                    ZLIB_INCD)
ot_get_env_path(ZLIB_INCR_PATH                    ZLIB_INCR)
ot_get_env_path(ZLIB_LIBPATHD_PATH                ZLIB_LIBPATHD)
ot_get_env_path(ZLIB_LIBPATHR_PATH                ZLIB_LIBPATHR)
ot_get_env_path(ZLIB_LIB_PATH                     ZLIB_LIB)
ot_get_env_path(ZLIB_ROOT_PATH                    ZLIB_ROOT)

ot_get_env_path(CURL_INCD_PATH                    CURL_INCD)
ot_get_env_path(CURL_INCR_PATH                    CURL_INCR)
ot_get_env_path(CURL_LIBPATHD_PATH                CURL_LIBPATHD)
ot_get_env_path(CURL_LIBPATHR_PATH                CURL_LIBPATHR)
ot_get_env_string(CURL_LIBD_NAME                  CURL_LIBD)
ot_get_env_string(CURL_LIBR_NAME                  CURL_LIBR)

ot_get_env_path(MONGO_C_INC_PATH                  MONGO_C_INC)
ot_get_env_path(MONGO_CXX_INC_PATH                MONGO_CXX_INC)
ot_get_env_path(MONGO_BOOST_INCD_PATH             MONGO_BOOST_INCD)
ot_get_env_path(MONGO_BOOST_INCR_PATH             MONGO_BOOST_INCR)
ot_get_env_path(MONGO_C_LIBPATHD_PATH             MONGO_C_LIBPATHD)
ot_get_env_path(MONGO_C_LIBPATHR_PATH             MONGO_C_LIBPATHR)
ot_get_env_path(MONGO_CXX_LIBPATHD_PATH           MONGO_CXX_LIBPATHD)
ot_get_env_path(MONGO_CXX_LIBPATHR_PATH           MONGO_CXX_LIBPATHR)
ot_get_env_string(MONGO_C_LIB_NAME                MONGO_C_LIB)
ot_get_env_string(MONGO_C_LIBD_NAME               MONGO_C_LIBD)
ot_get_env_string(MONGO_C_LIBR_NAME               MONGO_C_LIBR)
ot_get_env_string(MONGO_CXX_LIB_NAME              MONGO_CXX_LIB)

ot_get_env_path(MONGO_C_DLLD_PATH                 MONGO_C_DLLD)
ot_get_env_path(MONGO_C_DLLR_PATH                 MONGO_C_DLLR)
ot_get_env_path(MONGO_CXX_DLLD_PATH               MONGO_CXX_DLLD)
ot_get_env_path(MONGO_CXX_DLLR_PATH               MONGO_CXX_DLLR)

ot_get_env_path(MONGO_C_INCD_PATH                 MONGO_C_INCD)
ot_get_env_path(MONGO_C_INCR_PATH                 MONGO_C_INCR)
ot_get_env_path(MONGO_CXX_INCD_PATH               MONGO_CXX_INCD)
ot_get_env_path(MONGO_CXX_INCR_PATH               MONGO_CXX_INCR)

ot_get_env_path(VTK_DLLD_PATH                     VTK_DLLD)
ot_get_env_path(VTK_DLLR_PATH                     VTK_DLLR)
ot_get_env_path(OSG_DLLD_PATH                     OSG_DLLD)
ot_get_env_path(OSG_DLLR_PATH                     OSG_DLLR)
ot_get_env_path(QT_DLLD_PATH                      QT_DLLD)
ot_get_env_path(QT_DLLR_PATH                      QT_DLLR)
ot_get_env_path(QWT_LIB_DLLD_PATH                 QWT_LIB_DLLD)
ot_get_env_path(QWT_LIB_DLLR_PATH                 QWT_LIB_DLLR)
ot_get_env_path(QT_TT_DLLD_PATH                   QT_TT_DLLD)
ot_get_env_path(QT_TT_DLLR_PATH                   QT_TT_DLLR)

ot_get_env_path(OSG_INCD_PATH                     OSG_INCD)
ot_get_env_path(OSG_INCR_PATH                     OSG_INCR)
ot_get_env_string(OSG_LIBD_NAME                   OSG_LIBD)
ot_get_env_string(OSG_LIBR_NAME                   OSG_LIBR)
ot_get_env_path(OSG_LIBPATHD_PATH                 OSG_LIBPATHD)
ot_get_env_path(OSG_LIBPATHR_PATH                 OSG_LIBPATHR)

ot_get_env_path(QWT_LIB_INCD_PATH                 QWT_LIB_INCD)
ot_get_env_path(QWT_LIB_INCR_PATH                 QWT_LIB_INCR)
ot_get_env_string(QWT_LIB_LIBD_NAME               QWT_LIB_LIBD)
ot_get_env_string(QWT_LIB_LIBR_NAME               QWT_LIB_LIBR)
ot_get_env_path(QWT_LIB_LIBPATHD_PATH             QWT_LIB_LIBPATHD)
ot_get_env_path(QWT_LIB_LIBPATHR_PATH             QWT_LIB_LIBPATHR)

ot_get_env_path(QT_ADS_ROOT_PATH                  QT_ADS_ROOT)

ot_get_env_path(CGAL_INCD_PATH                    CGAL_INCD)
ot_get_env_path(CGAL_INCR_PATH                    CGAL_INCR)
ot_get_env_path(GMP_INCD_PATH                     GMP_INCD)
ot_get_env_path(GMP_INCR_PATH                     GMP_INCR)
ot_get_env_path(GMP_LIBPATHD_PATH                 GMP_LIBPATHD)
ot_get_env_path(GMP_LIBPATHR_PATH                 GMP_LIBPATHR)
ot_get_env_path(BOOST_INCD_PATH                   BOOST_INCD)
ot_get_env_path(BOOST_INCR_PATH                   BOOST_INCR)
ot_get_env_path(BOOST_LIBPATHD_PATH               BOOST_LIBPATHD)
ot_get_env_path(BOOST_LIBPATHR_PATH               BOOST_LIBPATHR)
ot_get_env_path(EARCUT_INC_PATH                   EARCUT_INC)

# ============================================================
# derived OT library directories
# ============================================================
ot_join_paths(OT_SYSTEM_DEBUG_LIB_DIR             "${OT_SYSTEM_ROOT_PATH}"               "${OT_CLIBD_PATH}")
ot_join_paths(OT_SYSTEM_RELEASE_LIB_DIR           "${OT_SYSTEM_ROOT_PATH}"               "${OT_CLIBR_PATH}")

ot_join_paths(OT_CORE_DEBUG_LIB_DIR               "${OT_CORE_ROOT_PATH}"                 "${OT_LIBD_PATH}")
ot_join_paths(OT_CORE_RELEASE_LIB_DIR             "${OT_CORE_ROOT_PATH}"                 "${OT_LIBR_PATH}")

ot_join_paths(OT_GUI_DEBUG_LIB_DIR                "${OT_GUI_ROOT_PATH}"                  "${OT_CLIBD_PATH}")
ot_join_paths(OT_GUI_RELEASE_LIB_DIR              "${OT_GUI_ROOT_PATH}"                  "${OT_CLIBR_PATH}")

ot_join_paths(OT_GUIAPI_DEBUG_LIB_DIR             "${OT_GUIAPI_ROOT_PATH}"               "${OT_LIBD_PATH}")
ot_join_paths(OT_GUIAPI_RELEASE_LIB_DIR           "${OT_GUIAPI_ROOT_PATH}"               "${OT_LIBR_PATH}")

ot_join_paths(OT_WIDGETS_DEBUG_LIB_DIR            "${OT_WIDGETS_ROOT_PATH}"              "${OT_CLIBD_PATH}")
ot_join_paths(OT_WIDGETS_RELEASE_LIB_DIR          "${OT_WIDGETS_ROOT_PATH}"              "${OT_CLIBR_PATH}")

ot_join_paths(OT_DATASTORAGE_DEBUG_LIB_DIR        "${OT_DATASTORAGE_ROOT_PATH}"          "${OT_CLIBD_PATH}")
ot_join_paths(OT_DATASTORAGE_RELEASE_LIB_DIR      "${OT_DATASTORAGE_ROOT_PATH}"          "${OT_CLIBR_PATH}")

ot_join_paths(OT_MODELENTITIES_DEBUG_LIB_DIR      "${OT_MODELENTITIES_ROOT_PATH}"        "${OT_CLIBD_PATH}")
ot_join_paths(OT_MODELENTITIES_RELEASE_LIB_DIR    "${OT_MODELENTITIES_ROOT_PATH}"        "${OT_CLIBR_PATH}")

ot_join_paths(OT_MODELAPI_DEBUG_LIB_DIR           "${OT_MODELAPI_ROOT_PATH}"             "${OT_LIBD_PATH}")
ot_join_paths(OT_MODELAPI_RELEASE_LIB_DIR         "${OT_MODELAPI_ROOT_PATH}"             "${OT_LIBR_PATH}")

ot_join_paths(OT_FOUNDATION_DEBUG_LIB_DIR         "${OT_FOUNDATION_ROOT_PATH}"           "${OT_CLIBD_PATH}")
ot_join_paths(OT_FOUNDATION_RELEASE_LIB_DIR       "${OT_FOUNDATION_ROOT_PATH}"           "${OT_CLIBR_PATH}")

ot_join_paths(OT_COMMUNICATION_DEBUG_LIB_DIR      "${OT_COMMUNICATION_ROOT_PATH}"        "${OT_CLIBD_PATH}")
ot_join_paths(OT_COMMUNICATION_RELEASE_LIB_DIR    "${OT_COMMUNICATION_ROOT_PATH}"        "${OT_CLIBR_PATH}")

ot_join_paths(OT_RUBBERBANDAPI_DEBUG_LIB_DIR      "${OT_RUBBERBANDAPI_ROOT_PATH}"        "${OT_CLIBD_PATH}")
ot_join_paths(OT_RUBBERBANDAPI_RELEASE_LIB_DIR    "${OT_RUBBERBANDAPI_ROOT_PATH}"        "${OT_CLIBR_PATH}")

ot_join_paths(OT_RUBBERBAND_OSG_DEBUG_LIB_DIR     "${OT_RUBBERBAND_OSG_ROOT_PATH}"       "${OT_LIBD_PATH}")
ot_join_paths(OT_RUBBERBAND_OSG_RELEASE_LIB_DIR   "${OT_RUBBERBAND_OSG_ROOT_PATH}"       "${OT_LIBR_PATH}")

ot_join_paths(OT_RESULTDATAACCESS_DEBUG_LIB_DIR   "${OT_RESULTDATAACCESS_ROOT_PATH}"     "${OT_CLIBD_PATH}")
ot_join_paths(OT_RESULTDATAACCESS_RELEASE_LIB_DIR "${OT_RESULTDATAACCESS_ROOT_PATH}"     "${OT_CLIBR_PATH}")

ot_join_paths(OT_CADENTITIES_DEBUG_LIB_DIR        "${OT_CADENTITIES_ROOT_PATH}"          "${OT_CLIBD_PATH}")
ot_join_paths(OT_CADENTITIES_RELEASE_LIB_DIR      "${OT_CADENTITIES_ROOT_PATH}"          "${OT_CLIBR_PATH}")

ot_join_paths(OT_BLOCKENTITIES_DEBUG_LIB_DIR      "${OT_BLOCKENTITIES_ROOT_PATH}"        "${OT_CLIBD_PATH}")
ot_join_paths(OT_BLOCKENTITIES_RELEASE_LIB_DIR    "${OT_BLOCKENTITIES_ROOT_PATH}"        "${OT_CLIBR_PATH}")

ot_join_paths(OT_FMC_DEBUG_LIB_DIR                "${OT_FMC_ROOT_PATH}"                  "${OT_LIBD_PATH}")
ot_join_paths(OT_FMC_RELEASE_LIB_DIR              "${OT_FMC_ROOT_PATH}"                  "${OT_LIBR_PATH}")

ot_join_paths(OT_FRONTENDCONNECTORAPI_DEBUG_LIB_DIR   "${OT_FRONTENDCONNECTORAPI_ROOT_PATH}" "${OT_LIBD_PATH}")
ot_join_paths(OT_FRONTENDCONNECTORAPI_RELEASE_LIB_DIR "${OT_FRONTENDCONNECTORAPI_ROOT_PATH}" "${OT_LIBR_PATH}")

ot_join_paths(OT_STUDIOSUITECONNECTOR_DEBUG_LIB_DIR   "${OT_STUDIOSUITECONNECTOR_ROOT_PATH}" "${OT_LIBD_PATH}")
ot_join_paths(OT_STUDIOSUITECONNECTOR_RELEASE_LIB_DIR "${OT_STUDIOSUITECONNECTOR_ROOT_PATH}" "${OT_LIBR_PATH}")

ot_join_paths(OT_LTSPICECONNECTOR_DEBUG_LIB_DIR   "${OT_LTSPICECONNECTOR_ROOT_PATH}"     "${OT_LIBD_PATH}")
ot_join_paths(OT_LTSPICECONNECTOR_RELEASE_LIB_DIR "${OT_LTSPICECONNECTOR_ROOT_PATH}"     "${OT_LIBR_PATH}")

# ============================================================
# aggregated env vars from SetupEnvironment.bat
# ============================================================
ot_get_env_path_list(OT_DEFAULT_SERVICE_INCD_LIST     OT_DEFAULT_SERVICE_INCD)
ot_get_env_path_list(OT_DEFAULT_SERVICE_INCR_LIST     OT_DEFAULT_SERVICE_INCR)
ot_get_env_path_list(OT_DEFAULT_SERVICE_LIBPATHD_LIST OT_DEFAULT_SERVICE_LIBPATHD)
ot_get_env_path_list(OT_DEFAULT_SERVICE_LIBPATHR_LIST OT_DEFAULT_SERVICE_LIBPATHR)
ot_get_env_list(OT_DEFAULT_SERVICE_LIBD_LIST          OT_DEFAULT_SERVICE_LIBD)
ot_get_env_list(OT_DEFAULT_SERVICE_LIBR_LIST          OT_DEFAULT_SERVICE_LIBR)

ot_get_env_path_list(OT_DEFAULT_SERVICE_DLLD_LIST     OT_DEFAULT_SERVICE_DLLD)
ot_get_env_path_list(OT_DEFAULT_SERVICE_DLLR_LIST     OT_DEFAULT_SERVICE_DLLR)

ot_get_env_path_list(OT_ALL_DLLD_LIST                 OT_ALL_DLLD)
ot_get_env_path_list(OT_ALL_DLLR_LIST                 OT_ALL_DLLR)

# ============================================================
# sanity checks
# ============================================================
if(NOT OT_SYSTEM_ROOT_PATH)
    message(STATUS "OT_SYSTEM_ROOT is not set")
endif()
