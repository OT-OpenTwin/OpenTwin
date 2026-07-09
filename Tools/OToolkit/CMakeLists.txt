cmake_minimum_required(VERSION 3.20)
project(OToolkit LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OToolkit OT_OTOOLKIT_ROOT_PATH)
ot_add_qt_resources(OToolkit)

ot_add_dependency(OToolkit
    OTSystem
    OTCore
    OTGui
    OTCommunication
    OTGuiAPI
    OTServiceFoundation
    OTModelEntities
    OTDataStorage
    OTModelAPI
    OTWidgets
    OToolkitAPI
    RJSON
    CURL
    MONGO_C
    MONGO_CXX
    MONGO_BOOST
    ADS
    QtCore
    QtGui
    QtNetwork
    QtWidgets
    QtWebSockets
)

ot_finalize_lib(OToolkit)

ot_tool_debug_launch(OToolkit
    ARGSD "unused" "127.0.0.1:8080" "unused" "unused"
    ARGSR "unused" "127.0.0.1:8094" "127.0.0.1:8095" "unused"
    PATHD "@OT_DEFAULT_SERVICE_DLLD@;@OT_GUI_ROOT@\\@OT_CDLLD@;@OT_WIDGETS_ROOT@\\@OT_CDLLD@;@OT_OTOOLKITAPI_ROOT@\\@OT_CDLLD@;@QT_ADS_ROOT@\\lib;@QT_DLLD@;@PATH@"
    PATHR "@QT_DLLR@;@OT_DEFAULT_SERVICE_DLLR@;@OT_WIDGETS_ROOT@\\@OT_CDLLR@;@OT_OTOOLKITAPI_ROOT@\\@OT_CDLLR@;@QT_ADS_ROOT@\\lib;@PATH@"
)
