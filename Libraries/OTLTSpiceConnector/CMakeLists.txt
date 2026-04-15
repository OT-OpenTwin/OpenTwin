cmake_minimum_required(VERSION 3.20)
project(OTLTSpiceConnector LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTLTSpiceConnector OT_LTSPICE_CONNECTOR_ROOT_PATH)
ot_add_export(OTLTSpiceConnector OT_LTSPICE_CONNECTOR_EXPORTS)

ot_add_dependency(OTLTSpiceConnector
    OTSystem
    OTCore
    OTGui
    OTWidgets
    OTCommunication
    OTFrontendConnectorAPI
    OTDataStorage
    OTModelEntities
    OTRubberband
    QtWidgets
    QtNetwork
    BASE64
    RJSON
    ZLIB
    MONGO_CXX
    BOOST
    OSG
)

ot_finalize_lib(OTLTSpiceConnector)
ot_add_test(OTLTSpiceConnector)
