cmake_minimum_required(VERSION 3.20)
project(uiService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_bin(uiService OT_UI_SERVICE_ROOT_PATH)
ot_add_compile_options(uiService NO_SDL)
ot_add_resources(uiService)

ot_add_dependency(uiService
    OTSystem
    OTCore
    OTCrypto
    OTGui
    OTGuiAPI
    OTWidgets
    OTViewer
    OTCommunication
    OTFrontendConnectorAPI
    OTFMC
    OTModelEntities
    OTDataStorage
    OTRubberband
    OTRubberbandOSG
    OTStudioSuiteConnector
    OTLTSpiceConnector
    UICore

    RJSON
    QtFull
    QtEntryPoint
    QT_TT
    ADS
    QWT
    ZLIB
    BASE64
    MONGO_CXX

    OSLibs
)

ot_finalize_bin(uiService uiFrontend)

#ot_bin_debug_launch(uiService
#    ARGS "--login-ticket"
#         "eyJUaWNrZXQiOiAiOTM2M2Q5NjdiYjk3YTU1NzhmOGVhNWMzZWUxZTdiZDE2Y2Q4NDI0MDI3MzBlMGQ2NDAwYjg0N2M2MjkzY2M0NiIsICJTZXJ2aWNlLkdTUy5VUkwiOiAiaHR0cHM6Ly8xMjcuMC4wLjE6ODA5MSJ9"
#)

ot_deploy_app_configuration(uiService)

ot_add_test(uiService)
