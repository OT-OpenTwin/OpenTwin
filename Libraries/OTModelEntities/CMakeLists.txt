cmake_minimum_required(VERSION 3.20)

project(OTModelEntities LANGUAGES CXX)

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

set(OTMODELENTITIESHEADER_FILES
    include/OTModelEntities/BoundingBox.h
    include/OTModelEntities/BsonArrayTypeGetterWrapper.h
    include/OTModelEntities/BsonArrayTypeGetterWrapper.hpp
    include/OTModelEntities/BSONToVariableConverter.h
    include/OTModelEntities/EntityCallbackBase.h
    include/OTModelEntities/CSVProperties.h
    include/OTModelEntities/CSVToTableTransformer.h
    include/OTModelEntities/EntityAPI.h
    include/OTModelEntities/EntityBatchImporter.h
    include/OTModelEntities/EntityCoordinates2D.h
    include/OTModelEntities/EntityDatabaseIndexManipulator.h
    include/OTModelEntities/EntityFactoryRegistrar.h
    include/OTModelEntities/EntityFileRawData.h
    include/OTModelEntities/EntityFileImage.h
    include/OTModelEntities/EntityGraphicsScene.h
    include/OTModelEntities/EntityInformation.h
    include/OTModelEntities/EntityNamingBehavior.h
    include/OTModelEntities/EntityPythonManifest.h
    include/OTModelEntities/EntityResult1DCurve.h
    include/OTModelEntities/EntityResult1DPlot.h
    include/OTModelEntities/EntityResultText.h
    include/OTModelEntities/EntityResultTextData.h
    include/OTModelEntities/EntityResultUnstructuredMeshVtk.h
    include/OTModelEntities/EntitySolverDataProcessing.h
    include/OTModelEntities/EntitySolverFDTD.h
    include/OTModelEntities/EntitySolverPyrit.h
    include/OTModelEntities/EntityVisUnstructuredVectorSurface.h
    include/OTModelEntities/InvalidUID.h
    include/OTModelEntities/IVisualisationCurve.h
    include/OTModelEntities/IVisualisationGraphicsView.h
    include/OTModelEntities/IVisualisationImage.h
    include/OTModelEntities/IVisualisationPlot1D.h
    include/OTModelEntities/IVisualisationTable.h
    include/OTModelEntities/IVisualisationText.h
    include/OTModelEntities/EntityMetadataSeries.h
    include/OTModelEntities/EntityMetadataCampaign.h
    include/OTModelEntities/EntityResultUnstructuredMesh.h
    include/OTModelEntities/EntityResultUnstructuredMeshData.h
    include/OTModelEntities/EntitySolverCircuitSimulator.h
    include/OTModelEntities/EntitySolverElmerFEM.h
    include/OTModelEntities/EntitySolverGetDP.h
    include/OTModelEntities/EntityFileText.h
    include/OTModelEntities/EntityVisUnstructuredScalarSurface.h
    include/OTModelEntities/EntityVisUnstructuredScalarVolume.h
    include/OTModelEntities/EntityVisUnstructuredVectorVolume.h
    include/OTModelEntities/GenericBsonDocument.h
    include/OTModelEntities/EntityFactory.h
    include/OTModelEntities/DataBase.h
    include/OTModelEntities/EntityAnnotation.h
    include/OTModelEntities/EntityAnnotationData.h
    include/OTModelEntities/EntityBase.h
    include/OTModelEntities/EntityBinaryData.h
    include/OTModelEntities/EntityCartesianVector.h
    include/OTModelEntities/EntityContainer.h
    include/OTModelEntities/EntityCompressedVector.h
    include/OTModelEntities/EntityFacetData.h
    include/OTModelEntities/EntityLoadingAPI.h
    include/OTModelEntities/EntityMaterial.h
    include/OTModelEntities/EntityMesh.h
    include/OTModelEntities/EntityMeshCartesian.h
    include/OTModelEntities/EntityMeshCartesianData.h
    include/OTModelEntities/EntityMeshCartesianFace.h
    include/OTModelEntities/EntityMeshCartesianFaceList.h
    include/OTModelEntities/EntityMeshCartesianItem.h
    include/OTModelEntities/EntityMeshCartesianNodes.h
    include/OTModelEntities/EntityMeshTet.h
    include/OTModelEntities/EntityMeshTetData.h
    include/OTModelEntities/EntityMeshTetFace.h
    include/OTModelEntities/EntityMeshTetFaceData.h
    include/OTModelEntities/EntityMeshTetInfo.h
    include/OTModelEntities/EntityMeshTetItem.h
    include/OTModelEntities/EntityMeshTetItemDataFaces.h
    include/OTModelEntities/EntityMeshTetItemDataTetedges.h
    include/OTModelEntities/EntityMeshTetItemDataTets.h
    include/OTModelEntities/EntityMeshTetNodes.h
    include/OTModelEntities/EntityParameter.h
    include/OTModelEntities/EntityParameterizedDataCategorization.h
    include/OTModelEntities/EntityWithDynamicFields.h
    include/OTModelEntities/EntityParameterizedDataPreviewTable.h
    include/OTModelEntities/EntityFile.h
    include/OTModelEntities/EntityFileCSV.h
    include/OTModelEntities/EntityParameterizedDataTable.h
    include/OTModelEntities/EntityProperties.h
    include/OTModelEntities/EntityPropertiesItems.h
    include/OTModelEntities/EntityResult3D.h
    include/OTModelEntities/EntityResult3DData.h
    include/OTModelEntities/EntityResultBase.h
    include/OTModelEntities/EntityResultTable.h
    include/OTModelEntities/EntityResultTable.hpp
    include/OTModelEntities/EntityResultTableData.h
    include/OTModelEntities/EntityResultTableData.hpp
    include/OTModelEntities/EntitySignalType.h
    include/OTModelEntities/EntitySolver.h
    include/OTModelEntities/EntitySolverFITTD.h
    include/OTModelEntities/EntitySolverKriging.h
    include/OTModelEntities/EntitySolverMonitor.h
    include/OTModelEntities/EntitySolverPHREEC.h
    include/OTModelEntities/EntitySolverPort.h
    include/OTModelEntities/EntityTableSelectedRanges.h
    include/OTModelEntities/EntityUnits.h
    include/OTModelEntities/EntityVis2D3D.h
    include/OTModelEntities/EntityVisCartesianFaceScalar.h
    include/OTModelEntities/GenericDocument.h
    include/OTModelEntities/Geometry.h
    include/OTModelEntities/ModelEntitiesAPIExport.h
    include/OTModelEntities/ModelState.h
    include/OTModelEntities/NewModelStateInfo.h
    include/OTModelEntities/PlaneProperties.h
    include/OTModelEntities/PropertiesVis2D3D.h
    include/OTModelEntities/PropertiesVisUnstructuredScalar.h
    include/OTModelEntities/PropertiesVisUnstructuredScalarSurface.h
    include/OTModelEntities/PropertiesVisUnstructuredVector.h
    include/OTModelEntities/PropertyBundle.h
    include/OTModelEntities/PropertyBundleDataHandle.h
    include/OTModelEntities/PropertyBundleDataHandlePlane.h
    include/OTModelEntities/PropertyBundleDataHandleScaling.h
    include/OTModelEntities/PropertyBundleDataHandleVisUnstructuredScalar.h
    include/OTModelEntities/PropertyBundleDataHandleVisUnstructuredScalarSurface.h
    include/OTModelEntities/PropertyBundleDataHandleVisUnstructuredVector.h
    include/OTModelEntities/PropertyBundleDataHandleVis2D3D.h
    include/OTModelEntities/PropertyBundleDataHandleVisUnstructuredVectorSurface.h
    include/OTModelEntities/PropertyBundlePlane.h
    include/OTModelEntities/PropertyBundleQuerySettings.h
    include/OTModelEntities/PropertyBundleScaling.h
    include/OTModelEntities/PropertyBundleVisUnstructuredScalarVolume.h
    include/OTModelEntities/PropertyBundleVisUnstructuredScalarSurface.h
    include/OTModelEntities/PropertyBundleVisUnstructuredVectorSurface.h
    include/OTModelEntities/PropertyBundleVisUnstructuredVectorVolume.h
    include/OTModelEntities/PropertyBundleVis2D3D.h
    include/OTModelEntities/PropertyHelper.h
    include/OTModelEntities/ScalingProperties.h
    include/OTModelEntities/SerializeWithBSON.h
    include/OTModelEntities/TemplateDefaultManager.h
    include/OTModelEntities/VariableToBSONConverter.h
)

