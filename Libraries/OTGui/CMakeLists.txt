cmake_minimum_required(VERSION 3.20)

project(OTGui LANGUAGES CXX)

# ============================================================
# Helper functions (same style as OTCore / DataStorage)
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
get_env_path(OT_GUI_ROOT_PATH      OT_GUI_ROOT)
get_env_path(OT_SYSTEM_ROOT_PATH   OT_SYSTEM_ROOT)
get_env_path(OT_CORE_ROOT_PATH     OT_CORE_ROOT)
get_env_path(OT_INC_PATH           OT_INC)

# OT library subdirs
get_env_path(OT_CLIBD_PATH         OT_CLIBD)
get_env_path(OT_CLIBR_PATH         OT_CLIBR)
get_env_path(OT_LIBD_PATH          OT_LIBD)
get_env_path(OT_LIBR_PATH          OT_LIBR)

# RapidJSON include
get_env_path(R_JSON_INCD_PATH      R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH      R_JSON_INCR)

# ============================================================
# C++ standard
# OTGui uses C++20 in the original vcxproj
# ============================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# Source and header lists (from .vcxproj)
# ============================================================

set(OTGUI_HEADER_FILES
    include/OTGui/BasicGraphicsIntersectionItem.h
    include/OTGui/Border.h
    include/OTGui/CheckBoxCfg.h
    include/OTGui/CheckerboardPainter2D.h
    include/OTGui/ColorStyleTypes.h
    include/OTGui/CopyEntityInformation.h
    include/OTGui/CopyInformation.h
    include/OTGui/DefaultPropertyValues.h
    include/OTGui/EntityTreeItem.h
    include/OTGui/ExtendedProjectInformation.h
    include/OTGui/FileExtension.h
    include/OTGui/GraphicsChangeEvent.h
    include/OTGui/GraphicsConnectionDropEvent.h
    include/OTGui/GraphicsHierarchicalProjectItemBuilder.h
    include/OTGui/GraphicsDoubleClickEvent.h
    include/OTGui/GraphicsItemDropEvent.h
    include/OTGui/GraphicsZValues.h
    include/OTGui/GuiEvent.h
    include/OTGui/LibraryModel.h
    include/OTGui/LineEditCfg.h
    include/OTGui/ManagedPropertyObject.h
    include/OTGui/ManagedPropertyObjectBase.h
    include/OTGui/ManagedPropertyRegistrar.h
    include/OTGui/ManagedSpecialPropertyRegistrar.h
    include/OTGui/MenuCfg.h
    include/OTGui/MenuClickableEntryCfg.h
    include/OTGui/MenuEntryCfg.h
    include/OTGui/MenuEntryCfgFactory.h
    include/OTGui/MenuButtonCfg.h
    include/OTGui/DefaultSyntaxHighlighterRules.h
    include/OTGui/GraphicsArcItemCfg.h
    include/OTGui/GraphicsConnectionCalculationData.h
    include/OTGui/GraphicsGroupItemCfg.h
    include/OTGui/GraphicsInvisibleItemCfg.h
    include/OTGui/GraphicsItemCfgFactory.h
    include/OTGui/GraphicsItemFileCfg.h
    include/OTGui/GraphicsLineItemCfg.h
    include/OTGui/GraphicsPickerItemInfo.h
    include/OTGui/GraphicsPolygonItemCfg.h
    include/OTGui/GraphicsShapeItemCfg.h
    include/OTGui/Grid.h
    include/OTGui/KeySequence.h
    include/OTGui/MenuSeparatorCfg.h
    include/OTGui/ModelLibraryDialogCfg.h
    include/OTGui/NavigationTreeItemIcon.h
    include/OTGui/Painter2DDialogFilter.h
    include/OTGui/Painter2DDialogFilterDefaults.h
    include/OTGui/PDFWidgetCfg.h
    include/OTGui/PenCfg.h
    include/OTGui/MessageDialogCfg.h
    include/OTGui/DialogCfg.h
    include/OTGui/FillPainter2D.h
    include/OTGui/Font.h
    include/OTGui/GradientPainter2D.h
    include/OTGui/GradientPainterStop2D.h
    include/OTGui/GraphicsBoxLayoutItemCfg.h
    include/OTGui/GraphicsPickerCollectionCfg.h
    include/OTGui/GraphicsConnectionCfg.h
    include/OTGui/GraphicsEllipseItemCfg.h
    include/OTGui/GraphicsGridLayoutItemCfg.h
    include/OTGui/GraphicsHBoxLayoutItemCfg.h
    include/OTGui/GraphicsImageItemCfg.h
    include/OTGui/GraphicsPackage.h
    include/OTGui/GraphicsFlowItemBuilder.h
    include/OTGui/GraphicsItemCfg.h
    include/OTGui/GraphicsLayoutItemCfg.h
    include/OTGui/GraphicsRectangularItemCfg.h
    include/OTGui/GraphicsStackItemCfg.h
    include/OTGui/GraphicsTextItemCfg.h
    include/OTGui/GraphicsTriangleItemCfg.h
    include/OTGui/GraphicsVBoxLayoutItemCfg.h
    include/OTGui/GuiTypes.h
    include/OTGui/LinearGradientPainter2D.h
    include/OTGui/Margins.h
    include/OTGui/NavigationTreeItem.h
    include/OTGui/NavigationTreePackage.h
    include/OTGui/OnePropertyDialogCfg.h
    include/OTGui/OTGuiAPIExport.h
    include/OTGui/Painter2D.h
    include/OTGui/Painter2DFactory.h
    include/OTGui/Path2D.h
    include/OTGui/Plot1DAxisCfg.h
    include/OTGui/Plot1DCfg.h
    include/OTGui/Plot1DCurveCfg.h
    include/OTGui/Plot1DDataBaseCfg.h
    include/OTGui/ProjectTemplateInformation.h
    include/OTGui/Property.h
    include/OTGui/PropertyManagerIterator.h
    include/OTGui/PropertyManagerSilencer.h
    include/OTGui/PropertyReadCallbackNotifier.h
    include/OTGui/PropertyBase.h
    include/OTGui/PropertyBool.h
    include/OTGui/PropertyColor.h
    include/OTGui/PropertyDialogCfg.h
    include/OTGui/PropertyDouble.h
    include/OTGui/PropertyFactory.h
    include/OTGui/PropertyFactoryRegistrar.h
    include/OTGui/PropertyFactoryRegistrar.hpp
    include/OTGui/PropertyPath.h
    include/OTGui/PropertyGridCfg.h
    include/OTGui/PropertyGroup.h
    include/OTGui/PropertyInt.h
    include/OTGui/PropertyManager.h
    include/OTGui/PropertyManagerNotifier.h
    include/OTGui/PropertyWriteCallbackNotifier.h
    include/OTGui/PropertyPainter2D.h
    include/OTGui/PropertyString.h
    include/OTGui/PropertyStringList.h
    include/OTGui/QuantityContainerEntryDescription.h
    include/OTGui/QueryInformation.h
    include/OTGui/RadialGradientPainter2D.h
    include/OTGui/PainterRainbowIterator.h
    include/OTGui/SelectEntitiesDialogCfg.h
    include/OTGui/StyledText.h
    include/OTGui/StyledTextBuilder.h
    include/OTGui/StyledTextEntry.h
    include/OTGui/StyledTextStyle.h
    include/OTGui/StyleRefPainter2D.h
    include/OTGui/StyleValue.h
    include/OTGui/SyntaxHighlighterRule.h
    include/OTGui/TableCfg.h
    include/OTGui/TableHeaderItemCfg.h
    include/OTGui/TableIndexSchemata.h
    include/OTGui/TableRange.h
    include/OTGui/TextEditorCfg.h
    include/OTGui/ToolBarButtonCfg.h
    include/OTGui/ToolButtonCfg.h
    include/OTGui/Transform.h
    include/OTGui/VersionGraphCfg.h
    include/OTGui/VersionGraphVersionCfg.h
    include/OTGui/VisualisationCfg.h
    include/OTGui/VisualisationTypes.h
    include/OTGui/WidgetBaseCfg.h
    include/OTGui/WidgetBaseCfgFactory.h
    include/OTGui/WidgetViewBase.h
)

