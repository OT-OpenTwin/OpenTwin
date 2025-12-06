cmake_minimum_required(VERSION 3.20)

project(ModelEntities LANGUAGES CXX)

# ============================================================
# Helper functions (same style as your other libs)
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

# Small helper for Visual Studio source_group mapping
function(assign_filter FILTER_NAME)
    foreach(FILE IN LISTS ARGN)
        set(_full_path "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}")
        if(EXISTS "${_full_path}")
            source_group("${FILTER_NAME}" FILES "${FILE}")
        endif()
    endforeach()
endfunction()

# ============================================================
# Environment configuration (mirrors .vcxproj usage)
# ============================================================

# OT roots and include subdir
get_env_path(OT_SYSTEM_ROOT_PATH        OT_SYSTEM_ROOT)
get_env_path(OT_CORE_ROOT_PATH          OT_CORE_ROOT)
get_env_path(OT_GUI_ROOT_PATH           OT_GUI_ROOT)
get_env_path(OT_COMMUNICATION_ROOT_PATH OT_COMMUNICATION_ROOT)
get_env_path(OT_DATASTORAGE_ROOT_PATH   OT_DATASTORAGE_ROOT)
get_env_path(OT_INC_PATH                OT_INC)

# OT library subdirs
get_env_path(OT_CLIBD_PATH              OT_CLIBD)
get_env_path(OT_CLIBR_PATH              OT_CLIBR)

# RapidJSON include
get_env_path(R_JSON_INCD_PATH           R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH           R_JSON_INCR)

# Mongo C++ / C / Boost
get_env_path(MONGO_CXX_INC_PATH         MONGO_CXX_INC)
get_env_path(MONGO_C_INC_PATH           MONGO_C_INC)

get_env_path(MONGO_BOOST_INCD_PATH      MONGO_BOOST_INCD)
get_env_path(MONGO_BOOST_INCR_PATH      MONGO_BOOST_INCR)

get_env_path(MONGO_CXX_LIBPATHD_PATH    MONGO_CXX_LIBPATHD)
get_env_path(MONGO_CXX_LIBPATHR_PATH    MONGO_CXX_LIBPATHR)
get_env_path(MONGO_C_LIBPATHD_PATH      MONGO_C_LIBPATHD)
get_env_path(MONGO_C_LIBPATHR_PATH      MONGO_C_LIBPATHR)

get_env_libname(MONGO_CXX_LIBS_NAME     MONGO_CXX_LIB)  # "bsoncxx.lib;mongocxx.lib"
get_env_libname(MONGO_C_LIBS_NAME       MONGO_C_LIB)    # "bson-1.0.lib;mongoc-1.0.lib"

# ============================================================
# C++ standard
# ============================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# Source and header lists (from .vcxproj)
# ============================================================