set(OTMODELENTITIESSOURCE_FILES
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
    src/VariableToBSONConverter.cpp
)

# ============================================================
# Core OBJECT library
# ============================================================
add_library(OTModelEntities_core OBJECT
    ${OTMODELENTITIESSOURCE_FILES}
    ${OTMODELENTITIESHEADER_FILES}
)

target_compile_definitions(OTModelEntities_core
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
target_include_directories(OTModelEntities_core
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# $(OT_*_ROOT)\$(OT_INC)
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelEntities_core PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelEntities_core PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_GUI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelEntities_core PRIVATE
        "${OT_GUI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_COMMUNICATION_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelEntities_core PRIVATE
        "${OT_COMMUNICATION_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

if(OT_DATASTORAGE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTModelEntities_core PRIVATE
        "${OT_DATASTORAGE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# RapidJSON
if(R_JSON_INCD_PATH)
    target_include_directories(OTModelEntities_core PRIVATE "${R_JSON_INCD_PATH}")
endif()
if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(OTModelEntities_core PRIVATE "${R_JSON_INCR_PATH}")
endif()

# Mongo C++ / C / Boost includes
if(MONGO_CXX_INC_PATH)
    target_include_directories(OTModelEntities_core PRIVATE "${MONGO_CXX_INC_PATH}")
endif()

if(MONGO_C_INC_PATH)
    target_include_directories(OTModelEntities_core PRIVATE "${MONGO_C_INC_PATH}")
endif()

if(MONGO_BOOST_INCD_PATH)
    target_include_directories(OTModelEntities_core PRIVATE "${MONGO_BOOST_INCD_PATH}")
endif()

if(MONGO_BOOST_INCR_PATH AND NOT MONGO_BOOST_INCR_PATH STREQUAL MONGO_BOOST_INCD_PATH)
    target_include_directories(OTModelEntities_core PRIVATE "${MONGO_BOOST_INCR_PATH}")
endif()

# ============================================================
# MSVC compiler flags
# ============================================================
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    target_compile_options(OTModelEntities_core PRIVATE
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
    ${OTMODELENTITIESSOURCE_FILES}
)

assign_filter("Header Files"
    ${OTMODELENTITIESHEADER_FILES}
)

# ============================================================
# Shared Library (DLL) OTModelEntities
# ============================================================
add_library(OTModelEntities SHARED
    $<TARGET_OBJECTS:OTModelEntities_core>
)

target_include_directories(OTModelEntities
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(WIN32)
    target_compile_definitions(OTModelEntities
        PRIVATE
            UNICODE
            _UNICODE
            _WINDOWS
            _USRDLL
            OPENTWINOTMODELENTITIESEXPORTS
    )

    # OTSystem / OTCore / OTGui / OTCommunication / OTDataStorage library directories
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
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_CORE_DEBUG_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_CORE_RELEASE_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_GUI_DEBUG_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_GUI_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_GUI_RELEASE_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_GUI_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_GUI_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_COMM_DEBUG_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_COMM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_COMM_RELEASE_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_COMM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_COMM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_DATASTORAGE_DEBUG_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Debug>:${OT_DATASTORAGE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_DATASTORAGE_RELEASE_LIB_DIR)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Release>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_DATASTORAGE_RELEASE_LIB_DIR}>
        )
    endif()

    # Mongo C++ / C library directories
    if(MONGO_CXX_LIBPATHD_PATH)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Debug>:${MONGO_CXX_LIBPATHD_PATH}>
        )
    endif()

    if(MONGO_CXX_LIBPATHR_PATH)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Release>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${MONGO_CXX_LIBPATHR_PATH}>
        )
    endif()

    if(MONGO_C_LIBPATHD_PATH)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Debug>:${MONGO_C_LIBPATHD_PATH}>
        )
    endif()

    if(MONGO_C_LIBPATHR_PATH)
        target_link_directories(OTModelEntities PRIVATE
            $<$<CONFIG:Release>:${MONGO_C_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${MONGO_C_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${MONGO_C_LIBPATHR_PATH}>
        )
    endif()

    # Libraries (equivalent to AdditionalDependencies in the vcxproj)
    target_link_libraries(OTModelEntities
        PRIVATE
            OTSystem
            OTCore
            OTGui
            OTCommunication
            OTDataStorage
            ${MONGO_CXX_LIBS_NAME}
            ${MONGO_C_LIBS_NAME}
    )

    if(MSVC)
        target_link_options(OTModelEntities PRIVATE
            /SUBSYSTEM:WINDOWS
            /MANIFESTUAC:NO
            $<$<CONFIG:Release>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:RelWithDebInfo>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:MinSizeRel>:/OPT:REF /OPT:ICF>
        )
    endif()
endif()

# ============================================================
# GoogleTest + test integration (OTModelEntities_tests in tests/)
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
