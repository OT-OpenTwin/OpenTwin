cmake_minimum_required(VERSION 3.20)

project(OTCore LANGUAGES CXX)

# ============================================================
# Helper functions
# ============================================================
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

# Small helper for source_group mapping
function(assign_filter FILTER_NAME)
    foreach(FILE IN LISTS ARGN)
        # Build absolute path for the existence check
        set(_full_path "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}")

        if(EXISTS "${_full_path}")
            # IMPORTANT:
            # Use the *same* path string that was used in add_library(...)
            # (i.e. "src/Variable.cpp" etc.), otherwise CMake won't match
            # the file against the filter.
            source_group("${FILTER_NAME}" FILES "${FILE}")
        endif()
    endforeach()
endfunction()

# ============================================================
# Environment configuration (mirrors original .vcxproj)
# ============================================================

# OT include roots
get_env_path(OT_SYSTEM_ROOT_PATH   OT_SYSTEM_ROOT)
get_env_path(OT_CORE_ROOT_PATH     OT_CORE_ROOT)
get_env_path(OT_INC_PATH           OT_INC)

# RapidJSON
get_env_path(R_JSON_INCD_PATH      R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH      R_JSON_INCR)

# OTSystem and OTCore lib directories
get_env_path(OT_CLIBD_PATH         OT_CLIBD)
get_env_path(OT_CLIBR_PATH         OT_CLIBR)
get_env_path(OT_LIBD_PATH          OT_LIBD)
get_env_path(OT_LIBR_PATH          OT_LIBR)

# Zlib & base64 & curl test paths
get_env_path(ZLIB_INCD_PATH        ZLIB_INCD)
get_env_path(ZLIB_INCR_PATH        ZLIB_INCR)
get_env_path(ZLIB_LIBPATHD_PATH    ZLIB_LIBPATHD)
get_env_path(ZLIB_LIBPATHR_PATH    ZLIB_LIBPATHR)
get_env_path(ZLIB_LIB_PATH         ZLIB_LIB)
get_env_path(CURL_LIB_PATH         CURL_LIB)
get_env_path(BASE64_ROOT_PATH      BASE64_ROOT)
get_env_path(THIRDPARTY_ROOT_PATH  OPENTWIN_THIRDPARTY_ROOT)

# ============================================================
# C++ standard
# ============================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# Set sources and headers
# ============================================================

set(OTCORE_HEADER_FILES
    include/OTCore/ComplexNumbers/ComplexNumberContainer.h
    include/OTCore/ComplexNumbers/ComplexNumberConversion.h
    include/OTCore/ComplexNumbers/ComplexNumberFormat.h
    include/OTCore/ComplexNumbers/SerialisationKeys.h

    include/OTCore/JSON/JSON.h
    include/OTCore/JSON/JSONArray.h
    include/OTCore/JSON/JSONArray.hpp
    include/OTCore/JSON/JSONDocument.h
    include/OTCore/JSON/JSONDocument.hpp
    include/OTCore/JSON/JSONHelper.h
    include/OTCore/JSON/JSONNullValue.h
    include/OTCore/JSON/JSONNumber.h
    include/OTCore/JSON/JSONObject.h
    include/OTCore/JSON/JSONString.h
    include/OTCore/JSON/JSONString.hpp
    include/OTCore/JSON/JSONTypes.h

    include/OTCore/Logging/AbstractLogNotifier.h
    include/OTCore/Logging/LogDispatcher.h
    include/OTCore/Logging/LogMessage.h
    include/OTCore/Logging/LogNotifierFileWriter.h
    include/OTCore/Logging/LogNotifierStdCout.h
    include/OTCore/Logging/LogTypes.h

    include/OTCore/RAII/AtomicDecrementRAII.h
    include/OTCore/RAII/AtomicIncrementRAII.h
    include/OTCore/RAII/AtomicValueRAII.h
    include/OTCore/RAII/DecrementRAII.h
    include/OTCore/RAII/FunctionRAII.h
    include/OTCore/RAII/IncrementRAII.h
    include/OTCore/RAII/ValueRAII.h

    include/OTCore/BasicEntityInformation.h
    include/OTCore/BasicServiceInformation.h
    include/OTCore/Color.h
    include/OTCore/ComparisonSymbols.h
    include/OTCore/ContainerHelper.h
    include/OTCore/ContainerHelper.hpp
    include/OTCore/CoreAPIExport.h
    include/OTCore/CoreTypes.h
    include/OTCore/DebugHelper.h
    include/OTCore/DefensiveProgramming.h
    include/OTCore/EncodingConverter_ISO88591ToUTF8.h
    include/OTCore/EncodingConverter_UTF16ToUTF8.h
    include/OTCore/EncodingGuesser.h
    include/OTCore/EntityName.h
    include/OTCore/ExplicitStringValueConverter.h
    include/OTCore/FactoryTemplate.h
    include/OTCore/FactoryTemplate.hpp
    include/OTCore/FileInformation.h
    include/OTCore/FolderNames.h
    include/OTCore/GenericDataStruct.h
    include/OTCore/GenericDataStructFactory.h
    include/OTCore/GenericDataStructMatrix.h
    include/OTCore/GenericDataStructSingle.h
    include/OTCore/GenericDataStructVector.h
    include/OTCore/GlobalTestingFlags.h
    include/OTCore/JSONToVariableConverter.h
    include/OTCore/LocalStateStack.h
    include/OTCore/LocalStateStack.hpp
    include/OTCore/LoginTypes.h
    include/OTCore/Math.h
    include/OTCore/Matrix.h
    include/OTCore/MatrixNested.h
    include/OTCore/MatrixVectorizedXAlign.h
    include/OTCore/Matrix2.h
    include/OTCore/Matrix2.hpp
    include/OTCore/Matrix2Factory.h
    include/OTCore/Matrix2Factory.hpp
    include/OTCore/ObjectManagerTemplate.h
    include/OTCore/OTClassHelper.h
    include/OTCore/OTObject.h
    include/OTCore/Owner.h
    include/OTCore/OwnerService.h
    include/OTCore/OwnerServiceGlobal.h
    include/OTCore/PointerList.h
    include/OTCore/PointerList.hpp
    include/OTCore/Point2D.h
    include/OTCore/Point3D.h
    include/OTCore/ProjectFilterData.h
    include/OTCore/ProjectInformation.h
    include/OTCore/Rect.h
    include/OTCore/ResultCollectionDefaultIndexes.h
    include/OTCore/ReturnMessage.h
    include/OTCore/ReturnValues.h
    include/OTCore/RuntimeTests.h
    include/OTCore/Serializable.h
    include/OTCore/ServiceBase.h
    include/OTCore/ServiceDebugInformation.h
    include/OTCore/Size2D.h
    include/OTCore/StateStack.h
    include/OTCore/StateStack.hpp
    include/OTCore/String.h
    include/OTCore/String.hpp
    include/OTCore/StringToNumericCheck.h
    include/OTCore/StringToVariableConverter.h
    include/OTCore/SystemServiceConfigInfo.h
    include/OTCore/TemplateTypeName.h
    include/OTCore/TemplateTypeName.hpp
    include/OTCore/TextEncoding.h
    include/OTCore/ThisComputerInfo.h
    include/OTCore/ThisService.h
    include/OTCore/TimeFormatter.h
    include/OTCore/TypeNames.h
    include/OTCore/UIDNamePair.h
    include/OTCore/ValueComparisonDefinition.h
    include/OTCore/Variable.h
    include/OTCore/VariableListToStringListConverter.h
    include/OTCore/VariableToBSONStringConverter.h
    include/OTCore/VariableToJSONConverter.h
    include/OTCore/VariableToStringConverter.h
)

