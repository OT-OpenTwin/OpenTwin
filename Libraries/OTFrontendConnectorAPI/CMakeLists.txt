cmake_minimum_required(VERSION 3.20)
project(OTFrontendConnectorAPI LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTFrontendConnectorAPI OT_FRONTEND_CONNECTOR_API_ROOT_PATH)
ot_add_export(OTFrontendConnectorAPI OT_FRONTEND_CONNECTOR_API_EXPORTS)

ot_add_dependancy(OTFrontendConnectorAPI
    OTSystem
    OTCore
    OTGui
    OTWidgets
    OTCommunication
    QtWidgets
    QtNetwork
    ZLIB
    RJSON
)

ot_finalize_lib(OTFrontendConnectorAPI)
ot_add_test(OTFrontendConnectorAPI)
