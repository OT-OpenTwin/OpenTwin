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
    include/OTGui/Dialog/DialogCfg.h
    include/OTGui/Dialog/PropertyDialogCfg.h
    include/OTGui/Dialog/Painter2DDialogFilter.h
    include/OTGui/Dialog/Painter2DDialogFilterDefaults.h
    include/OTGui/Dialog/ModelLibraryDialogCfg.h
    include/OTGui/Dialog/MessageDialogCfg.h
    include/OTGui/Dialog/OnePropertyDialogCfg.h
    include/OTGui/Event/GuiEvent.h
    include/OTGui/Event/GraphicsChangeEvent.h
    include/OTGui/Event/GraphicsConnectionDropEvent.h
    include/OTGui/Event/GraphicsDoubleClickEvent.h
    include/OTGui/Event/GraphicsItemDropEvent.h
    include/OTGui/Graphics/GraphicsArcItemCfg.h
    include/OTGui/Graphics/GraphicsConnectionCalculationData.h
    include/OTGui/Graphics/GraphicsGroupItemCfg.h
    include/OTGui/Graphics/GraphicsInvisibleItemCfg.h
    include/OTGui/Graphics/GraphicsItemCfgFactory.h
    include/OTGui/Graphics/GraphicsItemFileCfg.h
    include/OTGui/Graphics/GraphicsLineItemCfg.h
    include/OTGui/Graphics/GraphicsPickerItemInfo.h
    include/OTGui/Graphics/GraphicsPolygonItemCfg.h
    include/OTGui/Graphics/GraphicsShapeItemCfg.h
    include/OTGui/Graphics/GraphicsBoxLayoutItemCfg.h
    include/OTGui/Graphics/GraphicsPickerCollectionCfg.h
    include/OTGui/Graphics/GraphicsConnectionCfg.h
    include/OTGui/Graphics/GraphicsEllipseItemCfg.h
    include/OTGui/Graphics/GraphicsGridLayoutItemCfg.h
    include/OTGui/Graphics/GraphicsHBoxLayoutItemCfg.h
    include/OTGui/Graphics/GraphicsImageItemCfg.h
    include/OTGui/Graphics/GraphicsPackage.h
    include/OTGui/Graphics/GraphicsFlowItemBuilder.h
    include/OTGui/Graphics/GraphicsItemCfg.h
    include/OTGui/Graphics/GraphicsLayoutItemCfg.h
    include/OTGui/Graphics/GraphicsRectangularItemCfg.h
    include/OTGui/Graphics/GraphicsStackItemCfg.h
    include/OTGui/Graphics/GraphicsTextItemCfg.h
    include/OTGui/Graphics/GraphicsTriangleItemCfg.h
    include/OTGui/Graphics/GraphicsVBoxLayoutItemCfg.h
    include/OTGui/Graphics/BasicGraphicsIntersectionItem.h
    include/OTGui/Graphics/GraphicsHierarchicalProjectItemBuilder.h
    include/OTGui/Graphics/GraphicsZValues.h
    include/OTGui/Properties/Property.h
    include/OTGui/Properties/PropertyManagerIterator.h
    include/OTGui/Properties/PropertyManagerSilencer.h
    include/OTGui/Properties/PropertyReadCallbackNotifier.h
    include/OTGui/Properties/PropertyBase.h
    include/OTGui/Properties/PropertyBool.h
    include/OTGui/Properties/PropertyColor.h
    include/OTGui/Properties/PropertyDouble.h
    include/OTGui/Properties/PropertyFactory.h
    include/OTGui/Properties/PropertyFactoryRegistrar.h
    include/OTGui/Properties/PropertyFactoryRegistrar.hpp
    include/OTGui/Properties/PropertyPath.h
    include/OTGui/Properties/PropertyGridCfg.h
    include/OTGui/Properties/PropertyGroup.h
    include/OTGui/Properties/PropertyInt.h
    include/OTGui/Properties/PropertyManager.h
    include/OTGui/Properties/PropertyManagerNotifier.h
    include/OTGui/Properties/PropertyWriteCallbackNotifier.h
    include/OTGui/Properties/PropertyPainter2D.h
    include/OTGui/Properties/PropertyString.h
    include/OTGui/Properties/PropertyStringList.h
    include/OTGui/Properties/ManagedPropertyObject.h
    include/OTGui/Properties/ManagedPropertyObjectBase.h
    include/OTGui/Properties/ManagedPropertyRegistrar.h
    include/OTGui/Properties/ManagedSpecialPropertyRegistrar.h
    include/OTGui/Painter/FillPainter2D.h
    include/OTGui/Painter/Painter2D.h
    include/OTGui/Painter/Painter2DFactory.h
    include/OTGui/Painter/CheckerboardPainter2D.h
    include/OTGui/Painter/GradientPainter2D.h
    include/OTGui/Painter/GradientPainterStop2D.h
    include/OTGui/Painter/LinearGradientPainter2D.h
    include/OTGui/Painter/RadialGradientPainter2D.h
    include/OTGui/Painter/PainterRainbowIterator.h
    include/OTGui/Painter/StyleRefPainter2D.h
    include/OTGui/Border.h
    include/OTGui/CheckBoxCfg.h
    include/OTGui/ColorStyleTypes.h
    include/OTGui/CopyEntityInformation.h
    include/OTGui/CopyInformation.h
    include/OTGui/DefaultPropertyValues.h
    include/OTGui/EntityTreeItem.h
    include/OTGui/ExtendedProjectInformation.h
    include/OTGui/FileExtension.h
    include/OTGui/LibraryModel.h
    include/OTGui/LineEditCfg.h
    include/OTGui/MenuCfg.h
    include/OTGui/MenuClickableEntryCfg.h
    include/OTGui/MenuEntryCfg.h
    include/OTGui/MenuEntryCfgFactory.h
    include/OTGui/MenuButtonCfg.h
    include/OTGui/DefaultSyntaxHighlighterRules.h
    include/OTGui/Grid.h
    include/OTGui/KeySequence.h
    include/OTGui/MenuSeparatorCfg.h
    include/OTGui/NavigationTreeItemIcon.h
    include/OTGui/PDFWidgetCfg.h
    include/OTGui/PenCfg.h
    include/OTGui/Font.h
    include/OTGui/GuiTypes.h
    include/OTGui/Margins.h
    include/OTGui/NavigationTreeItem.h
    include/OTGui/NavigationTreePackage.h
    include/OTGui/OTGuiAPIExport.h
    include/OTGui/Path2D.h
    include/OTGui/Plot1DAxisCfg.h
    include/OTGui/Plot1DCfg.h
    include/OTGui/Plot1DCurveCfg.h
    include/OTGui/Plot1DDataBaseCfg.h
    include/OTGui/ProjectTemplateInformation.h
    include/OTGui/QuantityContainerEntryDescription.h
    include/OTGui/QueryInformation.h
    include/OTGui/StyledText.h
    include/OTGui/StyledTextBuilder.h
    include/OTGui/StyledTextEntry.h
    include/OTGui/StyledTextStyle.h
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
    src/Dialog/PropertyDialogCfg.cpp
    src/Dialog/Painter2DDialogFilter.cpp
    src/Dialog/Painter2DDialogFilterDefaults.cpp
    src/Dialog/ModelLibraryDialogCfg.cpp
    src/Dialog/MessageDialogCfg.cpp
    src/Dialog/DialogCfg.cpp
    src/Dialog/OnePropertyDialogCfg.cpp
    src/Dialog/SelectEntitiesDialogCfg.cpp
    src/Event/GuiEvent.cpp
    src/Event/GraphicsChangeEvent.cpp
    src/Event/GraphicsConnectionDropEvent.cpp
    src/Event/GraphicsDoubleClickEvent.cpp
    src/Event/GraphicsItemDropEvent.cpp
    src/Painter/CheckerboardPainter2D.cpp
    src/Painter/FillPainter2D.cpp
    src/Painter/GradientPainter2D.cpp
    src/Painter/GradientPainterStop2D.cpp
    src/Painter/LinearGradientPainter2D.cpp
    src/Painter/Painter2D.cpp
    src/Painter/Painter2DFactory.cpp
    src/Painter/RadialGradientPainter2D.cpp
    src/Painter/PainterRainbowIterator.cpp
    src/Painter/StyleRefPainter2D.cpp
    src/Graphics/BasicGraphicsIntersectionItem.cpp
    src/Graphics/GraphicsArcItemCfg.cpp
    src/Graphics/GraphicsConnectionCalculationData.cpp
    src/Graphics/GraphicsGroupItemCfg.cpp
    src/Graphics/GraphicsHierarchicalProjectItemBuilder.cpp
    src/Graphics/GraphicsInvisibleItemCfg.cpp
    src/Graphics/GraphicsItemCfgFactory.cpp
    src/Graphics/GraphicsItemFileCfg.cpp
    src/Graphics/GraphicsLineItemCfg.cpp
    src/Graphics/GraphicsPickerItemInfo.cpp
    src/Graphics/GraphicsPolygonItemCfg.cpp
    src/Graphics/GraphicsShapeItemCfg.cpp
    src/Graphics/GraphicsBoxLayoutItemCfg.cpp
    src/Graphics/GraphicsPickerCollectionCfg.cpp
    src/Graphics/GraphicsConnectionCfg.cpp
    src/Graphics/GraphicsEllipseItemCfg.cpp
    src/Graphics/GraphicsGridLayoutItemCfg.cpp
    src/Graphics/GraphicsHBoxLayoutItemCfg.cpp
    src/Graphics/GraphicsImageItemCfg.cpp
    src/Graphics/GraphicsPackage.cpp
    src/Graphics/GraphicsFlowItemBuilder.cpp
    src/Graphics/GraphicsItemCfg.cpp
    src/Graphics/GraphicsLayoutItemCfg.cpp
    src/Graphics/GraphicsRectangularItemCfg.cpp
    src/Graphics/GraphicsStackItemCfg.cpp
    src/Graphics/GraphicsTextItemCfg.cpp
    src/Graphics/GraphicsTriangleItem.cpp
    src/Graphics/GraphicsVBoxLayoutItemCfg.cpp
    src/Properties/Property.cpp
    src/Properties/PropertyManagerSilencer.cpp
    src/Properties/PropertyReadCallbackNotifier.cpp
    src/Properties/PropertyBase.cpp
    src/Properties/PropertyBool.cpp
    src/Properties/PropertyColor.cpp
    src/Properties/PropertyDouble.cpp
    src/Properties/PropertyFactory.cpp
    src/Properties/PropertyPath.cpp
    src/Properties/PropertyGridCfg.cpp
    src/Properties/PropertyGroup.cpp
    src/Properties/PropertyInt.cpp
    src/Properties/PropertyManager.cpp
    src/Properties/PropertyManagerNotifier.cpp
    src/Properties/PropertyPainter2D.cpp
    src/Properties/PropertyString.cpp
    src/Properties/PropertyStringList.cpp
    src/Properties/ManagedPropertyObject.cpp
    src/Border.cpp
    src/CheckBoxCfg.cpp
    src/ColorStyleTypes.cpp
    src/CopyEntityInformation.cpp
    src/CopyInformation.cpp
    src/DefaultPropertyValues.cpp
    src/DefaultSyntaxHighlighterRules.cpp
    src/EntityTreeItem.cpp
    src/ExtendedProjectInformation.cpp
    src/FileExtension.cpp
    src/Grid.cpp
    src/KeySequence.cpp
    src/LibraryModel.cpp
    src/LineEditCfg.cpp
    src/MenuCfg.cpp
    src/MenuClickableEntryCfg.cpp
    src/MenuEntryCfg.cpp
    src/MenuEntryCfgFactory.cpp
    src/MenuButtonCfg.cpp
    src/MenuSeparatorCfg.cpp
    src/NavigationTreeItemIcon.cpp
    src/PDFWidgetCfg.cpp
    src/PenCfg.cpp
    src/dllmain.cpp
    src/Font.cpp
    src/GuiTypes.cpp
    src/Margins.cpp
    src/NavigationTreeItem.cpp
    src/NavigationTreePackage.cpp
    src/Path2D.cpp
    src/Plot1DAxisCfg.cpp
    src/Plot1DCfg.cpp
    src/Plot1DCurveCfg.cpp
    src/Plot1DDataBaseCfg.cpp
    src/ProjectTemplateInformation.cpp
    src/QuantityContainerEntryDescription.cpp
    src/QueryInformation.cpp
    src/StyledText.cpp
    src/StyledTextBuilder.cpp
    src/StyledTextEntry.cpp
    src/StyledTextStyle.cpp
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
