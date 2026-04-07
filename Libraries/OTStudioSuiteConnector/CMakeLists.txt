cmake_minimum_required(VERSION 3.20)
project(OTStudioSuiteConnector LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTStudioSuiteConnector OT_STUDIOSUITECONNECTOR_ROOT_PATH)
ot_add_export(OTStudioSuiteConnector OT_STUDIOSUITE_CONNECTOR_EXPORTS)

ot_add_dependency(OTStudioSuiteConnector
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

ot_finalize_lib(OTStudioSuiteConnector)
ot_add_test(OTStudioSuiteConnector)
