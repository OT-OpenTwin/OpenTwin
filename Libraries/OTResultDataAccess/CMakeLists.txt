cmake_minimum_required(VERSION 3.20)

project(OTResultDataAccess LANGUAGES CXX)

# ============================================================
# Helper functions
# ============================================================

# Read a path from an environment variable and normalize it
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

# Read a semicolon-separated list of library names from an env var
function(get_env_libname OUT_VAR ENV_NAME)
    if(DEFINED ENV{${ENV_NAME}} AND NOT "$ENV{${ENV_NAME}}" STREQUAL "")
        set(_val "$ENV{${ENV_NAME}}")
        string(REPLACE "\"" "" _val "${_val}")
        set(${OUT_VAR} "${_val}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

# ============================================================
# Environment variables (mirroring the .vcxproj)
# ============================================================

# Global OT include root (was $(SolutionDir)$(OT_INC))
get_env_path(OT_GLOBAL_INC_ROOT          OT_INC)

if(OT_GLOBAL_INC_ROOT)
    if(NOT IS_ABSOLUTE "${OT_GLOBAL_INC_ROOT}")
        set(OT_GLOBAL_INC_ROOT "${CMAKE_SOURCE_DIR}/${OT_GLOBAL_INC_ROOT}")
        file(TO_CMAKE_PATH "${OT_GLOBAL_INC_ROOT}" OT_GLOBAL_INC_ROOT)
    endif()
endif()

# Default service include/lib paths
get_env_path(OT_DEFAULT_SERVICE_INCD_PATH OT_DEFAULT_SERVICE_INCD)
get_env_path(OT_DEFAULT_SERVICE_INCR_PATH OT_DEFAULT_SERVICE_INCR)
get_env_path(OT_DEFAULT_SERVICE_LIBPATHD_PATH OT_DEFAULT_SERVICE_LIBPATHD)
get_env_path(OT_DEFAULT_SERVICE_LIBPATHR_PATH OT_DEFAULT_SERVICE_LIBPATHR)

# Mongo C driver include/lib paths
get_env_path(MONGO_C_INCD_PATH           MONGO_C_INCD)
get_env_path(MONGO_C_INCR_PATH           MONGO_C_INCR)
get_env_path(MONGO_C_LIBPATHD_PATH       MONGO_C_LIBPATHD)
get_env_path(MONGO_C_LIBPATHR_PATH       MONGO_C_LIBPATHR)

# Mongo C++ driver lib paths (note env name typo LIPPATHD/R in vcxproj)
get_env_path(MONGO_CXX_LIPPATHD_PATH     MONGO_CXX_LIPPATHD)
get_env_path(MONGO_CXX_LIPPATHR_PATH     MONGO_CXX_LIPPATHR)

# Boost include/lib paths
get_env_path(BOOST_INCD_PATH             BOOST_INCD)
get_env_path(BOOST_INCR_PATH             BOOST_INCR)
get_env_path(BOOST_LIBPATHD_PATH         BOOST_LIBPATHD)
get_env_path(BOOST_LIBPATHR_PATH         BOOST_LIBPATHR)

# Library names (as in AdditionalDependencies)
get_env_libname(MONGO_CXX_LIBD_NAME      MONGO_CXX_LIBD)
get_env_libname(MONGO_CXX_LIBR_NAME      MONGO_CXX_LIBR)
get_env_libname(OT_DEFAULT_SERVICE_LIBD_NAME OT_DEFAULT_SERVICE_LIBD)
get_env_libname(OT_DEFAULT_SERVICE_LIBR_NAME OT_DEFAULT_SERVICE_LIBR)
get_env_libname(MONGO_C_LIB_NAME         MONGO_C_LIB)

# ============================================================
# Sources
# ============================================================

set(OTResultDataAccess_HEADERS
    include/OTResultDataAccess/ResultCollection/CrossCollectionAccess.h
    include/OTResultDataAccess/ResultCollection/IndexHandler.h
    include/OTResultDataAccess/ResultCollection/ProjectToCollectionConverter.h
    include/OTResultDataAccess/ResultCollection/ResultCollectionExtender.h
    include/OTResultDataAccess/ResultCollection/ResultCollectionMetadataAccess.h

    include/OTResultDataAccess/CurveFactory.h

    include/OTResultDataAccess/MetadataEntry/MetadataEntry.h
    include/OTResultDataAccess/MetadataEntry/MetadataEntryArray.h
    include/OTResultDataAccess/MetadataEntry/MetadataEntryComperator.h
    include/OTResultDataAccess/MetadataEntry/MetadataEntryObject.h
    include/OTResultDataAccess/MetadataEntry/MetadataEntrySingle.h
    
    include/OTResultDataAccess/MetadataHandle/MetadataParameter.h
    include/OTResultDataAccess/MetadataHandle/MetadataQuantity.h
    include/OTResultDataAccess/MetadataHandle/MetadataSeries.h
    include/OTResultDataAccess/MetadataHandle/MetadataCampaign.h
    include/OTResultDataAccess/MetadataHandle/MetadataEntityInterface.h
    
    include/OTResultDataAccess/SerialisationInterfaces/DatasetDescription.h
    include/OTResultDataAccess/SerialisationInterfaces/ParameterDescription.h
    include/OTResultDataAccess/SerialisationInterfaces/QuantityDescription.h
    include/OTResultDataAccess/SerialisationInterfaces/QuantityDescriptionCurve.h
    include/OTResultDataAccess/SerialisationInterfaces/QuantityDescriptionCurveComplex.h
    include/OTResultDataAccess/SerialisationInterfaces/QuantityDescriptionMatrix.h
    include/OTResultDataAccess/SerialisationInterfaces/QuantityDescriptionSParameter.h
    include/OTResultDataAccess/SerialisationInterfaces/TupleDescription.h
    include/OTResultDataAccess/SerialisationInterfaces/TupleDescriptionComplex.h

    include/OTResultDataAccess/ResultImportLogger/ResultImportLogger.h
    include/OTResultDataAccess/ResultImportLogger/ResultImportLoggerEntry.h
    include/OTResultDataAccess/ResultImportLogger/ResultImportLoggerVerbosity.h
    
    include/OTResultDataAccess/PlotBuilder.h
    include/OTResultDataAccess/QuantityContainer.h
    include/OTResultDataAccess/QuantityContainerSerialiser.h
    include/OTResultDataAccess/ResultDataAccessAPIExport.h
    include/OTResultDataAccess/ValueFormatSetter.h

)

set(OTResultDataAccess_SOURCES
    src/CrossCollectionAccess.cpp
    src/CurveFactory.cpp
    src/DatasetDescription.cpp
    src/IndexHandler.cpp
    src/MetadataCampaign.cpp
    src/MetadataEntityInterface.cpp
    src/MetadataEntryArray.cpp
    src/MetadataEntryComperator.cpp
    src/MetadataEntryObject.cpp
    src/MetadataEntrySingle.cpp
    src/MetadataParameter.cpp
    src/MetadataQuantity.cpp
    src/PlotBuilder.cpp
    src/ProjectToCollectionConverter.cpp
    src/QuantityContainer.cpp
    src/QuantityContainerSerialiser.cpp
    src/QuantityDescription.cpp
    src/QuantityDescriptionSParameter.cpp
    src/ResultCollectionExtender.cpp
    src/ResultCollectionMetadataAccess.cpp
    src/ResultImportLogger.cpp
)

# Core object library (for reuse in tests)
add_library(OTResultDataAccess_core OBJECT
    ${OTResultDataAccess_HEADERS}
    ${OTResultDataAccess_SOURCES}
)

target_include_directories(OTResultDataAccess_core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# === Include directories (config-dependent) ===

# $(SolutionDir)$(OT_INC)
if(OT_GLOBAL_INC_ROOT)
    target_include_directories(OTResultDataAccess_core PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>,$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${OT_GLOBAL_INC_ROOT}>
    )
endif()

# $(OT_DEFAULT_SERVICE_INCD / INCR)
if(OT_DEFAULT_SERVICE_INCD_PATH)
    target_include_directories(OTResultDataAccess_core PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${OT_DEFAULT_SERVICE_INCD_PATH}>
    )
endif()

if(OT_DEFAULT_SERVICE_INCR_PATH)
    target_include_directories(OTResultDataAccess_core PRIVATE
        $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${OT_DEFAULT_SERVICE_INCR_PATH}>
    )
endif()

# Mongo C includes
if(MONGO_C_INCD_PATH)
    target_include_directories(OTResultDataAccess_core PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${MONGO_C_INCD_PATH}>
    )
endif()

if(MONGO_C_INCR_PATH)
    target_include_directories(OTResultDataAccess_core PRIVATE
        $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${MONGO_C_INCR_PATH}>
    )
endif()

# Boost includes
if(BOOST_INCD_PATH)
    target_include_directories(OTResultDataAccess_core PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${BOOST_INCD_PATH}>
    )
endif()

if(BOOST_INCR_PATH)
    target_include_directories(OTResultDataAccess_core PRIVATE
        $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${BOOST_INCR_PATH}>
    )
endif()

# C++17 / MSVC Optionen
target_compile_features(OTResultDataAccess_core PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(OTResultDataAccess_core PRIVATE /MP /wd4996)
endif()

target_compile_definitions(OTResultDataAccess_core
    PRIVATE
        _CONSOLE
        _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
        OT_RESULTDATAACCESS_EXPORTS
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>:_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS>
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:_DEBUG>
        $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:NDEBUG>
)

# ============================================================
# Shared library OTResultDataAccess (DLL)
# ============================================================

add_library(OTResultDataAccess SHARED)
add_library(OT::OTResultDataAccess ALIAS OTResultDataAccess)

target_sources(OTResultDataAccess
    PRIVATE
        $<TARGET_OBJECTS:OTResultDataAccess_core>
)

target_include_directories(OTResultDataAccess
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_features(OTResultDataAccess PRIVATE cxx_std_17)

target_compile_definitions(OTResultDataAccess
    PRIVATE
        _CONSOLE
        _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
        OT_RESULTDATAACCESS_EXPORTS
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>:_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS>
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:_DEBUG>
        $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:NDEBUG>
)

set_target_properties(OTResultDataAccess PROPERTIES
    OUTPUT_NAME "OTResultDataAccess"
)

# ============================================================
# Linker-Einstellungen (Mongo / DefaultService / Boost)
# ============================================================

if(WIN32)
    # Library directories (config dependent)
    target_link_directories(OTResultDataAccess PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${OT_DEFAULT_SERVICE_LIBPATHD_PATH}>
        $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${OT_DEFAULT_SERVICE_LIBPATHR_PATH}>
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${MONGO_CXX_LIPPATHD_PATH}>
        $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${MONGO_CXX_LIPPATHR_PATH}>
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${BOOST_LIBPATHD_PATH}>
        $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${BOOST_LIBPATHR_PATH}>
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${MONGO_C_LIBPATHD_PATH}>
        $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${MONGO_C_LIBPATHR_PATH}>
    )

    # Libraries
    target_link_libraries(OTResultDataAccess
        PRIVATE
            # Debug / DebugTest
            $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${MONGO_CXX_LIBD_NAME}>
            $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${OT_DEFAULT_SERVICE_LIBD_NAME}>
            $<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugTest>>:${MONGO_C_LIB_NAME}>

            # Release / ReleaseTest
            $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${MONGO_CXX_LIBR_NAME}>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${OT_DEFAULT_SERVICE_LIBR_NAME}>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseTest>>:${MONGO_C_LIB_NAME}>
    )
endif()

# ============================================================
# Tests (optional)
# ============================================================

option(BUILD_TESTING "Build OTResultDataAccess tests" ON)

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