set(OTGUI_SOURCE_FILES
    src/BasicGraphicsIntersectionItem.cpp
    src/Border.cpp
    src/CheckBoxCfg.cpp
    src/CheckerboardPainter2D.cpp
    src/ColorStyleTypes.cpp
    src/CopyEntityInformation.cpp
    src/CopyInformation.cpp
    src/DefaultPropertyValues.cpp
    src/DefaultSyntaxHighlighterRules.cpp
    src/EntityTreeItem.cpp
    src/ExtendedProjectInformation.cpp
    src/FileExtension.cpp
    src/GraphicsArcItemCfg.cpp
    src/GraphicsConnectionCalculationData.cpp
    src/GraphicsChangeEvent.cpp
    src/GraphicsConnectionDropEvent.cpp
    src/GraphicsDoubleClickEvent.cpp
    src/GraphicsGroupItemCfg.cpp
    src/GraphicsHierarchicalProjectItemBuilder.cpp
    src/GraphicsInvisibleItemCfg.cpp
    src/GraphicsItemCfgFactory.cpp
    src/GraphicsItemDropEvent.cpp
    src/GraphicsItemFileCfg.cpp
    src/GraphicsLineItemCfg.cpp
    src/GraphicsPickerItemInfo.cpp
    src/GraphicsPolygonItemCfg.cpp
    src/GraphicsShapeItemCfg.cpp
    src/Grid.cpp
    src/GuiEvent.cpp
    src/KeySequence.cpp
    src/LibraryModel.cpp
    src/LineEditCfg.cpp
    src/ManagedPropertyObject.cpp
    src/MenuCfg.cpp
    src/MenuClickableEntryCfg.cpp
    src/MenuEntryCfg.cpp
    src/MenuEntryCfgFactory.cpp
    src/MenuButtonCfg.cpp
    src/MenuSeparatorCfg.cpp
    src/ModelLibraryDialogCfg.cpp
    src/NavigationTreeItemIcon.cpp
    src/Painter2DDialogFilter.cpp
    src/Painter2DDialogFilterDefaults.cpp
    src/PDFWidgetCfg.cpp
    src/PenCfg.cpp
    src/MessageDialogCfg.cpp
    src/DialogCfg.cpp
    src/dllmain.cpp
    src/FillPainter2D.cpp
    src/Font.cpp
    src/GradientPainter2D.cpp
    src/GradientPainterStop2D.cpp
    src/GraphicsBoxLayoutItemCfg.cpp
    src/GraphicsPickerCollectionCfg.cpp
    src/GraphicsConnectionCfg.cpp
    src/GraphicsEllipseItemCfg.cpp
    src/GraphicsGridLayoutItemCfg.cpp
    src/GraphicsHBoxLayoutItemCfg.cpp
    src/GraphicsImageItemCfg.cpp
    src/GraphicsPackage.cpp
    src/GraphicsFlowItemBuilder.cpp
    src/GraphicsItemCfg.cpp
    src/GraphicsLayoutItemCfg.cpp
    src/GraphicsRectangularItemCfg.cpp
    src/GraphicsStackItemCfg.cpp
    src/GraphicsTextItemCfg.cpp
    src/GraphicsTriangleItem.cpp
    src/GraphicsVBoxLayoutItemCfg.cpp
    src/GuiTypes.cpp
    src/LinearGradientPainter2D.cpp
    src/Margins.cpp
    src/NavigationTreeItem.cpp
    src/NavigationTreePackage.cpp
    src/OnePropertyDialogCfg.cpp
    src/Painter2D.cpp
    src/Painter2DFactory.cpp
    src/Path2D.cpp
    src/Plot1DAxisCfg.cpp
    src/Plot1DCfg.cpp
    src/Plot1DCurveCfg.cpp
    src/Plot1DDataBaseCfg.cpp
    src/ProjectTemplateInformation.cpp
    src/Property.cpp
    src/PropertyManagerSilencer.cpp
    src/PropertyReadCallbackNotifier.cpp
    src/PropertyBase.cpp
    src/PropertyBool.cpp
    src/PropertyColor.cpp
    src/PropertyDialogCfg.cpp
    src/PropertyDouble.cpp
    src/PropertyFactory.cpp
    src/PropertyPath.cpp
    src/PropertyGridCfg.cpp
    src/PropertyGroup.cpp
    src/PropertyInt.cpp
    src/PropertyManager.cpp
    src/PropertyManagerNotifier.cpp
    src/PropertyPainter2D.cpp
    src/PropertyString.cpp
    src/PropertyStringList.cpp
    src/QuantityContainerEntryDescription.cpp
    src/QueryInformation.cpp
    src/RadialGradientPainter2D.cpp
    src/PainterRainbowIterator.cpp
    src/SelectEntitiesDialogCfg.cpp
    src/StyledText.cpp
    src/StyledTextBuilder.cpp
    src/StyledTextEntry.cpp
    src/StyledTextStyle.cpp
    src/StyleRefPainter2D.cpp
    src/StyleValue.cpp
    src/SyntaxHighlighterRule.cpp
    src/TableCfg.cpp
    src/TableHeaderItemCfg.cpp
    src/TableIndexSchemata.cpp
    src/TableRange.cpp
    src/TextEditorCfg.cpp
    src/ToolBarButtonCfg.cpp
    src/ToolButtonCfg.cpp
    src/Transform.cpp
    src/VersionGraphCfg.cpp
    src/VersionGraphVersionCfg.cpp
    src/VisualisationCfg.cpp
    src/VisualisationTypes.cpp
    src/WidgetBaseCfg.cpp
    src/WidgetBaseCfgFactory.cpp
    src/WidgetViewBase.cpp
)

