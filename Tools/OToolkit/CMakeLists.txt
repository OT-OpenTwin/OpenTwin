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
