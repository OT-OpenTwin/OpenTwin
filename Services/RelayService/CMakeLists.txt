cmake_minimum_required(VERSION 3.20)
project(RelayService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(RelayService OT_RELAY_SERVICE_ROOT_PATH RELAYSERVICE_EXPORTS)

ot_add_dependency(RelayService
    OTSystem
    OTCore
    OTGui
    OTCommunication
    OTGuiAPI
    OTServiceFoundation
    OTModelEntities
    OTDataStorage
    OTModelAPI
    RJSON
    CURL
    MONGO_C
    MONGO_CXX
    BOOST
    BASE64
    QtCore
    QtGui
    QtWidgets
    QtNetwork
    QtWebSockets
)

ot_finalize_lib(RelayService)
ot_add_test(RelayService)