set(OTCORE_SOURCE_FILES
    src/JSON/JSONHelper.cpp
    src/JSON/JsonObject.cpp

    src/Logging/LogMessage.cpp
    src/Logging/LogNotifierFileWriter.cpp
    src/Logging/LogNotifierStdCout.cpp
    src/Logging/LogTypes.cpp
    src/Logging/LogDispatcher.cpp

    src/BasicEntityInformation.cpp
    src/BasicServiceInformation.cpp
    src/Color.cpp
    src/ComplexNumberConversion.cpp
    src/DebugHelper.cpp
    src/dllmain.cpp
    src/EncodingConverter_ISO88591ToUTF8.cpp
    src/EncodingConverter_UTF16ToUTF8.cpp
    src/EncodingGuesser.cpp
    src/EntityName.cpp
    src/ExplicitStringValueConverter.cpp
    src/FileInformation.cpp
    src/GenericDataStruct.cpp
    src/GenericDataStructFactory.cpp
    src/GenericDataStructMatrix.cpp
    src/GenericDataStructSingle.cpp
    src/GenericDataStructVector.cpp
    src/ProjectFilterData.cpp
    src/ProjectInformation.cpp
    src/Serializable.cpp
    src/ServiceDebugInformation.cpp
    src/ThisComputerInfo.cpp
    src/OTObject.cpp
    src/RuntimeTests.cpp
    src/StringToNumericCheck.cpp
    src/LoginTypes.cpp
    src/Math.cpp
    src/OwnerServiceGlobal.cpp
    src/JSONToVariableConverter.cpp
    src/Owner.cpp
    src/Point2D.cpp
    src/Rect.cpp
    src/ReturnMessage.cpp
    src/ReturnValues.cpp
    src/ServiceBase.cpp
    src/OwnerService.cpp
    src/Size2D.cpp
    src/String.cpp
    src/StringToVariableConverter.cpp
    src/SystemServiceConfigInfo.cpp
    src/TextEncoding.cpp
    src/ThisService.cpp
    src/UIDNamePair.cpp
    src/ValueComparisonDefinition.cpp
    src/Variable.cpp
    src/VariableListToStringListConverter.cpp
    src/VariableToBSONStringConverter.cpp
    src/VariableToJSONConverter.cpp
    src/VariableToStringConverter.cpp
    src/TimeFormatter.cpp
)

# Third-party source (base64.c)
set(OTCORE_THIRDPARTY_SOURCE "")
set(OTCORE_THIRDPARTY_HEADER "")