set(MODELENTITIES_HEADER_FILES
    include/BoundingBox.h
    include/BsonArrayTypeGetterWrapper.h
    include/BsonArrayTypeGetterWrapper.hpp
    include/BSONToVariableConverter.h
    include/EntityCallbackBase.h
    include/CSVProperties.h
    include/CSVToTableTransformer.h
    include/EntityAPI.h
    include/EntityBatchImporter.h
    include/EntityCoordinates2D.h
    include/EntityDatabaseIndexManipulator.h
    include/EntityFactoryRegistrar.h
    include/EntityFileRawData.h
    include/EntityFileImage.h
    include/EntityGraphicsScene.h
    include/EntityInformation.h
    include/EntityNamingBehavior.h
    include/EntityPythonManifest.h
    include/EntityResult1DCurve.h
    include/EntityResult1DPlot.h
    include/EntityResultText.h
    include/EntityResultTextData.h
    include/EntityResultUnstructuredMeshVtk.h
    include/EntitySolverDataProcessing.h
    include/EntitySolverFDTD.h
    include/EntitySolverPyrit.h
    include/EntityVisUnstructuredVectorSurface.h
    include/InvalidUID.h
    include/IVisualisationCurve.h
    include/IVisualisationGraphicsView.h
    include/IVisualisationImage.h
    include/IVisualisationPlot1D.h
    include/IVisualisationTable.h
    include/IVisualisationText.h
    include/EntityMetadataSeries.h
    include/EntityMetadataCampaign.h
    include/EntityResultUnstructuredMesh.h
    include/EntityResultUnstructuredMeshData.h
    include/EntitySolverCircuitSimulator.h
    include/EntitySolverElmerFEM.h
    include/EntitySolverGetDP.h
    include/EntityFileText.h
    include/EntityVisUnstructuredScalarSurface.h
    include/EntityVisUnstructuredScalarVolume.h
    include/EntityVisUnstructuredVectorVolume.h
    include/GenericBsonDocument.h
    include/EntityFactory.h
    include/DataBase.h
    include/EntityAnnotation.h
    include/EntityAnnotationData.h
    include/EntityBase.h
    include/EntityBinaryData.h
    include/EntityCartesianVector.h
    include/EntityContainer.h
    include/EntityCompressedVector.h
    include/EntityFacetData.h
    include/EntityLoadingAPI.h
    include/EntityMaterial.h
    include/EntityMesh.h
    include/EntityMeshCartesian.h
    include/EntityMeshCartesianData.h
    include/EntityMeshCartesianFace.h
    include/EntityMeshCartesianFaceList.h
    include/EntityMeshCartesianItem.h
    include/EntityMeshCartesianNodes.h
    include/EntityMeshTet.h
    include/EntityMeshTetData.h
    include/EntityMeshTetFace.h
    include/EntityMeshTetFaceData.h
    include/EntityMeshTetInfo.h
    include/EntityMeshTetItem.h
    include/EntityMeshTetItemDataFaces.h
    include/EntityMeshTetItemDataTetedges.h
    include/EntityMeshTetItemDataTets.h
    include/EntityMeshTetNodes.h
    include/EntityParameter.h
    include/EntityParameterizedDataCategorization.h
    include/EntityWithDynamicFields.h
    include/EntityParameterizedDataPreviewTable.h
    include/EntityFile.h
    include/EntityFileCSV.h
    include/EntityParameterizedDataTable.h
    include/EntityProperties.h
    include/EntityPropertiesItems.h
    include/EntityResult3D.h
    include/EntityResult3DData.h
    include/EntityResultBase.h
    include/EntityResultTable.h
    include/EntityResultTable.hpp
    include/EntityResultTableData.h
    include/EntityResultTableData.hpp
    include/EntitySignalType.h
    include/EntitySolver.h
    include/EntitySolverFITTD.h
    include/EntitySolverKriging.h
    include/EntitySolverMonitor.h
    include/EntitySolverPHREEC.h
    include/EntitySolverPort.h
    include/EntityTableSelectedRanges.h
    include/EntityUnits.h
    include/EntityVis2D3D.h
    include/EntityVisCartesianFaceScalar.h
    include/GenericDocument.h
    include/Geometry.h
    include/ModelEntitiesAPIExport.h
    include/ModelState.h
    include/NewModelStateInfo.h
    include/PlaneProperties.h
    include/PropertiesVis2D3D.h
    include/PropertiesVisUnstructuredScalar.h
    include/PropertiesVisUnstructuredScalarSurface.h
    include/PropertiesVisUnstructuredVector.h
    include/PropertyBundle.h
    include/PropertyBundleDataHandlePlane.h
    include/PropertyBundleDataHandleScaling.h
    include/PropertyBundleDataHandleVisUnstructuredScalar.h
    include/PropertyBundleDataHandleVisUnstructuredScalarSurface.h
    include/PropertyBundleDataHandleVisUnstructuredVector.h
    include/PropertyBundleDataHandleVis2D3D.h
    include/PropertyBundleDataHandleVisUnstructuredVectorSurface.h
    include/PropertyBundlePlane.h
    include/PropertyBundleQuerySettings.h
    include/PropertyBundleScaling.h
    include/PropertyBundleVisUnstructuredScalarVolume.h
    include/PropertyBundleVisUnstructuredScalarSurface.h
    include/PropertyBundleVisUnstructuredVectorSurface.h
    include/PropertyBundleVisUnstructuredVectorVolume.h
    include/PropertyBundleVis2D3D.h
    include/PropertyHelper.h
    include/ScalingProperties.h
    include/SerializeWithBSON.h
    include/TemplateDefaultManager.h
    include/OldTreeIcon.h
    include/VariableToBSONConverter.h
    include/VisualisationTypeSerialiser.h
)

