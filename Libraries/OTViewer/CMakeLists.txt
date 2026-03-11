cmake_minimum_required(VERSION 3.20)
project(OTViewer LANGUAGES CXX)

# shared env helpers
if(NOT DEFINED ENV{OT_CMAKE_DIR} OR "$ENV{OT_CMAKE_DIR}" STREQUAL "")
    message(FATAL_ERROR
        "OT_CMAKE_DIR environment variable is not set. "
        "Run SetupEnvironment.bat first (it should set OT_CMAKE_DIR)."
    )
endif()

# include helpers + parsed variables
include("$ENV{OT_CMAKE_DIR}/OTEnvironment.cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)

find_package(Qt6 REQUIRED COMPONENTS
    Core Gui Widgets Svg Xml OpenGLWidgets SvgWidgets
)

# sources
file(GLOB_RECURSE OTVIEWER_SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE OTVIEWER_HEADER_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

add_library(OTViewer_core OBJECT
    ${OTVIEWER_SOURCE_FILES}
    ${OTVIEWER_HEADER_FILES}
)

# compile happens here
target_link_libraries(OTViewer_core PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::Xml Qt6::OpenGLWidgets Qt6::SvgWidgets
)

target_compile_definitions(OTViewer_core PRIVATE
    UNICODE _UNICODE _WINDOWS
    OT_VIEWER_EXPORTS
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

target_include_directories(OTViewer_core PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# ---- includes ----

# OT_INC
if(DEFINED ENV{OT_INC} AND NOT "$ENV{OT_INC}" STREQUAL "")
    set(_ot_inc "$ENV{OT_INC}")
else()
    set(_ot_inc "include")
endif()

ot_add_env_include_dirs_with_suffix(OTViewer_core "${_ot_inc}"
    OT_SYSTEM_ROOT
    OT_CORE_ROOT
    OT_GUI_ROOT
    OT_WIDGETS_ROOT
    OT_DATASTORAGE_ROOT
    OT_MODELENTITIES_ROOT
    OT_FOUNDATION_ROOT
    OT_COMMUNICATION_ROOT
    OT_RUBBERBANDAPI_ROOT
    OT_RUBBERBAND_OSG_ROOT
)

ot_add_env_include_dirs_with_suffix(OTViewer_core "include"
    QT_ADS_ROOT
)

ot_add_env_include_dirs(OTViewer_core
    MONGO_CXX_INC MONGO_CXX_INCD MONGO_CXX_INCR MONGO_C_INCD MONGO_C_INCR
    OSG_INCD OSG_INCR
    QT_INC
    QWT_LIB_INCD QWT_LIB_INCR QWTPOLAR_LIB_INCD QWTPOLAR_LIB_INCR
    CGAL_INCD CGAL_INCR GMP_INCD GMP_INCR BOOST_INCD BOOST_INCR
    EARCUT_INC
    MONGO_BOOST_ROOT
)

if(R_JSON_INCD_PATH)
    target_include_directories(OTViewer_core PRIVATE "${R_JSON_INCD_PATH}")
endif()

if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(OTViewer_core PRIVATE "${R_JSON_INCR_PATH}")
endif()

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    target_compile_options(OTViewer_core PRIVATE /W3 /MP /permissive- /Zc:__cplusplus /wd4996)
endif()

# ---- library ----
add_library(OTViewer SHARED $<TARGET_OBJECTS:OTViewer_core>)
target_include_directories(OTViewer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# ---- link dirs ----
if(WIN32)
    if(OT_CLIBD_PATH AND OT_CLIBR_PATH)
        ot_add_env_link_dirs_with_config_suffixes(OTViewer "${OT_CLIBD_PATH}" "${OT_CLIBR_PATH}"
            OT_SYSTEM_ROOT
            OT_CORE_ROOT
            OT_GUI_ROOT
            OT_WIDGETS_ROOT
            OT_MODELENTITIES_ROOT
            OT_FOUNDATION_ROOT
            OT_COMMUNICATION_ROOT
            OT_DATASTORAGE_ROOT
            OT_RUBBERBANDAPI_ROOT
        )
    endif()

    if(OT_LIBD_PATH AND OT_LIBR_PATH)
        ot_add_env_link_dirs_with_config_suffixes(OTViewer "${OT_LIBD_PATH}" "${OT_LIBR_PATH}"
            OT_RUBBERBAND_OSG_ROOT
        )
    endif()

    ot_add_env_link_dirs(OTViewer
        MONGO_CXX_LIBPATHD MONGO_CXX_LIBPATHR MONGO_C_LIBPATHD MONGO_C_LIBPATHR
        OSG_LIBPATHD OSG_LIBPATHR
        QWT_LIB_LIBPATHD QWT_LIB_LIBPATHR QWTPOLAR_LIB_LIBPATHD QWTPOLAR_LIB_LIBPATHR
        GMP_LIBPATHD GMP_LIBPATHR BOOST_LIBPATHD BOOST_LIBPATHR
        CURL_LIBPATHD CURL_LIBPATHR
    )

    ot_add_env_link_dirs_with_suffix(OTViewer "lib"
        QT_ADS_ROOT
    )
endif()

# ---- link libs  ----
target_link_libraries(OTViewer PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::Xml Qt6::OpenGLWidgets Qt6::SvgWidgets

    OTSystem OTCore OTGui OTWidgets OTDataStorage OTRubberband OTRubberbandOSG
    OTCommunication OTServiceFoundation OTModelEntities

    $<$<CONFIG:Debug>:$ENV{OSG_LIBD}>
    $<$<NOT:$<CONFIG:Debug>>:$ENV{OSG_LIBR}>

    $<$<CONFIG:Debug>:$ENV{QWT_LIB_LIBD}>
    $<$<NOT:$<CONFIG:Debug>>:$ENV{QWT_LIB_LIBR}>

    $<$<CONFIG:Debug>:qtadvanceddockingd.lib>
    $<$<NOT:$<CONFIG:Debug>>:qtadvanceddocking.lib>

    $ENV{MONGO_CXX_LIB}
    $<$<CONFIG:Debug>:$ENV{MONGO_CXX_LIBD}>
    $<$<NOT:$<CONFIG:Debug>>:$ENV{MONGO_CXX_LIBR}>

    $<$<CONFIG:Debug>:$ENV{MONGO_C_LIBD}>
    $<$<NOT:$<CONFIG:Debug>>:$ENV{MONGO_C_LIBR}>

    libgmp-10.lib
)

if(MSVC)
    target_link_options(OTViewer PRIVATE
        /SUBSYSTEM:WINDOWS
        /MANIFESTUAC:NO
        $<$<CONFIG:Release>:/OPT:REF /OPT:ICF>
        $<$<CONFIG:RelWithDebInfo>:/OPT:REF /OPT:ICF>
        $<$<CONFIG:MinSizeRel>:/OPT:REF /OPT:ICF>
    )
endif()

# ============================================================
# test integration
# ============================================================
include(CTest)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