if(THIRDPARTY_ROOT_PATH)
    set(OTCORE_THIRDPARTY_SOURCE "${THIRDPARTY_ROOT_PATH}/base64/base64.c")
    set(OTCORE_THIRDPARTY_HEADER "${THIRDPARTY_ROOT_PATH}/base64/base64.h")
elseif(BASE64_ROOT_PATH)
    set(OTCORE_THIRDPARTY_SOURCE "${BASE64_ROOT_PATH}/base64.c")
    set(OTCORE_THIRDPARTY_HEADER "${BASE64_ROOT_PATH}/base64.h")
endif()

# ============================================================
# Core OBJECT library
# ============================================================
add_library(OTCore_core OBJECT
    ${OTCORE_SOURCE_FILES}
    ${OTCORE_THIRDPARTY_SOURCE}
    ${OTCORE_HEADER_FILES}
)

target_compile_definitions(OTCore_core
    PRIVATE
        UNICODE
        _UNICODE
        _WINDOWS
        _USRDLL
        OPENTWINCORE_EXPORTS
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

# ============================================================
# Include paths for OTCore_core
# ============================================================
target_include_directories(OTCore_core
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# $(OT_SYSTEM_ROOT)\$(OT_INC)
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTCore_core PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# $(OT_CORE_ROOT)\$(OT_INC)
if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTCore_core PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# RapidJSON
if(R_JSON_INCD_PATH)
    target_include_directories(OTCore_core PRIVATE "${R_JSON_INCD_PATH}")
endif()
if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(OTCore_core PRIVATE "${R_JSON_INCR_PATH}")
endif()

# zlib
if(ZLIB_INCD_PATH)
    target_include_directories(OTCore_core PRIVATE "${ZLIB_INCD_PATH}")
endif()
if(ZLIB_INCR_PATH AND NOT ZLIB_INCR_PATH STREQUAL ZLIB_INCD_PATH)
    target_include_directories(OTCore_core PRIVATE "${ZLIB_INCR_PATH}")
endif()

# base64 include
if(OTCORE_THIRDPARTY_HEADER)
    get_filename_component(_thirdparty_dir "${OTCORE_THIRDPARTY_HEADER}" DIRECTORY)
    target_include_directories(OTCore_core PRIVATE "${_thirdparty_dir}")
endif()

# ============================================================
# MSVC compiler flags
# ============================================================
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    target_compile_options(OTCore_core PRIVATE
        /W3
        /sdl
        /MP
        /permissive-
        /Zc:__cplusplus
    )
endif()

# ============================================================
# Shared Library (DLL)
# ============================================================
add_library(OTCore SHARED
    $<TARGET_OBJECTS:OTCore_core>
)

target_include_directories(OTCore
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(WIN32)
    target_compile_definitions(OTCore
        PRIVATE
            UNICODE
            _UNICODE
            _WINDOWS
            _USRDLL
            OPENTWINCORE_EXPORTS
    )

    # OTSystem / OTCore lib directories (matching OTRandom style)
    join_paths(OT_SYSTEM_DEBUG_LIB_DIR   "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBD_PATH}")
    join_paths(OT_SYSTEM_RELEASE_LIB_DIR "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBR_PATH}")
    join_paths(OT_CORE_DEBUG_LIB_DIR     "${OT_CORE_ROOT_PATH}"   "${OT_LIBD_PATH}")
    join_paths(OT_CORE_RELEASE_LIB_DIR   "${OT_CORE_ROOT_PATH}"   "${OT_LIBR_PATH}")

    if(OT_SYSTEM_DEBUG_LIB_DIR)
        target_link_directories(OTCore PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(OTCore PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_CORE_DEBUG_LIB_DIR)
        target_link_directories(OTCore PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_CORE_RELEASE_LIB_DIR)
        target_link_directories(OTCore PRIVATE
            $<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    # zlib lib paths
    if(ZLIB_LIBPATHD_PATH)
        target_link_directories(OTCore PRIVATE
            $<$<CONFIG:Debug>:${ZLIB_LIBPATHD_PATH}>
        )
    endif()

    if(ZLIB_LIBPATHR_PATH)
        target_link_directories(OTCore PRIVATE
            $<$<CONFIG:Release>:${ZLIB_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${ZLIB_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${ZLIB_LIBPATHR_PATH}>
        )
    endif()

    # Libraries to link (like .vcxproj: OTSystem + zlib + system libs)
    target_link_libraries(OTCore
        PRIVATE
            OTSystem
            $<$<CONFIG:Debug>:zlibd>
            $<$<CONFIG:Release>:zlib>
            $<$<CONFIG:RelWithDebInfo>:zlib>
            $<$<CONFIG:MinSizeRel>:zlib>
            Userenv
    )

    if(MSVC)
        target_link_options(OTCore PRIVATE
            /SUBSYSTEM:WINDOWS
            /MANIFESTUAC:NO
            $<$<CONFIG:Release>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:RelWithDebInfo>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:MinSizeRel>:/OPT:REF /OPT:ICF>
        )
    endif()
endif()

# ============================================================
# GoogleTest + test integration (OTCore_tests in tests/)
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