set(MODELENTITIES_SOURCE_FILES
    include/PropertyBundleDataHandle.h       # compiled in original .vcxproj
    src/BoundingBox.cpp
    src/BSONToVariableConverter.cpp
    src/EntityCallbackBase.cpp
    src/EntityDatabaseIndexManipulator.cpp
    src/EntityFactory.cpp
    src/CSVToTableTransformer.cpp
    src/DataBase.cpp
    src/EntityAnnotation.cpp
    src/EntityAnnotationData.cpp
    src/EntityAPI.cpp
    src/EntityBase.cpp
    src/EntityBatchImporter.cpp
    src/EntityBinaryData.cpp
    src/EntityCartesianVector.cpp
    src/EntityContainer.cpp
    src/EntityCompressedVector.cpp
    src/EntityCoordinates2D.cpp
    src/EntityFacetData.cpp
    src/EntityFileImage.cpp
    src/EntityFileRawData.cpp
    src/EntityFileText.cpp
    src/EntityGraphicsScene.cpp
    src/EntityInformation.cpp
    src/EntityMaterial.cpp
    src/EntityMesh.cpp
    src/EntityMetadataSeries.cpp
    src/EntityMeshCartesian.cpp
    src/EntityMeshCartesianData.cpp
    src/EntityMeshCartesianFace.cpp
    src/EntityMeshCartesianFaceList.cpp
    src/EntityMeshCartesianItem.cpp
    src/EntityMeshCartesianNodes.cpp
    src/EntityMeshTet.cpp
    src/EntityMeshTetData.cpp
    src/EntityMeshTetFace.cpp
    src/EntityMeshTetFaceData.cpp
    src/EntityMeshTetInfo.cpp
    src/EntityMeshTetItem.cpp
    src/EntityMeshTetItemDataFaces.cpp
    src/EntityMeshTetItemDataTetedges.cpp
    src/EntityMeshTetItemDataTets.cpp
    src/EntityMeshTetNodes.cpp
    src/EntityParameter.cpp
    src/EntityParameterizedDataCategorization.cpp
    src/EntityMetadataCampaign.cpp
    src/EntityPythonManifest.cpp
    src/EntityResult1DCurve.cpp
    src/EntityResult1DPlot.cpp
    src/EntityResultTable.cpp
    src/EntityResultTableData.cpp
    src/EntityResultText.cpp
    src/EntityResultTextData.cpp
    src/EntityResultUnstructuredMesh.cpp
    src/EntityResultUnstructuredMeshData.cpp
    src/EntityResultUnstructuredMeshVtk.cpp
    src/EntitySolverCircuitSimulator.cpp
    src/EntitySolverDataProcessing.cpp
    src/EntitySolverElmerFEM.cpp
    src/EntitySolverFDTD.cpp
    src/EntitySolverPyrit.cpp
    src/EntityVisUnstructuredScalarSurface.cpp
    src/EntityVisUnstructuredScalarVolume.cpp
    src/EntityVisUnstructuredVectorSurface.cpp
    src/EntityVisUnstructuredVectorVolume.cpp
    src/EntityWithDynamicFields.cpp
    src/EntityParameterizedDataPreviewTable.cpp
    src/EntityFile.cpp
    src/EntityFileCSV.cpp
    src/EntityParameterizedDataTable.cpp
    src/EntityProperties.cpp
    src/EntityPropertiesItems.cpp
    src/EntityResult3D.cpp
    src/EntityResult3DData.cpp
    src/EntitySignalType.cpp
    src/EntitySolver.cpp
    src/EntitySolverFITTD.cpp
    src/EntitySolverGetDP.cpp
    src/EntitySolverKriging.cpp
    src/EntitySolverMonitor.cpp
    src/EntitySolverPHREEC.cpp
    src/EntitySolverPort.cpp
    src/EntityTableSelectedRanges.cpp
    src/EntityUnits.cpp
    src/EntityVis2D3D.cpp
    src/EntityVisCartesianFaceScalar.cpp
    src/GenericBsonDocument.cpp
    src/GenericDocument.cpp
    src/Geometry.cpp
    src/ModelState.cpp
    src/NewModelStateInfo.cpp
    src/PropertyBundleDataHandlePlane.cpp
    src/PropertyBundleDataHandleScaling.cpp
    src/PropertyBundleDataHandleVis2D3D.cpp
    src/PropertyBundleDataHandleVisUnstructuredScalar.cpp
    src/PropertyBundleDataHandleVisUnstructuredScalarSurface.cpp
    src/PropertyBundleDataHandleVisUnstructuredVector.cpp
    src/PropertyBundleDataHandleVisUnstructuredVectorSurface.cpp
    src/PropertyBundlePlane.cpp
    src/PropertyBundleQuerySettings.cpp
    src/PropertyBundleScaling.cpp
    src/PropertyBundleVis2D3D.cpp
    src/PropertyBundleVisUnstructuredScalarVolume.cpp
    src/PropertyBundleVisUnstructuredScalarSurface.cpp
    src/PropertyBundleVisUnstructuredVectorSurface.cpp
    src/PropertyBundleVisUnstructuredVectorVolume.cpp
    src/PropertyHelper.cpp
    src/TemplateDefaultManager.cpp
    src/OldTreeIcon.cpp
    src/VariableToBSONConverter.cpp
    src/VisualisationTypeSerialiser.cpp
)