# ============================================================
# Core OBJECT library (equivalent to the main .vcxproj compilation)
# ============================================================
add_library(OTGui_core OBJECT
    ${OTGUI_SOURCE_FILES}
    ${OTGUI_HEADER_FILES}
)

target_compile_definitions(OTGui_core
    PRIVATE
        UNICODE
        _UNICODE
        _WINDOWS
        _USRDLL
        OTGUI_EXPORTS
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

# ============================================================
# Include directories
# Mirrors AdditionalIncludeDirectories from the .vcxproj
# ============================================================
target_include_directories(OTGui_core
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# $(OT_GUI_ROOT)\$(OT_INC)
if(OT_GUI_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTGui_core PRIVATE
        "${OT_GUI_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# $(OT_SYSTEM_ROOT)\$(OT_INC)
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTGui_core PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# $(OT_CORE_ROOT)\$(OT_INC)
if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(OTGui_core PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# RapidJSON
if(R_JSON_INCD_PATH)
    target_include_directories(OTGui_core PRIVATE "${R_JSON_INCD_PATH}")
endif()
if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(OTGui_core PRIVATE "${R_JSON_INCR_PATH}")
endif()

# ============================================================
# MSVC compiler flags
# ============================================================
if(MSVC)
    # Use dynamic runtime (same pattern as other libs)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    target_compile_options(OTGui_core PRIVATE
        /W3
        /sdl
        /MP
        /permissive-
        /Zc:__cplusplus
    )
endif()

# ============================================================
# Visual Studio filters (simple grouping)
# ============================================================
assign_filter("Source Files"
    ${OTGUI_SOURCE_FILES}
)

assign_filter("Header Files"
    ${OTGUI_HEADER_FILES}
)

# ============================================================
# Shared Library (DLL) OTGui
# Matches DynamicLibrary configurations from the vcxproj
# ============================================================
add_library(OTGui SHARED
    $<TARGET_OBJECTS:OTGui_core>
)

target_include_directories(OTGui
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(WIN32)
    target_compile_definitions(OTGui
        PRIVATE
            UNICODE
            _UNICODE
            _WINDOWS
            _USRDLL
            OTGUI_EXPORTS
    )

    # OTSystem / OTCore library directories
    join_paths(OT_SYSTEM_DEBUG_LIB_DIR   "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBD_PATH}")
    join_paths(OT_SYSTEM_RELEASE_LIB_DIR "${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBR_PATH}")
    join_paths(OT_CORE_DEBUG_LIB_DIR     "${OT_CORE_ROOT_PATH}"   "${OT_CLIBD_PATH}")
    join_paths(OT_CORE_RELEASE_LIB_DIR   "${OT_CORE_ROOT_PATH}"   "${OT_CLIBR_PATH}")

    if(OT_SYSTEM_DEBUG_LIB_DIR)
        target_link_directories(OTGui PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(OTGui PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

    if(OT_CORE_DEBUG_LIB_DIR)
        target_link_directories(OTGui PRIVATE
            $<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_CORE_RELEASE_LIB_DIR)
        target_link_directories(OTGui PRIVATE
            $<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_CORE_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_CORE_RELEASE_LIB_DIR}>
        )
    endif()

    # Libraries (equivalent to AdditionalDependencies in the vcxproj)
    target_link_libraries(OTGui
        PRIVATE
            OTSystem
            OTCore
            Userenv
            wsock32
            Advapi32
    )

    if(MSVC)
        target_link_options(OTGui PRIVATE
            /SUBSYSTEM:WINDOWS
            /MANIFESTUAC:NO
            $<$<CONFIG:Release>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:RelWithDebInfo>:/OPT:REF /OPT:ICF>
            $<$<CONFIG:MinSizeRel>:/OPT:REF /OPT:ICF>
        )
    endif()
endif()

# ============================================================
# GoogleTest + test integration (OTGui_tests in tests/)
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