# ============================================================
# Core OBJECT library
# ============================================================
add_library(ModelEntities_core OBJECT
    ${MODELENTITIES_SOURCE_FILES}
    ${MODELENTITIES_HEADER_FILES}
)

target_compile_definitions(ModelEntities_core
    PRIVATE
        UNICODE
        _UNICODE
        _WINDOWS
        _USRDLL
        OPENTWINMODELENTITIES_EXPORTS
        _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
        _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

# ============================================================
# Include directories
# Mirrors AdditionalIncludeDirectories from the .vcxproj
# ============================================================
target_include_directories(ModelEntities_core
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# $(OT_*_ROOT)\$(OT_INC)
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(ModelEntities_core PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(ModelEntities_core PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_GUI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(ModelEntities_core PRIVATE
        "${OT_GUI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_COMMUNICATION_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(ModelEntities_core PRIVATE
        "${OT_COMMUNICATION_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_DATASTORAGE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(ModelEntities_core PRIVATE
        "${OT_DATASTORAGE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# RapidJSON
if(R_JSON_INCD_PATH)
    target_include_directories(ModelEntities_core PRIVATE "${R_JSON_INCD_PATH}")
endif()
if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(ModelEntities_core PRIVATE "${R_JSON_INCR_PATH}")
endif()

# Mongo C++ / C / Boost includes
if(MONGO_CXX_INC_PATH)
    target_include_directories(ModelEntities_core PRIVATE "${MONGO_CXX_INC_PATH}")
endif()

if(MONGO_C_INC_PATH)
    target_include_directories(ModelEntities_core PRIVATE "${MONGO_C_INC_PATH}")
endif()

if(MONGO_BOOST_INCD_PATH)
    target_include_directories(ModelEntities_core PRIVATE "${MONGO_BOOST_INCD_PATH}")
endif()

if(MONGO_BOOST_INCR_PATH AND NOT MONGO_BOOST_INCR_PATH STREQUAL MONGO_BOOST_INCD_PATH)
    target_include_directories(ModelEntities_core PRIVATE "${MONGO_BOOST_INCR_PATH}")
endif()

# ============================================================
# MSVC compiler flags
# ============================================================
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    target_compile_options(ModelEntities_core PRIVATE
        /W3
        /sdl-
        /MP
        /permissive-
        /Zc:__cplusplus
        /wd4996
        /wd4828
    )
endif()

# ============================================================
# Visual Studio filters (simple grouping)
# ============================================================
assign_filter("Source Files"
    ${MODELENTITIES_SOURCE_FILES}
)

assign_filter("Header Files"
    ${MODELENTITIES_HEADER_FILES}
)

# ============================================================
# Shared Library (DLL) ModelEntities
# ============================================================
add_library(ModelEntities SHARED
    $<TARGET_OBJECTS:ModelEntities_core>
)

target_include_directories(ModelEntities
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(WIN32)
    target_compile_definitions(ModelEntities
        PRIVATE
            UNICODE
            _UNICODE
            _WINDOWS
            _USRDLL
            OPENTWINMODELENTITIES_EXPORTS
    )

    # OTSystem / OTCore / OTGui / OTCommunication / DataStorage library directories
    join_paths(OT_SYSTEM_DEBUG_LIB_DIR        "${OT_SYSTEM_ROOT_PATH}"        "${OT_CLIBD_PATH}")
    join_paths(OT_SYSTEM_RELEASE_LIB_DIR      "${OT_SYSTEM_ROOT_PATH}"        "${OT_CLIBR_PATH}")
    join_paths(OT_CORE_DEBUG_LIB_DIR          "${OT_CORE_ROOT_PATH}"          "${OT_CLIBD_PATH}")
    join_paths(OT_CORE_RELEASE_LIB_DIR        "${OT_CORE_ROOT_PATH}"          "${OT_CLIBR_PATH}")
    join_paths(OT_GUI_DEBUG_LIB_DIR           "${OT_GUI_ROOT_PATH}"           "${OT_CLIBD_PATH}")
    join_paths(OT_GUI_RELEASE_LIB_DIR         "${OT_GUI_ROOT_PATH}"           "${OT_CLIBR_PATH}")
    join_paths(OT_COMM_DEBUG_LIB_DIR          "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBD_PATH}")
    join_paths(OT_COMM_RELEASE_LIB_DIR        "${OT_COMMUNICATION_ROOT_PATH}" "${OT_CLIBR_PATH}")
    join_paths(OT_DATASTORAGE_DEBUG_LIB_DIR   "${OT_DATASTORAGE_ROOT_PATH}"   "${OT_CLIBD_PATH}")
    join_paths(OT_DATASTORAGE_RELEASE_LIB_DIR "${OT_DATASTORAGE_ROOT_PATH}"   "${OT_CLIBR_PATH}")

    if(OT_SYSTEM_DEBUG_LIB_DIR)
        target_link_directories(ModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(ModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_CORE_DEBUG_LIB_DIR)
        target_link_directories(ModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_CORE_RELEASE_LIB_DIR)
        target_link_directories(ModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_GUI_DEBUG_LIB_DIR)
        target_link_directories(ModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_GUI_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_GUI_RELEASE_LIB_DIR)
        target_link_directories(ModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_GUI_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_COMM_DEBUG_LIB_DIR)
        target_link_directories(ModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_COMM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_COMM_RELEASE_LIB_DIR)
        target_link_directories(ModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_COMM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_DATASTORAGE_DEBUG_LIB_DIR)
        target_link_directories(ModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_DATASTORAGE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_DATASTORAGE_RELEASE_LIB_DIR)
        target_link_directories(ModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
        )
    endif()

    # Mongo C++ / C library directories
    if(MONGO_CXX_LIBPATHD_PATH)
        target_link_directories(ModelEntities PRIVATE
            $<$<CONFIG:Debug>:${MONGO_CXX_LIBPATHD_PATH}>
        )
    endif()

    if(MONGO_CXX_LIBPATHR_PATH)
        target_link_directories(ModelEntities PRIVATE
            $<$<CONFIG:Release>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${MONGO_CXX_LIBPATHR_PATH}>
        )
    endif()

    if(MONGO_C_LIBPATHD_PATH)
        target_link_directories(ModelEntities PRIVATE
            $<$<CONFIG:Debug>:${MONGO_C_LIBPATHD_PATH}>
        )
    endif()

    if(MONGO_C_LIBPATHR_PATH)
        target_link_directories(ModelEntities PRIVATE
            $<$<CONFIG:Release>:${MONGO_C_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${MONGO_C_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${MONGO_C_LIBPATHR_PATH}>
        )
    endif()

    # Libraries (equivalent to AdditionalDependencies in the vcxproj)
    target_link_libraries(ModelEntities
        PRIVATE
            OTSystem
            OTCore
            OTGui
            OTCommunication
            DataStorage
            ${MONGO_CXX_LIBS_NAME}
            ${MONGO_C_LIBS_NAME}
    )

    if(MSVC)
        target_link_options(ModelEntities PRIVATE
            /SUBSYSTEM:WINDOWS
            /MANIFESTUAC:NO
            $<$<CONFIG:Release>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:RelWithDebInfo>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:MinSizeRel>:/OPT:REF /OPT:ICF>
        )
    endif()
endif()

# ============================================================
# GoogleTest + test integration (ModelEntities_tests in tests/)
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
